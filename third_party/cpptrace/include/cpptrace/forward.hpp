#ifndef CPPTRACE_FORWARD_HPP
#define CPPTRACE_FORWARD_HPP

#include <cstdint>

namespace cpptrace {
    // Some type sufficient for an instruction pointer, currently always an alias to std::uintptr_t
    using frame_ptr = std::uintptr_t;

    struct raw_trace;
    struct object_trace;
    struct stacktrace;

    struct object_frame;
    struct stacktrace_frame;
    struct safe_object_frame;
}

#endif
