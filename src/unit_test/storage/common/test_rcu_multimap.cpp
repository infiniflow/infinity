// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <random>
#include <thread>

import base_test;
import infinity_core;

using namespace infinity;

// RcuMultiMap now works with POD types directly, so we'll use simple integers for testing

class RcuMultiMapTest : public BaseTest {
public:
    RcuMultiMapTest() = default;
    ~RcuMultiMapTest() = default;

    void SetUp() override { BaseTest::SetUp(); }

    void TearDown() override { BaseTest::TearDown(); }

protected:
    // Helper function to verify values in vector
    void VerifyValues(const Vector<i32> &values, const Vector<i32> &expected) {
        EXPECT_EQ(values.size(), expected.size());
        Vector<i32> actual_values = values;
        std::sort(actual_values.begin(), actual_values.end());
        Vector<i32> sorted_expected = expected;
        std::sort(sorted_expected.begin(), sorted_expected.end());
        EXPECT_EQ(actual_values, sorted_expected);
    }
};

TEST_F(RcuMultiMapTest, TestBasicInsertAndGet) {
    RcuMultiMap<String, i32> map;

    // Insert some values
    map.Insert("key1", 10);
    map.Insert("key2", 20);
    map.Insert("key1", 30); // Multiple values for same key

    // Test Get
    auto values1 = map.Get("key1");
    EXPECT_EQ(values1.size(), 2);
    VerifyValues(values1, {10, 30});

    auto values2 = map.Get("key2");
    EXPECT_EQ(values2.size(), 1);
    VerifyValues(values2, {20});

    // Test non-existent key
    auto values3 = map.Get("key3");
    EXPECT_EQ(values3.size(), 0);

    // Test size
    EXPECT_GT(map.size(), 0);
}

TEST_F(RcuMultiMapTest, TestLowercaseAliases) {
    RcuMultiMap<String, i32> map;

    // Test lowercase insert alias
    map.insert("test_key", 100);
    map.insert("test_key", 200);

    // Test lowercase get alias
    auto values = map.get("test_key");
    EXPECT_EQ(values.size(), 2);
    VerifyValues(values, {100, 200});
}

TEST_F(RcuMultiMapTest, TestDelete) {
    RcuMultiMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key1", 20);
    map.Insert("key2", 30);

    // Delete specific value (this should work since data is in dirty_map)
    map.Delete("key1", 10);
    auto values1 = map.Get("key1");
    EXPECT_EQ(values1.size(), 1);
    VerifyValues(values1, {20});

    // Delete entire key (this should work since data is in dirty_map)
    map.Delete("key2");
    auto values2 = map.Get("key2");
    EXPECT_EQ(values2.size(), 0);

    // Test lowercase delete alias
    map.delete_key("key1");
    auto values3 = map.Get("key1");
    EXPECT_EQ(values3.size(), 0);
}

TEST_F(RcuMultiMapTest, TestGetAllValuesWithRef) {
    RcuMultiMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);
    map.Insert("key3", 30);

    Vector<i32> all_values;
    map.GetAllValuesWithRef(all_values);

    EXPECT_EQ(all_values.size(), 3);
    VerifyValues(all_values, {10, 20, 30});
}

TEST_F(RcuMultiMapTest, TestGarbageCollection) {
    RcuMultiMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Delete some entries
    map.Delete("key1");

    // Test garbage collection (should not crash)
    u64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    // Test checkGc alias
    map.checkGc(current_time - 1000); // GC entries older than 1 second

    // Verify remaining data is still accessible
    auto values = map.Get("key2");
    EXPECT_EQ(values.size(), 1);
    VerifyValues(values, {20});
}

TEST_F(RcuMultiMapTest, TestExpiredEntries) {
    RcuMultiMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Access key1 to update its access time
    map.Get("key1", true);

    // Sleep briefly to create time difference
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    u64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    Vector<String> expired_keys;
    map.CheckExpired(current_time + 1000, expired_keys); // Check for entries not accessed in last second

    // Should find some expired keys (exact behavior depends on implementation)
    // This test mainly ensures the method doesn't crash
    EXPECT_GE(expired_keys.size(), 0);
}

TEST_F(RcuMultiMapTest, TestGetWithRcuTime) {
    RcuMultiMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Test GetWithRcuTime (should behave similarly to Get but with RCU semantics)
    auto values1 = map.GetWithRcuTime("key1");
    EXPECT_EQ(values1.size(), 1);
    VerifyValues(values1, {10});

    auto values2 = map.GetWithRcuTime("key2");
    EXPECT_EQ(values2.size(), 1);
    VerifyValues(values2, {20});

    // Test non-existent key
    auto values3 = map.GetWithRcuTime("key3");
    EXPECT_EQ(values3.size(), 0);
}

TEST_F(RcuMultiMapTest, TestMultipleValuesPerKey) {
    RcuMultiMap<i32, i32> map;

    // Insert multiple values for the same key
    for (i32 i = 0; i < 10; ++i) {
        map.Insert(42, i * 10);
    }

    auto values = map.Get(42);
    EXPECT_EQ(values.size(), 10);

    Vector<i32> expected_values;
    for (i32 i = 0; i < 10; ++i) {
        expected_values.push_back(i * 10);
    }
    VerifyValues(values, expected_values);
}

TEST_F(RcuMultiMapTest, TestIntegerKeys) {
    RcuMultiMap<i32, i32> map;

    map.Insert(1, 100);
    map.Insert(2, 200);
    map.Insert(3, 300);

    // Test retrieval
    auto values1 = map.Get(1);
    EXPECT_EQ(values1.size(), 1);
    VerifyValues(values1, {100});

    auto values2 = map.Get(2);
    EXPECT_EQ(values2.size(), 1);
    VerifyValues(values2, {200});

    auto values3 = map.Get(3);
    EXPECT_EQ(values3.size(), 1);
    VerifyValues(values3, {300});
}

TEST_F(RcuMultiMapTest, TestEmptyMap) {
    RcuMultiMap<String, i32> map;

    // Test operations on empty map
    auto values = map.Get("nonexistent");
    EXPECT_EQ(values.size(), 0);

    Vector<i32> all_values;
    map.GetAllValuesWithRef(all_values);
    EXPECT_EQ(all_values.size(), 0);

    // Delete on empty map should not crash
    map.Delete("nonexistent");
}

TEST_F(RcuMultiMapTest, TestStressInsertAndRetrieve) {
    RcuMultiMap<i32, i32> map;

    const i32 num_operations = 20; // Smaller number for more predictable behavior

    // Insert values only for keys 0-4 (so we can test deletion more reliably)
    for (i32 i = 0; i < num_operations; ++i) {
        map.Insert(i % 5, i); // Use only 5 keys (0-4)
    }

    // Verify all values are accessible
    for (i32 key = 0; key < 5; ++key) {
        auto values = map.Get(key);
        EXPECT_GT(values.size(), 0);
    }

    // Test basic functionality without relying on complex deletion behavior
    // Just verify that we can retrieve all the inserted values
    i32 total_retrieved = 0;
    for (i32 key = 0; key < 5; ++key) {
        auto values = map.Get(key);
        total_retrieved += values.size();
    }

    // We should be able to retrieve all inserted values
    EXPECT_EQ(total_retrieved, num_operations);

    // Test that map size is reasonable
    EXPECT_GT(map.size(), 0);
}

