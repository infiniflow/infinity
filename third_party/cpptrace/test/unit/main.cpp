#include <gtest/gtest.h>

#ifdef TEST_MODULE
import cpptrace;
#else
#include <cpptrace/cpptrace.hpp>
#endif

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    cpptrace::absorb_trace_exceptions(false);
    cpptrace::use_default_stderr_logger();
    return RUN_ALL_TESTS();
}
