// https://github.com/jeremy-rifkin/libassert/issues/43

#include <cpptrace/cpptrace.hpp>

#include <gtest/gtest.h>

#include <exception>

TEST(TraceTest, trace_test) {
    ASSERT_THROW((cpptrace::generate_trace().print(), false), std::logic_error);
}
