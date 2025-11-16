#ifndef _OVERRIDE_SYS_STAT_H_
#define _OVERRIDE_SYS_STAT_H_

/* 本物を include */
#ifndef _WIN32
#include "/usr/include/sys/stat.h"
#else
/* FIXME: 何らかのかたちで、crt のパスを得る必要がある */
#include "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10\Include\10.0.26100.0\ucrt\sys\stat.h"
#endif // _WIN32

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_STAT_H_
#include <sys/mock_stat.h>
#undef _IN_OVERRIDE_HEADER_STAT_H_

#endif // _OVERRIDE_SYS_STAT_H_
