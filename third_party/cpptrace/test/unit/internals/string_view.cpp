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
}

}
