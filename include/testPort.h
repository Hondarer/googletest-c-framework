#ifndef _TEST_PORT_H
#define _TEST_PORT_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace testing
{

/**
 * テストが待ち受けに使うポートの番号を 1 つ払い出す。
 *
 * 20000 から 32767 のうち、OS の一時的なポートの範囲に重ならない番号を候補とする。
 * 一時ディレクトリの共有ファイルに直近の 1024 件を記録し、ほかのプロセスを含めて
 * 直近に払い出した番号を避ける。0.0.0.0 への TCP と UDP の bind がどちらも成功した番号を返す。
 *
 * テストが EXPECT_CALL でソケットやファイルの関数を設定する前 (SetUp の先頭など) に呼び出す。
 *
 * @return  払い出した番号。失敗時は 0 を返し、理由を標準エラー出力へ出す。
 */
extern uint16_t allocateTestPort();

/**
 * 互いに異なるポートの番号を count 個払い出す。
 *
 * 払い出しの規則は allocateTestPort() と同じ。連続した番号になるとは限らない。
 *
 * @param count  払い出す個数 (1024 以下)。
 * @param ports  払い出した番号の格納先。失敗時は空にする。
 * @return       すべて払い出せた場合は true。1 つでも失敗した場合は false を返し、理由を標準エラー出力へ出す。
 */
extern bool allocateTestPorts(size_t count, std::vector<uint16_t> &ports);

} // namespace testing

#endif // _TEST_PORT_H
