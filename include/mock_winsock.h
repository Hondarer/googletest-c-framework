#ifdef _WIN32

    #ifndef _MOCK_WINSOCK_H
        #define _MOCK_WINSOCK_H

        // In C++17 mode, std::byte conflicts with the Windows SDK's typedef unsigned char byte;
        // in rpcndr.h. Setting _HAS_STD_BYTE to 0 suppresses the conflicting using-declaration
        // in the Windows SDK headers.
        #ifndef _HAS_STD_BYTE
            #define _HAS_STD_BYTE 0
        #endif

        #include <winsock2.h>
        #include <ws2tcpip.h>

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern int mock_WSAStartup(const char *, const int, const char *, WORD, LPWSADATA);
    extern int mock_WSACleanup(const char *, const int, const char *);
    extern int mock_WSAGetLastError(const char *, const int, const char *);
    extern int mock_WSAPoll(const char *, const int, const char *, LPWSAPOLLFD, ULONG, INT);
    extern SOCKET mock_socket(const char *, const int, const char *, int, int, int);
    extern int mock_bind(const char *, const int, const char *, SOCKET, const struct sockaddr *, int);
    extern int mock_listen(const char *, const int, const char *, SOCKET, int);
    extern SOCKET mock_accept(const char *, const int, const char *, SOCKET, struct sockaddr *, int *);
    extern int mock_connect(const char *, const int, const char *, SOCKET, const struct sockaddr *, int);
    extern int mock_shutdown(const char *, const int, const char *, SOCKET, int);
    extern int mock_closesocket(const char *, const int, const char *, SOCKET);
    extern int mock_ioctlsocket(const char *, const int, const char *, SOCKET, long, u_long *);
    extern int mock_setsockopt(const char *, const int, const char *, SOCKET, int, int, const char *, int);
    extern int mock_getsockopt(const char *, const int, const char *, SOCKET, int, int, char *, int *);
    extern int mock_send(const char *, const int, const char *, SOCKET, const char *, int, int);
    extern int mock_recv(const char *, const int, const char *, SOCKET, char *, int, int);
    extern int mock_sendto(const char *, const int, const char *, SOCKET, const char *, int, int, const struct sockaddr *, int);
    extern int mock_recvfrom(const char *, const int, const char *, SOCKET, char *, int, int, struct sockaddr *, int *);
    extern INT mock_inet_pton(const char *, const int, const char *, INT, PCSTR, PVOID);
    extern PCSTR mock_inet_ntop(const char *, const int, const char *, INT, const void *, PSTR, size_t);
    extern INT mock_getaddrinfo(const char *, const int, const char *, PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
    extern void mock_freeaddrinfo(const char *, const int, const char *, PADDRINFOA);

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_WINSOCK_H

            #define WSAStartup(version_required, wsa_data) \
                mock_WSAStartup(__FILE__, __LINE__, __func__, version_required, wsa_data)
            #define WSACleanup()     mock_WSACleanup(__FILE__, __LINE__, __func__)
            #define WSAGetLastError() mock_WSAGetLastError(__FILE__, __LINE__, __func__)
            #define WSAPoll(fd_array, fds, timeout) \
                mock_WSAPoll(__FILE__, __LINE__, __func__, fd_array, fds, timeout)
            #define socket(af, type, protocol) mock_socket(__FILE__, __LINE__, __func__, af, type, protocol)
            #define bind(s, name, namelen)     mock_bind(__FILE__, __LINE__, __func__, s, name, namelen)
            #define listen(s, backlog)         mock_listen(__FILE__, __LINE__, __func__, s, backlog)
            #define accept(s, addr, addrlen)   mock_accept(__FILE__, __LINE__, __func__, s, addr, addrlen)
            #define connect(s, name, namelen)  mock_connect(__FILE__, __LINE__, __func__, s, name, namelen)
            #define shutdown(s, how)           mock_shutdown(__FILE__, __LINE__, __func__, s, how)
            #define closesocket(s)             mock_closesocket(__FILE__, __LINE__, __func__, s)
            #define ioctlsocket(s, cmd, argp)  mock_ioctlsocket(__FILE__, __LINE__, __func__, s, cmd, argp)
            #define setsockopt(s, level, optname, optval, optlen) \
                mock_setsockopt(__FILE__, __LINE__, __func__, s, level, optname, optval, optlen)
            #define getsockopt(s, level, optname, optval, optlen) \
                mock_getsockopt(__FILE__, __LINE__, __func__, s, level, optname, optval, optlen)
            #define send(s, buf, len, flags) mock_send(__FILE__, __LINE__, __func__, s, buf, len, flags)
            #define recv(s, buf, len, flags) mock_recv(__FILE__, __LINE__, __func__, s, buf, len, flags)
            #define sendto(s, buf, len, flags, to, tolen) \
                mock_sendto(__FILE__, __LINE__, __func__, s, buf, len, flags, to, tolen)
            #define recvfrom(s, buf, len, flags, from, fromlen) \
                mock_recvfrom(__FILE__, __LINE__, __func__, s, buf, len, flags, from, fromlen)
            #define inet_pton(family, addr_string, addr_buf) \
                mock_inet_pton(__FILE__, __LINE__, __func__, family, addr_string, addr_buf)
            #define inet_ntop(family, addr, string_buf, string_buf_size) \
                mock_inet_ntop(__FILE__, __LINE__, __func__, family, addr, string_buf, string_buf_size)
            #define getaddrinfo(node_name, service_name, hints, result) \
                mock_getaddrinfo(__FILE__, __LINE__, __func__, node_name, service_name, hints, result)
            #define freeaddrinfo(addr_info) mock_freeaddrinfo(__FILE__, __LINE__, __func__, addr_info)

        #else // _IN_OVERRIDE_HEADER_WINSOCK_H

            #include <gmock/gmock.h>

extern int delegate_real_WSAStartup(const char *, const int, const char *, WORD, LPWSADATA);
extern int delegate_real_WSACleanup(const char *, const int, const char *);
extern int delegate_real_WSAGetLastError(const char *, const int, const char *);
extern int delegate_real_WSAPoll(const char *, const int, const char *, LPWSAPOLLFD, ULONG, INT);
extern SOCKET delegate_real_socket(const char *, const int, const char *, int, int, int);
extern int delegate_real_bind(const char *, const int, const char *, SOCKET, const struct sockaddr *, int);
extern int delegate_real_listen(const char *, const int, const char *, SOCKET, int);
extern SOCKET delegate_real_accept(const char *, const int, const char *, SOCKET, struct sockaddr *, int *);
extern int delegate_real_connect(const char *, const int, const char *, SOCKET, const struct sockaddr *, int);
extern int delegate_real_shutdown(const char *, const int, const char *, SOCKET, int);
extern int delegate_real_closesocket(const char *, const int, const char *, SOCKET);
extern int delegate_real_ioctlsocket(const char *, const int, const char *, SOCKET, long, u_long *);
extern int delegate_real_setsockopt(const char *, const int, const char *, SOCKET, int, int, const char *, int);
extern int delegate_real_getsockopt(const char *, const int, const char *, SOCKET, int, int, char *, int *);
extern int delegate_real_send(const char *, const int, const char *, SOCKET, const char *, int, int);
extern int delegate_real_recv(const char *, const int, const char *, SOCKET, char *, int, int);
extern int delegate_real_sendto(const char *, const int, const char *, SOCKET, const char *, int, int, const struct sockaddr *, int);
extern int delegate_real_recvfrom(const char *, const int, const char *, SOCKET, char *, int, int, struct sockaddr *, int *);
extern INT delegate_real_inet_pton(const char *, const int, const char *, INT, PCSTR, PVOID);
extern PCSTR delegate_real_inet_ntop(const char *, const int, const char *, INT, const void *, PSTR, size_t);
extern INT delegate_real_getaddrinfo(const char *, const int, const char *, PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
extern void delegate_real_freeaddrinfo(const char *, const int, const char *, PADDRINFOA);

class Mock_winsock
{
  public:
    MOCK_METHOD(int, WSAStartup, (const char *, const int, const char *, WORD, LPWSADATA));
    MOCK_METHOD(int, WSACleanup, (const char *, const int, const char *));
    MOCK_METHOD(int, WSAGetLastError, (const char *, const int, const char *));
    MOCK_METHOD(int, WSAPoll, (const char *, const int, const char *, LPWSAPOLLFD, ULONG, INT));
    MOCK_METHOD(SOCKET, socket, (const char *, const int, const char *, int, int, int));
    MOCK_METHOD(int, bind, (const char *, const int, const char *, SOCKET, const struct sockaddr *, int));
    MOCK_METHOD(int, listen, (const char *, const int, const char *, SOCKET, int));
    MOCK_METHOD(SOCKET, accept, (const char *, const int, const char *, SOCKET, struct sockaddr *, int *));
    MOCK_METHOD(int, connect, (const char *, const int, const char *, SOCKET, const struct sockaddr *, int));
    MOCK_METHOD(int, shutdown, (const char *, const int, const char *, SOCKET, int));
    MOCK_METHOD(int, closesocket, (const char *, const int, const char *, SOCKET));
    MOCK_METHOD(int, ioctlsocket, (const char *, const int, const char *, SOCKET, long, u_long *));
    MOCK_METHOD(int, setsockopt, (const char *, const int, const char *, SOCKET, int, int, const char *, int));
    MOCK_METHOD(int, getsockopt, (const char *, const int, const char *, SOCKET, int, int, char *, int *));
    MOCK_METHOD(int, send, (const char *, const int, const char *, SOCKET, const char *, int, int));
    MOCK_METHOD(int, recv, (const char *, const int, const char *, SOCKET, char *, int, int));
    MOCK_METHOD(int, sendto, (const char *, const int, const char *, SOCKET, const char *, int, int, const struct sockaddr *, int));
    MOCK_METHOD(int, recvfrom, (const char *, const int, const char *, SOCKET, char *, int, int, struct sockaddr *, int *));
    MOCK_METHOD(INT, inet_pton, (const char *, const int, const char *, INT, PCSTR, PVOID));
    MOCK_METHOD(PCSTR, inet_ntop, (const char *, const int, const char *, INT, const void *, PSTR, size_t));
    MOCK_METHOD(INT, getaddrinfo, (const char *, const int, const char *, PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *));
    MOCK_METHOD(void, freeaddrinfo, (const char *, const int, const char *, PADDRINFOA));

    Mock_winsock();
    ~Mock_winsock();
};

extern Mock_winsock *_mock_winsock;

        #endif // _IN_OVERRIDE_HEADER_WINSOCK_H

    #endif // _MOCK_WINSOCK_H

#endif // _WIN32
