// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "page.h"
#include "page_io.h"
#include "btree_types.h"

namespace infinity {

struct JournalEntry {
    // Constructor - sets all fields to 0
    JournalEntry()
        : lsn_(0), followup_size_(0), type_(0),
          dbname_(0), reserved_(0) {
    }

    // the lsn of this entry
    uint64_t lsn_;

    // the size of the follow-up entry in bytes (may be padded)
    uint64_t followup_size_;

    // the type of this entry
    uint32_t type_;

    // the name of the database which is modified by this entry
    uint16_t dbname_;

    // a reserved value - reqd for padding
    uint16_t reserved_;
} __attribute__ ((packed));


//
// a Journal entry for an 'insert' operation
//
struct JournalEntryInsert {
    // Constructor - sets all fields to 0
    JournalEntryInsert()
        : key_size_(0), compressed_key_size_(0), record_size_(0),
          compressed_record_size_(0), insert_flags_(0) {
        data_[0] = 0;
    }

    // key size
    uint16_t key_size_;

    // compressed key size
    uint16_t compressed_key_size_;

    // record size
    uint32_t record_size_;

    // compressed record size
    uint32_t compressed_record_size_;

    // flags of ups_insert(), ups_cursor_insert()
    uint32_t insert_flags_;

    // data follows here - first |key_size| bytes for the key, then
    // |record_size| bytes for the record (and maybe some padding)
    //
    // this data can be compressed
    uint8_t data_[1];

    // Returns a pointer to the key data
    uint8_t *KeyData() {
        return &data_[0];
    }

    // Returns a pointer to the record data
    uint8_t *RecordData() {
        return &data_[key_size_];
    }
} __attribute__ ((packed));

// a Journal entry for 'erase' operations
//
struct JournalEntryErase {
    // Constructor - sets all fields to 0
    JournalEntryErase()
        : key_size_(0), compressed_key_size_(0), erase_flags_(0), duplicate_(0) {
        data_[0] = 0;
    }

    // key size
    uint16_t key_size_;

    // compressed key size
    uint16_t compressed_key_size_;

    // flags of ups_erase(), ups_cursor_erase()
    uint32_t erase_flags_;

    // which duplicate to erase
    int duplicate_;

    // the key data
    //
    // this data can be compressed
    uint8_t data_[1];

    // Returns a pointer to the key data
    uint8_t *KeyData() {
        return &data_[0];
    }
} __attribute__ ((packed));

//
// a Journal entry for a 'changeset' group
//
struct JournalEntryChangeset {
    // Constructor - sets all fields to 0
    JournalEntryChangeset()
        : num_pages_(0), last_blob_page_(0) {
    }

    // number of pages in this changeset
    uint32_t num_pages_;

    // address of the last blob page
    uint64_t last_blob_page_;
} __attribute__ ((packed));

//
// a Journal entry for a single page
//
struct JournalEntryPageHeader {
    // Constructor - sets all fields to 0
    JournalEntryPageHeader(uint64_t address = 0)
        : address_(address), compressed_size_(0) {
    }

    // the page address
    uint64_t address_;

    // the compressed size, if compression is enabled
    uint32_t compressed_size_;
} __attribute__ ((packed));

struct JournalState {
    JournalState();

    // The index of the file descriptor we are currently writing to (0 or 1)
    uint32_t current_fd_;

    // The two file descriptors
    File files_[2];

    // Buffer for writing data to the files
    ByteArray buffer_;

    // Set to false to disable logging; used during recovery
    bool disable_logging_;

    // Counting the flushed bytes (for ups_env_get_metrics)
    uint64_t count_bytes_flushed_;

};

struct Journal {
    enum {
        // marks the start of a new transaction
        //kEntryTypeTxnBegin   = 1,

        // marks the end of an aborted transaction
        //kEntryTypeTxnAbort   = 2,

        // marks the end of an committed transaction
        //kEntryTypeTxnCommit  = 3,

        // marks an insert operation
        kEntryTypeInsert     = 4,

        // marks an erase operation
        kEntryTypeErase      = 5,

        // marks a whole changeset operation (writes modified pages)
        kEntryTypeChangeset  = 6
    };

    struct Iterator {
        Iterator()
            : fdidx_(0), fdstart_(0), offset_(0) {
        }

        // selects the file descriptor [0..1]
        int fdidx_;

        // which file descriptor did we start with? [0..1]
        int fdstart_;

        // the offset in the file of the NEXT entry
        uint64_t offset_;
    };

    void Create();

    void Open();

    // Returns true if the journal is empty
    bool IsEmpty() const {
        if (!state_.files_[0].IsOpen() && !state_.files_[1].IsOpen())
            return true;

        for (int i = 0; i < 2; i++) {
            uint64_t size = state_.files_[i].FileSize();
            if (size > 0)
                return false;
        }

        return true;
    }

    int AppendChangeSet(std::vector<Page *> &pages, uint64_t last_blob_page, uint64_t lsn);

    // Empties the journal, removes all entries
    void Clear();

    // Closes the journal, frees all allocated resources
    void Close(bool noclear = false);

    JournalState state_;
};
}