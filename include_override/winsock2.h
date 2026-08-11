#ifdef _WIN32

    /* winsock2.h と ws2tcpip.h は相互に取り込み合うため、実ヘッダーの取り込み中は     */
    /* モックへの差し替えを抑止する。抑止しないと入れ子の取り込みで置換が無効化される。 */
    #if defined(_IN_OVERRIDE_WINSOCK_REAL_HEADERS)

        #pragma push_macro("_WINSOCK2API_")
        #undef _WINSOCK2API_
        #include <../um/winsock2.h>
        #pragma pop_macro("_WINSOCK2API_")

    #else // _IN_OVERRIDE_WINSOCK_REAL_HEADERS

        #ifndef _OVERRIDE_WINSOCK2_H
            #define _OVERRIDE_WINSOCK2_H

            /* 本物を include */
            #define _IN_OVERRIDE_WINSOCK_REAL_HEADERS
            #pragma push_macro("_WINSOCK2API_")
            #undef _WINSOCK2API_
            #include <../um/winsock2.h>
            #pragma pop_macro("_WINSOCK2API_")
            #undef _IN_OVERRIDE_WINSOCK_REAL_HEADERS

            /* モックにすげ替え */
            #define _IN_OVERRIDE_HEADER_WINSOCK_H
            #include <mock_winsock.h>
            #undef _IN_OVERRIDE_HEADER_WINSOCK_H

        #endif // _OVERRIDE_WINSOCK2_H

    #endif // _IN_OVERRIDE_WINSOCK_REAL_HEADERS

#endif // _WIN32
