#ifdef _WIN32

    #ifndef _MOCK_SHELLAPI_H
        #define _MOCK_SHELLAPI_H

        /*
         * 差し替え用の shellapi.h から取り込む場合は、SDK の shellapi.h と、それが前提とする windows.h の型を
         * 取り込み済みのため、ここでは何も取り込まない。取り込むと、windows.h が先に取り込んだ winsock.h と、
         * winsock2.h が衝突する。
         */
        #ifndef _IN_OVERRIDE_HEADER_SHELLAPI_H
            /* shellapi.h は windows.h の型を前提とするため、windows.h を先に取り込む。 */
            /* winsock2.h を windows.h より先に取り込み、winsock.h との衝突を防ぐ。   */
            #ifndef WIN32_LEAN_AND_MEAN
                #define WIN32_LEAN_AND_MEAN
            #endif /* WIN32_LEAN_AND_MEAN */
            #include <winsock2.h>
            #include <ws2tcpip.h>
            #include <windows.h>
            #include <shellapi.h>
            #ifdef byte
                #undef byte /* C++17 std::byte と Windows SDK byte typedef の競合を解消 */
            #endif          /* byte */
        #endif              /* _IN_OVERRIDE_HEADER_SHELLAPI_H */

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern BOOL mock_ShellExecuteExW(const char *, const int, const char *, SHELLEXECUTEINFOW *);

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_SHELLAPI_H

            #define ShellExecuteExW(exec_info) mock_ShellExecuteExW(__FILE__, __LINE__, __func__, exec_info)

        #else // _IN_OVERRIDE_HEADER_SHELLAPI_H

            #include <gmock/gmock.h>

extern BOOL delegate_real_ShellExecuteExW(const char *, const int, const char *, SHELLEXECUTEINFOW *);

class Mock_shellapi
{
  public:
    MOCK_METHOD(BOOL, ShellExecuteExW, (const char *, const int, const char *, SHELLEXECUTEINFOW *));

    Mock_shellapi();
    virtual ~Mock_shellapi();
};

extern Mock_shellapi *_mock_shellapi;

        #endif // _IN_OVERRIDE_HEADER_SHELLAPI_H

    #endif // _MOCK_SHELLAPI_H

#endif // _WIN32
