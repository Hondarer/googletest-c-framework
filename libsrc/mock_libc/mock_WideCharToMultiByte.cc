#include <mock_windows.h>
#ifdef _WIN32

    #include <test_com.h>

using namespace testing;

int delegate_real_WideCharToMultiByte(const char *file, const int line, const char *func, UINT code_page, DWORD flags,
                                     LPCWCH wide_char_str, int wide_char_count, LPSTR multi_byte_str,
                                     int multi_byte_count, LPCCH default_char, LPBOOL used_default_char)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return WideCharToMultiByte(code_page, flags, wide_char_str, wide_char_count, multi_byte_str, multi_byte_count,
                               default_char, used_default_char);
}

int mock_WideCharToMultiByte(const char *file, const int line, const char *func, UINT code_page, DWORD flags,
                            LPCWCH wide_char_str, int wide_char_count, LPSTR multi_byte_str, int multi_byte_count,
                            LPCCH default_char, LPBOOL used_default_char)
{
    int mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->WideCharToMultiByte(file, line, func, code_page, flags, wide_char_str,
                                                      wide_char_count, multi_byte_str, multi_byte_count, default_char,
                                                      used_default_char);
    }
    else
    {
        mock_ret = delegate_real_WideCharToMultiByte(file, line, func, code_page, flags, wide_char_str,
                                                     wide_char_count, multi_byte_str, multi_byte_count, default_char,
                                                     used_default_char);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > WideCharToMultiByte %u, 0x%lx, %d, %d", code_page, flags, wide_char_count, multi_byte_count);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d -> %d\n", file, line, mock_ret);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}

#endif // _WIN32
