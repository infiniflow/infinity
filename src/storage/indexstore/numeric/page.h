// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include <leveldb/status.h>
#include <leveldb/slice.h>

#include "intrusive_list.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/spinlock.h"
#include "main/logger.h"
#include <string>
#include <memory>

namespace infinity {
using Status = leveldb::Status;
using Slice = leveldb::Slice;

typedef struct PageHeader {
    // flags of this page - currently only used for the Page::kType* codes
    uint32_t flags_;

    // crc32
    uint32_t crc32_;

    // the lsn of the last operation
    uint64_t lsn_;

    // the persistent data blob
    uint8_t payload_[1];

} __attribute__ ((packed)) PageHeader;

typedef union PageData {
    // the persistent header
    struct PageHeader header_;

    // a char pointer to the allocated storage on disk
    uint8_t payload_[1];

} __attribute__ ((packed)) PageData;

class File;
class BtreeNodeProxy;
class Page {
public:
    struct PersistedData {
        PersistedData()
            : address_(0), size_(0), is_dirty_(false), is_allocated_(false),
              is_without_header_(false), raw_data_(0) {
        }

        PersistedData(const PersistedData &other)
            : address_(other.address_), size_(other.size_), is_dirty_(other.is_dirty_),
              is_allocated_(other.is_allocated_),
              is_without_header_(other.is_without_header_), raw_data_(other.raw_data_) {
        }

        ~PersistedData() {
            if (is_allocated_)
                std::free(raw_data_);
            raw_data_ = 0;
        }

        SpinLock mutex_;
        // address of this page - the absolute offset in the file
        uint64_t address_;

        // the size of this page
        uint32_t size_;

        // is this page dirty and needs to be flushed to disk?
        bool is_dirty_;

        // Page buffer was allocated with malloc() (if not then it was mapped
        // with mmap)
        bool is_allocated_;

        // True if page has no persistent header
        bool is_without_header_;

        // the persistent data of this page
        PageData *raw_data_;
    };

    static constexpr size_t kSize = 16 * 1024;

    enum {
        // sizeof the persistent page header
        kSizeofPersistentHeader = sizeof(PageHeader) - 1,

        // instruct Page::alloc() to reset the page with zeroes
        kInitializeWithZeroes,
    };
    // The various linked lists (indices in m_prev, m_next)
    enum {
        // list of all cached pages
        kListCache              = 0,

        // list of all pages in a changeset
        kListChangeset          = 1,

        // a bucket in the hash table of the cache
        kListBucket             = 2,

        // array limit
        kListMax                = 3
    };
    // non-persistent page flags
    enum {
        // page->m_data was allocated with malloc, not mmap
        kNpersMalloc            = 1,

        // page has no header (i.e. it's part of a large blob)
        kNpersNoHeader          = 2
    };

    enum {
        // unidentified db page type
        kTypeUnknown            =  0x00000000,

        // the header page: this is the first page in the environment (offset 0)
        kTypeHeader             =  0x10000000,

        // a B+tree root page
        kTypeBroot              =  0x20000000,

        // a B+tree node page
        kTypeBindex             =  0x30000000,

        // a page storing the state of the PageManager
        kTypePageManager        =  0x40000000,

        // a page which stores blobs
        kTypeBlob               =  0x50000000
    };

    Page(File *file);
    ~Page();

    uint64_t Address() const {
        return persisted_data_.address_;
    }

    // Sets the address of this page
    void SetAddress(uint64_t address) {
        persisted_data_.address_ = address;
    }

    // Returns the page's type (kType*)
    uint32_t Type() const {
        return persisted_data_.raw_data_->header_.flags_;
    }

    // Sets the page's type (kType*)
    void SetType(uint32_t type) {
        persisted_data_.raw_data_->header_.flags_ = type;
    }

    // Returns the crc32
    uint32_t Crc32() const {
        return persisted_data_.raw_data_->header_.crc32_;
    }

    // Sets the crc32
    void SetCrc32(uint32_t crc32) {
        persisted_data_.raw_data_->header_.crc32_ = crc32;
    }

    uint32_t UsablePageSize() {
        return kSize - kSizeofPersistentHeader;
    }

    // Returns the lsn
    uint64_t Lsn() const {
        return persisted_data_.raw_data_->header_.lsn_;
    }

    // Sets the lsn
    void SetLsn(uint64_t lsn) {
        persisted_data_.raw_data_->header_.lsn_ = lsn;
    }

    // Returns the pointer to the persistent data
    PageData *Data() {
        return persisted_data_.raw_data_;
    }

    // Sets the pointer to the persistent data
    void SetData(PageData *data) {
        persisted_data_.raw_data_ = data;
    }

    // Returns the persistent payload (after the header!)
    uint8_t *Payload() {
        return persisted_data_.raw_data_->header_.payload_;
    }

    // Returns the persistent payload (including the header!)
    uint8_t *RawPayload() {
        return persisted_data_.raw_data_->payload_;
    }

    // Returns true if this is the header page of the Environment
    bool IsHeader() const {
        return persisted_data_.address_ == 0;
    }

    // Returns true if this page is dirty (and needs to be flushed to disk)
    bool IsDirty() const {
        return persisted_data_.is_dirty_;
    }

    // Sets this page dirty/not dirty
    void SetDirty(bool dirty) {
        persisted_data_.is_dirty_ = dirty;
    }

    // Returns true if the page's buffer was allocated with malloc
    bool IsAllocated() const {
        return persisted_data_.is_allocated_;
    }

    // Returns true if the page has no persistent header
    bool IsWithoutHeader() const {
        return persisted_data_.is_without_header_;
    }

    // Sets the flag whether this page has a persistent header or not
    void SetWithoutHeader(bool is_without_header) {
        persisted_data_.is_without_header_ = is_without_header;
    }

    // Returns the spinlock
    SpinLock &Mutex() {
        return persisted_data_.mutex_;
    }
    // Assign a buffer which was allocated with malloc()
    void AssignAllocatedBuffer(void *buffer, uint64_t address) {
        FreeBuffer();
        persisted_data_.raw_data_ = (PageData *)buffer;
        persisted_data_.is_allocated_ = true;
        persisted_data_.address_ = address;
    }

    void FreeBuffer();

    // Allocates a new page from the device
    // |flags|: either 0 or kInitializeWithZeroes
    void Alloc(uint32_t type, uint32_t flags = 0);

    // Reads a page from the device
    void Fetch(uint64_t address);

    // Flushes the page to disk, clears the "dirty" flag
    void Flush();

    // Returns the next page in a linked list
    Page *Next(int list) {
        return list_node_.next_[list];
    }

    // Returns the previous page of a linked list
    Page *Previous(int list) {
        return list_node_.previous_[list];
    }

    // Returns the cached BtreeNodeProxy
    BtreeNodeProxy *NodeProxy() {
        return node_proxy_;
    }

    // Sets the cached BtreeNodeProxy
    void SetNodeProxy(BtreeNodeProxy *proxy) {
        node_proxy_ = proxy;
    }

    // tracks number of flushed pages
    static uint64_t ms_page_count_flushed_;

    // the persistent data of this page
    PersistedData persisted_data_;

    IntrusiveListNode<Page, Page::kListMax> list_node_;
private:
    File* file_;

    BtreeNodeProxy *node_proxy_;

};

}
