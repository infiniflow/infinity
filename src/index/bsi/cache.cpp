#include "cache.h"
#include <concurrentqueue.h>

namespace infinity {

static bool g_bitmap_cache_enable = true;
static moodycamel::ConcurrentQueue<roaring_bitmap_t*> g_bitmap_cache;
roaring_bitmap_t* acquire_bitmap() {
    roaring_bitmap_t* item = nullptr;
    if (g_bitmap_cache.try_dequeue(item)) {
        return item;
    }
    item = roaring_bitmap_create();
    // roaring_bitmap_set_copy_on_write(item, true);
    roaring_bitmap_init_cleared(item);
    return item;
}
void release_bitmap(roaring_bitmap_t* b) {
    if (!g_bitmap_cache_enable) {
        roaring_bitmap_free(b);
        return;
    }
    roaring_bitmap_clear(b);
    g_bitmap_cache.enqueue(b);
}
void clear_bitmap_cache() {
    g_bitmap_cache_enable = false;
    while (1) {
        roaring_bitmap_t* item = nullptr;
        if (g_bitmap_cache.try_dequeue(item)) {
            roaring_bitmap_free(item);
        } else {
            return;
        }
    }
}

void BitMapCacheGuard::Add(roaring_bitmap_t* b) {
    cache_.push_back(b);
}
BitMapCacheGuard::~BitMapCacheGuard() {
    for (roaring_bitmap_t* b : cache_) {
        release_bitmap(b);
    }
}
}  // namespace infinity
