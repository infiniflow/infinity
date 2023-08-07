// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#include "page_manager.h"
#include "page_io.h"
#include "btree_node.h"
#include "btree_node_proxy.h"
#include "common/utility/spinlock.h"

namespace infinity {
PageManagerState::PageManagerState() {

}

PageManagerState::~PageManagerState() {

}

PageManager::PageManager() {

}

void
PageManager::Initialize(uint64_t pageid) {
    Context context;

    state_->freelist_.Clear();

    if (state_->state_page_)
        delete state_->state_page_;
    state_->state_page_ = new Page(state_->file_);
    state_->state_page_->Fetch(pageid);

    Page *page = state_->state_page_;

    // the first page stores the page ID of the last blob
    state_->last_blob_page_id_ = *(uint64_t *)page->Payload();

    while (1) {
        assert(page->Type() == Page::kTypePageManager);
        uint8_t *p = page->Payload();
        // skip state_->last_blob_page_id?
        if (page == state_->state_page_)
            p += sizeof(uint64_t);

        // get the overflow address
        uint64_t overflow = *(uint64_t *)p;
        p += 8;

        state_->freelist_.DecodeState(p);

        // load the overflow page
        if (overflow)
            page = Fetch(&context, overflow, 0);
        else
            break;
    }
}

Page*
PageManager::Alloc(Context *context, uint32_t page_type, uint32_t flags) {
    ScopedSpinLock lock(state_->mutex_);
    return AllocUnlocked(context, page_type, flags);
}

Page*
PageManager::AllocUnlocked(Context *context, uint32_t page_type, uint32_t flags) {
    uint64_t address = 0;
    Page *page = 0;
    uint32_t page_size = Page::kSize;
    bool allocated = false;

    /* first check the internal list for a free page */
    if (NOTSET(flags, PageManager::kIgnoreFreelist)) {
        address = state_->freelist_.Alloc(1);

        if (address != 0) {
            assert(address % page_size == 0);
            state_->needs_flush_ = true;

            /* try to fetch the page from the cache_ */
            page = state_->cache_.Get(address);
            if (page)
                goto done;
            /* otherwise fetch the page from disk */
            page = new Page(state_->file_);
            page->Fetch(address);
            goto done;
        }
    }

    try {
        if (!page) {
            allocated = true;
            page = new Page(state_->file_);
        }

        page->Alloc(page_type);
    } catch (Exception &ex) {
        if (allocated)
            delete page;
        throw ex;
    }

done:
    /* clear the page with zeroes?  */
    if (ISSET(flags, PageManager::kClearWithZero))
        ::memset(page->Data(), 0, page_size);

    /* initialize the page; also set the 'dirty' flag to force logging */
    page->SetType(page_type);
    page->SetDirty(true);
    //page->set_db(context->db);
    page->SetWithoutHeader(false);
    page->SetCrc32(0);

    if (page->NodeProxy()) {
        delete page->NodeProxy();
        page->SetNodeProxy(0);
    }

    /* store the page in the cache_ and the Changeset */
    state_->cache_.Put(page);
    AddToChangeSet(&context->changeset_, page);

    /* write to disk (if necessary) */
    if (NOTSET(flags, PageManager::kDisableStoreState)
            && NOTSET(flags, PageManager::kReadOnly))
        //maybe_store_state(state_, context, false);

        switch (page_type) {
        case Page::kTypeBindex:
        case Page::kTypeBroot: {
            ::memset(page->Payload(), 0, sizeof(BtreeNode));
            state_->page_count_index_++;
            break;
        }
        case Page::kTypePageManager:
            state_->page_count_page_manager_++;
            break;
        case Page::kTypeBlob:
            state_->page_count_blob_++;
            break;
        default:
            break;
        }

    return page;
}

Page*
PageManager::Fetch(Context *context, uint32_t address, uint32_t flags) {
    ScopedSpinLock lock(state_->mutex_);
    return FetchUnlocked(context, address, flags);
}

Page *
PageManager::FetchUnlocked(Context *context, uint64_t address, uint32_t flags) {
    /* fetch the page from the cache_ */
    Page *page;

    if (address == 0)
        page = state_->header_->header_page_;
    else if (state_->state_page_ && address == state_->state_page_->Address())
        page = state_->state_page_;
    else
        page = state_->cache_.Get(address);

    if (page) {
        page->SetWithoutHeader(ISSET(flags, PageManager::kNoHeader));
        return AddToChangeSet(&context->changeset_, page);
    }

    page = new Page(state_->file_);
    try {
        page->Fetch(address);
    } catch (Exception &ex) {
        delete page;
        throw ex;
    }

    assert(page->Data());

    /* store the page in the list */
    state_->cache_.Put(page);

    /* write state_ to disk (if necessary) */
    if (NOTSET(flags, PageManager::kDisableStoreState)
            && NOTSET(flags, PageManager::kReadOnly))
        MaybeStoreState(context, false);

    /* only verify crc if the page has a header */
    page->SetWithoutHeader(ISSET(flags, PageManager::kNoHeader));

    state_->page_count_fetched_++;
    return AddToChangeSet(&context->changeset_, page);
}

uint64_t
PageManager::StoreStateImpl(Context *context) {
    // no modifications? then simply return the old blobid
    if (!state_->needs_flush_)
        return state_->state_page_ ? state_->state_page_->Address() : 0;

    state_->needs_flush_ = false;

    // no freelist pages, no freelist state_? then don't store anything
    if (!state_->state_page_ && state_->freelist_.Empty())
        return 0;

    // otherwise allocate a new page, if required
    if (!state_->state_page_) {
        state_->state_page_ = new Page(state_->file_);
        state_->state_page_->Alloc(Page::kTypePageManager, Page::kInitializeWithZeroes);
    }

    // don't bother locking the state_ page; it will never be accessed by
    // the worker thread because it's not stored in the cache_
    context->changeset_.Put(state_->state_page_);

    state_->state_page_->SetDirty(true);

    Page *page = state_->state_page_;
    uint8_t *p = page->Payload();

    // store page-ID of the last allocated blob
    *(uint64_t *)p = state_->last_blob_page_id_;
    p += sizeof(uint64_t);

    // reset the overflow pointer and the counter
    // TODO here we lose a whole chain of overflow pointers if there was such
    // a chain. We only save the first. That's not critical but also not nice.
    uint64_t next_pageid = *(uint64_t *)p;
    if (next_pageid) {
        state_->freelist_.Put(next_pageid, 1);
        *(uint64_t *)p = 0;
    }

    // No freelist entries? then we're done. Make sure that there's no
    // overflow pointer or other garbage in the page!
    if (state_->freelist_.Empty()) {
        p += sizeof(uint64_t);
        *(uint32_t *)p = 0;
        return state_->state_page_->Address();
    }

    std::pair<bool, Freelist::FreeMap::const_iterator> continuation;
    continuation.first = false;   // initialization
    continuation.second = state_->freelist_.free_pages_.end();
    do {
        int offset = page == state_->state_page_
                     ? sizeof(uint64_t)
                     : 0;
        continuation = state_->freelist_.EncodeState(continuation,
                       page->Payload() + offset,
                       Page::kSize - Page::kSizeofPersistentHeader - offset);

        if (continuation.first == false)
            break;

        // load the next page
        if (!next_pageid) {
            Page *new_page = AllocUnlocked(context, Page::kTypePageManager, PageManager::kIgnoreFreelist);
            // patch the overflow pointer in the old (current) page
            p = page->Payload() + offset;
            *(uint64_t *)p = new_page->Address();

            // reset the overflow pointer in the new page
            page = new_page;
            p = page->Payload();
            *(uint64_t *)p = 0;
        } else {
            page = FetchUnlocked(context, next_pageid, 0);
            p = page->Payload();
        }

        // make sure that the page is logged
        page->SetDirty(true);
    } while (true);

    return state_->state_page_->Address();
}

void PageManager::MaybeStoreState(Context *context, bool force) {
    if (force/* || state_->env->journal.get()*/) {
        uint64_t new_blobid = StoreStateImpl(context);
        if (new_blobid != state_->header_->PageManagerBlobid()) {
            state_->header_->SetPageManagerBlobid(new_blobid);
            // don't bother to lock the header page
            state_->header_->header_page_->SetDirty(true);
            context->changeset_.Put(state_->header_->header_page_);
        }
    }
}

void
PageManager::Del(Context *context, Page *page, size_t page_count) {
    assert(page_count > 0);

    ScopedSpinLock lock(state_->mutex_);

    // remove the page(s) from the changeset
    context->changeset_.Del(page);
    if (page_count > 1) {
        uint32_t page_size = Page::kSize;
        for (size_t i = 1; i < page_count; i++) {
            Page *p = state_->cache_.Get(page->Address() + i * page_size);
            if (p && context->changeset_.Has(p))
                context->changeset_.Del(p);
        }
    }

    state_->needs_flush_ = true;
    state_->freelist_.Put(page->Address(), page_count);
    assert(page->Address() % Page::kSize == 0);

    if (page->NodeProxy()) {
        delete page->NodeProxy();
        page->SetNodeProxy(0);
    }

    // do not call maybe_store_state() - this change in the state_ is not
    // relevant for logging.
}


void
PageManager::SetLastBlobPage(Page *page) {
    ScopedSpinLock lock(state_->mutex_);
    state_->last_blob_page_id_ = page ? page->Address() : 0;
    state_->last_blob_page_ = page;
}


Page *
PageManager::LastBlobPage(Context *context) {
    ScopedSpinLock lock(state_->mutex_);

    if (state_->last_blob_page_)
        return AddToChangeSet(&context->changeset_, state_->last_blob_page_);
    if (state_->last_blob_page_id_)
        return FetchUnlocked(context, state_->last_blob_page_id_, 0);
    return 0;
}

uint64_t
PageManager::LastBlobPageId() {
    ScopedSpinLock lock(state_->mutex_);
    if (state_->last_blob_page_id_)
        return state_->last_blob_page_id_;
    if (state_->last_blob_page_)
        return state_->last_blob_page_->Address();
    return 0;
}

void
PageManager::ReclaimSpace(Context *context) {
    ScopedSpinLock lock(state_->mutex_);

    if (state_->last_blob_page_) {
        state_->last_blob_page_id_ = state_->last_blob_page_->Address();
        state_->last_blob_page_ = 0;
    }

    bool do_truncate = false;
    uint32_t page_size = Page::kSize;
    uint64_t file_size = state_->file_->FileSize();
    uint64_t address = state_->freelist_.Truncate(file_size);

    if (address < file_size) {
        for (uint64_t page_id = address;
                page_id <= file_size - page_size;
                page_id += page_size) {
            Page *page = state_->cache_.Get(page_id);
            if (page) {
                state_->cache_.Del(page);
                delete page;
            }
        }

        do_truncate = true;
        file_size = address;
    }

    if (do_truncate) {
        state_->needs_flush_ = true;
        state_->file_->Truncate(file_size);
        MaybeStoreState(context, true);
    }
}


void
PageManager::PurgeCache(Context *context) {
    ScopedSpinLock lock(state_->mutex_);


    //if (unlikely(!state_->message))
    //  state_->message = new AsyncFlushMessage(this, state_->device, 0);

    //state_->message->page_ids.clear();
    state_->garbage_.clear();

    //state_->cache_.PurgeCandidates(state_->message->page_ids, state_->garbage_, state_->last_blob_page_);

    // don't bother if there are only few pages
    //if (state_->message->page_ids.size() > 10) {
    //  state_->message->in_progress = true;
    //  run_async(boost::bind(&async_flush_pages, state_->message));
    //}

    for (std::vector<Page *>::iterator it = state_->garbage_.begin();
            it != state_->garbage_.end();
            it++) {
        Page *page = *it;
        if (likely(page->Mutex().try_lock())) {
            state_->cache_.Del(page);
            page->Mutex().unlock();
            delete page;
        }
    }
}

Page *
PageManager::AllocMultipleBlobPages(Context *context, size_t num_pages) {
    ScopedSpinLock lock(state_->mutex_);

    // allocate only one page? then use the normal ::alloc() method
    if (num_pages == 1)
        return AllocUnlocked(context, Page::kTypeBlob, 0);

    Page *page = 0;
    uint32_t page_size = Page::kSize;

    // Now check the freelist
    uint64_t address = state_->freelist_.Alloc(num_pages);
    if (address != 0) {
        for (size_t i = 0; i < num_pages; i++) {
            if (i == 0) {
                page = FetchUnlocked(context, address, 0);
                page->SetType(Page::kTypeBlob);
            } else {
                Page *p = FetchUnlocked(context, address + (i * page_size), PageManager::kNoHeader);
                p->SetType(Page::kTypeBlob);
            }
        }

        return page;
    }

    // Freelist lookup was not successful -> allocate new pages. Only the first
    // page is a regular page; all others do not have page headers.
    //
    // disable "store state": the PageManager otherwise could alloc overflow
    // pages in the middle of our blob sequence.
    uint32_t flags = PageManager::kIgnoreFreelist | PageManager::kDisableStoreState;
    for (size_t i = 0; i < num_pages; i++) {
        if (page == 0)
            page = AllocUnlocked(context, Page::kTypeBlob, flags);
        else {
            Page *p = AllocUnlocked(context, Page::kTypeBlob, flags);
            p->SetWithoutHeader(true);
        }
    }

    // now store the state
    MaybeStoreState(context, false);
    return page;
}
}