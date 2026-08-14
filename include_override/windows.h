#ifdef _WIN32

    /* winsock2.h / ws2tcpip.h の実ヘッダー取り込み中に windows.h が再入する。 */
    /* この経路で mock_windows.h を取り込むと、C ソースから mock_winsock.h の */
    /* C++ 分岐へ到達するため、実ヘッダーだけを取り込む。                 */
    #if defined(_IN_OVERRIDE_WINSOCK_REAL_HEADERS)

        #pragma push_macro("_WINDOWS_")
        #undef _WINDOWS_
        #include <../um/windows.h>
        #pragma pop_macro("_WINDOWS_")

    #else // _IN_OVERRIDE_WINSOCK_REAL_HEADERS

        #ifndef _OVERRIDE_WINDOWS_H
            #define _OVERRIDE_WINDOWS_H

            /* winsock2.h を windows.h より先に取り込み、winsock.h との衝突を防ぐ。 */
            #ifndef WIN32_LEAN_AND_MEAN
                #define WIN32_LEAN_AND_MEAN
            #endif /* WIN32_LEAN_AND_MEAN */
            #define _IN_OVERRIDE_WINSOCK_REAL_HEADERS
            #include <../um/winsock2.h>
            #include <../um/ws2tcpip.h>
            #undef _IN_OVERRIDE_WINSOCK_REAL_HEADERS

            /* 本物を include */
            #pragma push_macro("_WINDOWS_")
            #undef _WINDOWS_
            #include <../um/windows.h>
            #pragma pop_macro("_WINDOWS_")

            /* モックにすげ替え */
            #define _IN_OVERRIDE_HEADER_WINDOWS_H
            #include <mock_windows.h>
            #undef _IN_OVERRIDE_HEADER_WINDOWS_H

        #endif // _OVERRIDE_WINDOWS_H

    #endif // _IN_OVERRIDE_WINSOCK_REAL_HEADERS

#endif // _WIN32
