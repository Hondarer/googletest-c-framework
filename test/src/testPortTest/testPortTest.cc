#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif /* _WIN32 */

#include <testfw.h>
#include <testfw/console/console_internal.h>
#include "testPort_impl.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{

/** 自分自身を子プロセスとして起動するときの第 1 引数。 */
const char CHILD_OPTION[] = "--test-port-child";
/** 子プロセスと並行して払い出す件数 (親と子のそれぞれ)。 */
const size_t PARALLEL_COUNT = 200U;

using testing::test_port_internal::REGISTRY_CAPACITY;
using testing::test_port_internal::REGISTRY_SIZE;
using testing::test_port_internal::REGISTRY_VERSION;

int run_child(const char *count_text);
std::string executable_path();
std::string create_temp_dir();
void remove_temp_dir(const std::string &dir);
void set_temp_dir_env(const std::string &dir);
void restore_temp_dir_env();
std::string test_registry_path();
std::vector<uint8_t> read_registry();
void write_registry(const std::vector<uint8_t> &bytes);
std::vector<uint8_t> make_registry(uint32_t version, uint32_t next);
uint32_t registry_u32(const std::vector<uint8_t> &bytes, size_t offset);
uint16_t registry_port(const std::vector<uint8_t> &bytes, size_t index);
size_t count_outside_range(const std::vector<uint16_t> &ports);
bool bind_wildcard(int type, uint16_t port, intptr_t &sock);
void close_socket(intptr_t sock);

std::vector<std::string> s_saved_env_names;
std::vector<std::string> s_saved_env_values;
std::vector<bool> s_saved_env_exists;

} // namespace

