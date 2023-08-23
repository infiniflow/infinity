// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "page.h"
#include "btree_types.h"

#include <roaring/roaring.hh>

namespace infinity {
using Roaring = roaring::Roaring;

struct BtreeNode {
public:
    // Result of the insert() operation
    struct InsertResult {
        InsertResult(int status = 0, int slot = 0)
            : status_(status), slot_(slot) {
        }

        // upscaledb status_ code
        int status_;

        // the slot_ of the new (or existing) key
        int slot_;
    };

    enum {
        // insert key at the beginning of the page_
        kInsertPrepend = 1,

        // append key to the end of the page_
        kInsertAppend = 2,
    };

    enum {
        // node_ is a leaf
        kLeafNode = 1
    };

    // Returns a BtreeNode from a Page
    static BtreeNode *FromPage(Page *page_) {
        return (BtreeNode *)page_->Payload();
    }

    // Returns the offset (in bytes) of the member |m_data|
    static uint32_t EntryOffset() {
        return sizeof(BtreeNode) - 1;
    }

    // Returns the flags of the btree node_ (|kLeafNode|)
    uint32_t Flags() const {
        return flags_;
    }

    // Sets the flags of the btree node_ (|kLeafNode|)
    void SetFlags(uint32_t flags) {
        flags_ = flags;
    }

    // Returns the number of entries in a BtreeNode
    uint32_t Length() const {
        return length_;
    }

    // Sets the number of entries in a BtreeNode
    void SetLength(uint32_t Length) {
        length_ = Length;
    }

    // Returns the address of the left sibling of this node_
    uint64_t LeftSibling() const {
        return left_sibling_;
    }

    // Sets the address of the left sibling of this node_
    void SetLeftSibling(uint64_t left) {
        left_sibling_ = left;
    }

    // Returns the address of the right sibling of this node_
    uint64_t RightSibling() const {
        return right_sibling_;
    }

    // Sets the address of the right sibling of this node_
    void SetRightSibling(uint64_t right) {
        right_sibling_ = right;
    }

    // Returns the left child pointer of this node_
    uint64_t LeftChild() const {
        return left_child_;
    }

    // Sets the ptr_down of this node_
    void SetLeftChild(uint64_t left_child) {
        left_child_ = left_child;
    }

    // Returns true if this btree node_ is a leaf node_
    bool IsLeaf() const {
        return (flags_ & kLeafNode) == kLeafNode;
    }

    // Returns a pointer to the key data
    uint8_t *Data() {
        return &data_[0];
    }

    const uint8_t *Data() const {
        return &data_[0];
    }

private:
    // flags of this node_
    uint32_t flags_;

    // number of used entries in the node_
    uint32_t length_;

    // address of left sibling
    uint64_t left_sibling_;

    // address of right sibling
    uint64_t right_sibling_;

    // address of child node_ whose items are smaller than all items
    // in this node_
    uint64_t left_child_;

    // the entries of this node_
    uint8_t data_[1];

} __attribute__ ((packed));


template<typename KeyList, typename RecordList>
struct BaseNodeImpl {
public:
    // Constructor
    BaseNodeImpl(Page *page_)
        : page_(page_), node_(BtreeNode::FromPage(page_)),
          estimated_capacity_(0), keys_(node_),
          records_(node_) {
    }

    // Returns the estimated page_'s capacity
    size_t EstimateCapacity() const {
        return estimated_capacity_;
    }

    // Returns a copy of a key and stores it in |dest|
    void Key(int slot, ByteArray *arena,
             btree_key_t *dest) {
        // copy (or assign) the key data
        keys_.Key(slot, arena, dest, true);
    }

    // Returns the record size of a key or one of its duplicates
    uint32_t RecordSize(int slot, int duplicate_index) {
        return records_.RecordSize(slot, duplicate_index);
    }

    // Returns the number of duplicate records_
    int RecordCount(int slot) {
        return records_.RecordCount(slot);
    }

    // Returns the full record and stores it in |dest|
    void Record(int slot, ByteArray *arena,
                btree_record_t *record, uint32_t flags, int duplicate_index) {
        // copy the record data
        records_.Record(slot, arena, record, flags, duplicate_index);
    }

    // Updates the record of a key
    void SetRecord(int slot, btree_record_t *record,
                   int duplicate_index, uint32_t flags,
                   uint32_t *new_duplicate_index) {
        // automatically overwrite an existing key unless this is a
        // duplicate operation
        //flags |= BTREE_OVERWRITE;

        records_.SetRecord(slot, duplicate_index, record, flags, new_duplicate_index);
    }

    // Erases the record
    void EraseRecord(int slot, int duplicate_index,
                     bool all_duplicates) {
        records_.EraseRecord(slot, duplicate_index, all_duplicates);
    }

    // Erases a key
    void Erase(int slot) {
        size_t node_length = node_->Length();

        keys_.Erase(node_length, slot);
        records_.Erase(node_length, slot);
    }

