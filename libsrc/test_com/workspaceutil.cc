#include <test_com.h>

#include <sys/stat.h>

#ifndef _WIN32
    #include <limits.h>
    #include <unistd.h>
#else /* _WIN32 */
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    /* C++17 の std::byte と Windows SDK (rpcndr.h 等) の byte 型の競合を解消する。
     * using namespace std; がある状態で windows.h をインクルードすると、
     * rpcndr.h の typedef unsigned char byte; と std::byte が ambiguous になる。 */
    #ifdef byte
        #undef byte
    #endif
#endif /* _WIN32 */

#ifndef _WIN32
static const char PATH_SEP = '/';
#else  /* _WIN32 */
static const char PATH_SEP = '\\';
#endif /* _WIN32 */

/* 実行ファイルのあるディレクトリを返す。失敗時は空文字列。 */
static string getExeDir()
{
#ifndef _WIN32
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1)
    {
        return "";
    }
    buf[len] = '\0';
#else  /* _WIN32 */
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (len == 0)
    {
        return "";
    }
#endif /* _WIN32 */
    string path(buf);
    size_t pos = path.rfind(PATH_SEP);
    return (pos != string::npos) ? path.substr(0, pos) : "";
}

/* dir がファイルシステムのルートであれば true を返す */
static bool isRootDir(const string &dir)
{
#ifndef _WIN32
    return dir == "/";
#else  /* _WIN32 */
    /* "C:" または "C:\" のようなドライブルート */
    return dir.size() >= 2 && dir[1] == ':' && dir.size() <= 3;
#endif /* _WIN32 */
}

string testing::findWorkspaceRoot()
{
    string dir = getExeDir();

    while (!dir.empty() && !isRootDir(dir))
    {
        struct stat st;
        string marker = dir + PATH_SEP + ".workspaceRoot";
        if (stat(marker.c_str(), &st) == 0)
        {
            return dir;
        }
        size_t pos = dir.rfind(PATH_SEP);
        if (pos == string::npos || pos == 0)
        {
            break;
        }
        dir = dir.substr(0, pos);
    }
    return "";
}
