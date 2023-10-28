module;
#include <atomic>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:atomic;
export namespace std {
    using std::memory_order;
    using std::memory_order_relaxed;
    using std::memory_order_consume;
    using std::memory_order_acquire;
    using std::memory_order_release;
    using std::memory_order_acq_rel;
    using std::memory_order_seq_cst;
    using std::atomic;
}
