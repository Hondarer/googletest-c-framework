#ifndef _SHARED_LIBRARY_H
#define _SHARED_LIBRARY_H

#include <string>

#if defined(__linux__)
    #define TESTFW_SHARED_LIBRARY_EXTENSION ".so"
#elif defined(_WIN32)
    #define TESTFW_SHARED_LIBRARY_EXTENSION ".dll"
#else
    #error "unsupported platform"
#endif

namespace testing {

struct SharedSymbolResult {
    void *symbol = nullptr;
    std::string diagnostic;
};

extern SharedSymbolResult tryResolveSharedSymbol(const std::string& lib_name,
                                                 const std::string& symbol_name);

extern void *resolveSharedSymbolOrExit(const std::string& lib_name,
                                       const std::string& symbol_name);

} // namespace testing

#endif // _SHARED_LIBRARY_H
