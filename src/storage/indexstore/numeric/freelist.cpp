// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#include "freelist.h"
#include "page.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct Pickle {
    /* encodes a uint64 number and stores it in |p|; returns the number of
     * bytes used */
    static size_t EncodeU64(uint8_t *p, uint64_t n) {
        if (n <= 0xf) {
            *p = (uint8_t)n;
            return 1;
        }
        if (n <= 0xff) {
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 2;
        }
        if (n <= 0xfff) {
            *(p + 2) = (n & 0xf00) >> 8;
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 3;
        }
        if (n <= 0xffff) {
            *(p + 3) = (n & 0xf000) >> 12;
            *(p + 2) = (n & 0xf00) >> 8;
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 4;
        }
        if (n <= 0xfffff) {
            *(p + 4) = (n & 0xf0000) >> 16;
            *(p + 3) = (n & 0xf000) >> 12;
            *(p + 2) = (n & 0xf00) >> 8;
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 5;
        }
        if (n <= 0xffffff) {
            *(p + 5) = (n & 0xf00000) >> 24;
            *(p + 4) = (n & 0xf0000) >> 16;
            *(p + 3) = (n & 0xf000) >> 12;
            *(p + 2) = (n & 0xf00) >> 8;
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 6;
        }
        if (n <= 0xfffffff) {
            *(p + 6) = (n & 0xf000000) >> 32;
            *(p + 5) = (n & 0xf00000) >> 24;
            *(p + 4) = (n & 0xf0000) >> 16;
            *(p + 3) = (n & 0xf000) >> 12;
            *(p + 2) = (n & 0xf00) >> 8;
            *(p + 1) = (n & 0xf0) >> 4;
            *(p + 0) = n & 0xf;
            return 7;
        }
        *(p + 7) = (n & 0xf0000000) >> 36;
        *(p + 6) = (n & 0xf000000) >> 32;
        *(p + 5) = (n & 0xf00000) >> 24;
        *(p + 4) = (n & 0xf0000) >> 16;
        *(p + 3) = (n & 0xf000) >> 12;
        *(p + 2) = (n & 0xf00) >> 8;
        *(p + 1) = (n & 0xf0) >> 4;
        *(p + 0) = n & 0xf;
        return 8;
    }

    /* decodes and returns a pickled number of |len| bytes */
    static uint64_t DecodeU64(size_t len, uint8_t *p) {
        uint64_t ret = 0;

        for (size_t i = 0; i < len - 1; i++) {
            ret += *(p + (len - i - 1));
            ret <<= 4;
        }

        // last assignment is without *= 10
        return ret + *p;
    }
};

std::pair<bool, Freelist::FreeMap::const_iterator>
Freelist::EncodeState(std::pair<bool, Freelist::FreeMap::const_iterator> cont, uint8_t *data, size_t data_size) {
    uint32_t page_size = Page::kSize;
    Freelist::FreeMap::const_iterator it = cont.second;
    if (cont.first == false)
        it = free_pages_.begin();
    else
        StorageAssert(it != free_pages_.end(), "it should not be equals to free_pages_.end()");

    uint32_t counter = 0;
    uint8_t *p = data;
    p += 8;   // leave room for the pointer to the next page
    p += 4;   // leave room for the counter

    while (it != free_pages_.end()) {
        // 9 bytes is the maximum amount of storage that we will need for a
        // new entry; if it does not fit then break
        if ((p + 9) - data >= (ptrdiff_t)data_size)
            break;

        // check if the next entry (and the following) are adjacent; if yes then
        // they are merged. Up to 16 pages can be merged.
        uint32_t page_counter = 1;
        uint64_t base = it->first;
        uint64_t current = it->first;

        // move to the next entry, then merge all adjacent pages
        for (it++; it != free_pages_.end() && page_counter < 16 - 1; it++) {
            if (it->first != current + page_size)
                break;
            current += page_size;
            page_counter++;
        }

        // now |base| is the start of a sequence of free pages, and the
        // sequence has |page_counter| pages
        //
        // This is encoded as
        // - 1 byte header
        //   - 4 bits for |page_counter|
        //   - 4 bits for the number of bytes following ("n")
        // - n byte page-id (div page_size)
        int num_bytes = Pickle::EncodeU64(p + 1, base / page_size);
        *p = (page_counter << 4) | num_bytes;
        p += 1 + num_bytes;

        counter++;
    }

    // now store the counter
    *(uint32_t *)(data + 8) = counter;

    std::pair<bool, Freelist::FreeMap::const_iterator> retval;
    retval.first = (it != free_pages_.end());
    retval.second = it;
    return retval;
}

void
Freelist::DecodeState(uint8_t *data) {
    uint32_t page_size = Page::kSize;

    // get the number of stored elements
    uint32_t counter = *(uint32_t *)data;
    data += 4;

    // now read all pages
    for (uint32_t i = 0; i < counter; i++) {
        // 4 bits page_counter, 4 bits for number of following bytes
        int page_counter = (*data & 0xf0) >> 4;
        int num_bytes = *data & 0x0f;
        assert(page_counter > 0);
        assert(num_bytes <= 8);
        data += 1;

        uint64_t id = Pickle::DecodeU64(num_bytes, data);
        data += num_bytes;

        free_pages_[id * page_size] = page_counter;
    }
}

uint64_t
Freelist::Alloc(size_t num_pages) {
    uint64_t address = 0;
    uint32_t page_size = Page::kSize;

    for (FreeMap::iterator it = free_pages_.begin();
            it != free_pages_.end();
            it++) {
        if (it->second == num_pages) {
            address = it->first;
            free_pages_.erase(it);
            break;
        }
        if (it->second > num_pages) {
            address = it->first;
            free_pages_[it->first + num_pages * page_size] = it->second - num_pages;
            free_pages_.erase(it);
            break;
        }
    }

    if (address != 0)
        freelist_hits_++;
    else
        freelist_misses_++;

    return address;
}

void
Freelist::Put(uint64_t page_id, size_t page_count) {
    free_pages_[page_id] = page_count;
}

bool
Freelist::Has(uint64_t page_id) const {
    return free_pages_.find(page_id) != free_pages_.end();
}

uint64_t
Freelist::Truncate(uint64_t file_size) {
    uint32_t page_size = Page::kSize;
    uint64_t lower_bound = file_size;

    if (free_pages_.empty())
        return file_size;

    for (FreeMap::reverse_iterator it = free_pages_.rbegin();
            it != free_pages_.rend();
            it++) {
        if (it->first + it->second * page_size == lower_bound)
            lower_bound = it->first;
        else
            break;
    }

    // remove all truncated pages
    while (!free_pages_.empty() && free_pages_.rbegin()->first >= lower_bound) {
        free_pages_.erase(free_pages_.rbegin()->first);
    }

    return lower_bound;
}

}