    void GetPayloads(int start_slot, int end_slot, std::shared_ptr<Roaring>& filter) {
        keys_.GetPayloads(start_slot, end_slot, filter);
    }

    // Inserts a new key
    //
    // Most KeyLists first calculate the slot of the new key, then insert
    // the key at this slot. Both operations are separate from each other.
    // However, compressed KeyLists can overwrite this behaviour and
    // combine both calls into one to save performance.
    template<typename Cmp>
    BtreeNode::InsertResult Insert(btree_key_t *key,
                                   uint32_t flags, Cmp &comparator) {
        BtreeNode::InsertResult result(0, 0);
        size_t node_length = node_->Length();

        /* KeyLists with a custom insert function don't need a slot; only
         * calculate the slot for the default insert functions */
        if (!KeyList::kCustomInsert) {
            if (node_length == 0)
                result.slot_ = 0;
            else if (flags & BtreeNode::kInsertPrepend == BtreeNode::kInsertPrepend)
                result.slot_ = 0;
            else if (flags & BtreeNode::kInsertAppend == BtreeNode::kInsertAppend)
                result.slot_ = node_length;
            else {
                int cmp;
                result.slot_ = LowerBoundImpl(key, comparator, &cmp);

                /* insert the new key at the beginning? */
                if (result.slot_ == -1) {
                    result.slot_ = 0;
                    assert(cmp != 0);
                }
                /* key exists already */
                else if (cmp == 0) {
                    result.status_ = BTREE_DUPLICATE_KEY;
                    return result;
                }
                /* if the new key is > than the slot key: move to the next slot */
                else if (cmp > 0)
                    result.slot_++;
            }
        }

        // Uncouple the cursors.
        //
        // for custom inserts we have to uncouple all cursors, because the
        // KeyList doesn't have access to the cursors in the page_. In this
        // case result.slot is 0.
        //if ((int)node_length > result.slot)
        //  BtreeCursor::uncouple_all_cursors(page_, result.slot);

        // make space for 1 additional element.
        // only store the key data; flags and record IDs are set by the caller
        result = keys_.Insert(node_length, key, flags, comparator,
                              result.slot_);
        if (result.status_ == 0)
            records_.Insert(node_length, result.slot_);
        return result;
    }

    // Compares two keys_ using the supplied comparator
    template<typename Cmp>
    int Compare(const btree_key_t *lhs,
                uint32_t rhs, Cmp &cmp) {
        if (KeyList::kHasSequentialData) {
            return cmp(lhs->data_, lhs->size_, keys_.KeyData(rhs),
                       keys_.KeySize(rhs));
        } else {
            btree_key_t tmp = {0};
            keys_.Key(rhs, &private_arena_, &tmp, false);
            return cmp(lhs->data_, lhs->size_, tmp.data_, tmp.size_);
        }
    }

    // Searches the node_ for the key and returns the slot of this key
    template<typename Cmp>
    int LowerBound(btree_key_t *key, Cmp &comparator,
                   uint64_t *precord_id, int *pcmp) {
        int slot = LowerBoundImpl(key, comparator, pcmp);
        if (precord_id) {
            if (slot == -1 || (slot == 0 && *pcmp == -1))
                *precord_id = node_->LeftChild();
            else
                *precord_id = records_.RecordId(slot);
        }
        return slot;
    }

    // Searches the node_ for the key and returns the slot of this key
    // - only for exact matches!
    template<typename Cmp>
    int Find(btree_key_t *key, Cmp &comparator) {
        return FindImpl(key, comparator);
    }

    // Splits a node_ and moves parts of the current node_ into |other|, starting
    // at the |pivot| slot
    void Split(BaseNodeImpl<KeyList, RecordList> *other,
               int pivot) {
        size_t node_length = node_->Length();
        size_t other_node_count = other->node_->Length();

        //
        // if a leaf page_ is split then the pivot element must be inserted in
        // the leaf page_ AND in the internal node_. the internal node_ update
        // is handled by the caller.
        //
        // in internal nodes the pivot element is only propagated to the
        // parent node_. the pivot element is skipped.
        //
        if (node_->IsLeaf()) {
            keys_.CopyTo(pivot, node_length, other->keys_,
                         other_node_count, 0);
            records_.CopyTo(pivot, node_length, other->records_,
                            other_node_count, 0);
        } else {
            keys_.CopyTo(pivot + 1, node_length, other->keys_,
                         other_node_count, 0);
            records_.CopyTo(pivot + 1, node_length, other->records_,
                            other_node_count, 0);
        }
    }

    // Returns true if the node_ requires a merge or a shift
    bool RequiresMerge() const {
        return node_->Length() <= 3;
    }

