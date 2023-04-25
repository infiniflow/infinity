#pragma once
#include <stdint.h>
#include <stdio.h>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>
#include <roaring/roaring.h>
#include "cache.h"

namespace infinity {

struct CRoaringBitmapDeleter {
    bool ignore_free = false;
    CRoaringBitmapDeleter(bool ignore = false) : ignore_free(ignore) {}
    void operator()(roaring_bitmap_t* ptr) const {
        if (!ignore_free) {
            release_bitmap(ptr);
        }
    }
};

typedef std::unique_ptr<roaring_bitmap_t, CRoaringBitmapDeleter> CRoaringBitmapPtr;

struct RoaringBitmap {
    CRoaringBitmapPtr bitmap;
    char* underly_buf_ = nullptr;
    bool readonly_ = false;
    RoaringBitmap() = default;
    RoaringBitmap(const RoaringBitmap&) = delete;
    RoaringBitmap& operator=(const RoaringBitmap&) = delete;

    void SetCRoaringBitmap(roaring_bitmap_t* b);
    void NewCRoaringBitmap();
    int Save(FILE* fp, bool readonly);
    int Load(FILE* fp);
    bool Put(uint32_t id);
    bool Remove(uint32_t id);
    bool IsReadonly() const {
        return readonly_;
    }
    ~RoaringBitmap();
};

typedef std::unique_ptr<RoaringBitmap> RoaringBitmapPtr;

void bitmap_extract_ids(const roaring_bitmap_t* out, std::vector<uint32_t>& ids);

int file_write_uint32(FILE* fp, uint32_t n);
int file_read_uint32(FILE* fp, uint32_t& n);
}