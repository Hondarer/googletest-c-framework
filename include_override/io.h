#ifdef _WIN32

    #ifndef _OVERRIDE_IO_H
        #define _OVERRIDE_IO_H

        /* 本物を include */
        #pragma push_macro("_INC_IO")
        #undef _INC_IO
        #include <../ucrt/io.h>
        #pragma pop_macro("_INC_IO")

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_IO_H
        #include <mock_unistd.h>
        #undef _IN_OVERRIDE_HEADER_IO_H

    #endif // _OVERRIDE_IO_H

#endif // _WIN32
