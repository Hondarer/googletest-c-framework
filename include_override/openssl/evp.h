#ifndef _OVERRIDE_OPENSSL_EVP_H
#define _OVERRIDE_OPENSSL_EVP_H

/* 本物を include */
#include_next <openssl/evp.h>

/* モックにすげ替え */
#define _IN_OVERRIDE_HEADER_OPENSSL_H
#include <mock_openssl.h>
#undef _IN_OVERRIDE_HEADER_OPENSSL_H

#endif // _OVERRIDE_OPENSSL_EVP_H
