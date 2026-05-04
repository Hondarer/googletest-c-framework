#ifndef _TESTFW_H
#define _TESTFW_H

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* _WIN32 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif /* _WIN32 */

#include <format_attr.h>
#include <test_com.h>
#include <processController.h>
#include <sharedLibrary.h>
#include <gtest_wrapmain.h>

using namespace std;
using namespace testing;

#ifndef _WIN32
#define WEAK_ATR __attribute__((weak))
#define MOCK_WEAK_IMPL(rettype, func, ...) \
    __attribute__((weak)) rettype func(__VA_ARGS__)
#else /* _WIN32 */
#define WEAK_ATR
#define MOCK_WEAK_IMPL(rettype, func, ...) \
    __pragma(comment(linker, "/ALTERNATENAME:" #func "=_mock_impl_" #func)) \
    extern "C" rettype _mock_impl_##func(__VA_ARGS__)
#endif /* _WIN32 */

#endif // _TESTFW_H