int main(int argc, char **argv)
{
    if (argc > 2 && strcmp(argv[1], CHILD_OPTION) == 0)
    {
        return run_child(argv[2]);
    }

    ScopedConsoleUtf8 scoped_console_utf8;

    printf("Running main() from %s\n", __FILE__);
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class testPortTest : public Test
{
  protected:
    std::string temp_dir_;

    // [サブ手順 名前=testPortTest.SetUp]
    void SetUp() override
    {
        // ほかのテストの払い出しの記録と混ざらないよう、一時ディレクトリを切り替える。
        temp_dir_ = create_temp_dir();   // [状態] - テストごとの一時ディレクトリを作成する。
        ASSERT_FALSE(temp_dir_.empty()); // [状態確認] - 一時ディレクトリを作成できたこと。
        set_temp_dir_env(
            temp_dir_); // [状態] - 一時ディレクトリの環境変数 (Linux は TMPDIR、Windows は TMP と TEMP) をテストごとの一時ディレクトリにする。
    }
    // [サブ手順終了]

    // [サブ手順 名前=testPortTest.TearDown]
    void TearDown() override
    {
        // 記録のファイルのパスは環境変数から決まるため、戻す前に消す。
        if (!temp_dir_.empty())
        {
            remove_temp_dir(temp_dir_);
        }
        restore_temp_dir_env();
    }
    // [サブ手順終了]
};

// 複数の払い出しが、範囲内で OS の一時的なポートの範囲の外にあり、互いに異なることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, allocates_distinct_ports_outside_ephemeral_range)
{
    // Arrange
    std::vector<uint16_t> ports;

    // Pre-Assert

    // Act
    bool actual_ret = allocateTestPorts(64U, ports); // [手順] - allocateTestPorts() で 64 個を払い出す。

    // Assert
    EXPECT_TRUE(actual_ret);      // [確認_正常系] - allocateTestPorts() の戻り値が true であること。
    EXPECT_EQ(64U, ports.size()); // [確認_正常系] - ports の要素数が 64 であること。
    EXPECT_EQ(
        0U,
        count_outside_range(
            ports)); // [確認_正常系] - 20000 から 32767 の外、または OS の一時的なポートの範囲の中にある番号が 0 個であること。
    EXPECT_EQ(64U, std::set<uint16_t>(ports.begin(), ports.end())
                       .size()); // [確認_正常系] - ports の重複を除いた個数が 64 であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 払い出した番号が、正しい形式の記録のファイルに記録されることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, records_allocated_port_in_registry)
{
    // Arrange

    // Pre-Assert

    // Act
    uint16_t actual_port = allocateTestPort(); // [手順] - allocateTestPort() で 1 個を払い出す。

    // Assert
    EXPECT_NE(0U, actual_port); // [確認_正常系] - allocateTestPort() の戻り値が 0 以外であること。
    std::vector<uint8_t> registry = read_registry();
    ASSERT_EQ(REGISTRY_SIZE, registry.size()); // [確認_正常系] - 記録のファイルの大きさが REGISTRY_SIZE であること。
    EXPECT_EQ(REGISTRY_VERSION, registry_u32(registry, 0U)); // [確認_正常系] - 記録の版が REGISTRY_VERSION であること。
    EXPECT_EQ(1U, registry_u32(registry, 4U));               // [確認_正常系] - 記録の次の書き込み位置が 1 であること。
    EXPECT_EQ(actual_port, registry_port(registry, 0U));     // [確認_正常系] - 記録の先頭が払い出した番号であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 直近の 1024 件の間は、同じ番号を再び払い出さないことの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, does_not_reallocate_recent_1024_ports)
{
    // Arrange
    std::vector<uint16_t> ports;

    // Pre-Assert

    // Act
    for (size_t index = 0U; index <= REGISTRY_CAPACITY; index++)
    {
        ports.push_back(allocateTestPort()); // [手順] - allocateTestPort() を 1 件ずつ 1025 回呼び出す。
    }

    // Assert
    std::set<uint16_t> recent(ports.begin(), ports.begin() + static_cast<std::ptrdiff_t>(REGISTRY_CAPACITY));
    EXPECT_EQ(0U, recent.count(0U));             // [確認_正常系] - 最初の 1024 回に失敗 (0) がないこと。
    EXPECT_EQ(REGISTRY_CAPACITY, recent.size()); // [確認_正常系] - 最初の 1024 回の番号が互いに異なること。
    EXPECT_NE(0U, ports.back());                 // [確認_正常系] - 1025 回目の払い出しが成功すること。
    EXPECT_EQ(0U,
              recent.count(ports.back())); // [確認_正常系] - 1025 回目の番号が、直近の 1024 件のどれとも異なること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 記録にある候補を除き、候補が尽きると失敗することの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, allocate_from_skips_recorded_ports)
{
    // Arrange
    std::vector<uint16_t> candidates;
    ASSERT_TRUE(allocateTestPorts(2U, candidates)); // [状態] - 空いている番号を 2 個用意する。
    // [状態確認] - allocateTestPorts() の戻り値が true であること。
    write_registry(make_registry(REGISTRY_VERSION, 0U)); // [状態] - 記録を空にする。
    std::vector<uint16_t> first;
    std::vector<uint16_t> second;
    std::vector<uint16_t> third = {1U};

    // Pre-Assert

    // Act
    bool actual_first = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, first); // [手順] - 2 個の候補から allocate_from() で 1 個を払い出す。
    bool actual_second = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, second); // [手順] - 同じ候補から、もう一度 1 個を払い出す。
    bool actual_third = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, third); // [手順] - 同じ候補から、3 回目の払い出しを行う。

    // Assert
    EXPECT_TRUE(actual_first);    // [確認_正常系] - 1 回目の allocate_from() の戻り値が true であること。
    EXPECT_TRUE(actual_second);   // [確認_正常系] - 2 回目の allocate_from() の戻り値が true であること。
    ASSERT_EQ(1U, first.size());  // [状態確認] - 1 回目の払い出しが 1 個であること。
    ASSERT_EQ(1U, second.size()); // [状態確認] - 2 回目の払い出しが 1 個であること。
    EXPECT_EQ((std::set<uint16_t>{candidates[0], candidates[1]}),
              (std::set<uint16_t>{
                  first[0], second[0]})); // [確認_正常系] - 1 回目と 2 回目で、2 個の候補を 1 個ずつ払い出すこと。
    EXPECT_FALSE(
        actual_third); // [確認_異常系] - 候補がすべて記録にある 3 回目の allocate_from() の戻り値が false であること。
    EXPECT_TRUE(third.empty()); // [確認_異常系] - 失敗した払い出しの格納先が空であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// TCP または UDP で使用中の番号を避けることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, skips_ports_in_use)
{
    // Arrange
    uint16_t port = allocateTestPort(); // [状態] - 空いている番号を 1 個用意する。
    ASSERT_NE(0U, port);                // [状態確認] - allocateTestPort() の戻り値が 0 以外であること。
    std::vector<uint16_t> candidates = {port};
    std::vector<uint16_t> tcp_ports;
    std::vector<uint16_t> udp_ports;
    std::vector<uint16_t> free_ports;
    intptr_t sock = -1;

    // Pre-Assert

    // Act
    write_registry(make_registry(REGISTRY_VERSION, 0U)); // [手順] - 記録を空にする。
    ASSERT_TRUE(bind_wildcard(SOCK_STREAM, port, sock)); // [手順] - TCP のソケットで 0.0.0.0 の番号を使用中にする。
    // [状態確認] - TCP の bind_wildcard() の戻り値が true であること。
    bool actual_tcp = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, tcp_ports); // [手順] - TCP で使用中の番号だけを候補として allocate_from() を呼び出す。
    close_socket(sock);                     // [手順] - TCP のソケットを閉じる。
    ASSERT_TRUE(bind_wildcard(SOCK_DGRAM, port, sock)); // [手順] - UDP のソケットで 0.0.0.0 の番号を使用中にする。
    // [状態確認] - UDP の bind_wildcard() の戻り値が true であること。
    bool actual_udp = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, udp_ports); // [手順] - UDP で使用中の番号だけを候補として allocate_from() を呼び出す。
    close_socket(sock);                     // [手順] - UDP のソケットを閉じる。
    bool actual_free = testing::test_port_internal::allocate_from(
        "test", candidates, 1U, free_ports); // [手順] - ソケットを閉じた後に、同じ候補で allocate_from() を呼び出す。

    // Assert
    EXPECT_FALSE(
        actual_tcp); // [確認_異常系] - TCP で使用中の番号に対する allocate_from() の戻り値が false であること。
    EXPECT_FALSE(
        actual_udp); // [確認_異常系] - UDP で使用中の番号に対する allocate_from() の戻り値が false であること。
    EXPECT_TRUE(
        actual_free); // [確認_正常系] - 使用中でなくなった番号に対する allocate_from() の戻り値が true であること。
    EXPECT_EQ(std::vector<uint16_t>{port}, free_ports); // [確認_正常系] - 使用中でなくなった番号を払い出すこと。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 大きさの違う記録のファイルを初期化して払い出しを続けることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, reinitializes_registry_with_wrong_size)
{
    // Arrange
    write_registry(std::vector<uint8_t>(10U, 0xFFU)); // [状態] - 記録のファイルを 10 バイトの不正な内容にする。

    // Pre-Assert

    // Act
    uint16_t actual_port = allocateTestPort(); // [手順] - allocateTestPort() で 1 個を払い出す。

    // Assert
    EXPECT_NE(0U, actual_port); // [確認_正常系] - allocateTestPort() の戻り値が 0 以外であること。
    std::vector<uint8_t> registry = read_registry();
    ASSERT_EQ(REGISTRY_SIZE, registry.size()); // [確認_正常系] - 記録のファイルの大きさが REGISTRY_SIZE に戻ること。
    EXPECT_EQ(REGISTRY_VERSION, registry_u32(registry, 0U)); // [確認_正常系] - 記録の版が REGISTRY_VERSION であること。
    EXPECT_EQ(1U, registry_u32(registry, 4U));               // [確認_正常系] - 記録の次の書き込み位置が 1 であること。
    EXPECT_EQ(actual_port, registry_port(registry, 0U));     // [確認_正常系] - 記録の先頭が払い出した番号であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 版の違う記録のファイルを初期化して払い出しを続けることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, reinitializes_registry_with_wrong_version)
{
    // Arrange
    write_registry(make_registry(REGISTRY_VERSION + 1U,
                                 5U)); // [状態] - 記録の版を REGISTRY_VERSION + 1、次の書き込み位置を 5 にする。

    // Pre-Assert

    // Act
    uint16_t actual_port = allocateTestPort(); // [手順] - allocateTestPort() で 1 個を払い出す。

    // Assert
    EXPECT_NE(0U, actual_port); // [確認_正常系] - allocateTestPort() の戻り値が 0 以外であること。
    std::vector<uint8_t> registry = read_registry();
    ASSERT_EQ(REGISTRY_SIZE, registry.size());               // [状態確認] - 記録のファイルの内容を読めること。
    EXPECT_EQ(REGISTRY_VERSION, registry_u32(registry, 0U)); // [確認_正常系] - 記録の版が REGISTRY_VERSION に戻ること。
    EXPECT_EQ(1U, registry_u32(
                      registry,
                      4U)); // [確認_正常系] - 記録の次の書き込み位置が 1 であること。(初期化してから記録したことの確認)
    EXPECT_EQ(actual_port, registry_port(registry, 0U)); // [確認_正常系] - 記録の先頭が払い出した番号であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 次の書き込み位置が範囲外の記録のファイルを初期化して払い出しを続けることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, reinitializes_registry_with_invalid_next)
{
    // Arrange
    write_registry(make_registry(
        REGISTRY_VERSION,
        static_cast<uint32_t>(REGISTRY_CAPACITY))); // [状態] - 記録の次の書き込み位置を 1024 (範囲外) にする。

    // Pre-Assert

    // Act
    uint16_t actual_port = allocateTestPort(); // [手順] - allocateTestPort() で 1 個を払い出す。

    // Assert
    EXPECT_NE(0U, actual_port); // [確認_正常系] - allocateTestPort() の戻り値が 0 以外であること。
    std::vector<uint8_t> registry = read_registry();
    ASSERT_EQ(REGISTRY_SIZE, registry.size()); // [状態確認] - 記録のファイルの内容を読めること。
    EXPECT_EQ(1U, registry_u32(
                      registry,
                      4U)); // [確認_正常系] - 記録の次の書き込み位置が 1 であること。(初期化してから記録したことの確認)
    EXPECT_EQ(actual_port, registry_port(registry, 0U)); // [確認_正常系] - 記録の先頭が払い出した番号であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 別のプロセスと並行して払い出しても、番号が重ならないことの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, does_not_overlap_with_parallel_process)
{
    // Arrange
    ProcessOptions opts;
#if defined(_WIN32)
    opts.capture_debug_output = false;
    opts.env_set["TMP"] = temp_dir_;
    opts.env_set["TEMP"] = temp_dir_;
#else
    opts.env_set["TMPDIR"] = temp_dir_;
#endif /* _WIN32 */
    AsyncProcessHandle child =
        startProcessAsync(executable_path(), {CHILD_OPTION, std::to_string(PARALLEL_COUNT)},
                          opts); // [状態] - 自分自身を、200 個を払い出す子プロセスとして起動する。
    ASSERT_NE(nullptr, child);   // [状態確認] - startProcessAsync() の戻り値が nullptr 以外であること。
    ASSERT_TRUE(tryWaitForOutput(child, "ready", 10000)); // [状態] - 子プロセスが開始の合図を待つまで待機する。
    // [状態確認] - tryWaitForOutput() の戻り値が true であること。
    std::vector<uint16_t> ports;

    // Pre-Assert

    // Act
    bool actual_go = writeLineStdin(child, "go"); // [手順] - 子プロセスへ開始の合図を送る。
    for (size_t index = 0U; index < PARALLEL_COUNT; index++)
    {
        ports.push_back(allocateTestPort()); // [手順] - 子プロセスと並行して、allocateTestPort() を 200 回呼び出す。
    }
    int actual_exit = waitForExit(child, 30000); // [手順] - 子プロセスの終了を待つ。

    // Assert
    EXPECT_TRUE(actual_go); // [確認_正常系] - writeLineStdin() の戻り値が true であること。
    EXPECT_EQ(
        0,
        actual_exit); // [確認_正常系] - 子プロセスの終了コードが 0 であること。(子プロセスの 200 回の払い出しが成功したことの確認)
    std::istringstream child_output(getStdout(child));
    std::string line;
    while (std::getline(child_output, line))
    {
        if (line.rfind("port=", 0) == 0)
        {
            ports.push_back(static_cast<uint16_t>(std::stoul(line.substr(5))));
        }
    }
    std::set<uint16_t> distinct(ports.begin(), ports.end());
    EXPECT_EQ(PARALLEL_COUNT * 2U, ports.size()); // [確認_正常系] - 親と子の払い出しの合計が 400 個であること。
    EXPECT_EQ(0U, distinct.count(0U));            // [確認_正常系] - 親の払い出しに失敗 (0) がないこと。
    EXPECT_EQ(ports.size(), distinct.size());     // [確認_正常系] - 親と子の番号が互いに重ならないこと。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 1024 個を超える払い出しと 0 個の払い出しの扱いの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, rejects_count_over_capacity)
{
    // Arrange
    std::vector<uint16_t> over = {1U};
    std::vector<uint16_t> none = {1U};

    // Pre-Assert

    // Act
    bool actual_over =
        allocateTestPorts(REGISTRY_CAPACITY + 1U, over); // [手順] - allocateTestPorts() で 1025 個を求める。
    bool actual_none = allocateTestPorts(0U, none);      // [手順] - allocateTestPorts() で 0 個を求める。

    // Assert
    EXPECT_FALSE(actual_over); // [確認_異常系] - 1025 個を求めた allocateTestPorts() の戻り値が false であること。
    EXPECT_TRUE(over.empty()); // [確認_異常系] - 失敗した払い出しの格納先が空であること。
    EXPECT_TRUE(actual_none);  // [確認_正常系] - 0 個を求めた allocateTestPorts() の戻り値が true であること。
    EXPECT_TRUE(none.empty()); // [確認_正常系] - 0 個の払い出しの格納先が空であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

// 候補の範囲から、OS の一時的なポートの範囲に重なる番号を除くことの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, removes_ephemeral_overlap)
{
    // Arrange
    std::vector<uint16_t> candidates_without_overlap;
    std::vector<uint16_t> candidates_upper_overlap;
    std::vector<uint16_t> candidates_inner_overlap;
    std::vector<uint16_t> candidates_full_overlap;
    std::vector<uint16_t> ports = {1U};

    // Pre-Assert

    // Act
    candidates_without_overlap = testing::test_port_internal::build_candidates(
        {32768U, 60999U}); // [手順] - 一時的なポートの範囲を 32768 から 60999 として build_candidates() を呼び出す。
    candidates_upper_overlap = testing::test_port_internal::build_candidates(
        {30000U, 40000U}); // [手順] - 一時的なポートの範囲を 30000 から 40000 として build_candidates() を呼び出す。
    candidates_inner_overlap = testing::test_port_internal::build_candidates(
        {25000U, 25999U}); // [手順] - 一時的なポートの範囲を 25000 から 25999 として build_candidates() を呼び出す。
    candidates_full_overlap = testing::test_port_internal::build_candidates(
        {1024U, 65535U}); // [手順] - 一時的なポートの範囲を 1024 から 65535 として build_candidates() を呼び出す。
    bool actual_empty = testing::test_port_internal::allocate_from(
        "test", candidates_full_overlap, 1U, ports); // [手順] - 空の候補で allocate_from() を呼び出す。

    // Assert
    EXPECT_EQ(12768U, candidates_without_overlap.size()); // [確認_正常系] - 重ならない場合の候補が 12768 個であること。
    EXPECT_EQ(10000U,
              candidates_upper_overlap.size()); // [確認_正常系] - 30000 以上が重なる場合の候補が 10000 個であること。
    EXPECT_EQ(
        29999U,
        candidates_upper_overlap.back()); // [確認_正常系] - 30000 以上が重なる場合の候補の末尾が 29999 であること。
    EXPECT_EQ(11768U,
              candidates_inner_overlap.size()); // [確認_正常系] - 範囲の途中が重なる場合の候補が 11768 個であること。
    EXPECT_TRUE(std::find(candidates_inner_overlap.begin(), candidates_inner_overlap.end(), 25500U) ==
                candidates_inner_overlap.end());  // [確認_正常系] - 範囲の途中が重なる場合の候補に 25500 がないこと。
    EXPECT_TRUE(candidates_full_overlap.empty()); // [確認_異常系] - すべて重なる場合の候補が空であること。
    EXPECT_FALSE(actual_empty); // [確認_異常系] - 空の候補に対する allocate_from() の戻り値が false であること。
    EXPECT_TRUE(ports.empty()); // [確認_異常系] - 失敗した払い出しの格納先が空であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]

#if !defined(_WIN32)
// ip_local_port_range の形式のファイルを読み、読めない場合は既定の範囲とすることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, reads_ephemeral_range_file)
{
    // Arrange
    std::string valid_path = temp_dir_ + "/valid_range";
    std::string invalid_path = temp_dir_ + "/invalid_range";
    std::string missing_path = temp_dir_ + "/missing_range";
    std::ofstream(valid_path) << "25000\t40000\n"; // [状態] - "25000\t40000\n" を書いたファイルを用意する。
    std::ofstream(invalid_path) << "abc\n";        // [状態] - "abc\n" を書いたファイルを用意する。

    // Pre-Assert

    // Act
    testing::test_port_internal::port_range actual_valid = testing::test_port_internal::read_ephemeral_range(
        valid_path.c_str()); // [手順] - 正しい形式のファイルで read_ephemeral_range() を呼び出す。
    testing::test_port_internal::port_range actual_invalid = testing::test_port_internal::read_ephemeral_range(
        invalid_path.c_str()); // [手順] - 不正な形式のファイルで read_ephemeral_range() を呼び出す。
    testing::test_port_internal::port_range actual_missing = testing::test_port_internal::read_ephemeral_range(
        missing_path.c_str()); // [手順] - 存在しないファイルで read_ephemeral_range() を呼び出す。

    // Assert
    EXPECT_EQ(25000U, actual_valid.first); // [確認_正常系] - 正しい形式のファイルの範囲の先頭が 25000 であること。
    EXPECT_EQ(40000U, actual_valid.last);  // [確認_正常系] - 正しい形式のファイルの範囲の末尾が 40000 であること。
    EXPECT_EQ(32768U,
              actual_invalid.first); // [確認_異常系] - 不正な形式のファイルの範囲の先頭が既定の 32768 であること。
    EXPECT_EQ(60999U,
              actual_invalid.last); // [確認_異常系] - 不正な形式のファイルの範囲の末尾が既定の 60999 であること。
    EXPECT_EQ(32768U,
              actual_missing.first);        // [確認_異常系] - 存在しないファイルの範囲の先頭が既定の 32768 であること。
    EXPECT_EQ(60999U, actual_missing.last); // [確認_異常系] - 存在しないファイルの範囲の末尾が既定の 60999 であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]
#else
// Windows の一時的なポートの範囲を、既定の動的ポートの範囲とすることの確認
// [サブ手順参照 名前=testPortTest.SetUp]
TEST_F(testPortTest, uses_default_dynamic_range)
{
    // Arrange

    // Pre-Assert

    // Act
    testing::test_port_internal::port_range actual =
        testing::test_port_internal::get_ephemeral_range(); // [手順] - get_ephemeral_range() を呼び出す。

    // Assert
    EXPECT_EQ(49152U, actual.first); // [確認_正常系] - 範囲の先頭が 49152 であること。
    EXPECT_EQ(65535U, actual.last);  // [確認_正常系] - 範囲の末尾が 65535 であること。
}
// [サブ手順参照 名前=testPortTest.TearDown]
#endif /* _WIN32 */

namespace
{

/**
 * 子プロセスの処理。"ready" を出力して開始の合図 (標準入力の 1 行) を待ち、count_text 個を 1 件ずつ払い出して出力する。
 */
int run_child(const char *count_text)
{
    unsigned long count = strtoul(count_text, nullptr, 10);
    char line[16] = {};

    printf("ready\n");
    fflush(stdout);
    if (fgets(line, sizeof(line), stdin) == nullptr)
    {
        return 2;
    }
    for (unsigned long index = 0U; index < count; index++)
    {
        uint16_t port = allocateTestPort();

        if (port == 0U)
        {
            return 1;
        }
        printf("port=%u\n", static_cast<unsigned>(port));
    }
    fflush(stdout);
    return 0;
}

std::string executable_path()
{
#if defined(_WIN32)
    char path[MAX_PATH] = {};
    DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);

    return (length == 0U || length >= MAX_PATH) ? std::string() : std::string(path, length);
#else
    char path[4096] = {};
    ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1U);

    return (length <= 0) ? std::string() : std::string(path, static_cast<size_t>(length));
#endif /* _WIN32 */
}

/**
 * 現在の一時ディレクトリの下に、このテスト専用のディレクトリを作る。失敗時は空文字列を返す。
 */
std::string create_temp_dir()
{
#if defined(_WIN32)
    char base[MAX_PATH + 1] = {};
    DWORD length = GetTempPathA(MAX_PATH + 1, base);

    if (length == 0U || length > MAX_PATH)
    {
        return std::string();
    }

    static unsigned long s_sequence = 0U;
    std::string dir = std::string(base, length) + "testPortTest-" + std::to_string(GetCurrentProcessId()) + "-" +
                      std::to_string(++s_sequence) + "\\";

    return CreateDirectoryA(dir.c_str(), nullptr) ? dir : std::string();
#else
    const char *base = getenv("TMPDIR");
    std::string pattern = std::string((base != nullptr && base[0] != '\0') ? base : "/tmp") + "/testPortTest-XXXXXX";
    std::vector<char> buffer(pattern.begin(), pattern.end());

    buffer.push_back('\0');
    return (mkdtemp(buffer.data()) == nullptr) ? std::string() : std::string(buffer.data());
#endif /* _WIN32 */
}

/**
 * テスト専用のディレクトリの中のファイル (記録のファイルと範囲のファイル) を消し、ディレクトリを消す。
 */
void remove_temp_dir(const std::string &dir)
{
#if defined(_WIN32)
    DeleteFileA((dir + "testfw-port-registry.bin").c_str());
    RemoveDirectoryA(dir.c_str());
#else
    const char *names[] = {"valid_range", "invalid_range"};

    for (const char *name : names)
    {
        unlink((dir + "/" + name).c_str());
    }
    unlink(testing::test_port_internal::registry_path().c_str());
    rmdir(dir.c_str());
#endif /* _WIN32 */
}

/**
 * 一時ディレクトリの環境変数の現在の値を保存し、dir に置き換える。
 */
void set_temp_dir_env(const std::string &dir)
{
#if defined(_WIN32)
    const char *names[] = {"TMP", "TEMP"};
#else
    const char *names[] = {"TMPDIR"};
#endif /* _WIN32 */

    s_saved_env_names.clear();
    s_saved_env_values.clear();
    s_saved_env_exists.clear();
    for (const char *name : names)
    {
#if defined(_WIN32)
        // GetTempPathW() はプロセスの環境変数 (GetEnvironmentVariableW) を読むため、Win32 の環境変数を切り替える。
        char value[MAX_PATH + 1] = {};
        DWORD length = GetEnvironmentVariableA(name, value, MAX_PATH + 1);

        s_saved_env_names.push_back(name);
        s_saved_env_values.push_back((length != 0U && length <= MAX_PATH) ? std::string(value, length) : "");
        s_saved_env_exists.push_back(length != 0U && length <= MAX_PATH);
        SetEnvironmentVariableA(name, dir.c_str());
#else
        const char *value = getenv(name);

        s_saved_env_names.push_back(name);
        s_saved_env_values.push_back(value != nullptr ? value : "");
        s_saved_env_exists.push_back(value != nullptr);
        setenv(name, dir.c_str(), 1);
#endif /* _WIN32 */
    }
}

/**
 * set_temp_dir_env() で保存した一時ディレクトリの環境変数を元に戻す。
 */
void restore_temp_dir_env()
{
    for (size_t index = 0U; index < s_saved_env_names.size(); index++)
    {
        const char *name = s_saved_env_names[index].c_str();
        const char *value = s_saved_env_values[index].c_str();
        bool exists = s_saved_env_exists[index];

#if defined(_WIN32)
        SetEnvironmentVariableA(name, exists ? value : nullptr);
#else
        if (exists)
        {
            setenv(name, value, 1);
        }
        else
        {
            unsetenv(name);
        }
#endif /* _WIN32 */
    }
    s_saved_env_names.clear();
    s_saved_env_values.clear();
    s_saved_env_exists.clear();
}

/**
 * テストが読み書きする記録のファイルのパスを返す。
 * Windows の registry_path() は UTF-8 を返し、std::fstream の char のパスは ANSI のコード ページで解釈されるため、
 * GetTempPathA() で作ったディレクトリから組み立てる。
 */
std::string test_registry_path()
{
#if defined(_WIN32)
    char dir[MAX_PATH + 1] = {};
    DWORD length = GetTempPathA(MAX_PATH + 1, dir);

    return (length == 0U || length > MAX_PATH) ? std::string() : std::string(dir, length) + "testfw-port-registry.bin";
#else
    return testing::test_port_internal::registry_path();
#endif /* _WIN32 */
}

std::vector<uint8_t> read_registry()
{
    std::ifstream input(test_registry_path(), std::ios::binary);

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

void write_registry(const std::vector<uint8_t> &bytes)
{
    std::ofstream output(test_registry_path(), std::ios::binary | std::ios::trunc);

    output.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

/**
 * 版と次の書き込み位置を指定し、番号をすべて 0 (空き) にした記録のファイルの内容を作る。
 */
std::vector<uint8_t> make_registry(uint32_t version, uint32_t next)
{
    std::vector<uint8_t> bytes(REGISTRY_SIZE, 0U);

    memcpy(bytes.data(), &version, sizeof(version));
    memcpy(bytes.data() + sizeof(version), &next, sizeof(next));
    return bytes;
}

uint32_t registry_u32(const std::vector<uint8_t> &bytes, size_t offset)
{
    uint32_t value = 0U;

    memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

uint16_t registry_port(const std::vector<uint8_t> &bytes, size_t index)
{
    uint16_t value = 0U;

    memcpy(&value, bytes.data() + sizeof(uint32_t) * 2U + sizeof(uint16_t) * index, sizeof(value));
    return value;
}

/**
 * 20000 から 32767 の外、または OS の一時的なポートの範囲の中にある番号の個数を返す。
 */
size_t count_outside_range(const std::vector<uint16_t> &ports)
{
    testing::test_port_internal::port_range ephemeral = testing::test_port_internal::get_ephemeral_range();
    size_t count = 0U;

    for (uint16_t port : ports)
    {
        if (port < testing::test_port_internal::RANGE_FIRST || port > testing::test_port_internal::RANGE_LAST ||
            (port >= ephemeral.first && port <= ephemeral.last))
        {
            count++;
        }
    }
    return count;
}

/**
 * type のソケットを作り、0.0.0.0 の port へ bind したまま sock に返す。
 */
bool bind_wildcard(int type, uint16_t port, intptr_t &sock)
{
#if defined(_WIN32)
    WSADATA data = {};

    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        return false;
    }

    SOCKET value = socket(AF_INET, type, 0);
#else
    int value = socket(AF_INET, type, 0);
#endif /* _WIN32 */
    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    sock = static_cast<intptr_t>(value);
    // testfw.h の using namespace std があると、bind は std::bind と曖昧になるため、グローバルの関数を指定する。
#if defined(_WIN32)
    return value != INVALID_SOCKET &&
           ::bind(value, reinterpret_cast<sockaddr *>(&addr), static_cast<int>(sizeof(addr))) == 0;
#else
    return value >= 0 && ::bind(value, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
#endif /* _WIN32 */
}

void close_socket(intptr_t sock)
{
#if defined(_WIN32)
    if (static_cast<SOCKET>(sock) != INVALID_SOCKET)
    {
        closesocket(static_cast<SOCKET>(sock));
    }
    WSACleanup();
#else
    if (sock >= 0)
    {
        close(static_cast<int>(sock));
    }
#endif /* _WIN32 */
}

} // namespace
