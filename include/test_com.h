#ifndef _TEST_COM_H_
#define _TEST_COM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <attr.h>

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

using namespace std;

namespace testing
{
    extern AssertionResult FileExists(const string &);
    extern AssertionResult FileNotExists(const string &);
    extern AssertionResult FileContains(const string &, const string &);

    constexpr int TRACE_NONE = 0;
    constexpr int TRACE_INFO = 1;
    constexpr int TRACE_DETAIL = 2;

    extern char *allocprintf(PRINTF_FMT const char *, ...) PRINTF_ATTR(1, 2);
    extern char *allocvprintf(PRINTF_FMT const char *, va_list) PRINTF_ATTR(1, 0);

    extern void resetTraceLevel(int = TRACE_NONE);
    extern int _getTraceLevel(const char *);
    extern void setDefaultTraceLevel(int);
    extern void setTraceLevel(const char *, int);
}

#define EXPECT_FILE_NOT_EXISTS(file_path) \
    EXPECT_TRUE(FileNotExists(file_path))

#define EXPECT_FILE_EXISTS(file_path) \
    EXPECT_TRUE(FileExists(file_path))

#define EXPECT_FILE_CONTAINS(file_path, expected_content) \
    EXPECT_TRUE(FileContains(file_path, expected_content))

#define getTraceLevel() _getTraceLevel(__func__)

#endif // _TEST_COM_H_
