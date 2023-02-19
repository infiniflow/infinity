#include "common/memory/arena.h"
#include "common/memory/stream_arena.h"
#include "common/memory/memory_tracker.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
#include <iostream>

#include <gtest/gtest.h>


namespace infinity {

const int32_t num_threads = 16;
const int32_t allocs_per_thread = 10000;
const int32_t alloc_size = 4;

using std::shared_ptr;
using std::string;
using std::thread;
using std::vector;

struct Slice{
    Slice():data_(reinterpret_cast<const uint8_t *>("")), size_(0) {}
    Slice(const uint8_t* d, size_t n) : data_(d), size_(n) {}

    const uint8_t* data() const { return data_; }
    size_t size() const { return size_; }

    const uint8_t* data_;
    size_t size_;
};

// From the "arena" allocate number of bytes required to copy the "to_write" buffer
// and add the allocated buffer to the output "ptrs" vector.
template<class ArenaType>
static void AllocateBytesAndWrite(ArenaType* arena, const Slice& to_write, vector<void*>* ptrs) {
    void *allocated_bytes = arena->AllocateBytes(to_write.size());
    ASSERT_NE(nullptr, allocated_bytes);
    memcpy(allocated_bytes, to_write.data(), to_write.size());
    ptrs->push_back(allocated_bytes);
}

template<class ArenaType>
static void AllocateAlignedBytesAndWrite(ArenaType* arena, const size_t alignment,
        const Slice& to_write, vector<void*>* ptrs) {
    // To test alignment we allocate random number of bytes within bounds
    // but write to fixed number of bytes "to_write".
    srand((unsigned) time(NULL));
    // Get a random number
    unsigned random = rand();
    random %= (128 - alloc_size + 1);
    size_t num_bytes = alloc_size + random;
    ASSERT_LE(to_write.size(), num_bytes);
    void* allocated_bytes = arena->AllocateBytesAligned(num_bytes, alignment);
    ASSERT_NE(nullptr, allocated_bytes);
    ASSERT_EQ(0, reinterpret_cast<uintptr_t>(allocated_bytes) % alignment) <<
            "failed to align on " << alignment << "b boundary: " << allocated_bytes;
    memcpy(allocated_bytes, to_write.data(), to_write.size());
    ptrs->push_back(allocated_bytes);
}

// Thread callback function used by bunch of test cases below.
template<class ArenaType, bool InvokeAligned = false>
static void AllocateAndTestThreadFunc(ArenaType *arena, uint8_t thread_index) {
    vector<void *> ptrs;
    ptrs.reserve(allocs_per_thread);

    uint8_t buf[alloc_size];
    memset(buf, thread_index, alloc_size);
    Slice data(buf, alloc_size);

    for (int i = 0; i < allocs_per_thread; i++) {
        if (InvokeAligned) {
            // Test alignment up to 64 bytes.
            const size_t alignment = 1 << (i % 7);
            AllocateAlignedBytesAndWrite(arena, alignment, data, &ptrs);
        } else {
            AllocateBytesAndWrite(arena, data, &ptrs);
        }
    }
}

// Non-templated function to forward to above -- simplifies thread creation
static void AllocateAndTestTSArenaFunc(ThreadSafeArena *arena, uint8_t thread_index) {
    AllocateAndTestThreadFunc(arena, thread_index);
}

template<typename ArenaType>
static void TestArenaAlignmentHelper() {
    for (size_t initial_size = 16; initial_size <= 2048; initial_size <<= 1) {
        ArenaType arena(initial_size);
        static constexpr bool kIsMultiThreaded = std::is_same<ThreadSafeArena, ArenaType>::value;
        if (kIsMultiThreaded) {
            vector<thread> threads;
            threads.reserve(num_threads);
            for (auto i = 0; i < num_threads; i++) {
                threads.emplace_back(
                    AllocateAndTestThreadFunc<ArenaType, true>, &arena, i);
            }
            for (thread& thr : threads) {
                thr.join();
            }
        } else {
            // Invoke the helper method on the same thread avoiding separate single
            // thread creation/join.
            AllocateAndTestThreadFunc<ArenaType, true >(&arena, 0);
        }
    }
}

TEST(TestArena, TestSingleThreaded) {
    Arena arena(128);
    AllocateAndTestThreadFunc(&arena, 0);
}

TEST(TestArena, TestMultiThreaded) {
    ThreadSafeArena arena(1024);

    vector<thread> threads;
    threads.reserve(num_threads);
    for (auto i = 0; i < num_threads; i++) {
        threads.emplace_back(AllocateAndTestTSArenaFunc, &arena, i);
    }

    for (thread& thr : threads) {
        thr.join();
    }
}

TEST(TestArena, TestAlignmentThreadSafe) {
    TestArenaAlignmentHelper<ThreadSafeArena>();
}

TEST(TestArena, TestAlignmentNotThreadSafe) {
    TestArenaAlignmentHelper<Arena>();
}

TEST(TestArena, TestAlignmentSmallArena) {
    // Start with small initial size and allocate bytes more than the size of the current
    // component to trigger fallback code path in Arena. Moreover allocate number of bytes
    // with alignment such that "aligned_size" exceeds "next_component_size".
    Arena arena(16);
    constexpr size_t alignment = 32;
    void *ret = arena.AllocateBytesAligned(33, alignment);
    ASSERT_NE(nullptr, ret);
    ASSERT_EQ(0, reinterpret_cast<uintptr_t>(ret) % alignment) <<
            "failed to align on " << alignment << "b boundary: " << ret;
}


TEST(TestArena, TestObjectAlignment) {
    struct MyStruct {
        int64_t v;
    };
    Arena a(256);
    // Allocate a junk byte to ensure that the next allocation isn't "accidentally" aligned.
    a.AllocateBytes(1);
    void* v = a.NewObject<MyStruct>();
    ASSERT_EQ(reinterpret_cast<uintptr_t>(v) % alignof(MyStruct), 0);
}


// MemTrackers update their ancestors when consuming and releasing memory to compute
// usage totals. However, the lifetimes of parent and child trackers can be different.
// Validate that child trackers can still correctly update their parent stats even when
// the parents go out of scope.

TEST(TestArena, TestMemoryTrackerParentReferences) {
    // Set up a parent and child MemoryTracker.
    const string parent_id = "parent-id";
    const string child_id = "child-id";
    shared_ptr<MemoryTracker> child_tracker;
    {
        shared_ptr<MemoryTracker> parent_tracker = MemoryTracker::Create(1024);
        child_tracker = parent_tracker->AddChild();
        // Parent falls out of scope here. Should still be owned by the child.
    }
    shared_ptr<MemoryTrackingBufferAllocator> allocator(
        new MemoryTrackingBufferAllocator(HeapBufferAllocator::Get(), child_tracker));
    MemoryTrackingArena arena(256, allocator);

    // Try some child operations.
    ASSERT_EQ(256, child_tracker->Consumption());
    void *allocated = arena.AllocateBytes(256);
    ASSERT_TRUE(allocated);
    ASSERT_EQ(256, child_tracker->Consumption());
    allocated = arena.AllocateBytes(256);
    ASSERT_TRUE(allocated);
    ASSERT_EQ(768, child_tracker->Consumption());
}

TEST(TestArena, TestMemoryTrackingDontEnforce) {
    shared_ptr<MemoryTracker> mem_tracker = MemoryTracker::Create(1024);
    shared_ptr<MemoryTrackingBufferAllocator> allocator(
        new MemoryTrackingBufferAllocator(HeapBufferAllocator::Get(), mem_tracker));
    MemoryTrackingArena arena(256, allocator);
    ASSERT_EQ(256, mem_tracker->Consumption());
    void *allocated = arena.AllocateBytes(256);
    ASSERT_TRUE(allocated);
    ASSERT_EQ(256, mem_tracker->Consumption());
    allocated = arena.AllocateBytes(256);
    ASSERT_TRUE(allocated);
    ASSERT_EQ(768, mem_tracker->Consumption());
}

TEST(TestArena, TestMemoryTrackingEnforced) {
    shared_ptr<MemoryTracker> mem_tracker = MemoryTracker::Create(1024);
    shared_ptr<MemoryTrackingBufferAllocator> allocator(
        new MemoryTrackingBufferAllocator(HeapBufferAllocator::Get(), mem_tracker,
                                          // enforce limit
                                          true));
    MemoryTrackingArena arena(256, allocator);
    ASSERT_EQ(256, mem_tracker->Consumption());
    void *allocated = arena.AllocateBytes(256);
    ASSERT_TRUE(allocated);
    ASSERT_EQ(256, mem_tracker->Consumption());
    allocated = arena.AllocateBytes(1024);
    ASSERT_FALSE(allocated);
    ASSERT_EQ(256, mem_tracker->Consumption());
}

TEST(TestArena, TestSTLAllocator) {
    Arena a(256);
    typedef vector<int, ArenaAllocator<int, false> > ArenaVector;
    ArenaAllocator<int, false> alloc(&a);
    ArenaVector v(alloc);
    for (int i = 0; i < 10000; i++) {
        v.push_back(i);
    }
    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(i, v[i]);
    }
}

template<class T>
using pmr_vector = std::vector<T, std::pmr::polymorphic_allocator<T>>;

TEST(TestArena, TestStreamArenaSTLAllocator) {
    shared_ptr<MemoryTracker> mem_tracker = MemoryTracker::Create(1024);
    StreamArenaMemoryResource memoryResource(mem_tracker);

    pmr_vector<int> vec(&memoryResource);
    for (int i = 0; i < 10000000; i++) {
        vec.push_back(i);
    }
    for (int i = 0; i < 10000000; i++) {
        ASSERT_EQ(i, vec[i]);
    }
}


} // namespace infinity
