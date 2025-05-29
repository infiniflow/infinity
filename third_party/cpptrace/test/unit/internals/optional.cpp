#include <gtest/gtest.h>

#include "utils/optional.hpp"

using cpptrace::detail::optional;
using cpptrace::detail::nullopt;

namespace {

TEST(OptionalTest, DefaultConstructor) {
    optional<int> o;
    EXPECT_FALSE(o.has_value());
    EXPECT_FALSE(static_cast<bool>(o));

    optional<int&> o1;
    EXPECT_FALSE(o1.has_value());
    EXPECT_FALSE(static_cast<bool>(o1));
}

TEST(OptionalTest, ConstructWithNullopt) {
    optional<int> o(nullopt);
    EXPECT_FALSE(o.has_value());

    optional<int&> o1(nullopt);
    EXPECT_FALSE(o1.has_value());
    EXPECT_FALSE(static_cast<bool>(o1));
}

TEST(OptionalTest, ValueConstructor) {
    optional<int> o(42);
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(o.unwrap(), 42);

    int x = 100;
    optional<int> o2(x);
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o2.unwrap(), 100);

    int y = 100;
    optional<int&> o3(y);
    EXPECT_TRUE(o3.has_value());
    EXPECT_EQ(o3.unwrap(), 100);
    y = 200;
    EXPECT_EQ(o3.unwrap(), 200);
}

TEST(OptionalTest, CopyConstructor) {
    optional<int> o1(42);
    optional<int> o2(o1);
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o2.unwrap(), 42);

    optional<int> o3(nullopt);
    optional<int> o4(o3);
    EXPECT_FALSE(o4.has_value());

    int y = 100;
    optional<int&> o5(y);
    optional<int&> o6(o5);
    EXPECT_TRUE(o5.has_value());
    EXPECT_EQ(o5.unwrap(), 100);
    EXPECT_TRUE(o6.has_value());
    EXPECT_EQ(o6.unwrap(), 100);
    y = 200;
    EXPECT_EQ(o5.unwrap(), 200);
    EXPECT_EQ(o6.unwrap(), 200);
}

TEST(OptionalTest, MoveConstructor) {
    optional<int> o1(42);
    optional<int> o2(std::move(o1));
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o2.unwrap(), 42);

    optional<int> o3(nullopt);
    optional<int> o4(std::move(o3));
    EXPECT_FALSE(o4.has_value());

    int y = 100;
    optional<int&> o5(y);
    optional<int&> o6(std::move(o5));
    EXPECT_TRUE(o6.has_value());
    EXPECT_EQ(o6.unwrap(), 100);
    y = 200;
    EXPECT_EQ(o6.unwrap(), 200);
}

TEST(OptionalTest, CopyAssignmentOperator) {
    optional<int> o1(42);
    optional<int> o2;
    o2 = o1;
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o2.unwrap(), 42);

    optional<int> o3(nullopt);
    optional<int> o4(100);
    o4 = o3;
    EXPECT_FALSE(o4.has_value());

    int y = 100;
    optional<int&> o5(y);
    optional<int&> o6;
    o6 = o5;
    EXPECT_TRUE(o5.has_value());
    EXPECT_EQ(o5.unwrap(), 100);
    EXPECT_TRUE(o6.has_value());
    EXPECT_EQ(o6.unwrap(), 100);
    y = 200;
    EXPECT_EQ(o5.unwrap(), 200);
    EXPECT_EQ(o6.unwrap(), 200);
}

TEST(OptionalTest, MoveAssignmentOperator) {
    optional<int> o1(42);
    optional<int> o2;
    o2 = std::move(o1);
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o2.unwrap(), 42);

    optional<int> o3(nullopt);
    optional<int> o4(99);
    o4 = std::move(o3);
    EXPECT_FALSE(o4.has_value());

    int y = 100;
    optional<int&> o5(y);
    optional<int&> o6;
    o6 = std::move(o5);
    EXPECT_TRUE(o6.has_value());
    EXPECT_EQ(o6.unwrap(), 100);
    y = 200;
    EXPECT_EQ(o6.unwrap(), 200);
}

TEST(OptionalTest, AssignmentFromValue) {
    optional<int> o;
    o = 123;
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(o.unwrap(), 123);

    o = nullopt;
    EXPECT_FALSE(o.has_value());

    optional<int&> o1;
    int x = 100;
    o1 = x;
    EXPECT_TRUE(o1.has_value());
    EXPECT_EQ(o1.unwrap(), x);
    EXPECT_EQ(&o1.unwrap(), &x);

    o1 = nullopt;
    EXPECT_FALSE(o1.has_value());
}

TEST(OptionalTest, Reset) {
    optional<int> o(42);
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(o.unwrap(), 42);
    o.reset();
    EXPECT_FALSE(o.has_value());

    int x = 44;
    optional<int&> o1(x);
    EXPECT_TRUE(o1.has_value());
    EXPECT_EQ(o1.unwrap(), 44);
    o1.reset();
    EXPECT_FALSE(o1.has_value());
}

TEST(OptionalTest, Swap) {
    optional<int> o1(42);
    optional<int> o2(100);

    o1.swap(o2);
    EXPECT_TRUE(o1.has_value());
    EXPECT_TRUE(o2.has_value());
    EXPECT_EQ(o1.unwrap(), 100);
    EXPECT_EQ(o2.unwrap(), 42);

    // Swap a value-holding optional with an empty optional
    optional<int> o3(7);
    optional<int> o4(nullopt);
    o3.swap(o4);
    EXPECT_FALSE(o3.has_value());
    EXPECT_TRUE(o4.has_value());
    EXPECT_EQ(o4.unwrap(), 7);

    int x = 20;
    int y = 40;
    optional<int&> o5 = x;
    optional<int&> o6 = y;
    EXPECT_EQ(o5.unwrap(), 20);
    EXPECT_EQ(o6.unwrap(), 40);
    o5.swap(o6);
    EXPECT_EQ(o5.unwrap(), 40);
    EXPECT_EQ(o6.unwrap(), 20);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 40);
}

TEST(OptionalTest, ValueOr) {
    optional<int> o1(42);
    EXPECT_EQ(o1.value_or(100), 42);

    optional<int> o2(nullopt);
    EXPECT_EQ(o2.value_or(100), 100);

    int x = 20;
    int y = 100;
    optional<int&> o3(x);
    EXPECT_EQ(o3.value_or(y), 20);
    o3.reset();
    EXPECT_EQ(o3.value_or(y), 100);
    EXPECT_EQ(&o3.value_or(y), &y);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 100);
}

}
