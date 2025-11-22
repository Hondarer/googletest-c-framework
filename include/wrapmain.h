#ifndef _WRAPMAIN_H
#define _WRAPMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern int __real_main(int, char **);
    extern int __wrap_main(int, char **);

#ifdef __cplusplus
}
#endif

#endif // _WRAPMAIN_H
