// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "btree_node.h"
#include "context.h"

namespace infinity {

struct BtreeNodeProxy {
    BtreeNodeProxy(Page *page) : page_(page) {}

    virtual ~BtreeNodeProxy() {}

    // Returns the flags of the btree node (|kLeafNode|)
    uint32_t Flags() const {
        return BtreeNode::FromPage(page_)->Flags();
    }

    // Sets the flags of the btree node (|kLeafNode|)
    void SetFlags(uint32_t flags) {
        BtreeNode::FromPage(page_)->SetFlags(flags);
    }

    // Returns the number of entries in the BtreeNode
    uint32_t Length() const {
        return BtreeNode::FromPage(page_)->Length();
    }

    // Sets the number of entries in the BtreeNode
    void SetLength(uint32_t length) {
        BtreeNode::FromPage(page_)->SetLength(length);
    }

    // Returns true if this btree node is a leaf node
    bool IsLeaf() const {
        return BtreeNode::FromPage(page_)->IsLeaf();
    }

    // Returns the address of the left sibling of this node
    uint64_t LeftSibling() const {
        return BtreeNode::FromPage(page_)->LeftSibling();
    }

    // Sets the address of the left sibling of this node
    void SetLeftSibling(uint64_t address) {
        BtreeNode::FromPage(page_)->SetLeftSibling(address);
    }

    // Returns the address of the right sibling of this node
    uint64_t RightSibling() const {
        return BtreeNode::FromPage(page_)->RightSibling();
    }

    // Sets the address of the right sibling of this node
    void SetRightSibling(uint64_t address) {
        BtreeNode::FromPage(page_)->SetRightSibling(address);
    }

    // Returns the ptr_down of this node
    uint64_t LeftChild() const {
        return BtreeNode::FromPage(page_)->LeftChild();
    }

    // Sets the ptr_down of this node
    void SetLeftChild(uint64_t address) {
        BtreeNode::FromPage(page_)->SetLeftChild(address);
    }

    // Returns the estimated capacity of this node
    virtual size_t EstimateCapacity() const = 0;

    // Compares the two keys. Returns 0 if both are equal, otherwise -1 (if
    // |lhs| is greater) or +1 (if |rhs| is greater).
    virtual int Compare(const btree_key_t *lhs, const btree_key_t *rhs) const = 0;

    // Compares a public key and an internal key
    virtual int Compare(const btree_key_t *lhs, int rhs) = 0;

    // Returns true if the public key (|lhs|) and an internal key (slot
    // |rhs|) are equal
    virtual bool Equals(const btree_key_t *lhs, int rhs) = 0;

    // Searches the node for the |key|, and returns the slot of this key.
    // If |record_id| is not null then it will store the result of the last
    // compare operation.
    // If |pcmp| is not null then it will store the result of the last
    // compare operation.
    virtual int LowerBound(btree_key_t *key, uint64_t *record_id = 0, int *pcmp = 0) = 0;

    // Searches the node for the |key|, but will always return -1 if
    // an exact match was not found
    virtual int Find(btree_key_t *key) = 0;

    // Returns the full key at the |slot|. Also resolves extended keys
    // and respects BTREE_KEY_USER_ALLOC in dest->flags.
    virtual void Key(int slot, ByteArray *arena, btree_key_t *dest) = 0;

    virtual void GetPayloads(int start_slot, int end_slot, std::shared_ptr<Roaring>& filter) = 0;
    // Returns the number of records of a key at the given |slot|. This is
    // either 1 or higher, but only if duplicate keys exist.
    virtual int RecordCount(int slot) = 0;

    // Returns the record size of a key or one of its duplicates.
    virtual uint32_t RecordSize(int slot,
                                int duplicate_index) = 0;

    // Returns the record id of the key at the given |slot|
    // Only for internal nodes!
    virtual uint64_t RecordId(int slot) const = 0;

    // Sets the record id of the key at the given |slot|
    // Only for internal nodes!
    virtual void SetRecordId(int slot, uint64_t id) = 0;

    // Returns the full record and stores it in |dest|. The record is identified
    // by |slot| and |duplicate_index|. TINY and SMALL records are handled
    // correctly, as well as BTREE_DIRECT_ACCESS.
    virtual void Record(int slot, ByteArray *arena,
                        btree_record_t *record, uint32_t flags,
                        int duplicate_index = 0) = 0;

    // High-level function to set a new record
    //
    // flags can be
    // - BTREE_OVERWRITE
    // - BTREE_DUPLICATE*
    //
    // a previously existing blob will be deleted if necessary
    virtual void SetRecord(int slot, btree_record_t *record,
                           int duplicate_index, uint32_t flags,
                           uint32_t *new_duplicate_index) = 0;

