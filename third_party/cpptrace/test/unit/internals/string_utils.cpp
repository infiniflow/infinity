#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "utils/utils.hpp"

using testing::ElementsAre;
using cpptrace::detail::split;
using cpptrace::detail::join;
using cpptrace::detail::trim;
using cpptrace::detail::starts_with;

namespace {

TEST(SplitTest, SplitBySingleDelimiter) {
    std::string input = "hello,world";
    auto tokens = split(input, ",");
    EXPECT_THAT(tokens, ElementsAre("hello", "world"));
}

TEST(SplitTest, SplitByMultipleDelimiters) {
    std::string input = "hello,world;test";
    auto tokens = split(input, ",;");
    EXPECT_THAT(tokens, ElementsAre("hello", "world", "test"));
}

TEST(SplitTest, HandlesNoDelimiterFound) {
    std::string input = "nodellimitershere";
    auto tokens = split(input, ", ");
    EXPECT_THAT(tokens, ElementsAre("nodellimitershere"));
}

TEST(SplitTest, HandlesEmptyString) {
    std::string input = "";
    auto tokens = split(input, ",");
    EXPECT_THAT(tokens, ElementsAre(""));
}

TEST(SplitTest, HandlesConsecutiveDelimiters) {
    std::string input = "one,,two,,,three";
    auto tokens = split(input, ",");
    EXPECT_THAT(tokens, ElementsAre("one", "", "two", "", "", "three"));
}

TEST(SplitTest, HandlesTrailingDelimiter) {
    std::string input = "abc,";
    auto tokens = split(input, ",");
    EXPECT_THAT(tokens, ElementsAre("abc", ""));
}

TEST(SplitTest, HandlesLeadingDelimiter) {
    std::string input = ",abc";
    auto tokens = split(input, ",");
    EXPECT_THAT(tokens, ElementsAre("", "abc"));
}

TEST(JoinTest, EmptyContainer) {
    std::vector<std::string> vec;
    EXPECT_EQ(join(vec, ","), "");
}

TEST(JoinTest, SingleElements) {
    std::vector<std::string> vec = {"one"};
    EXPECT_EQ(join(vec, ","), "one");
}

TEST(JoinTest, MultipleElements) {
    std::vector<std::string> vec = {"one", "two", "three"};
    EXPECT_EQ(join(vec, ","), "one,two,three");
}

TEST(TrimTest, Basic) {
    EXPECT_EQ(trim(""), "");
    EXPECT_EQ(trim("test"), "test");
    EXPECT_EQ(trim(" test "), "test");
    EXPECT_EQ(trim(" test\n "), "test");
    EXPECT_EQ(trim("\t   test\n "), "test");
}

TEST(StartsWith, Basic) {
    EXPECT_TRUE(starts_with("", ""));
    EXPECT_TRUE(starts_with("abc", ""));
    EXPECT_FALSE(starts_with("", "abc"));
    EXPECT_FALSE(starts_with("ab", "abc"));
    EXPECT_TRUE(starts_with("test", "test"));
    EXPECT_TRUE(starts_with("hello_world", "hello"));
    EXPECT_FALSE(starts_with("hello_world", "world"));
    EXPECT_FALSE(starts_with("abcd", "abce"));
}

}
