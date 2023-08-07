// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#include "journal.h"
#include "page_manager.h"
#include "context.h"

namespace infinity {

enum {
    // flush buffers if this limit is exceeded
    kBufferLimit = 1024 * 1024, // 1 mb
};


static inline void
ClearFile(JournalState &state, int idx) {
    if (state.files_[idx].IsOpen()) {
        state.files_[idx].Truncate(0);

        // after truncate, the file pointer is far beyond the new end of file;
        // reset the file pointer, or the next write will resize the file to
        // the original size
        state.files_[idx].Seek(0, SEEK_SET);
    }
}


// Sequentially returns the next journal entry, starting with
// the oldest entry.
//
// |iter| must be initialized with zeroes for the first call.
// |auxbuffer| returns the auxiliary data of the entry and is either
// a structure of type PJournalEntryInsert or PJournalEntryErase.
//
// Returns an empty entry (lsn is zero) after the last element.
static inline void
ReadEntry(JournalState &state, Journal::Iterator *iter, JournalEntry *entry, ByteArray *auxbuffer) {
    auxbuffer->clear();

    // if iter->offset is 0, then the iterator was created from scratch
    // and we start reading from the first (oldest) entry.
    //
    // The oldest of the two logfiles is always the "other" one (the one
    // NOT in current_fd).
    if (iter->offset_ == 0) {
        iter->fdstart_ = iter->fdidx_ =
                             state.current_fd_ == 0
                             ? 1
                             : 0;
    }

    // get the size of the journal file
    uint64_t filesize = state.files_[iter->fdidx_].FileSize();

    // reached EOF? then either skip to the next file or we're done
    if (filesize == iter->offset_) {
        if (iter->fdstart_ == iter->fdidx_) {
            iter->fdidx_ = iter->fdidx_ == 1 ? 0 : 1;
            iter->offset_ = 0;
            filesize = state.files_[iter->fdidx_].FileSize();
        } else {
            entry->lsn_ = 0;
            return;
        }
    }

    // second file is also empty? then return
    if (filesize == iter->offset_) {
        entry->lsn_ = 0;
        return;
    }

    // now try to read the next entry
    try {
        state.files_[iter->fdidx_].Read(iter->offset_, entry, sizeof(*entry));

        iter->offset_ += sizeof(*entry);

        // read auxiliary data if it's available
        if (entry->followup_size_) {
            auxbuffer->resize((uint32_t)entry->followup_size_);

            state.files_[iter->fdidx_].Read(iter->offset_, auxbuffer->data(),
                                             (size_t)entry->followup_size_);
            iter->offset_ += entry->followup_size_;
        }
    } catch (Exception &) {
        entry->lsn_ = 0; // this triggers the end of recovery
    }
}


void
Journal::Create() {
}

void
Journal::Open() {
}


}