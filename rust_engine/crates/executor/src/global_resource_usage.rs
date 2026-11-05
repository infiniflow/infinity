//! Process-wide object and raw-memory counters.
//!
//! C++: `GlobalResourceUsage` (`src/main/stats/global_resource_usage.cppm`).
//! The reference uses these counters in debug builds to assert that every
//! allocated object and raw buffer is released. The counters are signed:
//! over-decrementing is observable (the unit test exercises negative values).

use std::sync::atomic::{AtomicI64, Ordering};
use std::sync::OnceLock;

/// C++: `GlobalResourceUsage`.
#[derive(Debug, Default)]
pub struct GlobalResourceUsage {
    object_count: AtomicI64,
    raw_memory_count: AtomicI64,
}

impl GlobalResourceUsage {
    /// C++: `GlobalResourceUsage::Init`.
    pub const fn new() -> Self {
        Self {
            object_count: AtomicI64::new(0),
            raw_memory_count: AtomicI64::new(0),
        }
    }

    /// C++: the process-wide `GlobalResourceUsage` instance.
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<GlobalResourceUsage> = OnceLock::new();
        INSTANCE.get_or_init(GlobalResourceUsage::new)
    }

    /// C++: `GlobalResourceUsage::Init` — resets both counters.
    pub fn init(&self) {
        self.object_count.store(0, Ordering::SeqCst);
        self.raw_memory_count.store(0, Ordering::SeqCst);
    }

    /// C++: `GlobalResourceUsage::UnInit`.
    pub fn uninit(&self) {
        self.init();
    }

    /// C++: `IncrObjectCount(tag)`.
    pub fn incr_object_count(&self, _tag: &str) {
        self.object_count.fetch_add(1, Ordering::SeqCst);
    }

    /// C++: `DecrObjectCount(tag)`.
    pub fn decr_object_count(&self, _tag: &str) {
        self.object_count.fetch_sub(1, Ordering::SeqCst);
    }

    /// C++: `GetObjectCount`.
    pub fn get_object_count(&self) -> i64 {
        self.object_count.load(Ordering::SeqCst)
    }

    /// C++: `IncrRawMemCount(tag)`.
    pub fn incr_raw_mem_count(&self, _tag: &str) {
        self.raw_memory_count.fetch_add(1, Ordering::SeqCst);
    }

    /// C++: `DecrRawMemCount(tag)`.
    pub fn decr_raw_mem_count(&self, _tag: &str) {
        self.raw_memory_count.fetch_sub(1, Ordering::SeqCst);
    }

    /// C++: `GetRawMemoryCount`.
    pub fn get_raw_memory_count(&self) -> i64 {
        self.raw_memory_count.load(Ordering::SeqCst)
    }
}

#[cfg(test)]
mod reference_suite {
    //! Port of `src/unit_test/main/stats/global_resource_usage_ut.cpp`
    //! (`GlobalResourceUsageTest.usage_test`). The C++ body is compiled under
    //! `INFINITY_STATS`; the port keeps the same assertions.

    use super::*;

    /// C++: `GlobalResourceUsageTest.usage_test`.
    #[test]
    fn usage_test() {
        let usage = GlobalResourceUsage::instance();
        usage.init();

        const TAG: &str = "GlobalResourceUsageTest";

        // Object count.
        usage.incr_object_count(TAG);
        assert_eq!(usage.get_object_count(), 1);

        usage.incr_object_count(TAG);
        assert_eq!(usage.get_object_count(), 2);

        usage.decr_object_count(TAG);
        assert_eq!(usage.get_object_count(), 1);

        usage.decr_object_count(TAG);
        assert_eq!(usage.get_object_count(), 0);

        usage.decr_object_count(TAG);
        assert_eq!(usage.get_object_count(), -1);

        usage.incr_object_count(TAG);
        assert_eq!(usage.get_object_count(), 0);

        // Raw memory count.
        usage.incr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), 1);

        usage.incr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), 2);

        usage.decr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), 1);

        usage.decr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), 0);

        usage.decr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), -1);

        usage.incr_raw_mem_count(TAG);
        assert_eq!(usage.get_raw_memory_count(), 0);

        usage.uninit();
    }
}
