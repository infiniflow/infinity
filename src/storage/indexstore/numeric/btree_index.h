// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "page.h"
#include "btree_types.h"
#include "context.h"
#include "page_manager.h"
#include "btree_node_proxy.h"
#include "common/types/data_type.h"

#include <memory>

namespace infinity {
using Roaring = roaring::Roaring;

struct BtreeHeader {
    BtreeHeader() {
        memset(this, 0, sizeof(*this));
    }

    // address of the root-page
    uint64_t root_address_;

    // flags for this database
    uint32_t flags_;

    // The name of the database
    uint16_t dbname_;

    // key type
    uint16_t key_type_;

    // key size used in the pages
    uint16_t key_size_;

    // the record type
    uint16_t record_type_;

    // the record size
    uint32_t record_size_;

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
    BtreeHeader *btree_header_ = nullptr;

    // the root page of the Btree
    std::unique_ptr<Page> root_page_;
};

struct BtreeVisitor {
    // Specifies if the visitor modifies the node
    virtual bool IsReadOnly() const = 0;

    // called for each node
    virtual void operator()(Context *context, BtreeNodeProxy *node) = 0;
};

class BtreeIndex {
public:
    BtreeIndex(PageManager* page_manager) {
        state_.page_manager_ = page_manager;
    }
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
        state_.root_page_.reset(root_page);
    }

    void Create(Context *context, BtreeHeader *btree_header, LogicalType key_type);

    void Open(Context *context, BtreeHeader *btree_header);

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

    int Insert(Context *context, btree_key_t *key, btree_record_t *record, uint32_t flags);

    int Erase(Context *context, btree_key_t *key, int duplicate_index, uint32_t flags);

    int Find(Context *context, btree_key_t *start_key, btree_key_t *end_key, uint32_t flags, std::shared_ptr<Roaring>& filter);

public:
    BtreeIndexState state_;
};
}