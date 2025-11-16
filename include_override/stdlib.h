#ifndef _OVERRIDE_STDLIB_H_
#define _OVERRIDE_STDLIB_H_

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wpadded"
#include "/usr/include/stdlib.h"
#pragma GCC diagnostic pop
#else
/* FIXME: 何らかのかたちで、crt のパスを得る必要がある */
#include "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10\Include\10.0.26100.0\ucrt\stdlib.h"
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDLIB_H_
#include <mock_stdlib.h>
#undef _IN_OVERRIDE_HEADER_STDLIB_H_

#endif // _OVERRIDE_STDLIB_H_
