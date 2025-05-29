#include <gtest/gtest.h>

#include "utils/lru_cache.hpp"

using cpptrace::detail::lru_cache;
using cpptrace::detail::nullopt;

namespace {

TEST(LruCacheTest, DefaultConstructor) {
    lru_cache<int, int> cache;
    EXPECT_EQ(cache.size(), 0);
}

TEST(LruCacheTest, MaybeGet) {
    lru_cache<int, int> cache;
    auto result = cache.maybe_get(42);
    EXPECT_FALSE(result.has_value());
}

TEST(LruCacheTest, InsertAndGet) {
    lru_cache<int, int> cache;
    cache.insert(42, 50);
    auto result = cache.maybe_get(42);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.unwrap(), 50);
}

TEST(LruCacheTest, ConstGet) {
    lru_cache<int, int> cache;
    cache.insert(42, 50);
    const lru_cache<int, int>& cache_ref = cache;
    auto result = cache_ref.maybe_get(42);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.unwrap(), 50);
}

TEST(LruCacheTest, Set) {
    lru_cache<int, int> cache;
    cache.set(42, 50);
    auto result = cache.maybe_get(42);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.unwrap(), 50);
    cache.set(42, 60);
    auto result2 = cache.maybe_get(42);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.unwrap(), 60);
}

TEST(LruCacheTest, NoMaxSize) {
    lru_cache<int, int> cache;
    for(int i = 0; i < 1000; i++) {
        cache.insert(i, i + 50);
    }
    EXPECT_EQ(cache.size(), 1000);
    for(int i = 0; i < 1000; i++) {
        EXPECT_EQ(cache.maybe_get(i).unwrap(), i + 50);
    }
}

TEST(LruCacheTest, MaxSize) {
    lru_cache<int, int> cache(20);
    for(int i = 0; i < 1000; i++) {
        cache.insert(i, i + 50);
    }
    EXPECT_EQ(cache.size(), 20);
    for(int i = 0; i < 1000 - 20; i++) {
        EXPECT_FALSE(cache.maybe_get(i).has_value());
    }
    for(int i = 1000 - 20; i < 1000; i++) {
        EXPECT_EQ(cache.maybe_get(i).unwrap(), i + 50);
    }
}

TEST(LruCacheTest, SizeAfterInserts) {
    lru_cache<int, int> cache;
    for(int i = 0; i < 1000; i++) {
        cache.insert(i, i + 50);
    }
    EXPECT_EQ(cache.size(), 1000);
    cache.set_max_size(20);
    EXPECT_EQ(cache.size(), 20);
    for(int i = 0; i < 1000 - 20; i++) {
        EXPECT_FALSE(cache.maybe_get(i).has_value());
    }
    for(int i = 1000 - 20; i < 1000; i++) {
        EXPECT_EQ(cache.maybe_get(i).unwrap(), i + 50);
    }
}

TEST(LruCacheTest, Touch) {
    lru_cache<int, int> cache(20);
    for(int i = 0; i < 1000; i++) {
        cache.maybe_touch(0);
        cache.insert(i, i + 50);
    }
    EXPECT_EQ(cache.size(), 20);
    for(int i = 1000 - 19; i < 1000; i++) {
        EXPECT_EQ(cache.maybe_get(i).unwrap(), i + 50);
    }
    EXPECT_EQ(cache.maybe_get(0).unwrap(), 50);
}

}
