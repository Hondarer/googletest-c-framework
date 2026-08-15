#ifndef TESTFW_MOCK_INSTANCE_H
#define TESTFW_MOCK_INSTANCE_H

#include <gtest/gtest.h>

#include <cstddef>

namespace testfw
{
namespace internal
{

template <typename MockType> inline std::size_t &mockInstanceCount()
{
    static std::size_t count = 0U;

    return count;
}

template <typename MockType>
inline void registerMockInstance(MockType *&registered_mock, MockType *instance, const char *slot_name,
                                 const char *file, int line)
{
    std::size_t &count = mockInstanceCount<MockType>();

    if (count != 0U)
    {
        ADD_FAILURE_AT(file, line) << "Only one mock instance may exist for " << slot_name << " at a time.";
    }
    else
    {
        registered_mock = instance;
    }
    ++count;
}

template <typename MockType>
inline void unregisterMockInstance(MockType *&registered_mock, MockType *instance, const char *slot_name,
                                   const char *file, int line)
{
    std::size_t &count = mockInstanceCount<MockType>();

    if (count == 0U)
    {
        ADD_FAILURE_AT(file, line) << "Mock instance count is already zero for " << slot_name << ".";
        return;
    }

    --count;
    if (registered_mock == instance)
    {
        registered_mock = nullptr;
    }
}

} // namespace internal
} // namespace testfw

#define TESTFW_REGISTER_MOCK_INSTANCE(mock_slot) \
    ::testfw::internal::registerMockInstance((mock_slot), this, #mock_slot, __FILE__, __LINE__)
#define TESTFW_UNREGISTER_MOCK_INSTANCE(mock_slot) \
    ::testfw::internal::unregisterMockInstance((mock_slot), this, #mock_slot, __FILE__, __LINE__)

#endif // TESTFW_MOCK_INSTANCE_H
