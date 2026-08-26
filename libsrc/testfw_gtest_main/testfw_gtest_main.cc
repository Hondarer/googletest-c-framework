#include <gtest/gtest.h>
#include <testfw/console/console_internal.h>

using namespace testing;

// main() を持たないテスト プログラムのエントリ ポイント
int main(int argc, char **argv)
{
    ScopedConsoleUtf8 scoped_console_utf8;
    printf("Running main() from %s\n", __FILE__);
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
