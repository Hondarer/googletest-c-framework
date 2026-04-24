#ifdef _WIN32

#ifndef _OVERRIDE_WINDOWS_H
#define _OVERRIDE_WINDOWS_H

/* 本物を include */
#pragma push_macro("_WINDOWS_")
#undef _WINDOWS_
#include <../um/windows.h>
#pragma pop_macro("_WINDOWS_")

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_WINDOWS_H
#include <mock_windows.h>
#undef _IN_OVERRIDE_HEADER_WINDOWS_H

#endif // _OVERRIDE_WINDOWS_H

#endif // _WIN32
