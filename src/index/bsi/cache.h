#pragma once
#include <memory>
#include <vector>
#include <roaring/roaring.h>
namespace infinity {

roaring_bitmap_t* acquire_bitmap();
void release_bitmap(roaring_bitmap_t* b);

void clear_bitmap_cache();

class BitMapCacheGuard {
private:
    std::vector<roaring_bitmap_t*> cache_;

public:
    void Add(roaring_bitmap_t* b);
    ~BitMapCacheGuard();
};

}  // namespace infinity