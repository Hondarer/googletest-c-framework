/* テスト用ポートの払い出しのプラットフォーム共通の実装。
 * 記録のファイルの操作、OS の一時的なポートの範囲、bind による確認は
 * testPort_linux.cc / testPort_windows.cc に実装する。 */

#include "testPort_impl.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <random>
#include <unordered_set>

namespace testing
{
namespace test_port_internal
{
namespace
{

/** 払い出しの記録のファイルの内容。 */
struct registry_record
{
    size_t next = 0U;
    std::vector<uint16_t> ports = std::vector<uint16_t>(REGISTRY_CAPACITY, 0U);
};

bool allocate_locked(const char *caller, registry_file &file, const std::vector<uint16_t> &candidates, size_t count,
                     std::vector<uint16_t> &ports);
void decode_record(const std::vector<uint8_t> &bytes, registry_record &record);
std::vector<uint8_t> encode_record(const registry_record &record);
size_t select_start(size_t candidate_count);

} // namespace
} // namespace test_port_internal

uint16_t allocateTestPort()
{
    std::vector<uint16_t> ports;

    if (!test_port_internal::allocate_from(
            "allocateTestPort", test_port_internal::build_candidates(test_port_internal::get_ephemeral_range()), 1U,
            ports))
    {
        return 0U;
    }
    return ports[0];
}

bool allocateTestPorts(size_t count, std::vector<uint16_t> &ports)
{
    return test_port_internal::allocate_from(
        "allocateTestPorts", test_port_internal::build_candidates(test_port_internal::get_ephemeral_range()), count,
        ports);
}

namespace test_port_internal
{

std::vector<uint16_t> build_candidates(const port_range &excluded)
{
    std::vector<uint16_t> candidates;

    for (uint32_t port = RANGE_FIRST; port <= RANGE_LAST; port++)
    {
        if (port < excluded.first || port > excluded.last)
        {
            candidates.push_back(static_cast<uint16_t>(port));
        }
    }
    return candidates;
}

bool allocate_from(const char *caller, const std::vector<uint16_t> &candidates, size_t count,
                   std::vector<uint16_t> &ports)
{
    ports.clear();
    if (count > REGISTRY_CAPACITY)
    {
        fprintf(stderr, "%s: count %zu exceeds the registry capacity %zu.\n", caller, count, REGISTRY_CAPACITY);
        return false;
    }

    registry_file file;

    if (!registry_open(caller, file))
    {
        return false;
    }

    bool allocated = allocate_locked(caller, file, candidates, count, ports);

    registry_close(file);
    if (!allocated)
    {
        ports.clear();
    }
    return allocated;
}

namespace
{

/**
 * ロック済みの記録を読み、乱数で選んだ開始位置から候補を順に調べて count 個を払い出し、記録へ書き戻す。
 */
bool allocate_locked(const char *caller, registry_file &file, const std::vector<uint16_t> &candidates, size_t count,
                     std::vector<uint16_t> &ports)
{
    std::vector<uint8_t> bytes;

    if (!registry_read(caller, file, bytes))
    {
        return false;
    }

    registry_record record;

    decode_record(bytes, record);

    std::unordered_set<uint16_t> excluded(record.ports.begin(), record.ports.end());
    size_t start = select_start(candidates.size());

    for (size_t offset = 0U; offset < candidates.size() && ports.size() < count; offset++)
    {
        uint16_t candidate = candidates[(start + offset) % candidates.size()];

        if (excluded.count(candidate) != 0U)
        {
            continue;
        }

        probe_result probe = probe_port(caller, candidate);

        if (probe == probe_result::FAILED)
        {
            return false;
        }
        if (probe == probe_result::AVAILABLE)
        {
            ports.push_back(candidate);
            excluded.insert(candidate);
        }
    }
    if (ports.size() < count)
    {
        fprintf(stderr, "%s: no available port in %u..%u (%zu candidates, %zu requested, %zu allocated).\n", caller,
                static_cast<unsigned>(RANGE_FIRST), static_cast<unsigned>(RANGE_LAST), candidates.size(), count,
                ports.size());
        return false;
    }
    for (uint16_t port : ports)
    {
        record.ports[record.next] = port;
        record.next = (record.next + 1U) % REGISTRY_CAPACITY;
    }
    return registry_write(caller, file, encode_record(record));
}

/**
 * 記録のファイルの内容を解釈する。大きさ、版、次の書き込み位置のいずれかが不正な場合は空の記録とする。
 */
void decode_record(const std::vector<uint8_t> &bytes, registry_record &record)
{
    uint32_t version = 0U;
    uint32_t next = 0U;

    if (bytes.size() != REGISTRY_SIZE)
    {
        return;
    }
    memcpy(&version, bytes.data(), sizeof(version));
    memcpy(&next, bytes.data() + sizeof(version), sizeof(next));
    if (version != REGISTRY_VERSION || next >= REGISTRY_CAPACITY)
    {
        return;
    }
    record.next = next;
    memcpy(record.ports.data(), bytes.data() + sizeof(version) + sizeof(next), sizeof(uint16_t) * REGISTRY_CAPACITY);
}

std::vector<uint8_t> encode_record(const registry_record &record)
{
    std::vector<uint8_t> bytes(REGISTRY_SIZE, 0U);
    uint32_t version = REGISTRY_VERSION;
    auto next = static_cast<uint32_t>(record.next);

    memcpy(bytes.data(), &version, sizeof(version));
    memcpy(bytes.data() + sizeof(version), &next, sizeof(next));
    memcpy(bytes.data() + sizeof(version) + sizeof(next), record.ports.data(), sizeof(uint16_t) * REGISTRY_CAPACITY);
    return bytes;
}

/**
 * 候補を調べ始める位置を乱数で選ぶ。同時に起動したプロセスが同じ番号から調べ始めないよう、時刻も混ぜる。
 */
size_t select_start(size_t candidate_count)
{
    if (candidate_count == 0U)
    {
        return 0U;
    }

    std::random_device device;
    auto now = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::seed_seq seed{device(), device(), static_cast<uint32_t>(now), static_cast<uint32_t>(now >> 32)};
    std::mt19937 engine(seed);

    return static_cast<size_t>(engine() % candidate_count);
}

} // namespace
} // namespace test_port_internal

} // namespace testing
