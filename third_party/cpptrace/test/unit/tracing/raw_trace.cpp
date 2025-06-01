#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <cpptrace/cpptrace.hpp>

using namespace std::literals;

// Raw trace tests

// This is fickle, however, it's the only way to do it really. I've gotten it reasonably reliable test in practice.
// Sanitizers do interfere.

#ifndef CPPTRACE_SANITIZER_BUILD

// NOTE: MSVC likes creating trampoline-like entries for non-static functions
CPPTRACE_FORCE_NO_INLINE static void raw_trace_basic() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    auto raw_trace = cpptrace::generate_raw_trace();
    // look for within 90 bytes of the start of the function
    ASSERT_GE(raw_trace.frames.size(), 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_basic));
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_basic) + 90);
}

#ifndef _MSC_VER
CPPTRACE_FORCE_NO_INLINE void raw_trace_basic_precise() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    a:
    auto raw_trace = cpptrace::generate_raw_trace();
    b:
    // This is stupid, but without it gcc was optimizing both &&a and &&b to point to the start of the function's body
    volatile auto x = 0;
    if(x) {
        goto* &&a;
    }
    if(x) {
        goto* &&b;
    }
    ASSERT_GE(raw_trace.frames.size(), 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&a));
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&b));
}
#endif

TEST(RawTrace, Basic) {
    raw_trace_basic();
    #ifndef _MSC_VER
    raw_trace_basic_precise();
    #endif
    [[maybe_unused]] volatile int x = 0; // prevent raw_trace_basic_precise() above being a jmp
}



CPPTRACE_FORCE_NO_INLINE static void raw_trace_multi_2(
    cpptrace::frame_ptr parent_low_bound,
    cpptrace::frame_ptr parent_high_bound
) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    auto raw_trace = cpptrace::generate_raw_trace();
    ASSERT_GE(raw_trace.frames.size(), 2);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_multi_2));
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_multi_2) + 90);
    EXPECT_GE(raw_trace.frames[1], parent_low_bound);
    EXPECT_LE(raw_trace.frames[1], parent_high_bound);
}

CPPTRACE_FORCE_NO_INLINE static void raw_trace_multi_1() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    auto raw_trace = cpptrace::generate_raw_trace();
    raw_trace_multi_2(reinterpret_cast<uintptr_t>(raw_trace_multi_1), reinterpret_cast<uintptr_t>(raw_trace_multi_1) + 300);
    ASSERT_GE(raw_trace.frames.size(), 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_multi_1));
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(raw_trace_multi_1) + 90);
}

std::vector<std::pair<cpptrace::frame_ptr, cpptrace::frame_ptr>> parents;

CPPTRACE_FORCE_NO_INLINE void record_parent(uintptr_t low_bound, uintptr_t high_bound) {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    parents.insert(parents.begin(), {low_bound, high_bound});
}

#ifndef _MSC_VER
CPPTRACE_FORCE_NO_INLINE void raw_trace_multi_precise_3() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    a:
    auto raw_trace = cpptrace::generate_raw_trace();
    b:
    volatile auto x = 0;
    if(x) {
        goto* &&a;
    }
    if(x) {
        goto* &&b;
    }
    ASSERT_GE(raw_trace.frames.size(), parents.size() + 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&a)); // this frame
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&b));
    for(size_t i = 0; i < parents.size(); i++) { // parent frames
        EXPECT_GE(raw_trace.frames[i + 1], parents[i].first);
        EXPECT_LE(raw_trace.frames[i + 1], parents[i].second);
    }
}

CPPTRACE_FORCE_NO_INLINE void raw_trace_multi_precise_2() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    a:
    auto raw_trace = cpptrace::generate_raw_trace();
    b:
    volatile auto x = 0;
    if(x) {
        goto* &&a;
    }
    if(x) {
        goto* &&b;
    }
    ASSERT_GE(raw_trace.frames.size(), parents.size() + 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&a)); // this frame
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&b));
    for(size_t i = 0; i < parents.size(); i++) { // parent frames
        EXPECT_GE(raw_trace.frames[i + 1], parents[i].first);
        EXPECT_LE(raw_trace.frames[i + 1], parents[i].second);
    }
    record_parent(reinterpret_cast<uintptr_t>(&&c), reinterpret_cast<uintptr_t>(&&d));
    c:
    raw_trace_multi_precise_3();
    d:
    if(x) {
        goto* &&c;
    }
    if(x) {
        goto* &&d;
    }
}

CPPTRACE_FORCE_NO_INLINE void raw_trace_multi_precise_1() {
    static volatile int lto_guard; lto_guard = lto_guard + 1;
    a:
    auto raw_trace = cpptrace::generate_raw_trace();
    b:
    volatile auto x = 0;
    if(x) {
        goto* &&a;
    }
    if(x) {
        goto* &&b;
    }
    ASSERT_GE(raw_trace.frames.size(), 1);
    EXPECT_GE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&a));
    EXPECT_LE(raw_trace.frames[0], reinterpret_cast<uintptr_t>(&&b));
    record_parent(reinterpret_cast<uintptr_t>(&&c), reinterpret_cast<uintptr_t>(&&d));
    c:
    raw_trace_multi_precise_2();
    d:
    if(x) {
        goto* &&c;
    }
    if(x) {
        goto* &&d;
    }
}
#endif

TEST(RawTrace, MultipleCalls) {
    parents.clear();
    raw_trace_multi_1();
    #ifndef _MSC_VER
    raw_trace_multi_precise_1();
    #endif
}

#endif
