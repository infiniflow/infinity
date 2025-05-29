#include <algorithm>
#include <exception>
#include <string_view>
#include <string>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "common.hpp"

#ifdef TEST_MODULE
import cpptrace;

#include <cpptrace/from_current.hpp>
#else
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#endif

using namespace std::literals;

static volatile int truthy = 2;

// NOTE: returning something and then return stacktrace_multi_3(line_numbers) * rand(); is done to prevent TCO even
// under LTO https://github.com/jeremy-rifkin/cpptrace/issues/179#issuecomment-2467302052
CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_rethrow_3(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    if(truthy) { // due to a MSVC warning about unreachable code
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        throw std::runtime_error("foobar");
    }
    return 2;
}

CPPTRACE_FORCE_NO_INLINE
int stacktrace_from_current_rethrow_2(std::vector<int>& line_numbers, std::vector<int>& rethrow_line_numbers) {
    int ret;
    CPPTRACE_TRY {
        static volatile int lto_guard; lto_guard = lto_guard + 1;
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        ret = stacktrace_from_current_rethrow_3(line_numbers) * rand();
    } CPPTRACE_CATCH(const std::exception&) {
        rethrow_line_numbers.insert(rethrow_line_numbers.begin(), __LINE__ + 1);
        cpptrace::rethrow();
    }
    return ret;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702) // unreachable code
#endif
CPPTRACE_FORCE_NO_INLINE
int stacktrace_from_current_rethrow_1(std::vector<int>& line_numbers, std::vector<int>& rethrow_line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    rethrow_line_numbers.insert(rethrow_line_numbers.begin(), __LINE__ + 2);
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_rethrow_2(line_numbers, rethrow_line_numbers) * rand();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void clean_trace(cpptrace::stacktrace& trace, std::vector<cpptrace::stacktrace_frame>::iterator it) {
    // because stacktrace_from_current_rethrow_2 has a try/catch which uses lambdas under msvc, we need to filter
    // all but the first frame mentioning stacktrace_from_current_rethrow_2.
    auto frame_2_it = std::find_if(
        it,
        trace.frames.end(),
        [](const cpptrace::stacktrace_frame& frame) {
            return frame.symbol.find("stacktrace_from_current_rethrow_2") != std::string::npos;
        }
    );
    ASSERT_NE(frame_2_it, trace.frames.end());
    frame_2_it++;
    auto remove_it = std::remove_if(
        frame_2_it,
        trace.frames.end(),
        [](const cpptrace::stacktrace_frame& frame) {
            return frame.symbol.find("stacktrace_from_current_rethrow_2") != std::string::npos;
        }
    );
    if(remove_it != trace.frames.end()) {
        trace.frames.erase(remove_it);
    }
}

TEST(Rethrow, RethrowPreservesTrace) {
    std::vector<int> line_numbers;
    std::vector<int> rethrow_line_numbers;
    CPPTRACE_TRY {
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        static volatile int tco_guard = stacktrace_from_current_rethrow_1(line_numbers, rethrow_line_numbers);
        (void)tco_guard;
    } CPPTRACE_CATCH(const std::runtime_error& e) {
        EXPECT_TRUE(cpptrace::current_exception_was_rethrown());
        EXPECT_EQ(e.what(), "foobar"sv);
        auto trace = cpptrace::from_current_exception();
        ASSERT_GE(trace.frames.size(), 4);
        auto it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_rethrow_3") != std::string::npos;
            }
        );
        ASSERT_NE(it, trace.frames.end()) << trace;
        clean_trace(trace, it);
        size_t i = static_cast<size_t>(it - trace.frames.begin());
        int j = 0;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_rethrow_3"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        #ifndef _MSC_VER
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        #endif
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_rethrow_2"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_rethrow_1"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("Rethrow_RethrowPreservesTrace_Test::TestBody"));
    }
}

