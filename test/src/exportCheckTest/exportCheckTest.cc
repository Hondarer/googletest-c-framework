#include <testfw.h>
#include <gtest/gtest-spi.h>

#include <set>
#include <string>

// 一般の想定外シンボルを除外せずに報告することの確認
TEST(exportCheckTest, reports_unexpected_symbol)
{
    // Arrange
    const std::set<std::string> expected = {"expected_api"};                 // [状態] - 期待シンボルを 1 件設定する。
    const std::set<std::string> actual = {"expected_api", "unexpected_api"}; // [状態] - 想定外シンボルを加える。

    // Pre-Assert

    // Act

    // Assert
    EXPECT_NONFATAL_FAILURE(
        testing::expectExportNamesMatch(expected, actual),
        "unexpected_api"); // [確認_異常系] - unexpected_api が想定外のエクスポートとして報告されること。
}

// Linux のリンカー合成シンボルだけを検査対象から除外することの確認
TEST(exportCheckTest, handles_linker_synthetic_symbol_by_platform)
{
    // Arrange
    const std::set<std::string> expected;
    const std::set<std::string> actual = {"__bss_start"}; // [状態] - Linux のリンカー合成名を実際値へ設定する。

    // Pre-Assert

    // Act

    // Assert
#if defined(PLATFORM_LINUX)
    testing::expectExportNamesMatch(expected,
                                    actual); // [確認_正常系] - Linux ではリンカー合成シンボルが除外されること。
#elif defined(PLATFORM_WINDOWS)
    EXPECT_NONFATAL_FAILURE(testing::expectExportNamesMatch(expected, actual),
                            "__bss_start"); // [確認_異常系] - Windows では同名シンボルが想定外として報告されること。
#endif
}
