#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "utils/utils.hpp"

using cpptrace::detail::byteswap;
using cpptrace::detail::n_digits;
using cpptrace::detail::to;
using cpptrace::detail::raii_wrapper;
using cpptrace::detail::raii_wrap;
using cpptrace::detail::maybe_owned;

namespace {

TEST(ByteSwapTest, ByteSwapUint8) {
    uint8_t input = 0x12;
    uint8_t expected = 0x12;
    uint8_t result = byteswap(input);
    EXPECT_EQ(result, expected);
}

TEST(ByteSwapTest, ByteSwapInt8) {
    int8_t input = 0x7F;
    int8_t expected = 0x7F;
    int8_t result = byteswap(input);
    EXPECT_EQ(result, expected);

    int8_t neg_input = to<int8_t>(0x80);
    int8_t neg_expected = to<int8_t>(0x80);
    int8_t neg_result = byteswap(neg_input);
    EXPECT_EQ(neg_result, neg_expected);
}

TEST(ByteSwapTest, ByteSwapUint16)
{
    uint16_t input = 0x1234;
    uint16_t expected = 0x3412;
    uint16_t result = byteswap(input);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(byteswap(to<uint16_t>(0x0000)), to<uint16_t>(0x0000));
    EXPECT_EQ(byteswap(to<uint16_t>(0xFFFF)), to<uint16_t>(0xFFFF));
}

TEST(ByteSwapTest, ByteSwapInt16) {
    int16_t input = 0x1234;
    int16_t expected = to<int16_t>(0x3412);
    int16_t result = byteswap(input);
    EXPECT_EQ(result, expected);

    int16_t neg_input = to<uint16_t>(0xFEFF);
    int16_t neg_expected = to<int16_t>(0xFFFE);
    int16_t neg_result = byteswap(neg_input);
    EXPECT_EQ(neg_result, neg_expected);
}

TEST(ByteSwapTest, ByteSwapUint32)
{
    uint32_t input = 0x12345678;
    uint32_t expected = 0x78563412;
    uint32_t result = byteswap(input);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(byteswap(to<uint32_t>(0x00000000)), to<uint32_t>(0x00000000));
    EXPECT_EQ(byteswap(to<uint32_t>(0xFFFFFFFF)), to<uint32_t>(0xFFFFFFFF));
}

TEST(ByteSwapTest, ByteSwapInt32)
{
    int32_t input = 0x12345678;
    int32_t expected = to<int32_t>(0x78563412);
    int32_t result = byteswap(input);
    EXPECT_EQ(result, expected);

    int32_t neg_input = to<uint32_t>(0xFF000000);
    int32_t neg_expected = to<int32_t>(0x000000FF);
    int32_t neg_result = byteswap(neg_input);
    EXPECT_EQ(neg_result, neg_expected);
}

TEST(ByteSwapTest, ByteSwapUint64)
{
    uint64_t input = 0x1122334455667788ULL;
    uint64_t expected = 0x8877665544332211ULL;
    uint64_t result = byteswap(input);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(byteswap(to<uint64_t>(0x0000000000000000ULL)), to<uint64_t>(0x0000000000000000ULL));
    EXPECT_EQ(byteswap(to<uint64_t>(0xFFFFFFFFFFFFFFFFULL)), to<uint64_t>(0xFFFFFFFFFFFFFFFFULL));
}

TEST(ByteSwapTest, ByteSwapInt64)
{
    int64_t input = 0x1122334455667788LL;
    int64_t expected = to<int64_t>(0x8877665544332211ULL);
    int64_t result = byteswap(input);
    EXPECT_EQ(result, expected);

    int64_t neg_input = to<uint64_t>(0xFF00000000000000ULL);
    int64_t neg_expected = to<int64_t>(0x00000000000000FFULL);
    int64_t neg_result = byteswap(neg_input);
    EXPECT_EQ(neg_result, neg_expected);
}

TEST(NDigitsTest, Basic)
{
    static_assert(n_digits(1) == 1, "n_digits utility producing the wrong result");
    static_assert(n_digits(9) == 1, "n_digits utility producing the wrong result");
    static_assert(n_digits(10) == 2, "n_digits utility producing the wrong result");
    static_assert(n_digits(11) == 2, "n_digits utility producing the wrong result");
    static_assert(n_digits(1024) == 4, "n_digits utility producing the wrong result");

    EXPECT_EQ(n_digits(1), 1);
    EXPECT_EQ(n_digits(9), 1);
    EXPECT_EQ(n_digits(10), 2);
    EXPECT_EQ(n_digits(11), 2);
    EXPECT_EQ(n_digits(1024), 4);
}

struct test_deleter {
    static int call_count;
    static int last_value;

