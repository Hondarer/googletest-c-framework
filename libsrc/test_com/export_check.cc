/* DLL/SO のエクスポート シンボル検証と、公開ヘッダーの export マクロ付け忘れ検出の共通処理。
 * app 固有のエクスポート テーブル定義 (EXPORT_ENTRY マクロ) やパス組み立てはこのファイルに置かない。 */

#include <export_check.h>

#include <processController.h>
#include <test_com.h>

#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

namespace testing
{

string joinNames(const vector<string> &names)
{
    string joined;
    for (size_t i = 0; i < names.size(); ++i)
    {
        if (i > 0)
        {
            joined += ", ";
        }
        joined += names[i];
    }
    return joined;
}

namespace
{

// dumpbin /exports (Windows) の出力からエクスポート シンボル名を抽出する。
// 例: "          1    0 0001D370 ChangeServiceConfig2U = ChangeServiceConfig2U"
set<string> parseDumpbinExportNames(const string &stdout_text)
{
    set<string> names;
    static const regex line_pattern(R"(^\s*\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+(\S+))");
    istringstream iss(stdout_text);
    string line;
    while (getline(iss, line))
    {
        smatch m;
        if (regex_search(line, m, line_pattern))
        {
            names.insert(m[1].str());
        }
    }
    return names;
}

// nm -D --defined-only (Linux) の出力からエクスポート シンボル名を抽出する。
// 例: "0000000000012340 T mylib_open"
set<string> parseNmExportNames(const string &stdout_text)
{
    set<string> names;
    static const regex line_pattern(R"(^[0-9A-Fa-f]+\s+\S\s+(\S+)\s*$)");
    istringstream iss(stdout_text);
    string line;
    while (getline(iss, line))
    {
        smatch m;
        if (regex_search(line, m, line_pattern))
        {
            names.insert(m[1].str());
        }
    }
    return names;
}

} // namespace

set<string> getActualExportNames(const string &dll_or_so_path)
{
    ProcessOptions opts;

    printf("  > getActualExportNames 対象=\"%s\"\n", dll_or_so_path.c_str());

#ifndef _WIN32
    printf("  > getActualExportNames 実行コマンド: nm -D --defined-only \"%s\"\n", dll_or_so_path.c_str());
    ProcessResult res = startProcess("nm", {"-D", "--defined-only", dll_or_so_path}, opts);
    if (res.exit_code != 0)
    {
        ADD_FAILURE() << "nm -D --defined-only の実行に失敗しました (exit_code=" << res.exit_code
                       << "): " << res.stderr_out;
        return {};
    }
    set<string> names = parseNmExportNames(res.stdout_out);
#else
    printf("  > getActualExportNames 実行コマンド: dumpbin /exports \"%s\"\n", dll_or_so_path.c_str());
    ProcessResult res = startProcess("dumpbin", {"/exports", dll_or_so_path}, opts);
    if (res.exit_code != 0)
    {
        ADD_FAILURE() << "dumpbin /exports の実行に失敗しました (exit_code=" << res.exit_code
                       << "): " << res.stderr_out;
        return {};
    }
    set<string> names = parseDumpbinExportNames(res.stdout_out);
#endif

    if (names.empty())
    {
        ADD_FAILURE() << "エクスポート シンボルを 1 件も取得できませんでした。対象: " << dll_or_so_path
                       << " 出力: " << res.stdout_out;
    }

    printf("  > getActualExportNames 実際のエクスポート シンボル数=%zu\n", names.size());
    for (const auto &name : names)
    {
        printf("  >   actual: %s\n", name.c_str());
    }

    return names;
}

void expectExportNamesMatch(const set<string> &expected, const set<string> &actual,
                             const map<string, string> &signatures)
{
    printf("  > expectExportNamesMatch 期待シンボル数=%zu 実際のシンボル数=%zu\n", expected.size(), actual.size());

    vector<string> missing;
    for (const auto &name : expected)
    {
        bool found = actual.find(name) != actual.end();
        auto sig_it = signatures.find(name);
        if (sig_it != signatures.end())
        {
            printf("  >   expected: %s [%s] sig=%s\n", name.c_str(), found ? "OK" : "MISSING",
                   sig_it->second.c_str());
        }
        else
        {
            printf("  >   expected: %s [%s]\n", name.c_str(), found ? "OK" : "MISSING");
        }
        if (!found)
        {
            missing.push_back(name);
        }
    }
    EXPECT_TRUE(missing.empty()) << "不足しているエクスポート: " << joinNames(missing);

#ifdef _WIN32
    // Windows は __declspec(dllexport) で明示的にエクスポート対象を選ぶビルドのため、
    // 過不足なしの完全一致を要求する。
    vector<string> extra;
    for (const auto &name : actual)
    {
        if (expected.find(name) == expected.end())
        {
            printf("  >   extra: %s [EXTRA]\n", name.c_str());
            extra.push_back(name);
        }
    }
    EXPECT_TRUE(extra.empty()) << "想定外のエクスポート: " << joinNames(extra);
#endif
    // Linux (.so) は可視性制御 (-fvisibility=hidden 等) が未整備で、
    // include_internal 配下の内部共有関数もリンカ シンボルとして公開されるため、
    // 過剰なエクスポートは失敗要因にせず、期待シンボルが含まれることのみ確認する。
}

vector<string> findUndecoratedExternVariables(const string &include_dir, const string &export_macro_name)
{
    vector<string> undecorated;
    static const regex extern_c_pattern(R"(^\s*extern\s+"C")");
    static const regex extern_var_pattern(R"(^\s*extern\s+[^()]+;\s*(//.*)?$)");

    printf("  > findUndecoratedExternVariables include_dir=\"%s\" export_macro_name=\"%s\"\n", include_dir.c_str(),
           export_macro_name.c_str());

    namespace fs = std::filesystem;
    if (!fs::exists(include_dir))
    {
        printf("  > findUndecoratedExternVariables include_dir が存在しないため走査対象なし\n");
        return undecorated;
    }

    size_t header_count = 0;
    for (const auto &entry : fs::recursive_directory_iterator(include_dir))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".h")
        {
            continue;
        }
        ++header_count;
        printf("  >   header: %s\n", entry.path().string().c_str());

        ifstream ifs(entry.path());
        string line;
        while (getline(ifs, line))
        {
            if (regex_search(line, extern_c_pattern))
            {
                continue;
            }
            if (!regex_search(line, extern_var_pattern))
            {
                continue;
            }
            bool decorated = line.find(export_macro_name) != string::npos;
            printf("  >     variable: %s [%s]\n", line.c_str(), decorated ? "OK" : "MISSING_EXPORT");
            if (!decorated)
            {
                undecorated.push_back(entry.path().string() + ": " + line);
            }
        }
    }

    printf("  > findUndecoratedExternVariables 走査したヘッダー数=%zu %s を伴わない extern 変数宣言=%zu 件\n",
           header_count, export_macro_name.c_str(), undecorated.size());

    return undecorated;
}

string identManifestSymbolName(const string &target)
{
    // gen_ident_manifest.py の sanitize_symbol() と同じ規則: [^A-Za-z0-9_] を '_' に置換する。
    string sanitized = target;
    for (char &c : sanitized)
    {
        if (!isalnum(static_cast<unsigned char>(c)) && c != '_')
        {
            c = '_';
        }
    }
    if (!sanitized.empty() && isdigit(static_cast<unsigned char>(sanitized[0])))
    {
        sanitized = "_" + sanitized;
    }
    return "_ident_manifest_" + sanitized;
}

} // namespace testing
