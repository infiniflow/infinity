#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include <string>

#include "utils/string_view.hpp"

using cpptrace::detail::string_view;
using cpptrace::detail::cstring_view;

namespace {

TEST(StringViewTest, Basic) {
    string_view sv = "foo";
    EXPECT_EQ(sv.size(), 3);
    EXPECT_EQ(sv.data(), std::string("foo"));
    EXPECT_EQ(sv[0], 'f');
    EXPECT_EQ(sv[1], 'o');
    EXPECT_EQ(sv.find_last_of("f"), 0);
    EXPECT_EQ(sv.find_last_of("o"), 2);
    EXPECT_EQ(sv.find_last_of("asfd"), 0);
    EXPECT_EQ(sv.find_last_of("asod"), 2);
    EXPECT_EQ(sv.find_last_of("bar"), sv.npos);
    EXPECT_EQ(sv, "foo");
    EXPECT_NE(sv, "bar");
    string_view sv2 = "abc";
    EXPECT_EQ(sv2.front(), 'a');
    EXPECT_EQ(sv2.back(), 'c');
    EXPECT_TRUE(sv2.starts_with("ab"));
    EXPECT_TRUE(sv2.ends_with("bc"));
    EXPECT_TRUE(sv2.starts_with("abc"));
    EXPECT_TRUE(sv2.ends_with("abc"));
    EXPECT_FALSE(sv2.starts_with("abce"));
    EXPECT_FALSE(sv2.ends_with("abce"));
    EXPECT_EQ(sv2.substr(1), "bc");
    EXPECT_EQ(sv2.substr(0, 2), "ab");
    sv2.advance(1);
    EXPECT_EQ(sv2, "bc");
    std::string s = "foo";
    s += sv2;
    EXPECT_EQ(s, "foobc");
}

}
