#ifndef _OVERRIDE_LIBSSH_LIBSSH_H
#define _OVERRIDE_LIBSSH_LIBSSH_H

/* 本物を include (libssh がインストールされている場合) */
#ifdef USE_REAL_LIBSSH
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include_next <libssh/libssh.h>
#pragma GCC diagnostic pop
#else // _WIN32
/* Windows の場合のパス調整が必要な場合はここに追加 */
#include_next <libssh/libssh.h>
#endif // _WIN32
#endif // USE_REAL_LIBSSH

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_LIBSSH_H
#include <mock_libssh.h>
#undef _IN_OVERRIDE_HEADER_LIBSSH_H

#endif // _OVERRIDE_LIBSSH_LIBSSH_H
