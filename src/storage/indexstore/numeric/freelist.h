// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "common/types/internal_types.h"
#include <map>

namespace infinity {

struct Freelist {
    // The freelist maps page-id to number of free pages (usually 1)
    typedef std::map<uint64_t, size_t> FreeMap;

    // Constructor
    Freelist() {
        Clear();
    }

    // Clears the internal state
    void Clear() {
        freelist_hits_ = 0;
        freelist_misses_ = 0;
        free_pages_.clear();
    }

    // Returns true if the freelist is empty
    bool Empty() const {
        return free_pages_.empty();
    }

    // Encodes the freelist's state in |data|. Returns a bool which is set to
    // true if there is additional data, or false if the whole state was
    // encoded.
    // Set |cont.first| to false for the first call.
    std::pair<bool, Freelist::FreeMap::const_iterator>
    EncodeState(std::pair<bool, Freelist::FreeMap::const_iterator> cont, uint8_t *data, size_t data_size);

    // Decodes the freelist's state from raw data and adds it to the internal
    // map
    void DecodeState(uint8_t *data);

    // Allocates |num_pages| sequential pages from the freelist; returns the
    // page id of the first page, or 0 if not successfull
    uint64_t Alloc(size_t num_pages);

    // Stores a page in the freelist
    void Put(uint64_t page_id, size_t page_count);

    // Returns true if a page is in the freelist
    bool Has(uint64_t page_id) const;

    // Tries to truncate the file by counting how many pages at the file's end
    // are unused. Returns the address of the last unused page, or |file_size|
    // if there are no unused pages at the end.
    uint64_t Truncate(uint64_t file_size);

    // The map with free pages
    FreeMap free_pages_;

    // number of successful freelist hits
    uint64_t freelist_hits_;

    // number of freelist misses
    uint64_t freelist_misses_;
};

}