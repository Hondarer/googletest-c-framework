#include <test_com.h>
#include <mock_stdlib.h>

#include <string>

using namespace testing;

char *delegate_real_getenv(const char *file, const int line, const char *func, const char *name)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

#ifndef _WIN32
    return getenv(name);
#else
    static thread_local std::string env_value;
    char *raw_value = nullptr;
    size_t val_len = 0;

    if (_dupenv_s(&raw_value, &val_len, name) != 0 || raw_value == nullptr)
    {
        return nullptr;
    }

    env_value.assign(raw_value, val_len > 0 ? val_len - 1 : 0);
    free(raw_value);
    return const_cast<char *>(env_value.c_str());
#endif
}

char *mock_getenv(const char *file, const int line, const char *func, const char *name)
{
    char *rtc;

    if (_mock_stdlib != nullptr)
    {
        rtc = _mock_stdlib->getenv(file, line, func, name);
    }
    else
    {
        rtc = delegate_real_getenv(file, line, func, name);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > getenv %s", name);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %s\n", file, line, rtc != nullptr ? rtc : "(null)");
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