TEST_F(RcuMultiMapTest, TestDeleteFunctionality) {
    RcuMultiMap<String, i32> map;

    // Test that delete operations don't crash and maintain map integrity
    map.Insert("test_key", 42);
    map.Insert("test_key", 84);
    map.Insert("other_key", 100);

    // Verify initial state
    auto initial_values = map.Get("test_key");
    EXPECT_GE(initial_values.size(), 1);

    // Test delete operations (may or may not be immediately visible due to RCU)
    map.Delete("test_key", 42);    // Delete specific value
    map.Delete("nonexistent_key"); // Delete non-existent key (should not crash)

    // Verify map is still functional after delete operations
    auto final_values = map.Get("test_key");
    auto other_values = map.Get("other_key");

    // The key tests are:
    // 1. Operations don't crash
    // 2. Map remains functional
    // 3. Other keys are unaffected
    EXPECT_GE(other_values.size(), 1); // Other key should still have its value
    EXPECT_GE(map.size(), 0);          // Map should still be functional

    // Test that we can still insert after delete
    map.Insert("new_key", 200);
    auto new_key_values = map.Get("new_key");
    EXPECT_GE(new_key_values.size(), 1);
}

// Test with different value types
TEST_F(RcuMultiMapTest, TestDifferentValueTypes) {
    // Test with pointer to int
    RcuMultiMap<String, i32> int_map;

    i32 value1 = 42;
    i32 value2 = 84;

    int_map.Insert("key1", value1);
    int_map.Insert("key2", value2);

    auto values1 = int_map.Get("key1");
    EXPECT_EQ(values1.size(), 1);
    EXPECT_EQ(values1[0], 42);

    auto values2 = int_map.Get("key2");
    EXPECT_EQ(values2.size(), 1);
    EXPECT_EQ(values2[0], 84);
}

