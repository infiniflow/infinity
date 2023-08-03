#pragma once

#include "btree_types.h"
#include "btree_node.h"
#include "common/utility/infinity_assert.h"

#include <limits>

namespace infinity {

struct BaseRecordList : BaseList {
    enum {
        // A flag whether this RecordList supports the scan() call
        kSupportsBlockScans = 0,

        // A flag whether this RecordList has sequential data
        kHasSequentialData = 0
    };

    BaseRecordList(BtreeNode *node)
        : BaseList(node) {
    }


    // Returns the record id. Only required for internal nodes
    uint64_t RecordId(int slot, int duplicate_index = 0) const {
        assert(!"shouldn't be here");
        return 0;
    }

    // Sets the record id. Not required for fixed length leaf nodes
    void SetRecordId(int slot, uint64_t ptr) {
        assert(!"shouldn't be here");
    }
};

template<typename T>
struct PodRecordList : BaseRecordList {
    enum {
        // A flag whether this RecordList has sequential data
        kHasSequentialData = 1,

        // This RecordList implements the scan() method
        kSupportsBlockScans = 1,
    };

    PodRecordList(BtreeNode *node)
        : BaseRecordList(node) {
    }

    // Sets the data pointer
    void Create(uint8_t *ptr, size_t range_size) {
        range_data_ = (T *)ptr;
        range_size_ = range_size;
    }

    // Opens an existing RecordList
    void Open(uint8_t *ptr, size_t range_size, size_t node_count) {
        range_data_ = (T *)ptr;
        range_size_ = range_size;
    }

    // Returns the actual record size including overhead
    size_t FullRecordSize() const {
        return sizeof(T);
    }

    // Calculates the required size for a range with the specified |capacity|
    size_t RequiredRangeSize(size_t node_count) const {
        return node_count * sizeof(T);
    }

    // Returns the record counter of a key
    // This record list does not support duplicates, therefore always return 1
    int RecordCount(int) const {
        return 1;
    }

    // Returns the record size
    uint32_t RecordSize(int, int = 0) const {
        return sizeof(T);
    }

    // Returns the full record and stores it in |dest|; memory must be
    // allocated by the caller
    void Record(int slot, ByteArray *arena, btree_record_t *record,
                uint32_t flags, int) const {
        record->size_ = sizeof(T);

        arena->resize(record->size_);
        record->data_ = arena->data();
        memcpy(record->data_, &range_data_[slot], record->size_);
    }

    // Updates the record of a key
    void SetRecord(int slot, int, btree_record_t *record,
                   uint32_t flags, uint32_t * = 0) {
        assert(record->size == sizeof(T));
        range_data_[slot] = *(T *)record->data_;
    }

    // Erases the record by nulling it
    void EraseRecord(int slot, int = 0, bool = true) {
        range_data_[slot] = 0;
    }

    // Erases a whole slot by shifting all larger records to the "left"
    void Erase(size_t node_count, int slot) {
        if (slot < (int)node_count - 1)
            ::memmove(&range_data_[slot], &range_data_[slot + 1],
                      sizeof(T) * (node_count - slot - 1));
    }

    // Creates space for one additional record
    void Insert(size_t node_count, int slot) {
        if (slot < (int)node_count) {
            ::memmove(&range_data_[(slot + 1)], &range_data_[slot],
                      sizeof(T) * (node_count - slot));
        }
        range_data_[slot] = 0;
    }

    // Copies |count| records from this[sstart] to dest[dstart]
    void CopyTo(int sstart, size_t node_count, PodRecordList<T> &dest,
                size_t other_count, int dstart) {
        ::memcpy(&dest.range_data_[dstart], &range_data_[sstart],
                 sizeof(T) * (node_count - sstart));
    }

    // Returns true if there's not enough space for another record
    bool RequiresSplit(size_t node_count) const {
        if (unlikely(range_size_ == 0))
            return false;
        return (node_count + 1) * sizeof(T) >= range_size_;
    }

    // Change the capacity; for PAX layouts this just means copying the
    // data from one place to the other
    void ChangeRangeSize(size_t node_count, uint8_t *new_data_ptr,
                         size_t new_range_size, size_t capacity_hint) {
        ::memmove(new_data_ptr, range_data_, node_count * sizeof(T));
        range_size_ = new_range_size;
        range_data_ = (T *)new_data_ptr;
    }

