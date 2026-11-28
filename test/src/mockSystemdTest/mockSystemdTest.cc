#include <testfw.h>
#include <gtest/gtest-spi.h>
#include <mock_systemd.h>
#include "mock_systemd_target.h"

#ifndef _WIN32

using namespace testing;

// Mock_systemd の登録と多重生成検出の確認
TEST(mockSystemdTest, registers_and_rejects_multiple_instances)
{
    // Arrange
    Mock_systemd mock_systemd;

    // Pre-Assert
    EXPECT_EQ(&mock_systemd, _mock_systemd); // [確認_正常系] - _mock_systemd が生成した mock_systemd を指すこと。

    // Act

    // Assert
    EXPECT_NONFATAL_FAILURE(
        {
            Mock_systemd duplicate_mock_systemd;
            (void)duplicate_mock_systemd;
        },
        "Only one mock instance may exist for _mock_systemd at a time.");
    // [確認_異常系] - 2 個目の Mock_systemd 生成が多重生成の非致命的失敗を報告すること。
    EXPECT_EQ(&mock_systemd, _mock_systemd); // [確認_異常系] - 多重生成検出後も最初の登録が維持されること。
}

// include_override/systemd/ 経由の呼び出しが Mock_systemd へ差し替わることの確認
TEST(mockSystemdTest, intercepts_calls_via_override_headers)
{
    // Arrange
    Mock_systemd mock_systemd;
    EXPECT_CALL(mock_systemd, sd_notify(_, _, _, 0, StrEq("READY=1")))
        .WillOnce(Return(1)); // [確認_正常系] - sd_notify が mock_systemd へ委譲されること。
    EXPECT_CALL(mock_systemd, sd_bus_message_is_method_error(_, _, _, nullptr, StrEq("org.example.Err")))
        .WillOnce(Return(1)); // [確認_正常系] - sd_bus_message_is_method_error が mock_systemd へ委譲されること。

    // Pre-Assert

    // Act
    int actual_ret_notify = mock_systemd_target_notify("READY=1"); // [手順] - override 経由で sd_notify を呼び出す。
    int actual_ret_bus = mock_systemd_target_bus_is_error(
        nullptr, "org.example.Err"); // [手順] - override 経由で sd_bus 関数を呼び出す。

    // Assert
    EXPECT_EQ(1, actual_ret_notify); // [確認_正常系] - モックの戻り値 1 が返ること。
    EXPECT_EQ(1, actual_ret_bus);    // [確認_正常系] - モックの戻り値 1 が返ること。
}

// Mock_systemd の既定委譲および未注入時に libsystemd 実関数へ委譲されることの確認
TEST(mockSystemdTest, delegates_to_real_libsystemd)
{
    // Arrange

    // Pre-Assert
    EXPECT_EQ(nullptr, _mock_systemd); // [確認_正常系] - Mock_systemd 未生成時は _mock_systemd が nullptr であること。

    // Act
    int actual_ret_unmocked =
        mock_systemd_target_event_roundtrip(); // [手順] - Mock_systemd 未生成状態で sd_event_new / sd_event_unref を呼ぶ。
    int actual_ret_default = -1;
    {
        Mock_systemd mock_systemd;
        actual_ret_default =
            mock_systemd_target_event_roundtrip(); // [手順] - Mock_systemd の既定委譲 (ON_CALL) で sd_event_new / sd_event_unref を呼ぶ。
    }

    // Assert
    EXPECT_EQ(0, actual_ret_unmocked); // [確認_正常系] - 未注入時の実関数委譲が成功 (0) すること。
    EXPECT_EQ(0, actual_ret_default);  // [確認_正常系] - 既定委譲 (delegate_real_*) が成功 (0) すること。
    EXPECT_EQ(nullptr, _mock_systemd); // [確認_正常系] - Mock_systemd 破棄後に _mock_systemd が nullptr へ戻ること。
}

#endif // _WIN32
