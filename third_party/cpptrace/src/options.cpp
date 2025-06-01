#include <cpptrace/basic.hpp>

#include "options.hpp"

#include <atomic>

namespace cpptrace {
    namespace detail {
        std::atomic_bool absorb_trace_exceptions(true); // NOSONAR
        std::atomic_bool resolve_inlined_calls(true); // NOSONAR
        std::atomic<cache_mode> current_cache_mode(cache_mode::prioritize_speed); // NOSONAR
    }

    void absorb_trace_exceptions(bool absorb) {
        detail::absorb_trace_exceptions = absorb;
    }

     void enable_inlined_call_resolution(bool enable) {
        detail::resolve_inlined_calls = enable;
    }

    namespace experimental {
        void set_cache_mode(cache_mode mode) {
            detail::current_cache_mode = mode;
        }
    }

    namespace detail {
        bool should_absorb_trace_exceptions() {
            return absorb_trace_exceptions;
        }

        bool should_resolve_inlined_calls() {
            return resolve_inlined_calls;
        }

        cache_mode get_cache_mode() {
            return current_cache_mode;
        }
    }
}
