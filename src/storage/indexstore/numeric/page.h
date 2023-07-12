#pragma once

#include "page_id.h"

#include <leveldb/status.h>
#include <leveldb/slice.h>

#include <string>
#include <memory>

namespace infinity {
using Status = leveldb::Status;
using Slice = leveldb::Slice;

// An in-memory representation of numeric index store's on-disk "page" format.
class Page {
public:
    static constexpr size_t kSize = 4 * 1024;

    static size_t UsableSize();

    static size_t PerRecordMetadataSize();

    static size_t NumRecordsThatFit(size_t record_size, size_t total_fence_bytes);

    static size_t NumPagesNeeded(size_t n, size_t record_size,
                                 size_t total_fence_bytes);

    explicit Page(void* data) : data_(data) {}

    Page(void* data, const Slice& lower_key, const Slice& upper_key);

    Page(void* data, const Page& old_page);

    Slice GetKeyPrefix() const;

    Slice GetLowerBoundary() const;
    Slice GetUpperBoundary() const;

    Status Put(const Slice& key, const Slice& value);
    Status UpdateOrRemove(const Slice& key, const Slice& value);
    Status Get(const Slice& key, std::string* value_out);
    Status Delete(const Slice& key);

    PhysicalPageId GetOverflow() const;

    void SetOverflow(PhysicalPageId overflow);

    bool HasOverflow();

    const bool IsValid() const;

    uint16_t GetNumRecords() const;

    const bool IsOverflow() const;
    void MakeOverflow();
    void UnmakeOverflow();

    class Iterator;
    friend class Iterator;

    // Returns an iterator that can be used to iterate over the records stored in this page.
    Iterator GetIterator() const;

    Slice data() const {
        return Slice(reinterpret_cast<const char*>(data_), kSize);
    }

private:
    // Construct an empty `Page` where all keys will satisfy
    // `lower_key <= key <= upper_key`
    Page(void* data, const uint8_t* lower_key, unsigned lower_key_length,
         const uint8_t* upper_key, unsigned upper_key_length);

    void* data_;
};

// An iterator used to iterate over the records stored in this page. 
class Page::Iterator {
public:
    void Seek(const Slice& key);

    void Next();

    bool Valid() const;

    size_t RecordsLeft() const;

    Slice Key() const;

    Slice Value() const;

private:
    friend class Page;

    explicit Iterator(const Page& page);

    const void* data_;

    size_t current_slot_;

    size_t prefix_length_;
    mutable bool key_buffer_valid_;
    mutable std::string key_buffer_;
};

namespace detail {

class PageBufferDeleter {
 public:
  void operator()(char* buffer) { free(buffer); }
};

}

using PageBuffer = std::unique_ptr<char[], detail::PageBufferDeleter>;

//To be replaced with Arena
class PageMemoryAllocator {
public:
    static PageBuffer Allocate(size_t num_pages) {
        const size_t buffer_size = num_pages * Page::kSize;
        void* const buffer = std::aligned_alloc(Page::kSize, buffer_size);
        if (buffer == nullptr) {
            throw std::bad_alloc();
        }
        memset(buffer, 0, buffer_size);
        return PageBuffer(reinterpret_cast<char*>(buffer));
    }
};

}
