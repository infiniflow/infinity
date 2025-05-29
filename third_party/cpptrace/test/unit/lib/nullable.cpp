#include <cpptrace/basic.hpp>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

using cpptrace::nullable;

namespace {

TEST(NullableTest, Basic) {
    nullable<std::uint32_t> a{12};
    EXPECT_EQ(a.value(), 12);
    EXPECT_EQ(a.raw_value, 12);
    nullable<std::uint32_t> b = 20;
    EXPECT_EQ(b.value(), 20);
}

TEST(NullableTest, Null) {
    auto a = nullable<std::uint32_t>::null();
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.raw_value, (std::numeric_limits<std::uint32_t>::max)());
    nullable<std::uint32_t> b;
    EXPECT_FALSE(b.has_value());
    EXPECT_EQ(b.raw_value, nullable<std::uint32_t>::null_value());
}

TEST(NullableTest, Assignment) {
    nullable<std::uint32_t> a;
    a = 12;
    EXPECT_EQ(a.value(), 12);
    nullable<std::uint32_t> b = 20;
    a = b;
    EXPECT_EQ(a.value(), 20);
}

TEST(NullableTest, Reset) {
    nullable<std::uint32_t> a{12};
    a.reset();
    EXPECT_FALSE(a.has_value());
}

TEST(NullableTest, ValueOr) {
    auto a = nullable<std::uint32_t>::null();
    EXPECT_EQ(a.value_or(20), 20);
}

TEST(NullableTest, Comparison) {
    EXPECT_EQ(nullable<std::uint32_t>{12}, nullable<std::uint32_t>{12});
    EXPECT_NE(nullable<std::uint32_t>{12}, nullable<std::uint32_t>{20});
    EXPECT_NE(nullable<std::uint32_t>{12}, nullable<std::uint32_t>::null());
    EXPECT_EQ(nullable<std::uint32_t>::null(), nullable<std::uint32_t>::null());
}

TEST(NullableTest, Swap) {
    auto a = nullable<std::uint32_t>::null();
    nullable<std::uint32_t> b = 12;
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(b.value(), 12);
    a.swap(b);
    EXPECT_FALSE(b.has_value());
    EXPECT_EQ(a.value(), 12);
}

}
