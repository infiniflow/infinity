// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "btree_types.h"
#include "btree_node.h"
#include "simd.h"
#include "common/utility/builtin.h"
#include "common/utility/infinity_assert.h"

#include <algorithm>

namespace infinity {
struct BaseKeyList : BaseList {
    enum {
        // This KeyList cannot reduce its capacity in order to release storage
        kCanReduceCapacity = 0,

        // This KeyList does NOT have a custom insert() implementation
        kCustomInsert = 0,

        // This KeyList does NOT have a custom find() implementation
        kCustomFind = 0,

        // This KeyList does NOT have a custom find_lower_bound() implementation
        kCustomFindLowerBound = 0,

        // A flag whether this KeyList supports the scan() call
        kSupportsBlockScans = 0,

        // A flag whether this KeyList has sequential data
        kHasSequentialData = 0,
    };

    BaseKeyList(BtreeNode *node)
        : BaseList(node) {
    }

    // Erases the extended part of a key; nothing to do here
    void EraseExtendedKey(int slot) const {
    }

    template<typename Cmp>
    int LowerBound(size_t node_count, const btree_key_t *hkey, Cmp &comparator, int *pcmp) {
        return -1;
    }

    // Finds a key
    template<typename Cmp>
    int Find(size_t node_count, const btree_key_t *hkey, Cmp &comparator) {
        return -1;
    }

};

template<typename T>
struct PodKeyList : BaseKeyList {
    enum {
        // A flag whether this KeyList has sequential data
        kHasSequentialData = 1,

        // A flag whether this KeyList supports the scan() call
        kSupportsBlockScans = 1,

        // This KeyList has a custom find() implementation
        kCustomFind = 1,

        // This KeyList has a custom find_lower_bound() implementation
        kCustomFindLowerBound = 1,
    };

    // Constructor
    PodKeyList(BtreeNode *node)
        : BaseKeyList(node), data_(0) {
    }

    // Creates a new PodKeyList starting at |ptr|, total size is
    // |range_size_| (in bytes)
    void Create(uint8_t *ptr, size_t range_size) {
        data_ = (T *)ptr;
        range_size_ = range_size;
    }

    // Opens an existing PodKeyList starting at |ptr|
    void Open(uint8_t *ptr, size_t range_size, size_t) {
        data_ = (T *)ptr;
        range_size_ = range_size;
    }

    // Returns the required size for the current set of keys
    size_t RequiredRangeSize(size_t node_count) const {
        return node_count * sizeof(T);
    }

    // Returns the actual key size including overhead
    size_t FullKeySize(const btree_key_t * = 0) const {
        return sizeof(T);
    }

    // Finds a key
#ifdef __SSE__
    // Searches the node for the key and returns the slot of this key
    // - only for exact matches!
    //
    // This is the SIMD implementation. If SIMD is disabled then the
    // BaseKeyList::find method is used.
    template<typename Cmp>
    int Find(size_t node_count, const btree_key_t *key, Cmp &) {
        return FindSimd<T>(node_count, &data_[0], key);
    }
#else
    template<typename Cmp>
    int Find(size_t node_count, const btree_key_t *hkey, Cmp &) {
        T key = *(T *)hkey->data;
        T *result = std::lower_bound(&data_[0], &data_[node_count], key);
        if (unlikely(result == &data_[node_count] || *result != key))
            return -1;
        return result - &data_[0];
    }
#endif

    // Performs a lower-bound search for a key
    template<typename Cmp>
    int LowerBound(size_t node_count, const btree_key_t *hkey, Cmp &, int *pcmp) {
        T key = *(T *)hkey->data_;
        T *result = std::lower_bound(&data_[0], &data_[node_count], key);
        if (unlikely(result == &data_[node_count])) {
            if (key > data_[node_count - 1]) {
                *pcmp = +1;
                return node_count - 1;
            }
            if (key < data_[0]) {
                *pcmp = -1;
                return 0;
            }
            throw StorageException("shouldn't be here");
        }

        if (key > *result) {
            *pcmp = +1;
            return result - &data_[0];
        }

        if (key < *result) {
            *pcmp = +1;
            return (result - 1) - &data_[0];
        }

        *pcmp = 0;
        return result - &data_[0];
    }

    // Copies a key into |dest|
    void Key(int slot, ByteArray *arena, btree_key_t *dest,
             bool deep_copy = true) const {
        dest->size_ = sizeof(T);
        if (deep_copy == false) {
            dest->data_ = &data_[slot];
            return;
        }

        arena->resize(dest->size_);
        dest->data_ = arena->data();

        *(T *)dest->data_ = data_[slot];
    }

    // Erases a whole slot by shifting all larger keys to the "left"
    void Erase(size_t node_count, int slot) {
        if (slot < (int)node_count - 1)
            ::memmove(&data_[slot], &data_[slot + 1], sizeof(T) * (node_count - slot - 1));
    }

    // Inserts a key
    template<typename Cmp>
    BtreeNode::InsertResult Insert(size_t node_count, const btree_key_t *key, uint32_t flags, Cmp &, int slot) {
        if (node_count > (size_t)slot)
            ::memmove(&data_[slot + 1], &data_[slot], sizeof(T) * (node_count - slot));
        assert(key->size_ == sizeof(T));
        data_[slot] = *(T *)key->data_;
        return BtreeNode::InsertResult(0, slot);
    }

    // Copies |count| key from this[sstart] to dest[dstart]
    void CopyTo(int sstart, size_t node_count, PodKeyList<T> &dest,
                size_t other_count, int dstart) {
        ::memcpy(&dest.data_[dstart], &data_[sstart], sizeof(T) * (node_count - sstart));
    }

    // Returns true if the |key| no longer fits into the node
    bool RequiresSplit(size_t node_count, const btree_key_t *key) const {
        return (node_count + 1) * sizeof(T) >= range_size_;
    }

    // Change the range size; just copy the data from one place to the other
    void ChangeRangeSize(size_t node_count, uint8_t *new_data_ptr,
                         size_t new_range_size, size_t capacity_hint) {
        ::memmove(new_data_ptr, data_, node_count * sizeof(T));
        data_ = (T *)new_data_ptr;
        range_size_ = new_range_size;
    }

    // Returns the key size
    size_t KeySize(int) const {
        return sizeof(T);
    }

    // Returns a pointer to the key's data
    uint8_t *KeyData(int slot) const {
        return (uint8_t *)&data_[slot];
    }

    // The actual array of T's
    T *data_;
};

}