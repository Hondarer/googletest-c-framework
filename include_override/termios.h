#ifndef _WIN32

    #ifndef _OVERRIDE_TERMIOS_H
        #define _OVERRIDE_TERMIOS_H

        /* 本物を include */
        #include_next <termios.h>

        /* モックにすげ替え */
        #define _IN_OVERRIDE_HEADER_TERMIOS_H
        #include <mock_termios.h>
        #undef _IN_OVERRIDE_HEADER_TERMIOS_H

    #endif // _OVERRIDE_TERMIOS_H

#endif // _WIN32
