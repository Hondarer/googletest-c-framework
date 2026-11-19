#ifdef _WIN32

    #include <mock_windows.h>
    #include <test_com.h>

using namespace testing;

int delegate_real_MultiByteToWideChar(const char *file, const int line, const char *func, UINT code_page, DWORD flags,
                                      LPCCH multi_byte_str, int multi_byte_count, LPWSTR wide_char_str,
                                      int wide_char_count)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return MultiByteToWideChar(code_page, flags, multi_byte_str, multi_byte_count, wide_char_str, wide_char_count);
}

int mock_MultiByteToWideChar(const char *file, const int line, const char *func, UINT code_page, DWORD flags,
                             LPCCH multi_byte_str, int multi_byte_count, LPWSTR wide_char_str, int wide_char_count)
{
    int mock_ret;

    if (_mock_windows != nullptr)
    {
        mock_ret = _mock_windows->MultiByteToWideChar(file, line, func, code_page, flags, multi_byte_str,
                                                      multi_byte_count, wide_char_str, wide_char_count);
    }
    else
    {
        mock_ret = delegate_real_MultiByteToWideChar(file, line, func, code_page, flags, multi_byte_str,
                                                     multi_byte_count, wide_char_str, wide_char_count);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > MultiByteToWideChar %u, 0x%lx, %d, %d", code_page, flags, multi_byte_count, wide_char_count);
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
