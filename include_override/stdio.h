#ifndef _OVERRIDE_STDIO_H_
#define _OVERRIDE_STDIO_H_

/* 本物を include */
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "/usr/include/stdio.h"
#pragma GCC diagnostic pop
#else
/* FIXME: 何らかのかたちで、crt のパスを得る必要がある */
#include "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10\Include\10.0.26100.0\ucrt\stdio.h"
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STDIO_H_
#include <mock_stdio.h>
#undef _IN_OVERRIDE_HEADER_STDIO_H_

#endif // _OVERRIDE_STDIO_H_
