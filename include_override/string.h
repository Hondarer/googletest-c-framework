#ifndef _OVERRIDE_STRING_H_
#define _OVERRIDE_STRING_H_

/* 本物を include */
#ifndef _WIN32
#include "/usr/include/string.h"
#else
/* FIXME: 何らかのかたちで、crt のパスを得る必要がある */
#include "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10\Include\10.0.26100.0\ucrt\string.h"
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STRING_H_
#include <mock_string.h>
#undef _IN_OVERRIDE_HEADER_STRING_H_

#endif // _OVERRIDE_STRING_H_
