#include <algorithm>
#include <string_view>
#include <string>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "common.hpp"

#ifdef TEST_MODULE
import cpptrace;
#else
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#endif

using namespace std::literals;


static volatile int truthy = 2;

// NOTE: returning something and then return stacktrace_multi_3(line_numbers) * rand(); is done to prevent TCO even
// under LTO https://github.com/jeremy-rifkin/cpptrace/issues/179#issuecomment-2467302052
CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_try_catch_3(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    if(truthy) { // due to a MSVC warning about unreachable code
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        throw std::runtime_error("foobar");
    }
    return 2;
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_try_catch_2(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_try_catch_3(line_numbers) * rand();
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_try_catch_1(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_try_catch_2(line_numbers) * rand();
}

TEST(FromCurrentTryCatch, Basic) {
    std::vector<int> line_numbers;
    cpptrace::try_catch(
        [&] {
            line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
            static volatile int tco_guard = stacktrace_from_current_try_catch_1(line_numbers);
            (void)tco_guard;
        },
        [&] (const std::runtime_error& e) {
            EXPECT_FALSE(cpptrace::current_exception_was_rethrown());
            EXPECT_EQ(e.what(), "foobar"sv);
            const auto& trace = cpptrace::from_current_exception();
            ASSERT_GE(trace.frames.size(), 4);
            auto it = std::find_if(
                trace.frames.begin(),
                trace.frames.end(),
                [](const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("stacktrace_from_current_try_catch_3") != std::string::npos;
                }
            );
            ASSERT_NE(it, trace.frames.end());
            size_t i = static_cast<size_t>(it - trace.frames.begin());
            int j = 0;
            ASSERT_LT(i, trace.frames.size());
            ASSERT_LT(j, line_numbers.size());
            EXPECT_FILE(trace.frames[i].filename, "from_current_try_catch.cpp");
            EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
            EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_try_catch_3"));
            i++;
            j++;
            ASSERT_LT(i, trace.frames.size());
            ASSERT_LT(j, line_numbers.size());
            EXPECT_FILE(trace.frames[i].filename, "from_current_try_catch.cpp");
            EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
            EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_try_catch_2"));
            i++;
            j++;
            ASSERT_LT(i, trace.frames.size());
            ASSERT_LT(j, line_numbers.size());
            EXPECT_FILE(trace.frames[i].filename, "from_current_try_catch.cpp");
            EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
            EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_try_catch_1"));
            i++;
            j++;
            ASSERT_LT(i, trace.frames.size());
            ASSERT_LT(j, line_numbers.size());
            EXPECT_FILE(trace.frames[i].filename, "from_current_try_catch.cpp");
            EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        }
    );
}

TEST(FromCurrentTryCatch, CorrectHandler) {
    std::vector<int> line_numbers;
    cpptrace::try_catch(
        [&] {
            cpptrace::try_catch(
                [&] {
                    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
                    stacktrace_from_current_try_catch_1(line_numbers);
                },
                [&] (const std::logic_error&) {
                    FAIL();
                }
            );
        },
        [&] (const std::logic_error&) {
            FAIL();
        },
        [&] (const std::runtime_error& e) {
            EXPECT_EQ(e.what(), "foobar"sv);
            const auto& trace = cpptrace::from_current_exception();
            auto it = std::find_if(
                trace.frames.begin(),
                trace.frames.end(),
                [](const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("stacktrace_from_current_try_catch_3") != std::string::npos;
                }
            );
            EXPECT_NE(it, trace.frames.end());
            it = std::find_if(
                trace.frames.begin(),
                trace.frames.end(),
                [](const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("FromCurrentTryCatch_CorrectHandler_Test::TestBody") != std::string::npos;
                }
            );
            EXPECT_NE(it, trace.frames.end());
        }
    );
}

TEST(FromCurrentTryCatch, RawTrace) {
    std::vector<int> line_numbers;
    cpptrace::try_catch(
        [&] {
            line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
            static volatile int tco_guard = stacktrace_from_current_try_catch_1(line_numbers);
            (void)tco_guard;
        },
        [&] (const std::runtime_error& e) {
            EXPECT_EQ(e.what(), "foobar"sv);
            const auto& raw_trace = cpptrace::raw_trace_from_current_exception();
            auto trace = raw_trace.resolve();
            auto it = std::find_if(
                trace.frames.begin(),
                trace.frames.end(),
                [](const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("stacktrace_from_current_try_catch_3") != std::string::npos;
                }
            );
            EXPECT_NE(it, trace.frames.end());
            it = std::find_if(
                trace.frames.begin(),
                trace.frames.end(),
                [](const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("FromCurrentTryCatch_RawTrace_Test::TestBody") != std::string::npos;
                }
            );
            EXPECT_NE(it, trace.frames.end());
        }
    );
}
