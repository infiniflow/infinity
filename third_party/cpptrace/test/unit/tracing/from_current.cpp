#include <algorithm>
#include <string_view>
#include <string>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "common.hpp"
#include "utils/utils.hpp"

#ifdef TEST_MODULE
import cpptrace;

#include <cpptrace/from_current_macros.hpp>
#else
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#endif

using namespace std::literals;


static volatile int truthy = 2;

// NOTE: returning something and then return stacktrace_multi_3(line_numbers) * rand(); is done to prevent TCO even
// under LTO https://github.com/jeremy-rifkin/cpptrace/issues/179#issuecomment-2467302052
CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_3(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    if(truthy) { // due to a MSVC warning about unreachable code
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        throw std::runtime_error("foobar");
    }
    return 2;
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_2(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_3(line_numbers) * rand();
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_1(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_2(line_numbers) * rand();
}

TEST(FromCurrent, Basic) {
    std::vector<int> line_numbers;
    bool does_enter_catch = false;
    auto guard = cpptrace::detail::scope_exit([&] {
        EXPECT_TRUE(does_enter_catch);
    });
    CPPTRACE_TRY {
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        static volatile int tco_guard = stacktrace_from_current_1(line_numbers);
        (void)tco_guard;
    } CPPTRACE_CATCH(const std::runtime_error& e) {
        does_enter_catch = true;
        EXPECT_FALSE(cpptrace::current_exception_was_rethrown());
        EXPECT_EQ(e.what(), "foobar"sv);
        const auto& trace = cpptrace::from_current_exception();
        ASSERT_GE(trace.frames.size(), 4);
        auto it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_3") != std::string::npos;
            }
        );
        ASSERT_NE(it, trace.frames.end()) << trace;
        size_t i = static_cast<size_t>(it - trace.frames.begin());
        int j = 0;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "from_current.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_3"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "from_current.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_2"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "from_current.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_1"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "from_current.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("FromCurrent_Basic_Test::TestBody"));
    }
}

TEST(FromCurrent, CorrectHandler) {
    std::vector<int> line_numbers;
    bool wrong_handler = false;
    CPPTRACE_TRY {
        CPPTRACE_TRY {
            line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
            stacktrace_from_current_1(line_numbers);
        } CPPTRACE_CATCH(const std::logic_error&) {
            wrong_handler = true;
        }
    } CPPTRACE_CATCH(const std::exception& e) {
        EXPECT_EQ(e.what(), "foobar"sv);
        const auto& trace = cpptrace::from_current_exception();
        auto it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_3") != std::string::npos;
            }
        );
        EXPECT_NE(it, trace.frames.end());
        it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("FromCurrent_CorrectHandler_Test::TestBody") != std::string::npos;
            }
        );
        EXPECT_NE(it, trace.frames.end());
    }
    if(wrong_handler) {
        FAIL();
    }
}

TEST(FromCurrent, RawTrace) {
    std::vector<int> line_numbers;
    CPPTRACE_TRY {
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        static volatile int tco_guard = stacktrace_from_current_1(line_numbers);
        (void)tco_guard;
    } CPPTRACE_CATCH(const std::exception& e) {
        EXPECT_EQ(e.what(), "foobar"sv);
        const auto& raw_trace = cpptrace::raw_trace_from_current_exception();
        auto trace = raw_trace.resolve();
        auto it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_3") != std::string::npos;
            }
        );
        EXPECT_NE(it, trace.frames.end());
        it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("FromCurrent_RawTrace_Test::TestBody") != std::string::npos;
            }
        );
        EXPECT_NE(it, trace.frames.end());
    }
}

TEST(FromCurrent, NonThrowingPath) {
    bool does_enter_catch = false;
    bool does_reach_end = false;
    auto guard = cpptrace::detail::scope_exit([&] {
        EXPECT_FALSE(does_enter_catch);
        EXPECT_TRUE(does_reach_end);
    });
    CPPTRACE_TRY {
        // pass
    } CPPTRACE_CATCH(const std::runtime_error&) {
        does_enter_catch = true;
    }
    does_reach_end = true;
}

#ifdef _MSC_VER

CPPTRACE_FORCE_NO_INLINE
int my_div_function(int x, int y) {
    return x / y;
}

int divide_zero_filter(int code) {
    if(code == STATUS_INTEGER_DIVIDE_BY_ZERO || code == EXCEPTION_FLT_DIVIDE_BY_ZERO) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

TEST(FromCurrent, SEHBasic) {
    bool does_enter_catch = false;
    auto guard = cpptrace::detail::scope_exit([&] {
        EXPECT_TRUE(does_enter_catch);
    });
    [&] () {
        CPPTRACE_SEH_TRY {
            [&] () {
                volatile auto res = my_div_function(10, 0);
                (void)res;
            } ();
        } CPPTRACE_SEH_EXCEPT(divide_zero_filter(GetExceptionCode())) {
            [&] () {
                does_enter_catch = true;
                EXPECT_FALSE(cpptrace::current_exception_was_rethrown());
                const auto& trace = cpptrace::from_current_exception();
                auto it = std::find_if(
                    trace.frames.begin(),
                    trace.frames.end(),
                    [](const cpptrace::stacktrace_frame& frame) {
                        return frame.symbol.find("my_div_function") != std::string::npos;
                    }
                );
                ASSERT_NE(it, trace.frames.end()) << trace;
                size_t i = static_cast<size_t>(it - trace.frames.begin());
                EXPECT_FILE(trace.frames[i].filename, "from_current.cpp");
            } ();
        }
    } ();
    EXPECT_TRUE(does_enter_catch);
}

TEST(FromCurrent, SEHCorrectHandler) {
    bool does_enter_catch = false;
    auto guard = cpptrace::detail::scope_exit([&] {
        EXPECT_TRUE(does_enter_catch);
    });
    [&] () {
        CPPTRACE_SEH_TRY {
            [&] () {
                CPPTRACE_SEH_TRY {
                    [&] () {
                        volatile auto res = my_div_function(10, 0);
                        (void)res;
                    } ();
                } CPPTRACE_SEH_EXCEPT(EXCEPTION_CONTINUE_SEARCH) {
                    [&] () {
                        FAIL();
                    } ();
                }
            } ();
        } CPPTRACE_SEH_EXCEPT(divide_zero_filter(GetExceptionCode())) {
            does_enter_catch = true;
        }
    } ();
    EXPECT_TRUE(does_enter_catch);
}

#endif
