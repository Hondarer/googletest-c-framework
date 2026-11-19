#ifdef _WIN32

    #ifndef _MOCK_BCRYPT_H
        #define _MOCK_BCRYPT_H

        /*
         * 差し替え用の bcrypt.h から取り込む場合は、SDK の bcrypt.h と、それが前提とする windows.h の型を
         * 取り込み済みのため、ここでは何も取り込まない。取り込むと、windows.h が先に取り込んだ winsock.h と、
         * winsock2.h が衝突する (openssl のヘッダー -> windows.h -> wincrypt.h -> bcrypt.h の順など)。
         */
        #ifndef _IN_OVERRIDE_HEADER_BCRYPT_H
            /* bcrypt.h は windows.h の型を前提とするため、windows.h を先に取り込む。 */
            /* winsock2.h を windows.h より先に取り込み、winsock.h との衝突を防ぐ。   */
            #ifndef WIN32_LEAN_AND_MEAN
                #define WIN32_LEAN_AND_MEAN
            #endif /* WIN32_LEAN_AND_MEAN */
            #include <winsock2.h>
            #include <ws2tcpip.h>
            #include <windows.h>
            #include <bcrypt.h>
            #ifdef byte
                #undef byte /* C++17 std::byte と Windows SDK byte typedef の競合を解消 */
            #endif          /* byte */
        #endif              /* _IN_OVERRIDE_HEADER_BCRYPT_H */

        #ifdef __cplusplus
extern "C"
{
        #endif

    extern NTSTATUS mock_BCryptDestroyKey(const char *, const int, const char *, BCRYPT_KEY_HANDLE);
    extern NTSTATUS mock_BCryptDestroyHash(const char *, const int, const char *, BCRYPT_HASH_HANDLE);
    extern NTSTATUS mock_BCryptCloseAlgorithmProvider(const char *, const int, const char *, BCRYPT_ALG_HANDLE, ULONG);

        #ifdef __cplusplus
}
        #endif

        #ifdef _IN_OVERRIDE_HEADER_BCRYPT_H

            #define BCryptDestroyKey(key)   mock_BCryptDestroyKey(__FILE__, __LINE__, __func__, key)
            #define BCryptDestroyHash(hash) mock_BCryptDestroyHash(__FILE__, __LINE__, __func__, hash)
            #define BCryptCloseAlgorithmProvider(algorithm, flags) \
                mock_BCryptCloseAlgorithmProvider(__FILE__, __LINE__, __func__, algorithm, flags)

        #else // _IN_OVERRIDE_HEADER_BCRYPT_H

            #include <gmock/gmock.h>

extern NTSTATUS delegate_real_BCryptDestroyKey(const char *, const int, const char *, BCRYPT_KEY_HANDLE);
extern NTSTATUS delegate_real_BCryptDestroyHash(const char *, const int, const char *, BCRYPT_HASH_HANDLE);
extern NTSTATUS delegate_real_BCryptCloseAlgorithmProvider(const char *, const int, const char *, BCRYPT_ALG_HANDLE,
                                                           ULONG);

class Mock_bcrypt
{
  public:
    MOCK_METHOD(NTSTATUS, BCryptDestroyKey, (const char *, const int, const char *, BCRYPT_KEY_HANDLE));
    MOCK_METHOD(NTSTATUS, BCryptDestroyHash, (const char *, const int, const char *, BCRYPT_HASH_HANDLE));
    MOCK_METHOD(NTSTATUS, BCryptCloseAlgorithmProvider,
                (const char *, const int, const char *, BCRYPT_ALG_HANDLE, ULONG));

    Mock_bcrypt();
    virtual ~Mock_bcrypt();
};

extern Mock_bcrypt *_mock_bcrypt;

        #endif // _IN_OVERRIDE_HEADER_BCRYPT_H

    #endif // _MOCK_BCRYPT_H

#endif // _WIN32
