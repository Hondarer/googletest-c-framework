#include <iostream>
#include <unordered_map>
#include <stdexcept>

#include <test_com.h>

using namespace std;
using namespace testing;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
class TraceLevelDictionary
{
private:
    unordered_map<string, int> data;
    int defaultLavel = TRACE_NONE;

    // コンストラクタを private にする (外部からのインスタンス化禁止)
    TraceLevelDictionary() {}

public:
    // シングルトンのインスタンスを取得
    static TraceLevelDictionary &getInstance()
    {
        static TraceLevelDictionary instance;
        return instance;
    }

    // コピー・ムーブを禁止
    TraceLevelDictionary(const TraceLevelDictionary &) = delete;
    TraceLevelDictionary &operator=(const TraceLevelDictionary &) = delete;

    // デフォルト値を設定する
    void setDefault(int defaultTraceLevel)
    {
        defaultLavel = defaultTraceLevel;
    }

    // データをリセットする
    void clear()
    {
        data.clear();
    }

    // 値を更新または追加する
    void update(const string &func, int traceLevel)
    {
        data[func] = traceLevel;
    }

    // 値を取得する（キーが存在しない場合は 0 を返す）
    int get(const string &func) const
    {
        auto it = data.find(func);
        if ((it == data.end()) || (it->second < defaultLavel))
        {
            return defaultLavel;
        }
        return it->second;
    }
};
#pragma GCC diagnostic pop

void testing::resetTraceLevel(int defaultTraceLevel)
{
    TraceLevelDictionary &dict = TraceLevelDictionary::getInstance();
    dict.clear();
    dict.setDefault(defaultTraceLevel);
}

int testing::_getTraceLevel(const char *key)
{
    TraceLevelDictionary &dict = TraceLevelDictionary::getInstance();
    return dict.get(key);
}

void testing::setDefaultTraceLevel(int defaultTraceLevel)
{
    TraceLevelDictionary &dict = TraceLevelDictionary::getInstance();
    dict.setDefault(defaultTraceLevel);
}

void testing::setTraceLevel(const char *func, int traceLevel)
{
    TraceLevelDictionary &dict = TraceLevelDictionary::getInstance();
    dict.update(func, traceLevel);
}
