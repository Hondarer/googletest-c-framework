#ifndef _GTEST_WRAPMAIN_H_
#define _GTEST_WRAPMAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

    extern "C" int __wrap_main(int, char **);
    extern "C" int __real_main(int, char **);

#ifdef __cplusplus
}
#endif

#endif // _GTEST_WRAPMAIN_H_
