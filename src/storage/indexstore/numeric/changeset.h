#pragma once

#include "page.h"
#include "page_collection.h"

namespace infinity {

struct ChangeSet {
    ChangeSet() {
    }

    Page *Get(uint64_t address) {
        return collection_.Get(address);
    }

    void Put(Page *page) {
        if (!Has(page))
            page->Mutex().lock();
        collection_.Put(page);
    }

    void Del(Page *page) {
        page->Mutex().unlock();
        collection_.Del(page);
    }

    bool Has(Page *page) const {
        return collection_.Has(page);
    }

    bool IsEmpty() const {
        return collection_.IsEmpty();
    }

    void Clear();

    void Flush(uint64_t lsn);

    PageCollection<Page::kListChangeset> collection_;
};

}