    // Merges this node_ with the |other| node_
    void MergeFrom(BaseNodeImpl<KeyList, RecordList> *other) {
        size_t node_length = node_->Length();
        size_t other_node_count = other->node_->Length();

        // shift items from the sibling to this page_
        if (other_node_count > 0) {
            other->keys_.CopyTo(0, other_node_count, keys_,
                                node_length, node_length);
            other->records_.CopyTo(0, other_node_count, records_,
                                   node_length, node_length);
        }
    }

    // Reorganize this node_; re-arranges capacities of KeyList and RecordList
    // in order to free space and avoid splits
    bool Reorganize(const btree_key_t *key) const {
        return false;
    }

    // Returns the record id
    uint64_t RecordId(int slot) const {
        return records_.RecordId(slot);
    }

    // Sets the record id
    void SetRecordId(int slot, uint64_t ptr) {
        records_.SetRecordId(slot, ptr);
    }

    // The page_ we're operating on
    Page *page_;

    // The node_ we're operating on
    BtreeNode *node_;

    // Capacity of this node_ (maximum number of key/record pairs that
    // can be stored)
    size_t estimated_capacity_;

    // for accessing the keys_
    KeyList keys_;

    // for accessing the records_
    RecordList records_;

private:
    // Implementation of the find method for lower-bound matches. If there
    // is no exact match then the lower bound is returned, and the compare value
    // is returned in |*pcmp|.
    template<typename Cmp>
    int LowerBoundImpl(const btree_key_t *key,
                       Cmp &comparator, int *pcmp) {
        if (KeyList::kCustomFindLowerBound)
            return keys_.LowerBound(node_->Length(), key,
                                    comparator, pcmp);

        return FindImplBinary(key, comparator, pcmp);
    }

    // Implementation of the find method for exact matches. Supports a custom
    // search implementation in the KeyList (i.e. for SIMD).
    template<typename Cmp>
    int FindImpl(const btree_key_t *key, Cmp &comparator) {
        if (KeyList::kCustomFind)
            return keys_.Find(node_->Length(), key, comparator);

        int cmp = 0;
        int slot = FindImplBinary(key, comparator, &cmp);
        if (slot == -1 || cmp != 0)
            return -1;
        return slot;
    }

    // Binary search
    template<typename Cmp>
    int FindImplBinary(const btree_key_t *key,
                       Cmp &comparator, int *pcmp) {
        int right = (int)node_->Length();
        int left = 0;
        int last = right + 1;

        *pcmp = -1;

        while (right - left > 0) {
            /* get the median item; if it's identical with the "last" item,
             * we've found the slot */
            int middle = (left + right) / 2;

            if (middle == last) {
                *pcmp = 1;
                return middle;
            }

            /* compare it against the key */
            *pcmp = Compare(key, middle, comparator);

            /* found it? */
            if (*pcmp == 0) {
                return middle;
            }
            /* if the key is bigger than the item: search "to the left" */
            if (*pcmp < 0) {
                if (right == 0) {
                    assert(middle == 0);
                    return -1;
                }
                right = middle;
            }
            /* otherwise search "to the right" */
            else {
                last = middle;
                left = middle;
            }
        }

        return -1;
    }

    // A memory arena for various tasks
    ByteArray private_arena_;
};


template<typename KeyList, typename RecordList>
struct PaxNodeImpl : public BaseNodeImpl<KeyList, RecordList> {
    // C++ does not allow access to members of base classes unless they're
    // explicitly named; this typedef helps to make the code "less" ugly,
    // but it still sucks that i have to use it
    //
    // http://stackoverflow.com/questions/1120833/derived-template-class-access-to-base-class-member-data
    typedef BaseNodeImpl<KeyList, RecordList> P;

    // Constructor
    PaxNodeImpl(Page *page)
        : BaseNodeImpl<KeyList, RecordList>(page) {
        Initialize();
    }

    // Returns true if |key| cannot be inserted because a split is required
    bool RequiresSplit(const btree_key_t *key) const {
        return P::node_->Length() >= P::estimated_capacity_;
    }

    void Initialize() {
        uint32_t usable_nodesize = P::page_->UsablePageSize()
                                   - BtreeNode::EntryOffset();
        size_t ks = P::keys_.FullKeySize();
        size_t rs = P::records_.FullRecordSize();
        size_t capacity = usable_nodesize / (ks + rs);

        uint8_t *p = P::node_->Data();
        if (P::node_->Length() == 0) {
            P::keys_.Create(&p[0], capacity * ks);
            P::records_.Create(&p[capacity * ks], capacity * rs);
        } else {
            size_t key_range_size = capacity * ks;
            size_t record_range_size = capacity * rs;

            P::keys_.Open(p, key_range_size, P::node_->Length());
            P::records_.Open(p + key_range_size, record_range_size,
                             P::node_->Length());
        }

        P::estimated_capacity_ = capacity;
    }
};

struct BaseList {
    BaseList(BtreeNode *node)
        : node_(node), range_size_(0) {
    }

    // The node which stores this list
    BtreeNode *node_;

    // The size of the range (in bytes)
    uint32_t range_size_;
};

}