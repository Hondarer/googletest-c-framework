#ifndef _MOCK_TERMIOS_H
#define _MOCK_TERMIOS_H

#ifndef _WIN32

    #include <termios.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_tcgetattr(const char *, const int, const char *, int, struct termios *);
    extern int mock_tcsetattr(const char *, const int, const char *, int, int, const struct termios *);

    #ifdef __cplusplus
}
    #endif

    #ifdef _IN_OVERRIDE_HEADER_TERMIOS_H

        #define tcgetattr(fd, termios_p) mock_tcgetattr(__FILE__, __LINE__, __func__, fd, termios_p)
        #define tcsetattr(fd, optional_actions, termios_p) \
            mock_tcsetattr(__FILE__, __LINE__, __func__, fd, optional_actions, termios_p)

    #else // _IN_OVERRIDE_HEADER_TERMIOS_H

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpadded"
        #include <gmock/gmock.h>
        #pragma GCC diagnostic pop

extern int delegate_real_tcgetattr(const char *, const int, const char *, int, struct termios *);
extern int delegate_real_tcsetattr(const char *, const int, const char *, int, int, const struct termios *);

class Mock_termios
{
  public:
    MOCK_METHOD(int, tcgetattr, (const char *, const int, const char *, int, struct termios *));
    MOCK_METHOD(int, tcsetattr, (const char *, const int, const char *, int, int, const struct termios *));

    Mock_termios();
    ~Mock_termios();
};

extern Mock_termios *_mock_termios;

    #endif // _IN_OVERRIDE_HEADER_TERMIOS_H

#endif // _WIN32

#endif // _MOCK_TERMIOS_H
