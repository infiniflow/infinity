#include <string_view>
#include <string>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include <cpptrace/cpptrace.hpp>

#include "common.hpp"

using namespace std::literals;


static volatile int truthy = 2;

// NOTE: returning something and then return stacktrace_multi_3(line_numbers) * rand(); is done to prevent TCO even
// under LTO https://github.com/jeremy-rifkin/cpptrace/issues/179#issuecomment-2467302052
CPPTRACE_FORCE_NO_INLINE int stacktrace_traced_object_3(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    if(truthy) { // due to a MSVC warning about unreachable code
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        throw cpptrace::runtime_error("foobar");
    }
    return 2;
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_traced_object_2(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_traced_object_3(line_numbers) * rand();
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_traced_object_1(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_traced_object_2(line_numbers) * rand();
}

TEST(TracedException, Basic) {
    std::vector<int> line_numbers;
    try {
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        stacktrace_traced_object_1(line_numbers);
    } catch(cpptrace::exception& e) {
        EXPECT_EQ(e.message(), "foobar"sv);
        const auto& trace = e.trace();
        ASSERT_GE(trace.frames.size(), 4);
        size_t i = 0;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(i, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "traced_exception.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[i]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_traced_object_3"));
        i++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(i, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "traced_exception.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[i]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_traced_object_2"));
        i++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(i, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "traced_exception.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[i]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_traced_object_1"));
        i++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(i, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "traced_exception.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[i]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("TracedException_Basic_Test::TestBody"));
    }
}