    void operator()(int value) {
        call_count++;
        last_value = value;
    }
};
int test_deleter::call_count = 0;
int test_deleter::last_value = 0;

class RaiiWrapperTest : public ::testing::Test {
protected:
    RaiiWrapperTest() {
        test_deleter::call_count = 0;
        test_deleter::last_value = 0;
    }
};

TEST_F(RaiiWrapperTest, construct_and_destroy_calls_deleter) {
    {
        auto w = raii_wrap(42, test_deleter{});
        EXPECT_EQ(test_deleter::call_count, 0);
    }
    EXPECT_EQ(test_deleter::call_count, 1);
    EXPECT_EQ(test_deleter::last_value, 42);
}

TEST_F(RaiiWrapperTest, move_constructor_transfers_ownership) {
    {
        auto w1 = raii_wrap(123, test_deleter{});
        auto w2 = std::move(w1);
        EXPECT_EQ(test_deleter::call_count, 0);
        EXPECT_EQ(static_cast<int>(w2), 123);
    }
    EXPECT_EQ(test_deleter::call_count, 1);
    EXPECT_EQ(test_deleter::last_value, 123);
}

TEST_F(RaiiWrapperTest, operator_t_and_get) {
    {
        auto w = raii_wrap(999, test_deleter{});
        int i = w;
        EXPECT_EQ(i, 999);
        w.get() = 1000;
        EXPECT_EQ(static_cast<int>(w), 1000);
    }
    EXPECT_EQ(test_deleter::call_count, 1);
    EXPECT_EQ(test_deleter::last_value, 1000);
}

class counting_helper {
public:
    static int active;
    int value;
    counting_helper(int value) : value(value) {
        ++active;
    }
    ~counting_helper() {
        --active;
    }
    counting_helper(const counting_helper&) = delete;
    counting_helper(counting_helper&&) = delete;
    counting_helper& operator=(const counting_helper&) = delete;
    counting_helper& operator=(counting_helper&&) = delete;
    int foo() const {
        return value;
    }
};
int counting_helper::active = 0;

TEST(MaybeOwnedTest, NonOwningPointer) {
    ASSERT_EQ(counting_helper::active, 0);
    auto instance = std::make_unique<counting_helper>(42);
    EXPECT_EQ(counting_helper::active, 1);
    {
        maybe_owned<counting_helper> non_owning(instance.get());
        EXPECT_EQ(counting_helper::active, 1);
        EXPECT_EQ(non_owning->foo(), 42);
    }
    EXPECT_EQ(counting_helper::active, 1);
    instance.reset();
    EXPECT_EQ(counting_helper::active, 0);
}

TEST(MaybeOwnedTest, OwningPointer) {
    ASSERT_EQ(counting_helper::active, 0);
    auto instance = std::make_unique<counting_helper>(42);
    EXPECT_EQ(counting_helper::active, 1);
    {
        maybe_owned<counting_helper> non_owning(std::move(instance));
        EXPECT_EQ(counting_helper::active, 1);
        EXPECT_EQ(non_owning->foo(), 42);
    }
    EXPECT_EQ(counting_helper::active, 0);
    instance.reset();
    EXPECT_EQ(counting_helper::active, 0);
}

}