    // The actual record data
    T *range_data_;
};


struct InternalRecordList : BaseRecordList {
    enum {
        // A flag whether this RecordList has sequential data
        kHasSequentialData = 1
    };

    // Constructor
    InternalRecordList(BtreeNode *node)
        : BaseRecordList(node) {
        page_size_ = Page::kSize;
        inmemory_ = false;
    }

    // Sets the data pointer
    void Create(uint8_t *ptr, size_t range_size) {
        range_size_ = range_size;
        range_data_ = ArrayView<uint64_t>((uint64_t *)ptr, range_size_ / 8);
    }

    // Opens an existing RecordList
    void Open(uint8_t *ptr, size_t range_size_, size_t node_count) {
        range_size_ = range_size_;
        range_data_ = ArrayView<uint64_t>((uint64_t *)ptr, range_size_ / 8);
    }

    // Returns the actual size including overhead
    size_t FullRecordSize() const {
        return sizeof(uint64_t);
    }

    // Calculates the required size for a range with the specified |capacity|
    size_t RequiredRangeSize(size_t node_count) const {
        return node_count * sizeof(uint64_t);
    }

    // Returns the record counter of a key; this implementation does not
    // support duplicates, therefore the record count is always 1
    int RecordCount(int) const {
        return 1;
    }

    // Returns the record size
    uint64_t RecordSize(int, int = 0) const {
        return sizeof(uint64_t);
    }

    // Returns the full record and stores it in |dest|; memory must be
    // allocated by the caller
    void Record(int slot, ByteArray *arena, btree_record_t *record, uint32_t flags, int) const {
        // the record is stored inline
        record->size_ = sizeof(uint64_t);
        arena->resize(record->size_);
        record->data_ = arena->data();
        ::memcpy(record->data_, &range_data_[slot], record->size_);
    }

    // Updates the record of a key
    void SetRecord(int slot, int, btree_record_t *record, uint32_t flags, uint32_t * = 0) {
        assert(record->size_ == sizeof(uint64_t));
        range_data_[slot] = *(uint64_t *)record->data_;
    }

    // Erases the record
    void EraseRecord(int slot, int = 0, bool = true) {
        range_data_[slot] = 0;
    }

    // Erases a whole slot by shifting all larger records to the "left"
    void Erase(size_t node_count, int slot) {
        if (likely(slot < (int)node_count - 1))
            ::memmove(&range_data_[slot], &range_data_[slot + 1],
                      sizeof(uint64_t) * (node_count - slot - 1));
    }

    // Creates space for one additional record
    void Insert(size_t node_count, int slot) {
        if (slot < (int)node_count)
            ::memmove(&range_data_[slot + 1], &range_data_[slot],
                      sizeof(uint64_t) * (node_count - slot));
        range_data_[slot] = 0;
    }

    // Copies |count| records from this[sstart] to dest[dstart]
    void CopyTo(int sstart, size_t node_count, InternalRecordList &dest,
                size_t other_count, int dstart) {
        ::memcpy(&dest.range_data_[dstart], &range_data_[sstart],
                 sizeof(uint64_t) * (node_count - sstart));
    }

    // Sets the record id
    void SetRecordId(int slot, uint64_t value) {
        assert(inmemory_ ? 1 : value % page_size_ == 0);
        range_data_[slot] = inmemory_ ? value : value / page_size_;
    }

    // Returns the record id
    uint64_t RecordId(int slot, int = 0) const {
        return inmemory_ ? range_data_[slot] : page_size_ * range_data_[slot];
    }

    // Returns true if there's not enough space for another record
    bool RequiresSplit(size_t node_count) const {
        return (node_count + 1) * sizeof(uint64_t)
               >= range_data_.size_ * sizeof(uint64_t);
    }

    // Change the capacity; for PAX layouts this just means copying the
    // data from one place to the other
    void ChangeRangeSize(size_t node_count, uint8_t *new_data_ptr,
                         size_t new_range_size, size_t capacity_hint) {
        if ((uint64_t *)new_data_ptr != range_data_.data_) {
            ::memmove(new_data_ptr, range_data_.data_, node_count * sizeof(uint64_t));
            range_data_ = ArrayView<uint64_t>((uint64_t *)new_data_ptr,
                                              new_range_size / 8);
        }
        range_size_ = new_range_size;
    }

    // The record data is an array of page IDs
    ArrayView<uint64_t> range_data_;

    // The page size
    size_t page_size_;

    // Store page ID % page size or the raw page ID?
    bool inmemory_;
};

}
