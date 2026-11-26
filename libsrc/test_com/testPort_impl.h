#pragma once

/* テスト用ポートの払い出しの内部定義。
 * testPort*.cc と、その動作を検査する testfw のテストだけが include する非公開ヘッダー。 */

#include <testPort.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace testing
{
namespace test_port_internal
{

/** 払い出しの候補の範囲の先頭。 */
constexpr uint16_t RANGE_FIRST = 20000U;
/** 払い出しの候補の範囲の末尾。 */
constexpr uint16_t RANGE_LAST = 32767U;
/** 払い出しの記録に残す直近の件数。 */
constexpr size_t REGISTRY_CAPACITY = 1024U;
/** 払い出しの記録のファイルの形式の版。 */
constexpr uint32_t REGISTRY_VERSION = 1U;
/** 払い出しの記録のファイルの大きさ (版、次の書き込み位置、番号の固定長の配列)。 */
constexpr size_t REGISTRY_SIZE = sizeof(uint32_t) * 2U + sizeof(uint16_t) * REGISTRY_CAPACITY;

/** ポートの番号の閉区間。 */
struct port_range
{
    uint16_t first;
    uint16_t last;
};

/** bind による確認の結果。 */
enum class probe_result
{
    AVAILABLE, ///< TCP と UDP の両方で bind できた。
    IN_USE,    ///< どちらかの bind が失敗した。
    FAILED     ///< bind 以外の失敗 (ソケットの作成など)。理由は標準エラー出力へ出力済み。
};

/** ロックした払い出しの記録のファイル。Linux はファイル記述子、Windows は HANDLE の値を持つ。 */
struct registry_file
{
    intptr_t value = -1;
};

/**
 * 候補の範囲から excluded に重なる番号を除いた候補を返す。
 */
std::vector<uint16_t> build_candidates(const port_range &excluded);

/**
 * 払い出しの記録をロックし、candidates のうち直近の記録になく bind できる番号を count 個払い出して記録する。
 * 失敗時は false を返し、理由を標準エラー出力へ出す。ports は失敗時に空にする。
 */
bool allocate_from(const char *caller, const std::vector<uint16_t> &candidates, size_t count,
                   std::vector<uint16_t> &ports);

/* -------- プラットフォーム固有 (testPort_linux.cc / testPort_windows.cc) -------- */

/** OS の一時的なポートの範囲を返す。 */
port_range get_ephemeral_range();

/** 払い出しの記録のファイルのパスを返す。 */
std::string registry_path();

/** 払い出しの記録のファイルを開き (なければ作成し)、ファイル全体を排他でロックする。 */
bool registry_open(const char *caller, registry_file &file);

/** ロックした記録のファイルの内容をすべて読む。 */
bool registry_read(const char *caller, registry_file &file, std::vector<uint8_t> &bytes);

/** ロックした記録のファイルの内容を bytes で置き換える。 */
bool registry_write(const char *caller, registry_file &file, const std::vector<uint8_t> &bytes);

/** ロックを解放し、記録のファイルを閉じる。 */
void registry_close(registry_file &file);

/** 0.0.0.0 の port へ、アドレスの再利用の指定なしで TCP と UDP の bind を試みる。 */
probe_result probe_port(const char *caller, uint16_t port);

#ifndef _WIN32
/** ip_local_port_range の形式のファイルを読む。読めない場合は既定の範囲 (32768 から 60999) を返す。 */
port_range read_ephemeral_range(const char *path);
#endif /* _WIN32 */

} // namespace test_port_internal
} // namespace testing
