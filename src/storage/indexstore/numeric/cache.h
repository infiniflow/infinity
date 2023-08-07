// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "page.h"
#include "page_collection.h"

namespace infinity {

static constexpr uint32_t default_cache_size = 2 * 1024 * 1024;

struct CacheState {
    typedef PageCollection<Page::kListBucket> CacheLine;

    enum {
        // The number of buckets_ should be a prime number or similar, as it
        // is used in a MODULO hash scheme
        kBucketSize = 10317,
    };

    CacheState()
        : capacity_bytes_(default_cache_size),
          page_size_bytes_(Page::kSize), alloc_elements_(0),
          buckets_(kBucketSize), cache_hits_(0), cache_misses_(0) {
    }

    // the capacity (in bytes)
    uint64_t capacity_bytes_;

    // the current page size (in bytes)
    uint64_t page_size_bytes_;

    // the current number of cached elements that were allocated (and not
    // mapped)
    size_t alloc_elements_;

    // linked list of ALL cached pages
    PageCollection<Page::kListCache> totallist_;

    // The hash table buckets_ - each is a linked list of Page pointers
    std::vector<CacheLine> buckets_;

    // counts the cache hits
    uint64_t cache_hits_;

    // counts the cache misses
    uint64_t cache_misses_;
};

namespace detail {
static inline size_t
CalcHash(uint64_t value) {
    return (size_t)(value % CacheState::kBucketSize);
}
} // namespace detail

struct Cache {
    template<typename Purger>
    struct PurgeIfSelector {
        PurgeIfSelector(Cache *cache, Purger &purger)
            : cache_(cache), purger_(purger) {
        }

        bool operator()(Page *page) {
            if (purger_(page))
                cache_->Del(page);
            // don't remove page from list; it was already removed above
            return false;
        }

        Cache *cache_;
        Purger &purger_;
    };

    // The default constructor
    Cache() {
    }

    // Retrieves a page from the cache, also removes the page from the cache
    // and re-inserts it at the front. Returns null if the page was not cached.
    Page *Get(uint64_t address) {
        size_t hash = detail::CalcHash(address);

        Page *page = state_.buckets_[hash].Get(address);
        if (!page) {
            state_.cache_misses_++;
            return 0;
        }

        // Now re-insert the page at the head of the "totallist_", and
        // thus move far away from the tail. The pages at the tail are highest
        // candidates to be deleted when the cache is purged.
        state_.totallist_.Del(page);
        state_.totallist_.Put(page);
        state_.cache_hits_++;
        return page;
    }

    // Stores a page in the cache
    void Put(Page *page) {
        size_t hash = detail::CalcHash(page->Address());

        /* First remove the page from the cache, if it's already cached
         *
         * Then re-insert the page at the head of the list. The tail will
         * point to the least recently used page.
         */
        state_.totallist_.Del(page);
        state_.totallist_.Put(page);
        if (page->IsAllocated())
            state_.alloc_elements_++;

        state_.buckets_[hash].Put(page);
    }

    // Removes a page from the cache
    void Del(Page *page) {
        assert(page->Address() != 0);

        /* remove it from the list of all cached pages */
        if (state_.totallist_.Del(page) && page->IsAllocated())
            state_.alloc_elements_--;

        /* remove the page from the cache buckets_ */
        size_t hash = detail::CalcHash(page->Address());
        state_.buckets_[hash].Del(page);
    }

    // Purges the cache. Implements a LRU eviction algorithm. Dirty pages are
    // forwarded to the |processor()| for flushing.
    // The |ignore_page| is passed by the caller; this page will not be purged
    // under any circumstance. This is used by the PageManager to make sure
    // that the "last blob page" is not evicted by the cache.
    void PurgeCandidates(std::vector<uint64_t> &candidates,
                         std::vector<Page *> &garbage,
                         Page *ignore_page) {
        int limit = (int)(CurrentElements()
                          - (state_.capacity_bytes_ / state_.page_size_bytes_));

        Page *page = state_.totallist_.Tail();
        for (int i = 0; i < limit && page != 0; i++) {
            if (page->Mutex().try_lock()) {
                if (//page->cursor_list_.Size() == 0 &&
                    page != ignore_page &&
                    page->Type() != Page::kTypeBroot) {
                    if (page->IsDirty())
                        candidates.push_back(page->Address());
                    else
                        garbage.push_back(page);
                }
                page->Mutex().unlock();
            }

            page = page->Previous(Page::kListCache);
        }
    }

    // Visits all pages in the "totallist_". If |cb| returns true then the
    // page is removed and deleted. This is used by the Environment
    // to flush (and delete) pages.
    template<typename Purger>
    void PurgeIf(Purger &purger) {
        PurgeIfSelector<Purger> selector(this, purger);
        state_.totallist_.Extract(selector);
    }

    // Returns true if the capacity limits are exceeded
    bool IsCacheFull() const {
        return state_.totallist_.Size() * state_.page_size_bytes_
               > state_.capacity_bytes_;
    }

    // Returns the capacity (in bytes)
    uint64_t Capacity() const {
        return state_.capacity_bytes_;
    }

    // Returns the number of currently cached elements
    size_t CurrentElements() const {
        return state_.totallist_.Size();
    }

    // Returns the number of currently cached elements (excluding those that
    // are mmapped)
    size_t AllocatedElements() const {
        return state_.alloc_elements_;
    }

    CacheState state_;
};

}