#ifndef _WIN32

    #ifndef _OVERRIDE_SYS_RESOURCE_H
        #define _OVERRIDE_SYS_RESOURCE_H

        /* libc のヘッダーを include */
        #include_next <sys/resource.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_SYS_RESOURCE_H
        #include <sys/mock_resource.h>
        #undef _IN_OVERRIDE_HEADER_SYS_RESOURCE_H

    #endif // _OVERRIDE_SYS_RESOURCE_H

#endif // _WIN32
