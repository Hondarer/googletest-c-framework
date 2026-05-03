#include <sharedLibrary.h>

#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>

#ifndef _WIN32
    #include <dlfcn.h>
#else
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #ifdef byte
        #undef byte
    #endif
#endif

namespace testing {

namespace {

// Meyers singleton pattern to avoid static initialization order fiasco.
// These may be accessed during shared library initialization (before global
// constructors of the executable have run), so they must be initialized on
// first use rather than as plain global objects.
std::mutex& getSharedLibraryMutex()
{
    static std::mutex mtx;
    return mtx;
}

std::map<std::string, void *>& getSharedLibraryHandles()
{
    static std::map<std::string, void *> handles;
    return handles;
}

std::map<std::string, void *>& getSharedLibrarySymbols()
{
    static std::map<std::string, void *> symbols;
    return symbols;
}

#ifdef _WIN32
static bool utf8_to_utf16(const std::string& src, std::wstring *dst)
{
    int needed;
    std::wstring out;

    if (dst == nullptr) {
        return false;
    }

    needed = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
    if (needed <= 0) {
        return false;
    }

    out.resize((size_t)needed);
    if (MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &out[0], needed) <= 0) {
        return false;
    }

    if (!out.empty() && out.back() == L'\0') {
        out.pop_back();
    }

    *dst = out;
    return true;
}

static std::string utf16_to_utf8(const wchar_t *src)
{
    int needed;
    std::string out;

    if (src == nullptr) {
        return "";
    }

    needed = WideCharToMultiByte(CP_UTF8, 0, src, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 0) {
        return "";
    }

    out.resize((size_t)needed);
    if (WideCharToMultiByte(CP_UTF8, 0, src, -1, &out[0], needed, nullptr, nullptr) <= 0) {
        return "";
    }

    if (!out.empty() && out.back() == '\0') {
        out.pop_back();
    }

    return out;
}

static std::string format_windows_error(const DWORD error_code)
{
    wchar_t *message = nullptr;
    std::string result;
    DWORD length;

    length = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                                | FORMAT_MESSAGE_IGNORE_INSERTS,
                            nullptr,
                            error_code,
                            0,
                            (LPWSTR)&message,
                            0,
                            nullptr);
    if (length == 0 || message == nullptr) {
        return "FormatMessageW failed";
    }

    result = utf16_to_utf8(message);
    LocalFree(message);

    while (!result.empty()
           && (result.back() == '\r' || result.back() == '\n' || result.back() == ' ')) {
        result.pop_back();
    }

    return result;
}
#endif

} // namespace

SharedSymbolResult tryResolveSharedSymbol(const std::string& lib_name,
                                          const std::string& symbol_name)
{
    SharedSymbolResult result;
    const std::string symbol_key = lib_name + "\n" + symbol_name;
    void *handle = nullptr;

    std::lock_guard<std::mutex> lock(getSharedLibraryMutex());

    {
        const auto cached_symbol = getSharedLibrarySymbols().find(symbol_key);
        if (cached_symbol != getSharedLibrarySymbols().end()) {
            result.symbol = cached_symbol->second;
            return result;
        }
    }

    {
        const auto cached_handle = getSharedLibraryHandles().find(lib_name);
        if (cached_handle != getSharedLibraryHandles().end()) {
            handle = cached_handle->second;
        }
    }

    if (handle == nullptr) {
#ifndef _WIN32
        dlerror();
        handle = dlopen(lib_name.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (handle == nullptr) {
            const char *error_message = dlerror();

            result.diagnostic = (error_message != nullptr) ? error_message : "dlopen failed";
            return result;
        }
#else
        std::wstring lib_name_wide;

        if (!utf8_to_utf16(lib_name, &lib_name_wide)) {
            result.diagnostic = "failed to convert library name from UTF-8 to UTF-16";
            return result;
        }

        handle = (void *)LoadLibraryW(lib_name_wide.c_str());
        if (handle == nullptr) {
            result.diagnostic = format_windows_error(GetLastError());
            return result;
        }
#endif
        getSharedLibraryHandles()[lib_name] = handle;
    }

#ifndef _WIN32
    dlerror();
    result.symbol = dlsym(handle, symbol_name.c_str());
    {
        const char *error_message = dlerror();
        if (error_message != nullptr || result.symbol == nullptr) {
            result.symbol = nullptr;
            result.diagnostic = (error_message != nullptr) ? error_message : "dlsym failed";
            return result;
        }
    }
#else
    result.symbol = (void *)GetProcAddress((HMODULE)handle, symbol_name.c_str());
    if (result.symbol == nullptr) {
        result.diagnostic = format_windows_error(GetLastError());
        return result;
    }
#endif

    getSharedLibrarySymbols()[symbol_key] = result.symbol;
    return result;
}

void *resolveSharedSymbolOrExit(const std::string& lib_name,
                                const std::string& symbol_name)
{
    SharedSymbolResult result = tryResolveSharedSymbol(lib_name, symbol_name);

    if (result.symbol != nullptr) {
        return result.symbol;
    }

    fprintf(stderr,
            "shared library resolve failed: %s!%s: %s\n",
            lib_name.c_str(),
            symbol_name.c_str(),
            result.diagnostic.c_str());
    fflush(stderr);
    exit(1);
}

} // namespace testing