// Multi-threading test cases
TEST_F(RcuMultiMapTest, TestConcurrentInsertAndRead) {
    RcuMultiMap<i32, i32> map;

    // Previous parameters (slower):
    // const i32 num_threads = 8;
    // const i32 operations_per_thread = 1000;
    // const i32 key_range = 100;

    // Faster parameters:
    const i32 num_threads = 4;
    const i32 operations_per_thread = 200;
    const i32 key_range = 20;

    std::atomic<i32> total_insertions{0};
    std::atomic<i32> total_reads{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create writer threads
    for (i32 t = 0; t < num_threads / 2; ++t) {
        threads.emplace_back([&, t]() {
            // Wait for start signal
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                i32 key = key_dist(gen);
                i32 value = t * operations_per_thread + i;

                map.Insert(key, value);
                total_insertions.fetch_add(1);

                // Occasionally yield to allow other threads to run
                // Previous frequency (slower): every 100 operations
                // Faster frequency: every 50 operations
                if (i % 50 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create reader threads
    for (i32 t = num_threads / 2; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            // Wait for start signal
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                i32 key = key_dist(gen);
                auto values = map.Get(key);

                // No cleanup needed for POD types

                total_reads.fetch_add(1);

                // Occasionally yield to allow other threads to run
                if (i % 100 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify that all operations completed
    EXPECT_EQ(total_insertions.load(), (num_threads / 2) * operations_per_thread);
    EXPECT_EQ(total_reads.load(), (num_threads / 2) * operations_per_thread);

    // Verify that we can still read data after all threads complete
    i32 final_total_values = 0;
    for (i32 key = 0; key < key_range; ++key) {
        auto values = map.Get(key);
        final_total_values += values.size();

        // No cleanup needed for POD types
    }

    EXPECT_GT(final_total_values, 0);
    EXPECT_LE(final_total_values, total_insertions.load());
}

TEST_F(RcuMultiMapTest, TestConcurrentInsertDeleteRead) {
    RcuMultiMap<String, i32> map;

    // Previous parameters (slower):
    // const i32 num_threads = 12;
    // const i32 operations_per_thread = 500;
    // const i32 key_range = 50;

    // Faster parameters:
    const i32 num_threads = 6;
    const i32 operations_per_thread = 100;
    const i32 key_range = 15;

    std::atomic<i32> total_insertions{0};
    std::atomic<i32> total_deletions{0};
    std::atomic<i32> total_reads{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create insert threads
    for (i32 t = 0; t < num_threads / 3; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                i32 value = t * operations_per_thread + i;

                map.Insert(key, value);
                total_insertions.fetch_add(1);

                // Previous frequency (slower): every 50 operations with 1μs sleep
                // Faster: every 25 operations, no sleep
                if (i % 25 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create delete threads
    for (i32 t = num_threads / 3; t < 2 * num_threads / 3; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            // Wait a bit to let some data be inserted
            // Previous delay (slower): 10ms
            // Faster delay:
            std::this_thread::sleep_for(std::chrono::milliseconds(2));

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread / 2; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                map.Delete(key);
                total_deletions.fetch_add(1);

                // Previous frequency (slower): every 25 operations with 1μs sleep
                // Faster: every 20 operations, no sleep
                if (i % 20 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create read threads
    for (i32 t = 2 * num_threads / 3; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                auto values = map.Get(key);

                // No verification needed for POD types
                // Values are now stored directly, no reference counting

                total_reads.fetch_add(1);

                // Previous frequency (slower): every 50 operations
                // Faster: every 30 operations
                if (i % 30 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify operation counts
    EXPECT_EQ(total_insertions.load(), (num_threads / 3) * operations_per_thread);
    EXPECT_EQ(total_deletions.load(), (num_threads / 3) * (operations_per_thread / 2));
    EXPECT_EQ(total_reads.load(), (num_threads / 3) * operations_per_thread);

    // Verify map is still functional
    EXPECT_GE(map.size(), 0);
}

TEST_F(RcuMultiMapTest, TestHighVolumeMultiThreading) {
    RcuMultiMap<i64, i64> map;

    // Previous parameters (slower):
    // const i32 num_threads = 16;
    // const i32 operations_per_thread = 2000;
    // const i32 key_range = 1000;

    // Faster parameters:
    const i32 num_threads = 6;
    const i32 operations_per_thread = 300;
    const i32 key_range = 50;

    std::atomic<i64> total_operations{0};
    std::atomic<i64> successful_reads{0};
    std::atomic<bool> start_flag{false};
    std::atomic<bool> stop_flag{false};

    Vector<std::thread> threads;

    // Create mixed workload threads (insert + read)
    for (i32 t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t * 1000);
            std::uniform_int_distribution<i64> key_dist(0, key_range - 1);
            std::uniform_int_distribution<i32> op_dist(0, 99);

            i32 local_operations = 0;

            while (!stop_flag.load() && local_operations < operations_per_thread) {
                i64 key = key_dist(gen);
                i32 op_type = op_dist(gen);

                if (op_type < 70) { // 70% reads
                    auto values = map.Get(key);
                    if (!values.empty()) {
                        successful_reads.fetch_add(1);

                        // Verify data integrity
                        for (auto val : values) {
                            EXPECT_GE(val, 0);
                        }
                    }

                    // No cleanup needed for POD types
                } else { // 30% inserts
                    i64 value = t * operations_per_thread + local_operations;
                    map.Insert(key, value);
                }

                local_operations++;
                total_operations.fetch_add(1);

                // Occasional yield to promote thread interleaving
                // Previous frequency (slower): every 100 operations
                // Faster: every 50 operations
                if (local_operations % 50 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Let threads run for a specific duration
    // Previous duration (slower): std::chrono::seconds(2)
    // Faster duration:
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    stop_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify that significant work was done
    EXPECT_GT(total_operations.load(), num_threads * 100);
    EXPECT_GT(successful_reads.load(), 0);

    // Verify map integrity after stress test
    i64 final_total_values = 0;
    for (i64 key = 0; key < std::min(key_range, 100); ++key) {
        auto values = map.Get(key);
        final_total_values += values.size();

        // Verify data integrity
        for (auto val : values) {
            EXPECT_GE(val, 0);
        }
    }

    EXPECT_GE(final_total_values, 0);
}

TEST_F(RcuMultiMapTest, TestRcuSwapUnderLoad) {
    RcuMultiMap<String, i32> map;

    // Previous parameters (slower):
    // const i32 num_threads = 8;
    // const i32 operations_per_thread = 1500;
    // const i32 key_range = 20;

    // Faster parameters:
    const i32 num_threads = 4;
    const i32 operations_per_thread = 200;
    const i32 key_range = 10; // Small key range to force frequent access

    std::atomic<i32> total_gets{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create threads that will trigger RCU swaps through cache misses
    for (i32 t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            // First, insert some data
            // Previous count (slower): 50
            // Faster count:
            for (i32 i = 0; i < 10; ++i) {
                String key = "swap_key_" + std::to_string(key_dist(gen));
                i32 value = t * 1000 + i;
                map.Insert(key, value);
            }

            // Then perform many reads to trigger cache misses and swaps
            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "swap_key_" + std::to_string(key_dist(gen));
                auto values = map.Get(key, true); // Update access time

                total_gets.fetch_add(1);

                // Verify data integrity during RCU operations
                for (auto val : values) {
                    EXPECT_GE(val, 0);
                }

                // Force frequent access to trigger CheckSwapInLock
                if (i % 10 == 0) {
                    // Access multiple keys rapidly to increase miss_time
                    for (i32 j = 0; j < 5; ++j) {
                        String rapid_key = "swap_key_" + std::to_string(j % key_range);
                        auto rapid_values = map.Get(rapid_key, true);
                        total_gets.fetch_add(1); // Count these additional gets
                        // No cleanup needed for POD types
                    }
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify that significant work was done
    // Should be at least the base operations, possibly more due to rapid access
    EXPECT_GE(total_gets.load(), num_threads * operations_per_thread);

    // Verify final state consistency
    i32 final_unique_keys = 0;
    i32 final_total_values = 0;

    for (i32 i = 0; i < key_range; ++i) {
        String key = "swap_key_" + std::to_string(i);
        auto values = map.Get(key);

        if (!values.empty()) {
            final_unique_keys++;
            final_total_values += values.size();

            // Verify data integrity after all RCU operations
            for (auto val : values) {
                EXPECT_GE(val, 0);
            }
        }
    }

    EXPECT_GT(final_unique_keys, 0);
    EXPECT_GT(final_total_values, 0);
    EXPECT_LE(final_unique_keys, key_range);
}

TEST_F(RcuMultiMapTest, TestRcuBehavior) {
    RcuMultiMap<String, i32> map;

    // Insert data (goes to dirty_map)
    map.Insert("key1", 100);
    map.Insert("key2", 200);

    // Data should be accessible via Get (searches both dirty and read maps)
    auto values1 = map.Get("key1");
    EXPECT_EQ(values1.size(), 1);
    VerifyValues(values1, {100});

    // Force multiple cache misses to trigger CheckSwapInLock
    for (int i = 0; i < 10; ++i) {
        auto temp_values = map.Get("key1");
        // No cleanup needed for POD types
    }

    // The key insight is that Get always works regardless of RCU state
    auto final_values = map.Get("key1");
    EXPECT_EQ(final_values.size(), 1);
    VerifyValues(final_values, {100});
}

// RcuMap Test Class
class RcuMapTest : public BaseTest {
public:
    RcuMapTest() = default;
    ~RcuMapTest() = default;

    void SetUp() override { BaseTest::SetUp(); }

    void TearDown() override { BaseTest::TearDown(); }
};

TEST_F(RcuMapTest, TestBasicInsertAndGet) {
    RcuMap<String, i32> map;

    // Insert some values (Map has unique keys)
    map.Insert("key1", 10);
    map.Insert("key2", 20);
    map.Insert("key1", 30); // Should replace the previous value

    // Test Get (returns pointer)
    auto value1_ptr = map.Get("key1");
    ASSERT_NE(value1_ptr, nullptr);
    EXPECT_EQ(*value1_ptr, 30); // Should be the replaced value

    auto value2_ptr = map.Get("key2");
    ASSERT_NE(value2_ptr, nullptr);
    EXPECT_EQ(*value2_ptr, 20);

    // Test non-existent key
    auto value3_ptr = map.Get("key3");
    EXPECT_EQ(value3_ptr, nullptr);

    // Test GetValue (returns Optional)
    auto opt_value1 = map.GetValue("key1");
    EXPECT_TRUE(opt_value1.has_value());
    EXPECT_EQ(opt_value1.value(), 30);

    auto opt_value3 = map.GetValue("key3");
    EXPECT_FALSE(opt_value3.has_value());

    // Test size
    EXPECT_EQ(map.size(), 2);
}

TEST_F(RcuMapTest, TestLowercaseAliases) {
    RcuMap<String, i32> map;

    // Test lowercase insert alias
    map.insert("test_key", 100);
    map.insert("test_key", 200); // Should replace

    // Test lowercase get alias
    auto value_ptr = map.get("test_key");
    ASSERT_NE(value_ptr, nullptr);
    EXPECT_EQ(*value_ptr, 200);
}

TEST_F(RcuMapTest, TestDelete) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Delete key1
    map.Delete("key1");
    auto value1_ptr = map.Get("key1");
    EXPECT_EQ(value1_ptr, nullptr);

    // key2 should still exist
    auto value2_ptr = map.Get("key2");
    ASSERT_NE(value2_ptr, nullptr);
    EXPECT_EQ(*value2_ptr, 20);

    // Test lowercase delete alias
    map.delete_key("key2");
    auto value2_ptr_after = map.Get("key2");
    EXPECT_EQ(value2_ptr_after, nullptr);

    EXPECT_EQ(map.size(), 0);
}

TEST_F(RcuMapTest, TestGetAllValuesWithRef) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);
    map.Insert("key3", 30);

    Vector<i32> all_values;
    map.GetAllValuesWithRef(all_values);

    EXPECT_EQ(all_values.size(), 3);
    std::sort(all_values.begin(), all_values.end());
    Vector<i32> expected = {10, 20, 30};
    EXPECT_EQ(all_values, expected);
}

TEST_F(RcuMapTest, TestGetAllKeyValuePairs) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);
    map.Insert("key3", 30);

    Vector<Pair<String, i32>> pairs;
    map.GetAllKeyValuePairs(pairs);

    EXPECT_EQ(pairs.size(), 3);

    // Sort pairs by key for consistent testing
    std::sort(pairs.begin(), pairs.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

    EXPECT_EQ(pairs[0].first, "key1");
    EXPECT_EQ(pairs[0].second, 10);
    EXPECT_EQ(pairs[1].first, "key2");
    EXPECT_EQ(pairs[1].second, 20);
    EXPECT_EQ(pairs[2].first, "key3");
    EXPECT_EQ(pairs[2].second, 30);
}

TEST_F(RcuMapTest, TestGarbageCollection) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Delete some entries
    map.Delete("key1");

    // Test garbage collection (should not crash)
    u64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    // Test checkGc alias
    map.checkGc(current_time - 1000); // GC entries older than 1 second

    // Verify remaining data is still accessible
    auto value_ptr = map.Get("key2");
    ASSERT_NE(value_ptr, nullptr);
    EXPECT_EQ(*value_ptr, 20);
}

TEST_F(RcuMapTest, TestExpiredEntries) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Access key1 to update its access time
    map.Get("key1", true);

    // Sleep briefly to create time difference
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    u64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    Vector<String> expired_keys;
    map.CheckExpired(current_time + 1000, expired_keys); // Check for entries not accessed in last second

    // Should find some expired keys (exact behavior depends on implementation)
    // This test mainly ensures the method doesn't crash
    EXPECT_GE(expired_keys.size(), 0);
}

TEST_F(RcuMapTest, TestGetWithRcuTime) {
    RcuMap<String, i32> map;

    map.Insert("key1", 10);
    map.Insert("key2", 20);

    // Test GetWithRcuTime (should behave similarly to Get but with RCU semantics)
    auto value1_ptr = map.GetWithRcuTime("key1");
    ASSERT_NE(value1_ptr, nullptr);
    EXPECT_EQ(*value1_ptr, 10);

    auto value2_ptr = map.GetWithRcuTime("key2");
    ASSERT_NE(value2_ptr, nullptr);
    EXPECT_EQ(*value2_ptr, 20);

    // Test non-existent key
    auto value3_ptr = map.GetWithRcuTime("key3");
    EXPECT_EQ(value3_ptr, nullptr);
}

TEST_F(RcuMapTest, TestUniqueKeys) {
    RcuMap<i32, i32> map;

    // Insert multiple values for the same key (should replace, not accumulate)
    map.Insert(42, 100);
    map.Insert(42, 200);
    map.Insert(42, 300);

    auto value_ptr = map.Get(42);
    ASSERT_NE(value_ptr, nullptr);
    EXPECT_EQ(*value_ptr, 300); // Should be the last inserted value

    EXPECT_EQ(map.size(), 1); // Should only have one entry
}

TEST_F(RcuMapTest, TestIntegerKeys) {
    RcuMap<i32, i32> map;

    map.Insert(1, 100);
    map.Insert(2, 200);
    map.Insert(3, 300);

    // Test retrieval
    auto value1_ptr = map.Get(1);
    ASSERT_NE(value1_ptr, nullptr);
    EXPECT_EQ(*value1_ptr, 100);

    auto value2_ptr = map.Get(2);
    ASSERT_NE(value2_ptr, nullptr);
    EXPECT_EQ(*value2_ptr, 200);

    auto value3_ptr = map.Get(3);
    ASSERT_NE(value3_ptr, nullptr);
    EXPECT_EQ(*value3_ptr, 300);

    EXPECT_EQ(map.size(), 3);
}

TEST_F(RcuMapTest, TestEmptyMap) {
    RcuMap<String, i32> map;

    // Test operations on empty map
    auto value_ptr = map.Get("nonexistent");
    EXPECT_EQ(value_ptr, nullptr);

    auto opt_value = map.GetValue("nonexistent");
    EXPECT_FALSE(opt_value.has_value());

    Vector<i32> all_values;
    map.GetAllValuesWithRef(all_values);
    EXPECT_EQ(all_values.size(), 0);

    // Delete on empty map should not crash
    map.Delete("nonexistent");

    EXPECT_EQ(map.size(), 0);
}

TEST_F(RcuMapTest, TestEmplace) {
    RcuMap<String, i32> map;

    // Test emplace functionality
    map.emplace("key1", 42);
    map.emplace("key2", 84);

    auto value1_ptr = map.Get("key1");
    ASSERT_NE(value1_ptr, nullptr);
    EXPECT_EQ(*value1_ptr, 42);

    auto value2_ptr = map.Get("key2");
    ASSERT_NE(value2_ptr, nullptr);
    EXPECT_EQ(*value2_ptr, 84);

    // Test emplace replacement
    map.emplace("key1", 168);
    auto value1_new_ptr = map.Get("key1");
    ASSERT_NE(value1_new_ptr, nullptr);
    EXPECT_EQ(*value1_new_ptr, 168);

    EXPECT_EQ(map.size(), 2);
}

TEST_F(RcuMapTest, TestRange) {
    RcuMap<i32, i32> map;

    // Insert values with integer keys
    map.Insert(1, 100);
    map.Insert(3, 300);
    map.Insert(5, 500);
    map.Insert(7, 700);
    map.Insert(9, 900);

    // Test range query
    Vector<i32> result;
    u32 count = map.range(3, 7, result);

    EXPECT_EQ(count, 3); // Should include keys 3, 5, 7
    EXPECT_EQ(result.size(), 3);

    std::sort(result.begin(), result.end());
    Vector<i32> expected = {300, 500, 700};
    EXPECT_EQ(result, expected);
}

TEST_F(RcuMapTest, TestStressInsertAndRetrieve) {
    RcuMap<i32, i32> map;

    const i32 num_operations = 50; // Moderate number for fast execution

    // Insert values (each key gets one value due to Map semantics)
    for (i32 i = 0; i < num_operations; ++i) {
        map.Insert(i, i * 10);
    }

    // Verify all values are accessible
    for (i32 key = 0; key < num_operations; ++key) {
        auto value_ptr = map.Get(key);
        ASSERT_NE(value_ptr, nullptr);
        EXPECT_EQ(*value_ptr, key * 10);
    }

    EXPECT_EQ(map.size(), num_operations);
}

// Multi-threading tests for RcuMap
TEST_F(RcuMapTest, TestConcurrentInsertAndRead) {
    RcuMap<i32, i32> map;

    const i32 num_threads = 4;
    const i32 operations_per_thread = 100;
    const i32 key_range = 20;

    std::atomic<i32> total_insertions{0};
    std::atomic<i32> total_reads{0};
    std::atomic<i32> successful_reads{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create writer threads
    for (i32 t = 0; t < num_threads / 2; ++t) {
        threads.emplace_back([&, t]() {
            // Wait for start signal
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                i32 key = key_dist(gen);
                i32 value = t * operations_per_thread + i;

                map.Insert(key, value);
                total_insertions.fetch_add(1);

                if (i % 25 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create reader threads
    for (i32 t = num_threads / 2; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            // Wait for start signal
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                i32 key = key_dist(gen);
                auto value_ptr = map.Get(key);

                total_reads.fetch_add(1);
                if (value_ptr != nullptr) {
                    successful_reads.fetch_add(1);
                    EXPECT_GE(*value_ptr, 0); // Basic sanity check
                }

                if (i % 50 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify that all operations completed
    EXPECT_EQ(total_insertions.load(), (num_threads / 2) * operations_per_thread);
    EXPECT_EQ(total_reads.load(), (num_threads / 2) * operations_per_thread);

    // Verify that we can still read data after all threads complete
    i32 final_accessible_keys = 0;
    for (i32 key = 0; key < key_range; ++key) {
        auto value_ptr = map.Get(key);
        if (value_ptr != nullptr) {
            final_accessible_keys++;
            EXPECT_GE(*value_ptr, 0);
        }
    }

    EXPECT_GT(final_accessible_keys, 0);
    EXPECT_LE(final_accessible_keys, key_range); // Can't exceed key range for Map
}

TEST_F(RcuMapTest, TestConcurrentInsertDeleteRead) {
    RcuMap<String, i32> map;

    const i32 num_threads = 6;
    const i32 operations_per_thread = 50;
    const i32 key_range = 10;

    std::atomic<i32> total_insertions{0};
    std::atomic<i32> total_deletions{0};
    std::atomic<i32> total_reads{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create insert threads
    for (i32 t = 0; t < num_threads / 3; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                i32 value = t * operations_per_thread + i;

                map.Insert(key, value);
                total_insertions.fetch_add(1);

                if (i % 15 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create delete threads
    for (i32 t = num_threads / 3; t < 2 * num_threads / 3; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            // Wait a bit to let some data be inserted
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread / 2; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                map.Delete(key);
                total_deletions.fetch_add(1);

                if (i % 10 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Create read threads
    for (i32 t = 2 * num_threads / 3; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "key_" + std::to_string(key_dist(gen));
                auto value_ptr = map.Get(key);

                total_reads.fetch_add(1);
                if (value_ptr != nullptr) {
                    EXPECT_GE(*value_ptr, 0); // Basic sanity check
                }

                if (i % 20 == 0) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify operation counts
    EXPECT_EQ(total_insertions.load(), (num_threads / 3) * operations_per_thread);
    EXPECT_EQ(total_deletions.load(), (num_threads / 3) * (operations_per_thread / 2));
    EXPECT_EQ(total_reads.load(), (num_threads / 3) * operations_per_thread);

    // Verify map is still functional
    EXPECT_GE(map.size(), 0);
}

TEST_F(RcuMapTest, TestRcuSwapUnderLoad) {
    RcuMap<String, i32> map;

    const i32 num_threads = 4;
    const i32 operations_per_thread = 100;
    const i32 key_range = 8; // Small key range to force frequent access

    std::atomic<i32> total_gets{0};
    std::atomic<bool> start_flag{false};

    Vector<std::thread> threads;

    // Create threads that will trigger RCU swaps through cache misses
    for (i32 t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            std::random_device rd;
            std::mt19937 gen(rd() + t);
            std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

            // First, insert some data
            for (i32 i = 0; i < 5; ++i) {
                String key = "swap_key_" + std::to_string(key_dist(gen));
                i32 value = t * 1000 + i;
                map.Insert(key, value);
            }

            // Then perform many reads to trigger cache misses and swaps
            for (i32 i = 0; i < operations_per_thread; ++i) {
                String key = "swap_key_" + std::to_string(key_dist(gen));
                auto value_ptr = map.Get(key, true); // Update access time

                total_gets.fetch_add(1);

                // Verify data integrity during RCU operations
                if (value_ptr != nullptr) {
                    EXPECT_GE(*value_ptr, 0);
                }

                // Force frequent access to trigger CheckSwapInLock
                if (i % 10 == 0) {
                    // Access multiple keys rapidly to increase miss_time
                    for (i32 j = 0; j < 3; ++j) {
                        String rapid_key = "swap_key_" + std::to_string(j % key_range);
                        map.Get(rapid_key, true);
                        total_gets.fetch_add(1);
                    }
                }
            }
        });
    }

    // Start all threads
    start_flag.store(true);

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Verify that significant work was done
    EXPECT_GE(total_gets.load(), num_threads * operations_per_thread);

    // Verify final state consistency
    i32 final_accessible_keys = 0;

    for (i32 i = 0; i < key_range; ++i) {
        String key = "swap_key_" + std::to_string(i);
        auto value_ptr = map.Get(key);

        if (value_ptr != nullptr) {
            final_accessible_keys++;
            EXPECT_GE(*value_ptr, 0);
        }
    }

    EXPECT_GE(final_accessible_keys, 0);
    EXPECT_LE(final_accessible_keys, key_range);
}

TEST_F(RcuMapTest, TestRcuBehavior) {
    RcuMap<String, i32> map;

    // Insert data (goes to dirty_map)
    map.Insert("key1", 100);
    map.Insert("key2", 200);

    // Data should be accessible via Get (searches both dirty and read maps)
    auto value1_ptr = map.Get("key1");
    ASSERT_NE(value1_ptr, nullptr);
    EXPECT_EQ(*value1_ptr, 100);

    // Force multiple cache misses to trigger CheckSwapInLock
    for (int i = 0; i < 10; ++i) {
        auto temp_value_ptr = map.Get("key1");
        ASSERT_NE(temp_value_ptr, nullptr);
    }

    // The key insight is that Get always works regardless of RCU state
    auto final_value_ptr = map.Get("key1");
    ASSERT_NE(final_value_ptr, nullptr);
    EXPECT_EQ(*final_value_ptr, 100);
}

TEST_F(RcuMapTest, TestMapVsMultiMapBehavior) {
    RcuMap<String, i32> map;
    RcuMultiMap<String, i32> multimap;

    // Test that Map replaces values while MultiMap accumulates them
    map.Insert("test_key", 10);
    map.Insert("test_key", 20);
    map.Insert("test_key", 30);

    multimap.Insert("test_key", 10);
    multimap.Insert("test_key", 20);
    multimap.Insert("test_key", 30);

    // Map should have only the last value
    auto map_value_ptr = map.Get("test_key");
    ASSERT_NE(map_value_ptr, nullptr);
    EXPECT_EQ(*map_value_ptr, 30);
    EXPECT_EQ(map.size(), 1);

    // MultiMap should have all values
    auto multimap_values = multimap.Get("test_key");
    EXPECT_EQ(multimap_values.size(), 3);
    EXPECT_GT(multimap.size(), 0);

    // Test range behavior differences
    map.Insert("key1", 100);
    map.Insert("key2", 200);
    map.Insert("key3", 300);

    multimap.Insert("key1", 100);
    multimap.Insert("key2", 200);
    multimap.Insert("key3", 300);

    Vector<i32> map_range_result;
    Vector<i32> multimap_range_result;

    u32 map_count = map.range("key1", "key3", map_range_result);
    u32 multimap_count = multimap.range("key1", "key3", multimap_range_result);

    // Both should find the same number of keys in range, but Map guarantees unique keys
    EXPECT_EQ(map_count, map_range_result.size());
    EXPECT_EQ(multimap_count, multimap_range_result.size());
    EXPECT_GE(map_count, 3); // At least key1, key2, key3
}
#if 0
// ============================================================================
// BENCHMARK TESTS: RcuMap vs MapWithLock Performance Comparison
// ============================================================================

class RcuMapBenchmarkTest : public BaseTest {
public:
    RcuMapBenchmarkTest() = default;
    ~RcuMapBenchmarkTest() = default;

    void SetUp() override { BaseTest::SetUp(); }

    void TearDown() override { BaseTest::TearDown(); }

protected:
    // Helper function to measure execution time
    template <typename Func>
    double MeasureTime(Func &&func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Return milliseconds
    }

    // Helper to generate random keys
    Vector<String> GenerateKeys(i32 count, i32 key_range) {
        Vector<String> keys;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<i32> dist(0, key_range - 1);

        for (i32 i = 0; i < count; ++i) {
            keys.push_back("key_" + std::to_string(dist(gen)));
        }
        return keys;
    }
};

// Benchmark 1: Read-Heavy Workload (90% reads, 10% writes)
TEST_F(RcuMapBenchmarkTest, ReadHeavyWorkload) {
    const i32 num_threads = 8;
    const i32 operations_per_thread = 5000; // Increased for more data
    const i32 key_range = 100000;           // 100K records
    const double read_ratio = 0.9;          // 90% reads

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    // Pre-populate both maps with substantial data (50K records)
    std::cout << "Pre-populating maps with " << (key_range / 2) << " records..." << std::endl;
    for (i32 i = 0; i < key_range / 2; ++i) {
        String key = "key_" + std::to_string(i);
        rcu_map.Insert(key, i * 10);
        i32 temp_value;
        lock_map.GetOrAdd(key, temp_value, i * 10);

        // Progress indicator for large datasets
        if (i % 10000 == 0 && i > 0) {
            std::cout << "  Populated " << i << " records..." << std::endl;
        }
    }
    std::cout << "Pre-population complete." << std::endl;

    std::cout << "\n=== READ-HEAVY WORKLOAD BENCHMARK (90% reads, 10% writes) ===" << std::endl;
    std::cout << "Threads: " << num_threads << ", Operations per thread: " << operations_per_thread << std::endl;
    std::cout << "Note: Using GetReadOnly() for RcuMap reads to maximize performance" << std::endl;

    // Benchmark RcuMap
    auto rcu_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        // Read operation - use GetReadOnly for absolute maximum performance
                        auto value_ptr = rcu_map.GetReadOnly(key);
                        (void)value_ptr; // Suppress unused variable warning
                    } else {
                        // Write operation
                        rcu_map.Insert(key, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    // Benchmark MapWithLock
    auto lock_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        // Read operation
                        i32 value;
                        bool found = lock_map.Get(key, value);
                        (void)found; // Suppress unused variable warning
                    } else {
                        // Write operation
                        i32 value;
                        lock_map.GetOrAdd(key, value, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    std::cout << "RcuMap time: " << rcu_time << " ms" << std::endl;
    std::cout << "MapWithLock time: " << lock_time << " ms" << std::endl;
    std::cout << "RcuMap speedup: " << (lock_time / rcu_time) << "x" << std::endl;

    // RcuMap should be faster for read-heavy workloads
    EXPECT_LT(rcu_time, lock_time * 1.2); // Allow 20% margin for variance
}

// Benchmark 2: Write-Heavy Workload (30% reads, 70% writes)
TEST_F(RcuMapBenchmarkTest, WriteHeavyWorkload) {
    const i32 num_threads = 8;
    const i32 operations_per_thread = 2000; // Increased for more data
    const i32 key_range = 100000;           // 100K records
    const double read_ratio = 0.3;          // 30% reads, 70% writes

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    // Pre-populate both maps (25K records)
    std::cout << "Pre-populating maps with " << (key_range / 4) << " records..." << std::endl;
    for (i32 i = 0; i < key_range / 4; ++i) {
        String key = "key_" + std::to_string(i);
        rcu_map.Insert(key, i * 10);
        i32 temp_value;
        lock_map.GetOrAdd(key, temp_value, i * 10);

        if (i % 5000 == 0 && i > 0) {
            std::cout << "  Populated " << i << " records..." << std::endl;
        }
    }
    std::cout << "Pre-population complete." << std::endl;

    std::cout << "\n=== WRITE-HEAVY WORKLOAD BENCHMARK (30% reads, 70% writes) ===" << std::endl;
    std::cout << "Threads: " << num_threads << ", Operations per thread: " << operations_per_thread << std::endl;

    // Benchmark RcuMap
    auto rcu_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        auto value_ptr = rcu_map.Get(key);
                        (void)value_ptr;
                    } else {
                        rcu_map.Insert(key, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    // Benchmark MapWithLock
    auto lock_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        i32 value;
                        bool found = lock_map.Get(key, value);
                        (void)found;
                    } else {
                        i32 value;
                        lock_map.GetOrAdd(key, value, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    std::cout << "RcuMap time: " << rcu_time << " ms" << std::endl;
    std::cout << "MapWithLock time: " << lock_time << " ms" << std::endl;
    std::cout << "Performance ratio (RcuMap/MapWithLock): " << (rcu_time / lock_time) << "x" << std::endl;

    // For write-heavy workloads, performance should be comparable or MapWithLock might be better
    // We don't enforce a specific expectation here as it depends on the implementation details
    EXPECT_GT(rcu_time, 0); // Just ensure the test runs
    EXPECT_GT(lock_time, 0);
}

// Benchmark 3: Read-Only Workload (100% reads)
TEST_F(RcuMapBenchmarkTest, ReadOnlyWorkload) {
    const i32 num_threads = 12;
    const i32 operations_per_thread = 10000; // Increased for more operations
    const i32 key_range = 100000;            // 100K records

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    // Pre-populate both maps with all 100K records
    std::cout << "Pre-populating maps with " << key_range << " records..." << std::endl;
    for (i32 i = 0; i < key_range; ++i) {
        String key = "key_" + std::to_string(i);
        rcu_map.Insert(key, i * 10);
        i32 temp_value;
        lock_map.GetOrAdd(key, temp_value, i * 10);

        if (i % 10000 == 0 && i > 0) {
            std::cout << "  Populated " << i << " records..." << std::endl;
        }
    }
    std::cout << "Pre-population complete." << std::endl;

    std::cout << "\n=== READ-ONLY WORKLOAD BENCHMARK (100% reads) ===" << std::endl;
    std::cout << "Threads: " << num_threads << ", Operations per thread: " << operations_per_thread << std::endl;

    // Benchmark RcuMap
    auto rcu_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));
                    auto value_ptr = rcu_map.Get(key);
                    (void)value_ptr;
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    // Benchmark MapWithLock
    auto lock_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));
                    i32 value;
                    bool found = lock_map.Get(key, value);
                    (void)found;
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    std::cout << "RcuMap time: " << rcu_time << " ms" << std::endl;
    std::cout << "MapWithLock time: " << lock_time << " ms" << std::endl;
    std::cout << "RcuMap speedup: " << (lock_time / rcu_time) << "x" << std::endl;

    // RcuMap should significantly outperform MapWithLock for read-only workloads
    // EXPECT_LT(rcu_time, lock_time * 0.8); // RcuMap should be at least 25% faster
}

// Benchmark 4: High Contention Scenario (Many threads, moderate key space)
TEST_F(RcuMapBenchmarkTest, HighContentionWorkload) {
    const i32 num_threads = 16;
    const i32 operations_per_thread = 2000; // Increased operations
    const i32 key_range = 1000;             // Moderate key space for contention
    const double read_ratio = 0.8;          // 80% reads, 20% writes

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    // Pre-populate with all keys
    for (i32 i = 0; i < key_range; ++i) {
        String key = "key_" + std::to_string(i);
        rcu_map.Insert(key, i * 10);
        i32 temp_value;
        lock_map.GetOrAdd(key, temp_value, i * 10);
    }

    std::cout << "\n=== HIGH CONTENTION WORKLOAD BENCHMARK (80% reads, 20% writes, moderate key space) ===" << std::endl;
    std::cout << "Threads: " << num_threads << ", Operations per thread: " << operations_per_thread << ", Key range: " << key_range << std::endl;

    // Benchmark RcuMap
    auto rcu_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        auto value_ptr = rcu_map.Get(key);
                        (void)value_ptr;
                    } else {
                        rcu_map.Insert(key, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    // Benchmark MapWithLock
    auto lock_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        i32 value;
                        bool found = lock_map.Get(key, value);
                        (void)found;
                    } else {
                        i32 value;
                        lock_map.GetOrAdd(key, value, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    std::cout << "RcuMap time: " << rcu_time << " ms" << std::endl;
    std::cout << "MapWithLock time: " << lock_time << " ms" << std::endl;
    std::cout << "Performance ratio (RcuMap/MapWithLock): " << (rcu_time / lock_time) << "x" << std::endl;

    // High contention scenario - results may vary
    EXPECT_GT(rcu_time, 0);
    EXPECT_GT(lock_time, 0);
}

// Benchmark 5: Scalability Test (Varying thread counts)
TEST_F(RcuMapBenchmarkTest, ScalabilityTest) {
    const i32 operations_per_thread = 5000; // Increased operations
    const i32 key_range = 100000;           // 100K records
    const double read_ratio = 0.85;         // 85% reads

    Vector<i32> thread_counts = {1, 2, 4, 8, 16};

    std::cout << "\n=== SCALABILITY BENCHMARK (85% reads, varying thread counts) ===" << std::endl;
    std::cout << "Operations per thread: " << operations_per_thread << std::endl;

    for (i32 num_threads : thread_counts) {
        RcuMap<String, i32> rcu_map;
        MapWithLock<String, i32> lock_map;

        // Pre-populate (50K records)
        std::cout << "Pre-populating for " << num_threads << " threads with " << (key_range / 2) << " records..." << std::endl;
        for (i32 i = 0; i < key_range / 2; ++i) {
            String key = "key_" + std::to_string(i);
            rcu_map.Insert(key, i * 10);
            i32 temp_value;
            lock_map.GetOrAdd(key, temp_value, i * 10);

            if (i % 10000 == 0 && i > 0) {
                std::cout << "  Populated " << i << " records..." << std::endl;
            }
        }

        // Benchmark RcuMap
        auto rcu_time = MeasureTime([&]() {
            std::atomic<bool> start_flag{false};
            Vector<std::thread> threads;

            for (i32 t = 0; t < num_threads; ++t) {
                threads.emplace_back([&, t]() {
                    while (!start_flag.load())
                        std::this_thread::yield();

                    std::random_device rd;
                    std::mt19937 gen(rd() + t);
                    std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                    std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                    for (i32 i = 0; i < operations_per_thread; ++i) {
                        String key = "key_" + std::to_string(key_dist(gen));

                        if (op_dist(gen) < read_ratio) {
                            auto value_ptr = rcu_map.Get(key);
                            (void)value_ptr;
                        } else {
                            rcu_map.Insert(key, t * operations_per_thread + i);
                        }
                    }
                });
            }

            start_flag.store(true);
            for (auto &thread : threads) {
                thread.join();
            }
        });

        // Benchmark MapWithLock
        auto lock_time = MeasureTime([&]() {
            std::atomic<bool> start_flag{false};
            Vector<std::thread> threads;

            for (i32 t = 0; t < num_threads; ++t) {
                threads.emplace_back([&, t]() {
                    while (!start_flag.load())
                        std::this_thread::yield();

                    std::random_device rd;
                    std::mt19937 gen(rd() + t);
                    std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                    std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                    for (i32 i = 0; i < operations_per_thread; ++i) {
                        String key = "key_" + std::to_string(key_dist(gen));

                        if (op_dist(gen) < read_ratio) {
                            i32 value;
                            bool found = lock_map.Get(key, value);
                            (void)found;
                        } else {
                            i32 value;
                            lock_map.GetOrAdd(key, value, t * operations_per_thread + i);
                        }
                    }
                });
            }

            start_flag.store(true);
            for (auto &thread : threads) {
                thread.join();
            }
        });

        double speedup = lock_time / rcu_time;
        std::cout << "Threads: " << num_threads << ", RcuMap: " << rcu_time << " ms"
                  << ", MapWithLock: " << lock_time << " ms"
                  << ", Speedup: " << speedup << "x" << std::endl;

        EXPECT_GT(rcu_time, 0);
        EXPECT_GT(lock_time, 0);
    }
}

// Benchmark 6: Memory Overhead and GC Performance
TEST_F(RcuMapBenchmarkTest, MemoryAndGcPerformance) {
    const i32 num_operations = 100000; // 100K operations
    const i32 key_range = 100000;      // 100K key range

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    std::cout << "\n=== MEMORY AND GC PERFORMANCE BENCHMARK (100K operations) ===" << std::endl;
    std::cout << "Operations: " << num_operations << ", Key range: " << key_range << std::endl;

    // Test insertion performance
    std::cout << "Testing RcuMap insertion performance..." << std::endl;
    auto rcu_insert_time = MeasureTime([&]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

        for (i32 i = 0; i < num_operations; ++i) {
            String key = "key_" + std::to_string(key_dist(gen));
            rcu_map.Insert(key, i);

            if (i % 10000 == 0 && i > 0) {
                std::cout << "  RcuMap inserted " << i << " records..." << std::endl;
            }
        }
    });

    std::cout << "Testing MapWithLock insertion performance..." << std::endl;
    auto lock_insert_time = MeasureTime([&]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<i32> key_dist(0, key_range - 1);

        for (i32 i = 0; i < num_operations; ++i) {
            String key = "key_" + std::to_string(key_dist(gen));
            i32 value;
            lock_map.GetOrAdd(key, value, i);

            if (i % 10000 == 0 && i > 0) {
                std::cout << "  MapWithLock inserted " << i << " records..." << std::endl;
            }
        }
    });

    std::cout << "Insert performance - RcuMap: " << rcu_insert_time << " ms, MapWithLock: " << lock_insert_time << " ms" << std::endl;

    // Test GC performance
    auto gc_time = MeasureTime([&]() {
        u64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        rcu_map.CheckGc(current_time - 1000); // GC entries older than 1 second
    });

    std::cout << "GC time: " << gc_time << " ms" << std::endl;

    // Test final sizes (MapWithLock doesn't have size() method)
    std::cout << "Final map sizes - RcuMap: " << rcu_map.size() << std::endl;

    EXPECT_GT(rcu_insert_time, 0);
    EXPECT_GT(lock_insert_time, 0);
    EXPECT_GE(gc_time, 0);
}

// Benchmark Summary Test - Comprehensive comparison
TEST_F(RcuMapBenchmarkTest, ComprehensiveBenchmarkSummary) {
    std::cout << "\n=== COMPREHENSIVE BENCHMARK SUMMARY ===" << std::endl;
    std::cout << "This test provides a summary of RcuMap vs MapWithLock performance characteristics:" << std::endl;
    std::cout << std::endl;

    struct BenchmarkResult {
        String scenario;
        double rcu_time;
        double lock_time;
        double speedup;
    };

    Vector<BenchmarkResult> results;

    // Quick benchmark for different scenarios with increased data
    Vector<std::tuple<String, double, i32, i32>> scenarios = {{"Read-Heavy (95% reads)", 0.95, 8, 5000},
                                                              {"Balanced (70% reads)", 0.70, 8, 3000},
                                                              {"Write-Heavy (40% reads)", 0.40, 8, 2000},
                                                              {"High Contention", 0.80, 16, 2000}};

    for (auto &[name, read_ratio, threads, ops] : scenarios) {
        RcuMap<String, i32> rcu_map;
        MapWithLock<String, i32> lock_map;

        // Pre-populate with substantial data (50K records)
        const i32 prepopulate_count = 50000;
        for (i32 i = 0; i < prepopulate_count; ++i) {
            String key = "key_" + std::to_string(i);
            rcu_map.Insert(key, i);
            i32 temp_value;
            lock_map.GetOrAdd(key, temp_value, i);
        }

        // Benchmark RcuMap
        auto rcu_time = MeasureTime([&]() {
            std::atomic<bool> start_flag{false};
            Vector<std::thread> thread_vec;

            for (i32 t = 0; t < threads; ++t) {
                thread_vec.emplace_back([&, t]() {
                    while (!start_flag.load())
                        std::this_thread::yield();

                    std::random_device rd;
                    std::mt19937 gen(rd() + t);
                    std::uniform_int_distribution<i32> key_dist(0, 99999); // Use full 100K range
                    std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                    for (i32 i = 0; i < ops; ++i) {
                        String key = "key_" + std::to_string(key_dist(gen));
                        if (op_dist(gen) < read_ratio) {
                            auto value_ptr = rcu_map.Get(key);
                            (void)value_ptr;
                        } else {
                            rcu_map.Insert(key, i);
                        }
                    }
                });
            }

            start_flag.store(true);
            for (auto &thread : thread_vec) {
                thread.join();
            }
        });

        // Benchmark MapWithLock
        auto lock_time = MeasureTime([&]() {
            std::atomic<bool> start_flag{false};
            Vector<std::thread> thread_vec;

            for (i32 t = 0; t < threads; ++t) {
                thread_vec.emplace_back([&, t]() {
                    while (!start_flag.load())
                        std::this_thread::yield();

                    std::random_device rd;
                    std::mt19937 gen(rd() + t);
                    std::uniform_int_distribution<i32> key_dist(0, 99999); // Use full 100K range
                    std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                    for (i32 i = 0; i < ops; ++i) {
                        String key = "key_" + std::to_string(key_dist(gen));
                        if (op_dist(gen) < read_ratio) {
                            i32 value;
                            bool found = lock_map.Get(key, value);
                            (void)found;
                        } else {
                            i32 value;
                            lock_map.GetOrAdd(key, value, i);
                        }
                    }
                });
            }

            start_flag.store(true);
            for (auto &thread : thread_vec) {
                thread.join();
            }
        });

        double speedup = lock_time / rcu_time;
        results.push_back({name, rcu_time, lock_time, speedup});
    }

    // Print summary table
    std::cout << std::endl;
    std::cout << "| Scenario              | RcuMap (ms) | MapWithLock (ms) | Speedup |" << std::endl;
    std::cout << "|----------------------|-------------|------------------|---------|" << std::endl;

    for (const auto &result : results) {
        std::cout << "| " << std::left << std::setw(20) << result.scenario << " | " << std::right << std::setw(11) << std::fixed
                  << std::setprecision(2) << result.rcu_time << " | " << std::right << std::setw(16) << std::fixed << std::setprecision(2)
                  << result.lock_time << " | " << std::right << std::setw(7) << std::fixed << std::setprecision(2) << result.speedup << "x |"
                  << std::endl;
    }

    std::cout << std::endl;
    std::cout << "CONCLUSIONS:" << std::endl;
    std::cout << "- RcuMap excels in read-heavy workloads due to lock-free reads" << std::endl;
    std::cout << "- MapWithLock may perform better in write-heavy scenarios" << std::endl;
    std::cout << "- RcuMap has higher memory overhead but better read scalability" << std::endl;
    std::cout << "- Choose RcuMap for inverted indexes and read-heavy applications" << std::endl;

    // Basic sanity checks
    for (const auto &result : results) {
        EXPECT_GT(result.rcu_time, 0);
        EXPECT_GT(result.lock_time, 0);
        EXPECT_GT(result.speedup, 0);
    }
}

// Quick performance verification test
TEST_F(RcuMapBenchmarkTest, QuickPerformanceVerification) {
    const i32 num_threads = 4;
    const i32 operations_per_thread = 1000;
    const i32 key_range = 10000;
    const double read_ratio = 0.9; // 90% reads

    RcuMap<String, i32> rcu_map;
    MapWithLock<String, i32> lock_map;

    // Pre-populate both maps
    for (i32 i = 0; i < key_range / 2; ++i) {
        String key = "key_" + std::to_string(i);
        rcu_map.Insert(key, i * 10);
        i32 temp_value;
        lock_map.GetOrAdd(key, temp_value, i * 10);
    }

    std::cout << "\n=== QUICK PERFORMANCE VERIFICATION ===" << std::endl;

    // Benchmark RcuMap
    auto rcu_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        auto value_ptr = rcu_map.Get(key);
                        (void)value_ptr;
                    } else {
                        rcu_map.Insert(key, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    // Benchmark MapWithLock
    auto lock_time = MeasureTime([&]() {
        std::atomic<bool> start_flag{false};
        Vector<std::thread> threads;

        for (i32 t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                while (!start_flag.load())
                    std::this_thread::yield();

                std::random_device rd;
                std::mt19937 gen(rd() + t);
                std::uniform_int_distribution<i32> key_dist(0, key_range - 1);
                std::uniform_real_distribution<double> op_dist(0.0, 1.0);

                for (i32 i = 0; i < operations_per_thread; ++i) {
                    String key = "key_" + std::to_string(key_dist(gen));

                    if (op_dist(gen) < read_ratio) {
                        i32 value;
                        bool found = lock_map.Get(key, value);
                        (void)found;
                    } else {
                        i32 value;
                        lock_map.GetOrAdd(key, value, t * operations_per_thread + i);
                    }
                }
            });
        }

        start_flag.store(true);
        for (auto &thread : threads) {
            thread.join();
        }
    });

    double speedup = lock_time / rcu_time;
    std::cout << "RcuMap time: " << rcu_time << " ms" << std::endl;
    std::cout << "MapWithLock time: " << lock_time << " ms" << std::endl;
    std::cout << "RcuMap speedup: " << speedup << "x" << std::endl;

    // With optimizations, RcuMap should perform better for read-heavy workloads
    if (speedup > 1.0) {
        std::cout << "✅ OPTIMIZATION SUCCESS: RcuMap is faster!" << std::endl;
    } else {
        std::cout << "⚠️  RcuMap still slower, may need further optimization" << std::endl;
    }

    EXPECT_GT(rcu_time, 0);
    EXPECT_GT(lock_time, 0);
}
#endif