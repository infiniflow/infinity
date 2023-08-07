#pragma once

#include "page.h"
#include "freelist.h"
#include "cache.h"
#include "context.h"

#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>


namespace infinity {

typedef struct {
    // magic_ cookie - always "ham\0"
    uint8_t magic_[4];

    // version_ information - major, minor, rev, file
    uint8_t version_[4];

    // reserved
    uint64_t reserved1_;

    // size of the page
    uint32_t page_size_;

    // maximum number of databases in this environment
    uint16_t max_databases_;

    // for storing journal compression algorithm
    uint8_t journal_compression_;

    // reserved
    uint8_t reserved2_;

    // blob id of the PageManager's state
    uint64_t page_manager_blobid_;

} __attribute__ ((packed)) EnvironmentHeader;


struct EnvHeader {
    // Constructor
    EnvHeader(Page *page)
        : header_page_(page) {
    }

    // Sets the 'magic_' field of a file Header
    void SetMagic(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4) {
        Header()->magic_[0] = m1;
        Header()->magic_[1] = m2;
        Header()->magic_[2] = m3;
        Header()->magic_[3] = m4;
    }

    // Returns true if the magic_ matches
    bool VerifyMagic(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4) {
        if (Header()->magic_[0] != m1)
            return false;
        if (Header()->magic_[1] != m2)
            return false;
        if (Header()->magic_[2] != m3)
            return false;
        if (Header()->magic_[3] != m4)
            return false;
        return true;
    }

    // Returns byte |i| of the 'version_'-Header
    uint8_t Version(int i) {
        return Header()->version_[i];
    }

    // Sets the version_ of a file Header
    void SetVersion(uint8_t major, uint8_t minor, uint8_t revision, uint8_t file) {
        Header()->version_[0] = major;
        Header()->version_[1] = minor;
        Header()->version_[2] = revision;
        Header()->version_[3] = file;
    }

    // Returns get the maximum number of databases for this file
    uint16_t MaxDatabases() {
        return Header()->max_databases_;
    }

    // Sets the maximum number of databases for this file
    void SetMaxDatabases(uint16_t max_databases) {
        Header()->max_databases_ = max_databases;
    }

    // Returns the page size from the Header page
    uint32_t PageSize() {
        return Header()->page_size_;
    }

    // Sets the page size in the Header page
    void SetPageSize(uint32_t page_size) {
        Header()->page_size_ = page_size;
    }

    // Returns the PageManager's blob id
    uint64_t PageManagerBlobid() {
        return Header()->page_manager_blobid_;
    }

    // Sets the page size in the Header page
    void SetPageManagerBlobid(uint64_t blobid) {
        Header()->page_manager_blobid_ = blobid;
    }

    // Returns the Journal compression configuration
    int JournalCompression() {
        return Header()->journal_compression_ >> 4;
    }

    // Sets the Journal compression configuration
    void SetJournalCompression(int algorithm) {
        Header()->journal_compression_ = algorithm << 4;
    }

    // Returns a pointer to the Header data
    EnvironmentHeader *Header() {
        return (EnvironmentHeader *)(header_page_->Payload());
    }

    // The Header page of the Environment
    Page *header_page_;
};

struct PageManagerState {
    // constructor
    PageManagerState();

    // destructor
    ~PageManagerState();

    //  For serializing access
    SpinLock mutex_;

    EnvHeader *header_;

    // The File
    File *file_;

    // The lsn manager
    //LsnManager *lsn_manager;

    // The cache
    Cache cache_;

    // The freelist
    Freelist freelist_;

    // Whether |m_free_pages| must be flushed or not
    bool needs_flush_;

    // Page with the persisted state data. If multiple pages are allocated
    // then these pages form a linked list, with |m_state_page| being the head
    Page *state_page_;

    // Cached page where to add more blobs
    Page *last_blob_page_;

    // Page where to add more blobs - if |m_last_blob_page| was flushed
    uint64_t last_blob_page_id_;

    // tracks number of fetched pages
    uint64_t page_count_fetched_;

    // tracks number of index pages
    uint64_t page_count_index_;

    // tracks number of blob pages
    uint64_t page_count_blob_;

    // tracks number of page manager pages
    uint64_t page_count_page_manager_;

    // tracks number of cache hits
    uint64_t cache_hits_;

    // tracks number of cache misses
    uint64_t cache_misses_;

    // For sending information to the worker thread; cached to avoid memory
    // allocations
    //AsyncFlushMessage *message;

    // For collecting unused pages; cached to avoid memory allocations
    std::vector<Page *> garbage_;

    // The worker thread which flushes dirty pages
    //ScopedPtr<WorkerPool> worker;
};


class PageManager {
public:
    PageManager();

    enum {
        // flag for alloc(): Clear the full page with zeroes
        kClearWithZero     = 1,

        // flag for alloc(): Ignores the freelist
        kIgnoreFreelist    = 2,

        // flag for alloc(): Do not persist the PageManager state to disk
        kDisableStoreState = 4,

        // Flag for fetch(): only fetches from cache, not from disk
        kOnlyFromCache = 1,

        // Flag for fetch(): does not add page to the ChangeSet
        kReadOnly = 2,

        // Flag for fetch(): page is part of a multi-page blob, has no Header
        kNoHeader = 4
    };

    void Initialize(uint64_t pageid);

    Page* Alloc(Context *context, uint32_t page_type, uint32_t flags = 0);

    Page* Fetch(Context *context, uint32_t address, uint32_t flags = 0);

    void Del(Context *context, Page *page, size_t page_count = 1);

    void SetLastBlobPage(Page *page);

    Page * LastBlobPage(Context *context);

    uint64_t LastBlobPageId();

    void ReclaimSpace(Context *context);

    void PurgeCache(Context *context);

    Page * AllocMultipleBlobPages(Context *context, size_t num_pages);
private:
    Page* AllocUnlocked(Context *context, uint32_t page_type, uint32_t flags);

    Page * FetchUnlocked(Context *context, uint64_t address, uint32_t flags);

    Page * AddToChangeSet(ChangeSet *changeset, Page *page) {
        changeset->Put(page);
        assert(page->Mutex().try_lock() == false);
        return page;
    }

    void MaybeStoreState(Context *context, bool force);

    uint64_t StoreStateImpl(Context *context);

public:
    std::unique_ptr<PageManagerState> state_;
};
}