    // Removes the record (or the duplicate of it, if |duplicate_index| is > 0).
    // If |all_duplicates| is set then all duplicates of this key are deleted.
    // |has_duplicates_left| will return true if there are more duplicates left
    // after the current one was deleted.
    virtual void EraseRecord(int slot, int duplicate_index,
                             bool all_duplicates, bool *has_duplicates_left) = 0;

    // High level function to remove an existing entry
    // TODO required?
    virtual void Erase(int slot) = 0;

    virtual void EraseEverything(Context *context) = 0;
    // High level function to insert a new key. Only inserts the key. The
    // actual record is then updated with |set_record|.
    virtual BtreeNode::InsertResult Insert(btree_key_t *key, uint32_t flags) = 0;

    // Returns true if a node requires a split to insert a new |key|
    virtual bool RequiresSplit(const btree_key_t *key = 0) = 0;

    // Returns true if a node requires a merge or a shift
    virtual bool RequiresMerge() const = 0;

    // Splits a page_ and moves all elements at a position >= |pivot|
    // to the |other| page_. If the node is a leaf node then the pivot element
    // is also copied, otherwise it is not because it will be propagated
    // to the parent node instead (by the caller).
    virtual void Split(BtreeNodeProxy *other, int pivot) = 0;

    // Merges all keys from the |other| node to this node
    virtual void MergeFrom(BtreeNodeProxy *other) = 0;

    Page *page_;
};


template<typename T>
struct NumericCompare {
    NumericCompare() {
    }

    int operator()(const void *lhs_data, uint32_t lhs_size,
                   const void *rhs_data, uint32_t rhs_size) const {
        assert(lhs_size == rhs_size);
        assert(lhs_size == sizeof(T));
        T l = *(T *)lhs_data;
        T r = *(T *)rhs_data;
        return l < r ? -1 : (l > r ? +1 : 0);
    }
};

template<class NodeImpl, class Comparator>
struct BtreeNodeProxyImpl : BtreeNodeProxy {
    typedef BtreeNodeProxyImpl<NodeImpl, Comparator> ClassType;

    // Constructor
    BtreeNodeProxyImpl(Page *page)
        : BtreeNodeProxy(page), impl_(page) {
    }

    // Returns the estimated capacity of this node
    virtual size_t EstimateCapacity() const {
        return impl_.EstimateCapacity();
    }

    // Compares two internal keys using the supplied comparator
    virtual int Compare(const btree_key_t *lhs, const btree_key_t *rhs) const {
        Comparator cmp;
        return cmp(lhs->data_, lhs->size_, rhs->data_, rhs->size_);
    }

    // Compares a public key and an internal key
    virtual int Compare(const btree_key_t *lhs, int rhs) {
        Comparator cmp;
        return impl_.Compare(lhs, rhs, cmp);
    }

    // Returns true if the public key and an internal key are equal
    virtual bool Equals(const btree_key_t *lhs, int rhs) {
        return 0 == Compare(lhs, rhs);
    }

    // Searches the node for the key and returns the slot of this key.
    // If |pcmp| is not null then it will store the result of the last
    // compare operation.
    virtual int LowerBound(btree_key_t *key,
                           uint64_t *precord_id = 0, int *pcmp = 0) {
        int dummy;
        if (unlikely(Length() == 0)) {
            if (pcmp)
                *pcmp = 1;
            if (precord_id)
                *precord_id = LeftChild();
            return -1;
        }
        Comparator cmp;
        return impl_.LowerBound(key, cmp, precord_id ? precord_id : 0,
                                pcmp ? pcmp : &dummy);
    }

    // Searches the node for the |key|, but will always return -1 if
    // an exact match was not found
    virtual int Find(btree_key_t *key) {
        if (unlikely(Length() == 0))
            return -1;
        Comparator cmp;
        return impl_.Find(key, cmp);
    }

    // Returns the full key at the |slot|. Also resolves extended keys
    // and respects BTREE_KEY_USER_ALLOC in dest->flags.
    virtual void Key(int slot, ByteArray *arena,
                     btree_key_t *dest) {
        impl_.Key(slot, arena, dest);
    }


    virtual void GetPayloads(int start_slot, int end_slot, std::shared_ptr<Roaring>& filter) {
        impl_.GetPayloads(start_slot, end_slot, filter);
    }

    // Returns the number of records of a key at the given |slot|
    virtual int RecordCount(int slot) {
        assert(slot < (int)Length());
        return impl_.RecordCount(slot);
    }

