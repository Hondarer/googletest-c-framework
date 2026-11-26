#ifndef _WIN32

/* テスト用ポートの払い出しの Linux 固有の実装。 */

    #include "testPort_impl.h"

    #include <cerrno>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <string>

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/file.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <unistd.h>

namespace testing
{
namespace test_port_internal
{
namespace
{

/** ip_local_port_range の既定の範囲。 */
constexpr port_range DEFAULT_EPHEMERAL_RANGE = {32768U, 60999U};

bool try_bind(const char *caller, int type, uint16_t port, probe_result &result);
void report_errno(const char *caller, const char *function, int error);

} // namespace

port_range get_ephemeral_range()
{
    return read_ephemeral_range("/proc/sys/net/ipv4/ip_local_port_range");
}

port_range read_ephemeral_range(const char *path)
{
    FILE *fp = fopen(path, "r");

    if (fp == nullptr)
    {
        return DEFAULT_EPHEMERAL_RANGE;
    }

    unsigned int first = 0U;
    unsigned int last = 0U;
    int scanned = fscanf(fp, "%u %u", &first, &last);

    fclose(fp);
    if (scanned != 2 || first > last || last > 65535U)
    {
        return DEFAULT_EPHEMERAL_RANGE;
    }
    return port_range{static_cast<uint16_t>(first), static_cast<uint16_t>(last)};
}

std::string registry_path()
{
    const char *dir = getenv("TMPDIR");

    if (dir == nullptr || dir[0] == '\0')
    {
        dir = "/tmp";
    }
    return std::string(dir) + "/testfw-port-registry-" + std::to_string(static_cast<unsigned long>(getuid())) + ".bin";
}

bool registry_open(const char *caller, registry_file &file)
{
    std::string path = registry_path();
    int fd = open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        report_errno(caller, "open", errno);
        return false;
    }

    int locked;

    do
    {
        locked = flock(fd, LOCK_EX);
    } while (locked == -1 && errno == EINTR);
    if (locked == -1)
    {
        report_errno(caller, "flock", errno);
        close(fd);
        return false;
    }
    file.value = fd;
    return true;
}

bool registry_read(const char *caller, registry_file &file, std::vector<uint8_t> &bytes)
{
    int fd = static_cast<int>(file.value);
    struct stat st = {};

    if (fstat(fd, &st) == -1)
    {
        report_errno(caller, "fstat", errno);
        return false;
    }
    if (st.st_size < 0 || static_cast<unsigned long long>(st.st_size) != REGISTRY_SIZE)
    {
        /* 大きさが違う記録は読まずに初期化の対象とする。 */
        bytes.clear();
        return true;
    }
    bytes.assign(REGISTRY_SIZE, 0U);

    ssize_t read_size = pread(fd, bytes.data(), bytes.size(), 0);

    if (read_size == -1)
    {
        report_errno(caller, "pread", errno);
        return false;
    }
    bytes.resize(static_cast<size_t>(read_size));
    return true;
}

bool registry_write(const char *caller, registry_file &file, const std::vector<uint8_t> &bytes)
{
    int fd = static_cast<int>(file.value);
    ssize_t written = pwrite(fd, bytes.data(), bytes.size(), 0);

    if (written == -1)
    {
        report_errno(caller, "pwrite", errno);
        return false;
    }
    if (static_cast<size_t>(written) != bytes.size())
    {
        fprintf(stderr, "%s: pwrite wrote %zd of %zu bytes.\n", caller, written, bytes.size());
        return false;
    }
    if (ftruncate(fd, static_cast<off_t>(bytes.size())) == -1)
    {
        report_errno(caller, "ftruncate", errno);
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

    int fd = static_cast<int>(file.value);

    (void)flock(fd, LOCK_UN); /* 閉じればロックは解放されるため、失敗しても続ける。 */
    close(fd);
    file.value = -1;
}

probe_result probe_port(const char *caller, uint16_t port)
{
    probe_result result = probe_result::AVAILABLE;

    if (!try_bind(caller, SOCK_STREAM, port, result) || result != probe_result::AVAILABLE)
    {
        return result;
    }
    (void)try_bind(caller, SOCK_DGRAM, port, result);
    return result;
}

namespace
{

/**
 * type のソケットを作り、0.0.0.0 の port へ bind して閉じる。
 * bind の失敗は result を IN_USE にして true を返す。それ以外の失敗は result を FAILED にして false を返す。
 */
bool try_bind(const char *caller, int type, uint16_t port, probe_result &result)
{
    int fd = socket(AF_INET, type | SOCK_CLOEXEC, 0);

    if (fd == -1)
    {
        report_errno(caller, "socket", errno);
        result = probe_result::FAILED;
        return false;
    }

    struct sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
    {
        result = probe_result::IN_USE;
    }
    close(fd);
    return true;
}

void report_errno(const char *caller, const char *function, int error)
{
    fprintf(stderr, "%s: %s failed: errno=%d (%s)\n", caller, function, error, strerror(error));
}

} // namespace
} // namespace test_port_internal
} // namespace testing

#endif /* _WIN32 */
