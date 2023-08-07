#pragma once

#include "page.h"
#include "btree_types.h"
#include "context.h"
#include "page_manager.h"
#include "btree_node_proxy.h"
#include "common/types/data_type.h"

#include <memory>

namespace infinity {

struct BtreeHeader {
    BtreeHeader() {
        memset(this, 0, sizeof(*this));
    }

    // Returns the record compression
    uint8_t RecordCompression() const {
        return (compression_ >> 4);
    }

    // Sets the record compression
    void SetRecordCompression(int algorithm) {
        compression_ |= algorithm << 4;
    }

    // Returns the key compression
    uint8_t KeyCompression() const {
        return (compression_ & 0xf);
    }

    // Sets the key compression
    void SetKeyCompression(int algorithm) {
        compression_ |= algorithm & 0xf;
    }

    // address of the root-page
    uint64_t root_address_;

    // flags for this database
    uint32_t flags_;

    // The name of the database
    uint16_t dbname_;

    // key size used in the pages
    uint16_t key_size_;

    // key type
    uint16_t key_type_;

    // for storing key and record compression algorithm */
    uint8_t compression_;

    // reserved
    uint8_t reserved1_;

    // the record size
    uint32_t record_size_;

    // hash of the custom compare function
    uint32_t compare_hash_;

    // the record type
    uint16_t record_type_;
} __attribute__ ((packed));

struct BtreeIndexTraits {
    // virtual destructor
    virtual ~BtreeIndexTraits() { }

    virtual int CompareKeys(btree_key_t *lhs, btree_key_t *rhs) const = 0;

    virtual BtreeNodeProxy *GetNodeFromPageImpl(Page *page) const = 0;
};


struct BtreeIndexState {
    // The Environment's page manager
    PageManager *page_manager_;

    // the Traits class wrapping the template parameters (factory for
    // leaf nodes)
    std::unique_ptr<BtreeIndexTraits> leaf_traits_;

    // the Traits class wrapping the template parameters (factory for
    // internal nodes)
    std::unique_ptr<BtreeIndexTraits> internal_traits_;

    // the index of the PBtreeHeader in the Environment's header page
    BtreeHeader *btree_header_;

    // the root page of the Btree
    Page *root_page_;
};

struct BtreeVisitor {
    // Specifies if the visitor modifies the node
    virtual bool IsReadOnly() const = 0;

    // called for each node
    virtual void operator()(Context *context, BtreeNodeProxy *node) = 0;
};

class BtreeIndex {
public:
    enum {
        // for get_node_from_page(): Page is a leaf
        kLeafPage = 1,

        // for get_node_from_page(): Page is an internal node
        kInternalPage = 2
    };

    Page * RootPage(Context *context);

    void SetRootPage(Page *root_page) {
        root_page->SetType(Page::kTypeBroot);
        state_.btree_header_->root_address_ = root_page->Address();
        state_.root_page_ = root_page;
    }

    void Create(Context *context, BtreeHeader *btree_header);

    // Iterates over the whole index and calls |visitor| on every node
    void VisitNodes(Context *context, BtreeVisitor &visitor, bool visit_internal_nodes);

    // Counts the keys in the btree
    uint64_t Count(Context *context);

    // Drops this index. Deletes all records, overflow areas, extended
    // keys etc from the index; also used to avoid memory leaks when closing
    // in-memory Databases and to clean up when deleting on-disk Databases.
    void Drop(Context *context);

    Page *LowerBound(Context *context, Page *parent, const btree_key_t *key, uint32_t page_manager_flags, int *idxptr);


    int CompareKeys(btree_key_t *lhs, btree_key_t *rhs) const {
        return state_.leaf_traits_->CompareKeys(lhs, rhs);
    }

    // Implementation of get_node_from_page() (for leaf nodes)
    BtreeNodeProxy *LeafNodeFromPageImpl(Page *page) const;

    // Implementation of get_node_from_page() (for internal nodes)
    BtreeNodeProxy *InternalNodeFromPageImpl(Page *page) const;

    BtreeNodeProxy *GetNodeFromPage(Page *page);

    int Insert(Context *context, btree_key_t *key_, btree_record_t *record_, uint32_t flags_);

    int Erase(Context *context, btree_key_t *key, int duplicate_index, uint32_t flags);

    int Find(Context *context, btree_key_t *key, ByteArray *key_arena, btree_record_t *record, ByteArray *record_arena, uint32_t flags);

public:
    BtreeIndexState state_;
};
}