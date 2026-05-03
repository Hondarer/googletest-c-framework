#include <testfw/console/console_internal.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef byte
#undef byte
#endif

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <thread>

#include <fcntl.h>
#include <io.h>

namespace testing {

namespace {

constexpr DWORD kReadBufferSize = 4096;

struct StreamState {
    HANDLE      orig_handle = nullptr;
    HANDLE      pipe_read   = nullptr;
    int         orig_crt_fd = -1;
    std::thread reader_thread;
    bool        active      = false;
};

std::mutex g_console_mutex;
int        g_console_refcount = 0;
StreamState g_stdout_state;
StreamState g_stderr_state;

int utf8_complete_length(const char *buf, int len)
{
    int i;
    unsigned char lead;
    int seq_len;

    if (len == 0) {
        return 0;
    }

    i = len - 1;
    while (i >= 0 && (static_cast<unsigned char>(buf[i]) & 0xC0) == 0x80) {
        --i;
    }
    if (i < 0) {
        return 0;
    }

    lead = static_cast<unsigned char>(buf[i]);
    if ((lead & 0x80) == 0x00) {
        seq_len = 1;
    } else if ((lead & 0xE0) == 0xC0) {
        seq_len = 2;
    } else if ((lead & 0xF0) == 0xE0) {
        seq_len = 3;
    } else if ((lead & 0xF8) == 0xF0) {
        seq_len = 4;
    } else {
        return len;
    }

    return (len - i >= seq_len) ? len : i;
}

bool is_console_handle(HANDLE handle)
{
    DWORD mode = 0;

    return handle != nullptr
        && handle != INVALID_HANDLE_VALUE
        && GetFileType(handle) == FILE_TYPE_CHAR
        && GetConsoleMode(handle, &mode);
}

void reset_stream_state(StreamState *state)
{
    state->orig_handle = nullptr;
    state->pipe_read = nullptr;
    state->orig_crt_fd = -1;
    state->reader_thread = std::thread();
    state->active = false;
}

void reader_thread_proc(StreamState *state)
{
    char    buf[kReadBufferSize];
    wchar_t wide_buf[kReadBufferSize];
    int     pending_len = 0;
    const bool console_output = is_console_handle(state->orig_handle);

    for (;;) {
        DWORD nread = 0;
        const BOOL ok = ReadFile(state->pipe_read,
                                 buf + pending_len,
                                 kReadBufferSize - static_cast<DWORD>(pending_len),
                                 &nread,
                                 nullptr);
        if (!ok || nread == 0) {
            break;
        }

        const int total = pending_len + static_cast<int>(nread);

        if (!console_output) {
            DWORD nwritten = 0;
            WriteFile(state->orig_handle, buf, static_cast<DWORD>(total), &nwritten, nullptr);
            pending_len = 0;
            continue;
        }

        const int complete = utf8_complete_length(buf, total);
        const int new_pending = total - complete;

        if (complete > 0) {
            const int wide_len = MultiByteToWideChar(CP_UTF8, 0, buf, complete,
                                                     wide_buf, kReadBufferSize);
            if (wide_len > 0) {
                DWORD nwritten = 0;
                if (!WriteConsoleW(state->orig_handle,
                                   wide_buf,
                                   static_cast<DWORD>(wide_len),
                                   &nwritten,
                                   nullptr)) {
                    WriteFile(state->orig_handle,
                              buf,
                              static_cast<DWORD>(complete),
                              &nwritten,
                              nullptr);
                }
            } else {
                DWORD nwritten = 0;
                WriteFile(state->orig_handle,
                          buf,
                          static_cast<DWORD>(complete),
                          &nwritten,
                          nullptr);
            }
        }

        if (new_pending > 0) {
            memmove(buf, buf + complete, static_cast<size_t>(new_pending));
        }
        pending_len = new_pending;
    }

    if (pending_len > 0) {
        DWORD nwritten = 0;
        WriteFile(state->orig_handle,
                  buf,
                  static_cast<DWORD>(pending_len),
                  &nwritten,
                  nullptr);
    }
}

void cleanup_stream_resources(StreamState *state)
{
    if (state->reader_thread.joinable()) {
        state->reader_thread.join();
    }
    if (state->pipe_read != nullptr) {
        CloseHandle(state->pipe_read);
    }
    if (state->orig_handle != nullptr) {
        CloseHandle(state->orig_handle);
    }
    reset_stream_state(state);
}

int init_stream(StreamState *state, DWORD std_handle_id, FILE *crt_stream)
{
    HANDLE proc = GetCurrentProcess();
    HANDLE orig = GetStdHandle(std_handle_id);
    HANDLE pipe_read = nullptr;
    HANDLE pipe_write = nullptr;
    SECURITY_ATTRIBUTES sa = {};
    int new_fd = -1;

    if (orig == nullptr || orig == INVALID_HANDLE_VALUE) {
        return -1;
    }

    if (!DuplicateHandle(proc, orig, proc, &state->orig_handle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        return -1;
    }

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&pipe_read, &pipe_write, &sa, 0)) {
        CloseHandle(state->orig_handle);
        state->orig_handle = nullptr;
        return -1;
    }
    SetHandleInformation(pipe_read, HANDLE_FLAG_INHERIT, 0);
    state->pipe_read = pipe_read;

