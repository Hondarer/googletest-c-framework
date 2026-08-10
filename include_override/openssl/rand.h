#ifndef _OVERRIDE_OPENSSL_RAND_H
#define _OVERRIDE_OPENSSL_RAND_H

/* 本物を include */
#include_next <openssl/rand.h>

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_OPENSSL_H
#include <mock_openssl.h>
#undef _IN_OVERRIDE_HEADER_OPENSSL_H

#endif // _OVERRIDE_OPENSSL_RAND_H