    // Returns the full record and stores it in |dest|. The record is identified
    // by |slot| and |duplicate_index|. TINY and SMALL records are handled
    // correctly, as well as BTREE_DIRECT_ACCESS.
    virtual void Record(int slot, ByteArray *arena,
                        btree_record_t *record, uint32_t flags,
                        int duplicate_index = 0) {
        assert(slot < (int)Length());
        impl_.Record(slot, arena, record, flags, duplicate_index);
    }

    virtual void SetRecord(int slot, btree_record_t *record,
                           int duplicate_index, uint32_t flags,
                           uint32_t *new_duplicate_index) {
        impl_.SetRecord(slot, record, duplicate_index, flags,
                        new_duplicate_index);
    }

    // Returns the record size of a key or one of its duplicates
    virtual uint32_t RecordSize(int slot,
                                int duplicate_index) {
        assert(slot < (int)Length());
        return impl_.RecordSize(slot, duplicate_index);
    }

    // Returns the record id of the key at the given |slot|
    // Only for internal nodes!
    virtual uint64_t RecordId(int slot) const {
        assert(slot < (int)Length());
        return impl_.RecordId(slot);
    }

    // Sets the record id of the key at the given |slot|
    // Only for internal nodes!
    virtual void SetRecordId(int slot, uint64_t id) {
        return impl_.SetRecordId(slot, id);
    }

    // High level function to remove an existing entry. Will call
    // |erase_extended_key| to clean up (a potential) extended key,
    // and |erase_record| on each record that is associated with the key.
    virtual void Erase(int slot) {
        assert(slot < (int)Length());
        impl_.Erase(slot);
        SetLength(Length() - 1);
    }

    // Removes the record (or the duplicate of it, if |duplicate_index| is > 0).
    // If |all_duplicates| is set then all duplicates of this key are deleted.
    // |has_duplicates_left| will return true if there are more duplicates left
    // after the current one was deleted.
    virtual void EraseRecord(int slot, int duplicate_index,
                             bool all_duplicates, bool *has_duplicates_left) {
        assert(slot < (int)Length());
        impl_.EraseRecord(slot, duplicate_index, all_duplicates);
        if (has_duplicates_left)
            *has_duplicates_left = RecordCount(slot) > 0;
    }

    // Erases all extended keys, overflow areas and records that are
    // linked from this page; usually called when the Database is deleted
    // or an In-Memory Database is closed
    virtual void EraseEverything(Context *context) {
        uint32_t max = Length();
        for (uint32_t i = 0; i < max; i++) {
            // If we're in the leaf page, delete the associated record. (Only
            // leaf nodes have records; internal nodes have record IDs that
            // reference other pages, and these pages must not be deleted.)
            if (IsLeaf())
                EraseRecord(i, 0, true, 0);
        }
    }

    // High level function to insert a new key. Only inserts the key. The
    // actual record is then updated with |set_record|.
    virtual BtreeNode::InsertResult Insert(btree_key_t *key, uint32_t flags) {
        BtreeNode::InsertResult result(0, 0);
        if (impl_.RequiresSplit(key)) {
            result.status_ = BTREE_LIMITS_REACHED;
            return result;
        }

        Comparator cmp;
        try {
            result = impl_.Insert(key, flags, cmp);
        } catch (Exception &ex) {
            //result.status_ = ex.code;
        }

        // split required? then reorganize the node, try again
        if (result.status_ == BTREE_LIMITS_REACHED) {
            try {
                if (impl_.Reorganize(key))
                    result = impl_.Insert(key, flags, cmp);
            } catch (Exception &ex) {
                //result.status_ = ex.code;
            }
        }

        if (result.status_ == BTREE_SUCCESS)
            SetLength(Length() + 1);

        return result;
    }

    // Returns true if a node requires a split to insert |key|
    virtual bool RequiresSplit(const btree_key_t *key = 0) {
        return impl_.RequiresSplit(key);
    }

    // Returns true if a node requires a merge or a shift
    virtual bool RequiresMerge() const {
        return impl_.RequiresMerge();
    }

    // Splits the node
    virtual void Split(BtreeNodeProxy *other_node, int pivot) {
        ClassType *other = dynamic_cast<ClassType *>(other_node);
        assert(other != 0);

        impl_.Split(&other->impl_, pivot);

        uint32_t old_length = Length();
        SetLength(pivot);

        if (IsLeaf())
            other->SetLength(old_length - pivot);
        else
            other->SetLength(old_length - pivot - 1);
    }

    // Merges all keys from the |other| node into this node
    virtual void MergeFrom(BtreeNodeProxy *other_node) {
        ClassType *other = dynamic_cast<ClassType *>(other_node);
        assert(other != 0);

        impl_.MergeFrom(&other->impl_);

        SetLength(Length() + other->Length());
        other->SetLength(0);
    }


    NodeImpl impl_;
};

}