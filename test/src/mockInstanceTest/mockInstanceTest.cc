#include <testfw.h>
#include <gtest/gtest-spi.h>

namespace
{

class Mock_testfw;
Mock_testfw *s_mock_testfw = nullptr;

class Mock_testfw
{
  public:
    Mock_testfw()
    {
        TESTFW_REGISTER_MOCK_INSTANCE(s_mock_testfw);
    }

    ~Mock_testfw()
    {
        TESTFW_UNREGISTER_MOCK_INSTANCE(s_mock_testfw);
    }
};

} // namespace

// 同じクラスの mock を同時に生成した場合にテストが失敗し、最初の注入が維持されることの確認
TEST(mockInstanceTest, rejects_multiple_instances)
{
    // Arrange
    Mock_testfw mock_testfw;

    // Pre-Assert

    // Act

    // Assert
    EXPECT_NONFATAL_FAILURE(
        {
            Mock_testfw duplicate_mock_testfw;
            (void)duplicate_mock_testfw;
        },
        "Only one mock instance may exist for s_mock_testfw at a time.");
    // [確認_異常系] - 2 個目の Mock_testfw の生成が "Only one mock instance may exist for s_mock_testfw at a time." を含む非致命的な失敗を報告すること。

    EXPECT_EQ(
        &mock_testfw,
        s_mock_testfw); // [確認_異常系] - s_mock_testfw が &mock_testfw と一致すること。(多重生成を検出した後も最初の注入が維持されることの確認)
}

// 先に生成した mock の破棄後は同じクラスの mock を再生成できることの確認
TEST(mockInstanceTest, allows_sequential_instances)
{
    // Arrange
    bool first_instance_registered = false;

    // Pre-Assert

    // Act
    {
        Mock_testfw first_mock_testfw;
        first_instance_registered = s_mock_testfw == &first_mock_testfw; // [手順] - 1 個目の Mock_testfw を生成する。
    }
    Mock_testfw second_mock_testfw; // [手順] - 1 個目の破棄後に 2 個目の Mock_testfw を生成する。

    // Assert
    EXPECT_TRUE(
        first_instance_registered); // [確認_正常系] - first_instance_registered が true であること。(1 個目の Mock_testfw が生成中に注入されていたことの確認)
    EXPECT_EQ(&second_mock_testfw,
              s_mock_testfw); // [確認_正常系] - s_mock_testfw が &second_mock_testfw と一致すること。
}
