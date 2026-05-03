#ifndef TESTFW_CONSOLE_CONSOLE_INTERNAL_H
#define TESTFW_CONSOLE_CONSOLE_INTERNAL_H

namespace testing {

class ScopedConsoleUtf8 {
public:
    ScopedConsoleUtf8();
    ~ScopedConsoleUtf8();

    ScopedConsoleUtf8(const ScopedConsoleUtf8&) = delete;
    ScopedConsoleUtf8& operator=(const ScopedConsoleUtf8&) = delete;
};

} // namespace testing

#endif // TESTFW_CONSOLE_CONSOLE_INTERNAL_H
