#ifndef _EXPORT_CHECK_H
#define _EXPORT_CHECK_H

#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

namespace testing
{

/**
 * EXPORT_ENTRY マクロ テーブルのエントリからシグネチャの static_assert を生成する。
 * GetProcAddress/dlsym 等でアドレスを取得するのではなく、公開ヘッダーの宣言が
 * テーブルの期待シグネチャと一致するかをコンパイル時に検証する。関数は呼び出さない。
 * 関数ポインタ型・オブジェクト ポインタ型 (変数用) のどちらのシグネチャにも使える。
 */
#define TESTFW_EXPORT_STATIC_ASSERT_ENTRY(name, sig) \
    static_assert(std::is_same<decltype(&name), sig>::value, #name " のエクスポート シグネチャが変更されています");

/** EXPORT_ENTRY マクロ テーブルのエントリから期待シンボル名の配列要素を生成する。 */
#define TESTFW_EXPORT_NAME_ENTRY(name, sig) #name,

/**
 * EXPORT_ENTRY マクロ テーブルのエントリから「名前 → シグネチャ文字列」の
 * map 初期化要素を生成する。シグネチャはソース上の記述がプリプロセッサの # によって
 * そのまま文字列化される (decltype 等の型を実行時にリフレクションで文字列化する
 * ことはできないため、コンパイル時のトークン列を流用する) 。
 */
#define TESTFW_EXPORT_SIGNATURE_ENTRY(name, sig) {#name, #sig},

/**
 * 文字列一覧をカンマ区切りで連結する (テスト失敗メッセージの整形用) 。
 */
extern string joinNames(const vector<string> &names);

/**
 * 実際にビルドされた DLL (Windows) / SO (Linux) から、エクスポートされている
 * シンボル名の一覧を取得する。
 * Windows では dumpbin /exports、Linux では nm -D --defined-only を内部で実行する。
 * プラットフォーム分岐はこの関数の内部に閉じ込められており、呼び出し側で
 * #if defined(PLATFORM_WINDOWS) 等を書く必要はない。
 *
 * @param dll_or_so_path  検査対象の DLL/SO の絶対パス
 */
extern set<string> getActualExportNames(const string &dll_or_so_path);

/**
 * 期待シンボル名一覧 (EXPORT_ENTRY マクロ テーブルから生成) と、実際のエクスポート一覧を突き合わせる。
 * 不足しているシンボルはプラットフォーム共通で EXPECT_TRUE により報告する。
 * Windows は __declspec(dllexport) で明示的にエクスポート対象を選ぶビルドのため、
 * 想定外のシンボル (テーブル未登録なのに実際にはエクスポートされている) も併せて報告する。
 * Linux (.so) は可視性制御 (-fvisibility=hidden 等) が未整備な構成が多く、
 * include_internal 配下の内部共有関数もリンカ シンボルとして公開されがちなため、
 * 想定外チェックは行わない (不足チェックのみ) 。
 *
 * @param signatures  名前 → シグネチャ文字列 (TESTFW_EXPORT_SIGNATURE_ENTRY で生成) 。
 *                    渡された場合、該当する expected シンボルの stdout にシグネチャを併記する。
 *                    省略した場合、シグネチャなしで名前と OK/MISSING のみ出力する。
 */
extern void expectExportNamesMatch(const set<string> &expected, const set<string> &actual,
                                    const map<string, string> &signatures = {});

/**
 * include_dir 配下のヘッダー (*.h) から、export_macro_name を伴わない
 * 「extern <型> <名前>;」形式のファイル スコープ変数宣言を検出する。
 * 関数宣言 ('(' を含む行) と extern "C" ブロックの開始行は対象外とする。
 * テンプレートやマクロ展開後にしか判定できない宣言のように、複雑なものは対象外とする
 * (誤検出より見逃しを許容する) 。
 *
 * static 専用ライブラリ (DLL/SO を生成しない include サブツリー) には export マクロが
 * そもそも不要なため、呼び出し側は DLL/SO エクスポートに対応する include サブディレクトリ
 * のみを指定すること。app 全体の prod/include を丸ごと渡さないこと。
 *
 * @param include_dir        走査対象ディレクトリ (対応する DLL/SO のソースとなる、
 *                            特定ライブラリの公開ヘッダー サブディレクトリ)
 * @param export_macro_name  必須とする export マクロ名 (例: "MYLIB_EXPORT")
 */
extern vector<string> findUndecoratedExternVariables(const string &include_dir, const string &export_macro_name);

/**
 * IDENT 機能 (framework/makefw/bin/gen_ident_manifest.py) が自動生成する
 * ビルド識別データ シンボルの名前を組み立てる。
 * 生成規則は gen_ident_manifest.py の sanitize_symbol() と揃える必要がある
 * (対象名に含まれる英数字・アンダースコア以外の文字はアンダースコアに置換される) 。
 * Windows の共有ライブラリ ビルドでのみこのシンボルが実際にエクスポートされる。
 *
 * @param target  makepart.mk の TARGET に対応するファイル名 (拡張子込み、例: "libmylib.dll") 。
 *                sanitize_symbol() は英数字とアンダースコア以外をアンダースコアへ置換するため、
 *                拡張子の "." も "_" になる。
 */
extern string identManifestSymbolName(const string &target);

} // namespace testing

#endif // _EXPORT_CHECK_H
