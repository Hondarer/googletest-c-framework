#ifdef _WIN32

    #ifndef _OVERRIDE_BCRYPT_H
        #define _OVERRIDE_BCRYPT_H

        /* Windows SDK のヘッダーを include */
        #include <../shared/bcrypt.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_BCRYPT_H
        #include <mock_bcrypt.h>
        #undef _IN_OVERRIDE_HEADER_BCRYPT_H

    #endif // _OVERRIDE_BCRYPT_H

#endif // _WIN32
