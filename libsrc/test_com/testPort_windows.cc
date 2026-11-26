#ifdef _WIN32

/* テスト用ポートの払い出しの Windows 固有の実装。 */

    #include "testPort_impl.h"

    #include <cstdio>
    #include <mutex>
    #include <string>

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")

namespace testing
{
namespace test_port_internal
{
namespace
{

/** Windows の既定の動的ポートの範囲。 */
constexpr port_range DEFAULT_DYNAMIC_RANGE = {49152U, 65535U};

bool get_registry_wide_path(std::wstring &path, unsigned long &error);
bool start_winsock(const char *caller);
bool try_bind(const char *caller, int type, int protocol, uint16_t port, probe_result &result);
std::string to_utf8(const std::wstring &text);
void report_error(const char *caller, const char *function, unsigned long error);

} // namespace

port_range get_ephemeral_range()
{
    /* 既定の動的ポートの範囲の外から払い出す。予約された除外範囲は bind の確認で避ける。
     * see: https://learn.microsoft.com/en-us/troubleshoot/windows-server/networking/default-dynamic-port-range-tcpip-chang */
    return DEFAULT_DYNAMIC_RANGE;
}

std::string registry_path()
{
    std::wstring path;
    unsigned long error = 0U;

    if (!get_registry_wide_path(path, error))
    {
        return std::string();
    }
    return to_utf8(path);
}

bool registry_open(const char *caller, registry_file &file)
{
    std::wstring path;
    unsigned long error = 0U;

    if (!get_registry_wide_path(path, error))
    {
        report_error(caller, "GetTempPathW", error);
        return false;
    }

    HANDLE handle = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (handle == INVALID_HANDLE_VALUE)
    {
        report_error(caller, "CreateFileW", GetLastError());
        return false;
    }

    OVERLAPPED overlapped = {};

    if (!LockFileEx(handle, LOCKFILE_EXCLUSIVE_LOCK, 0U, MAXDWORD, MAXDWORD, &overlapped))
    {
        report_error(caller, "LockFileEx", GetLastError());
        CloseHandle(handle);
        return false;
    }
    file.value = reinterpret_cast<intptr_t>(handle);
    return true;
}

bool registry_read(const char *caller, registry_file &file, std::vector<uint8_t> &bytes)
{
    HANDLE handle = reinterpret_cast<HANDLE>(file.value);
    LARGE_INTEGER size = {};

    if (!GetFileSizeEx(handle, &size))
    {
        report_error(caller, "GetFileSizeEx", GetLastError());
        return false;
    }
    if (size.QuadPart != static_cast<LONGLONG>(REGISTRY_SIZE))
    {
        /* 大きさが違う記録は読まずに初期化の対象とする。 */
        bytes.clear();
        return true;
    }
    bytes.assign(REGISTRY_SIZE, 0U);

    OVERLAPPED overlapped = {};
    DWORD read_size = 0U;

    if (!ReadFile(handle, bytes.data(), static_cast<DWORD>(bytes.size()), &read_size, &overlapped))
    {
        report_error(caller, "ReadFile", GetLastError());
        return false;
    }
    bytes.resize(read_size);
    return true;
}

bool registry_write(const char *caller, registry_file &file, const std::vector<uint8_t> &bytes)
{
    HANDLE handle = reinterpret_cast<HANDLE>(file.value);
    OVERLAPPED overlapped = {};
    DWORD written = 0U;

    if (!WriteFile(handle, bytes.data(), static_cast<DWORD>(bytes.size()), &written, &overlapped))
    {
        report_error(caller, "WriteFile", GetLastError());
        return false;
    }
    if (written != bytes.size())
    {
        fprintf(stderr, "%s: WriteFile wrote %lu of %zu bytes.\n", caller, static_cast<unsigned long>(written),
                bytes.size());
        return false;
    }

    LARGE_INTEGER end = {};

    end.QuadPart = static_cast<LONGLONG>(bytes.size());
    if (!SetFilePointerEx(handle, end, nullptr, FILE_BEGIN) || !SetEndOfFile(handle))
    {
        report_error(caller, "SetEndOfFile", GetLastError());
        return false;
    }
    return true;
}

void registry_close(registry_file &file)
{
    if (file.value == -1)
    {
        return;
    }

    HANDLE handle = reinterpret_cast<HANDLE>(file.value);
    OVERLAPPED overlapped = {};

    /* 閉じればロックは解放されるため、失敗しても続ける。 */
    (void)UnlockFileEx(handle, 0U, MAXDWORD, MAXDWORD, &overlapped);
    CloseHandle(handle);
    file.value = -1;
}

probe_result probe_port(const char *caller, uint16_t port)
{
    if (!start_winsock(caller))
    {
        return probe_result::FAILED;
    }

    probe_result result = probe_result::AVAILABLE;

    if (!try_bind(caller, SOCK_STREAM, IPPROTO_TCP, port, result) || result != probe_result::AVAILABLE)
    {
        return result;
    }
    (void)try_bind(caller, SOCK_DGRAM, IPPROTO_UDP, port, result);
    return result;
}

namespace
{

/**
 * %TEMP% (GetTempPathW の結果) の下の記録のファイルのパスを返す。
 */
bool get_registry_wide_path(std::wstring &path, unsigned long &error)
{
    wchar_t dir[MAX_PATH + 1] = {};
    DWORD length = GetTempPathW(MAX_PATH + 1, dir);

    if (length == 0U)
    {
        error = GetLastError();
        return false;
    }
    if (length > MAX_PATH)
    {
        error = ERROR_INSUFFICIENT_BUFFER;
        return false;
    }
    path = std::wstring(dir, length) + L"testfw-port-registry.bin";
    return true;
}

/**
 * プロセスで 1 回だけ WSAStartup を呼ぶ。WSACleanup は呼ばず、プロセスの終了で解放する。
 */
bool start_winsock(const char *caller)
{
    static std::once_flag s_once;
    static int s_result = 0;

    std::call_once(s_once,
                   []()
                   {
                       WSADATA data = {};
                       s_result = WSAStartup(MAKEWORD(2, 2), &data);
                   });
    if (s_result != 0)
    {
        report_error(caller, "WSAStartup", static_cast<unsigned long>(s_result));
        return false;
    }
    return true;
}

/**
 * type のソケットを作り、0.0.0.0 の port へ bind して閉じる。
 * bind の失敗は result を IN_USE にして true を返す。それ以外の失敗は result を FAILED にして false を返す。
 */
bool try_bind(const char *caller, int type, int protocol, uint16_t port, probe_result &result)
{
    SOCKET sock = socket(AF_INET, type, protocol);

    if (sock == INVALID_SOCKET)
    {
        report_error(caller, "socket", static_cast<unsigned long>(WSAGetLastError()));
        result = probe_result::FAILED;
        return false;
    }

    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(sock, reinterpret_cast<sockaddr *>(&addr), static_cast<int>(sizeof(addr))) == SOCKET_ERROR)
    {
        /* WSAEADDRINUSE のほか、予約された除外範囲は WSAEACCES になる。どちらも使用できない番号として扱う。 */
        result = probe_result::IN_USE;
    }
    closesocket(sock);
    return true;
}

std::string to_utf8(const std::wstring &text)
{
    int size =
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);

    if (size <= 0)
    {
        return std::string();
    }

    std::string utf8(static_cast<size_t>(size), '\0');

    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &utf8[0], size, nullptr, nullptr);
    return utf8;
}

void report_error(const char *caller, const char *function, unsigned long error)
{
    fprintf(stderr, "%s: %s failed: error=%lu\n", caller, function, error);
}

} // namespace
} // namespace test_port_internal
} // namespace testing

#endif /* _WIN32 */