TEST(Rethrow, RethrowTraceCorrect) {
    std::vector<int> line_numbers;
    std::vector<int> rethrow_line_numbers;
    CPPTRACE_TRY {
        rethrow_line_numbers.insert(rethrow_line_numbers.begin(), __LINE__ + 2);
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        static volatile int tco_guard = stacktrace_from_current_rethrow_1(line_numbers, rethrow_line_numbers);
        (void)tco_guard;
    } CPPTRACE_CATCH(const std::runtime_error& e) {
        EXPECT_TRUE(cpptrace::current_exception_was_rethrown());
        EXPECT_EQ(e.what(), "foobar"sv);
        auto rethrow_trace = cpptrace::from_current_exception_rethrow();
        ASSERT_GE(rethrow_trace.frames.size(), 4);
        // reverse to get the last one matching instead of "`stacktrace_from_current_rethrow_2'::`1'::catch$4()" on msvc
        auto rit = std::find_if(
            rethrow_trace.frames.rbegin(),
            rethrow_trace.frames.rend(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_rethrow_2") != std::string::npos
                    && frame.symbol.find("::catch") == std::string::npos;
            }
        );
        ASSERT_NE(rit, rethrow_trace.frames.rend()) << rethrow_trace;
        size_t i = static_cast<size_t>(&*rit - &*rethrow_trace.frames.begin());
        auto it = rethrow_trace.frames.begin() + i;
        clean_trace(rethrow_trace, it);
        int j = 0;
        ASSERT_LT(i, rethrow_trace.frames.size());
        ASSERT_LT(j, rethrow_line_numbers.size());
        EXPECT_FILE(rethrow_trace.frames[i].filename, "rethrow.cpp");
        #ifndef _MSC_VER
        EXPECT_LINE(rethrow_trace.frames[i].line.value(), rethrow_line_numbers[j]);
        #endif
        EXPECT_THAT(rethrow_trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_rethrow_2"));
        i++;
        j++;
        ASSERT_LT(i, rethrow_trace.frames.size());
        ASSERT_LT(j, rethrow_line_numbers.size());
        EXPECT_FILE(rethrow_trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(rethrow_trace.frames[i].line.value(), rethrow_line_numbers[j]);
        EXPECT_THAT(rethrow_trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_rethrow_1"));
        i++;
        j++;
        ASSERT_LT(i, rethrow_trace.frames.size());
        ASSERT_LT(j, rethrow_line_numbers.size());
        EXPECT_FILE(rethrow_trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(rethrow_trace.frames[i].line.value(), rethrow_line_numbers[j]);
        EXPECT_THAT(rethrow_trace.frames[i].symbol, testing::HasSubstr("Rethrow_RethrowTraceCorrect_Test::TestBody"));
    }
}

// NOTE: returning something and then return stacktrace_multi_3(line_numbers) * rand(); is done to prevent TCO even
// under LTO https://github.com/jeremy-rifkin/cpptrace/issues/179#issuecomment-2467302052
CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_basic_3(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    if(truthy) { // due to a MSVC warning about unreachable code
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        throw std::runtime_error("foobar");
    }
    return 2;
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_basic_2(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_basic_3(line_numbers) * rand();
}

CPPTRACE_FORCE_NO_INLINE int stacktrace_from_current_basic_1(std::vector<int>& line_numbers) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
    return stacktrace_from_current_basic_2(line_numbers) * rand();
}

TEST(Rethrow, RethrowDoesntInterfereWithSubsequentTraces) {
    std::vector<int> line_numbers;
    std::vector<int> rethrow_line_numbers;
    CPPTRACE_TRY {
        // do rethrow path so a rethrow happens
        try {
            static volatile int tco_guard = stacktrace_from_current_rethrow_1(line_numbers, rethrow_line_numbers);
            (void)tco_guard;
        } catch(...) {}
        // now do real test
        line_numbers.clear();
        line_numbers.insert(line_numbers.begin(), __LINE__ + 1);
        stacktrace_from_current_basic_1(line_numbers);
    } CPPTRACE_CATCH(const std::runtime_error& e) {
        EXPECT_FALSE(cpptrace::current_exception_was_rethrown());
        EXPECT_EQ(e.what(), "foobar"sv);
        auto trace = cpptrace::from_current_exception();
        ASSERT_GE(trace.frames.size(), 4);
        auto it = std::find_if(
            trace.frames.begin(),
            trace.frames.end(),
            [](const cpptrace::stacktrace_frame& frame) {
                return frame.symbol.find("stacktrace_from_current_basic_3") != std::string::npos;
            }
        );
        ASSERT_NE(it, trace.frames.end()) << trace;
        size_t i = static_cast<size_t>(it - trace.frames.begin());
        int j = 0;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_basic_3"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_basic_2"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        EXPECT_THAT(trace.frames[i].symbol, testing::HasSubstr("stacktrace_from_current_basic_1"));
        i++;
        j++;
        ASSERT_LT(i, trace.frames.size());
        ASSERT_LT(j, line_numbers.size());
        EXPECT_FILE(trace.frames[i].filename, "rethrow.cpp");
        #ifndef _MSC_VER
        EXPECT_LINE(trace.frames[i].line.value(), line_numbers[j]);
        #endif
        EXPECT_THAT(
            trace.frames[i].symbol,
            testing::HasSubstr("Rethrow_RethrowDoesntInterfereWithSubsequentTraces_Test::TestBody")
        );
    }
}
