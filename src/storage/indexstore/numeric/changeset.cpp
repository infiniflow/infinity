#include "changeset.h"

#include <vector>

namespace infinity {

struct UnlockPage {
    bool operator()(Page *page) {
        page->Mutex().unlock();
        return true;
    }
};

struct FlushChangesetVisitor {
    bool operator()(Page *page) {
        assert(page->Mutex().try_lock() == false);

        if (page->IsDirty())
            list_.push_back(page);
        else
            page->Mutex().unlock();
        return true; // remove this page from the PageCollection
    }

    std::vector<Page *> list_;
};

void
ChangeSet::Clear() {
    UnlockPage unlocker;
    collection_.ForEach(unlocker);
    collection_.Clear();
}

void
ChangeSet::Flush(uint64_t lsn) {
    // now flush all modified pages to disk
    if (collection_.IsEmpty())
        return;

    // Fetch the pages, ignoring all pages that are not dirty
    FlushChangesetVisitor visitor;
    collection_.Extract(visitor);

    if (visitor.list_.empty())
        return;

    // Append all changes to the journal. This operation basically
    // "write-ahead logs" all changes.
    //env->journal->append_changeset(visitor.list_, env->page_manager->LastBlobPageId(), lsn);

    // The modified pages are now flushed (and unlocked) asynchronously
    // to the database file
    //env->page_manager->RunAsync(boost::bind(&flush_changeset_to_file,
    //                        visitor.list_, env->device.get(), env->journal.get(),
    //                        lsn, ISSET(env->config.flags, UPS_ENABLE_FSYNC)));
}

}