    state->orig_crt_fd = _dup(_fileno(crt_stream));
    if (state->orig_crt_fd == -1) {
        CloseHandle(pipe_write);
        CloseHandle(state->pipe_read);
        CloseHandle(state->orig_handle);
        reset_stream_state(state);
        return -1;
    }

    new_fd = _open_osfhandle(reinterpret_cast<intptr_t>(pipe_write), _O_WRONLY | _O_BINARY);
    if (new_fd == -1) {
        _close(state->orig_crt_fd);
        CloseHandle(state->pipe_read);
        CloseHandle(state->orig_handle);
        reset_stream_state(state);
        return -1;
    }

    if (_dup2(new_fd, _fileno(crt_stream)) != 0) {
        _close(new_fd);
        _close(state->orig_crt_fd);
        CloseHandle(state->pipe_read);
        CloseHandle(state->orig_handle);
        reset_stream_state(state);
        return -1;
    }
    _close(new_fd);
    setvbuf(crt_stream, nullptr, _IONBF, 0);

    try {
        state->reader_thread = std::thread(reader_thread_proc, state);
    } catch (...) {
        _dup2(state->orig_crt_fd, _fileno(crt_stream));
        _close(state->orig_crt_fd);
        CloseHandle(state->pipe_read);
        CloseHandle(state->orig_handle);
        reset_stream_state(state);
        return -1;
    }

    state->active = true;
    return 0;
}

void dispose_stream(StreamState *state, FILE *crt_stream)
{
    if (!state->active) {
        return;
    }

    fflush(crt_stream);
    _dup2(state->orig_crt_fd, _fileno(crt_stream));
    _close(state->orig_crt_fd);
    state->orig_crt_fd = -1;
    cleanup_stream_resources(state);
}

bool should_activate_console_utf8()
{
    return is_console_handle(GetStdHandle(STD_OUTPUT_HANDLE))
        || is_console_handle(GetStdHandle(STD_ERROR_HANDLE));
}

void initialize_console_utf8()
{
    if (!should_activate_console_utf8()) {
        return;
    }

    if (init_stream(&g_stdout_state, STD_OUTPUT_HANDLE, stdout) != 0) {
        fprintf(stderr, "ScopedConsoleUtf8: stdout initialization failed.\n");
        fflush(stderr);
        return;
    }

    if (init_stream(&g_stderr_state, STD_ERROR_HANDLE, stderr) != 0) {
        dispose_stream(&g_stdout_state, stdout);
        fprintf(stderr, "ScopedConsoleUtf8: stderr initialization failed.\n");
        fflush(stderr);
    }
}

void dispose_console_utf8()
{
    dispose_stream(&g_stderr_state, stderr);
    dispose_stream(&g_stdout_state, stdout);
}

} // namespace

ScopedConsoleUtf8::ScopedConsoleUtf8()
{
    std::lock_guard<std::mutex> lock(g_console_mutex);

    if (g_console_refcount == 0) {
        initialize_console_utf8();
    }
    ++g_console_refcount;
}

ScopedConsoleUtf8::~ScopedConsoleUtf8()
{
    std::lock_guard<std::mutex> lock(g_console_mutex);

    if (g_console_refcount == 0) {
        return;
    }

    --g_console_refcount;
    if (g_console_refcount == 0) {
        dispose_console_utf8();
    }
}

} // namespace testing

#else

namespace testing {

ScopedConsoleUtf8::ScopedConsoleUtf8() = default;
ScopedConsoleUtf8::~ScopedConsoleUtf8() = default;

} // namespace testing

#endif
