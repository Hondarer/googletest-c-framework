#ifndef _TESTFW_H
#define _TESTFW_H

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <format_attr.h>
#include <test_com.h>
#include <gtest_wrapmain.h>

using namespace std;
using namespace testing;

#ifndef _WIN32
#define WEAK_ATR __attribute__((weak))
#else // _WIN32
#define WEAK_ATR
#endif // _WIN32

#endif // _TESTFW_H
