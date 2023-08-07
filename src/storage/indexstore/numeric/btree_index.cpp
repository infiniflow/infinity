// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#include "btree_index.h"
#include "btree_index_factory.h"
#include "common/utility/builtin.h"

namespace infinity {

Page *
BtreeIndex::RootPage(Context *context) {
    if (unlikely(state_.root_page_ == 0))
        state_.root_page_ = state_.page_manager_->Fetch(context, state_.btree_header_->root_address_, 0);
    else
        context->changeset_.Put(state_.root_page_);
    return state_.root_page_;
}

void
BtreeIndex::Create(Context *context, BtreeHeader *btree_header) {
    LogicalType key_type = kInteger;
    LogicalType value_type = kInteger;
    state_.btree_header_ = btree_header;
    state_.leaf_traits_.reset(BtreeIndexFactory::Create(key_type, value_type, true));
    state_.internal_traits_.reset(BtreeIndexFactory::Create(key_type, value_type, false));

    /* allocate a new root page */
    SetRootPage(state_.page_manager_->Alloc(context, Page::kTypeBroot, PageManager::kClearWithZero));

    /* initialize the root page */
    BtreeNode *node = BtreeNode::FromPage(state_.root_page_);
    node->SetFlags(BtreeNode::kLeafNode);
}

// Implementation of GetNodeFromPage() (for leaf nodes)
BtreeNodeProxy*
BtreeIndex::LeafNodeFromPageImpl(Page *page) const {
    return state_.leaf_traits_->GetNodeFromPageImpl(page);
}

// Implementation of GetNodeFromPage() (for internal nodes)
BtreeNodeProxy*
BtreeIndex::InternalNodeFromPageImpl(Page *page) const {
    return state_.internal_traits_->GetNodeFromPageImpl(page);
}

BtreeNodeProxy*
BtreeIndex::GetNodeFromPage(Page *page) {
    if (likely(page->NodeProxy() != 0))
        return page->NodeProxy();

    BtreeNodeProxy *proxy;
    BtreeNode *node = BtreeNode::FromPage(page);
    if (node->IsLeaf())
        proxy = LeafNodeFromPageImpl(page);
    else
        proxy = InternalNodeFromPageImpl(page);

    page->SetNodeProxy(proxy);
    return proxy;
}

struct CalcKeysVisitor : public BtreeVisitor {
    CalcKeysVisitor()
        : count_(0) {
    }

    virtual bool IsReadOnly() const {
        return true;
    }

    virtual void operator()(Context *context, BtreeNodeProxy *node) {
        size_t length = node->Length();

        for (size_t i = 0; i < length; i++)
            count_ += node->RecordCount(i);
    }
    uint64_t count_;
};

uint64_t
BtreeIndex::Count(Context *context) {
    CalcKeysVisitor visitor;
    VisitNodes(context, visitor, false);
    return visitor.count_;
}

struct FreeBlobsVisitor : public BtreeVisitor {
    FreeBlobsVisitor(PageManager *page_manager)
        : page_manager_(page_manager) {
    }

    virtual void operator()(Context *context, BtreeNodeProxy *node) {
        node->EraseEverything(context);
        page_manager_->Del(context, node->page_, 1);
    }

    virtual bool IsReadOnly() const {
        return false;
    }

    PageManager *page_manager_;
};

void
BtreeIndex::Drop(Context *context) {
    FreeBlobsVisitor visitor(state_.page_manager_);
    VisitNodes(context, visitor, true);
}


Page *
BtreeIndex::LowerBound(Context *context, Page *page, const btree_key_t *key, uint32_t page_manager_flags, int *idxptr) {
    BtreeNodeProxy *node = GetNodeFromPage(page);

    // make sure that we're not in a leaf page, and that the
    // page is not empty
    assert(node->LeftChild() != 0);

    uint64_t record_id;
    int slot = node->LowerBound((btree_key_t *)key, &record_id);

    if (idxptr)
        *idxptr = slot;

    return state_.page_manager_->Fetch(context, record_id, page_manager_flags);
}

struct BtreeVisitAction {
    BtreeVisitAction(BtreeIndex *btree_, Context *context,
                     BtreeVisitor &visitor, bool visit_internal_nodes)
        : btree_(btree_), context_(context), visitor_(visitor),
          visit_internal_nodes_(visit_internal_nodes) {
    }

    void Run() {
        uint32_t page_manager_flags = 0;
        if (visitor_.IsReadOnly())
            page_manager_flags = PageManager::kReadOnly;

        // get the root page of the tree
        Page *page = btree_->RootPage(context_);

        // go down to the leaf
        while (page) {
            BtreeNodeProxy *node = btree_->GetNodeFromPage(page);
            uint64_t LeftChild = node->LeftChild();

            // visit internal nodes as well?
            if (LeftChild != 0 && visit_internal_nodes_) {
                while (page) {
                    node = btree_->GetNodeFromPage(page);
                    uint64_t right = node->RightSibling();

                    visitor_(context_, node);

                    // load the right sibling
                    if (likely(right))
                        page = btree_->state_.page_manager_->Fetch(context_, right, page_manager_flags);
                    else
                        page = 0;
                }
            }

            // follow the pointer to the smallest child
            if (likely(LeftChild))
                page = btree_->state_.page_manager_->Fetch(context_, LeftChild, page_manager_flags);
            else
                break;
        }

        assert(page != 0);

        // now visit all leaf nodes
        while (page) {
            BtreeNodeProxy *node = btree_->GetNodeFromPage(page);
            uint64_t right = node->RightSibling();

            visitor_(context_, node);

            /* follow the pointer to the right sibling */
            if (likely(right))
                page = btree_->state_.page_manager_->Fetch(context_, right, page_manager_flags);
            else
                break;
        }
    }

    BtreeIndex *btree_;
    Context *context_;
    BtreeVisitor &visitor_;
    bool visit_internal_nodes_;
};

void
BtreeIndex::VisitNodes(Context *context, BtreeVisitor &visitor, bool visit_internal_nodes) {
    BtreeVisitAction bva(this, context, visitor, visit_internal_nodes);
    bva.Run();
}

struct BtreeUpdateAction {
    BtreeUpdateAction(BtreeIndex *btree, Context *context, uint32_t duplicate_index)
        : btree_(btree), context_(context), duplicate_index_(duplicate_index) {
    }

    // Traverses the tree, looking for the leaf with the specified |key|. Will
    // split or merge nodes while descending.
    // Returns the leaf page and the |parent| of the leaf (can be null if
    // there is no parent).
    Page *TraverseTree(Context *context, const btree_key_t *key, Page **parent);

    // Splits |page| and updates the |parent|. If |parent| is null then
    // it's assumed that |page| is the root node.
    // Returns the new page in the path for |key|; caller can immediately
    // continue the traversal.
    Page *SplitPage(Page *old_page, Page *parent, const btree_key_t *key);

    // Inserts a key in a page
    int InsertInPage(Page *page, btree_key_t *key,
                     btree_record_t *record,
                     bool force_prepend = false, bool force_append = false);

    // the current btree_
    BtreeIndex *btree_;

    // The caller's Context
    Context *context_;


    // the duplicate index (in case the update is for a duplicate key)
    // 1-based (if 0 then this update is not for a duplicate)
    uint32_t duplicate_index_;
};

// Calculates the pivot index of a split.
//
// For databases with sequential access (this includes recno databases):
// do not split in the middle, but at the very end of the page.
//
// If this page is the right-most page in the index, and the new key is
// inserted at the very end, then we select the same pivot as for
// sequential access.
static inline int
PivotPosition(BtreeUpdateAction &state, BtreeNodeProxy *old_node, const btree_key_t *key) {
    uint32_t old_count = old_node->Length();
    assert(old_count > 2);

    bool pivot_at_end = false;
    if (old_node->RightSibling() == 0) {
        int cmp = old_node->Compare(key, old_node->Length() - 1);
        if (cmp > 0)
            pivot_at_end = true;
    }

    /* The position of the pivot key depends on the previous inserts; if most
     * of them were appends then pick a pivot key at the "end" of the node */
    int pivot;
    if (pivot_at_end)
        pivot = old_count - 2;
    else
        pivot = old_count / 2;

    assert(pivot > 0 && pivot <= (int)old_count - 2);

    return pivot;
}

// Allocates a new root page and sets it up in the btree_
static inline Page *
AllocateNewRoot(BtreeUpdateAction &state, Page *old_root) {
    Page *new_root = state.btree_->state_.page_manager_->Alloc(state.context_, Page::kTypeBroot);
    BtreeNodeProxy *new_node = state.btree_->GetNodeFromPage(new_root);
    new_node->SetLeftChild(old_root->Address());

    state.btree_->SetRootPage(new_root);
    Page *header = state.btree_->state_.page_manager_->Fetch(state.context_, 0);
    header->SetDirty(true);

    old_root->SetType(Page::kTypeBindex);

    return new_root;
}

/* Merges the |sibling| into |page|, returns the merged page and moves
 * the sibling to the freelist */
static inline Page *
MergePage(BtreeUpdateAction &state, Page *page, Page *sibling) {
    BtreeNodeProxy *node = state.btree_->GetNodeFromPage(page);
    BtreeNodeProxy *sib_node = state.btree_->GetNodeFromPage(sibling);

    //if (sib_node->IsLeaf())
    //    BtreeCursor::uncouple_all_cursors(state.context_, sibling, 0);

    node->MergeFrom(sib_node);
    page->SetDirty(true);

    // fix the linked list
    node->SetRightSibling(sib_node->RightSibling());
    if (node->RightSibling()) {
        Page *p = state.btree_->state_.page_manager_->Fetch(state.context_, node->RightSibling());
        BtreeNodeProxy *new_right_node = state.btree_->GetNodeFromPage(p);
        new_right_node->SetLeftSibling(page->Address());
        p->SetDirty(true);
    }

    state.btree_->state_.page_manager_->Del(state.context_, sibling);

    return page;
}

// collapse the root node; returns the new root
static inline Page *
CollapseRoot(BtreeUpdateAction &state, Page *RootPage) {
    BtreeNodeProxy *node = state.btree_->GetNodeFromPage(RootPage);
    assert(node->Length() == 0);

    Page *header = state.btree_->state_.page_manager_->Fetch(state.context_, 0);
    header->SetDirty(true);

    Page *new_root = state.btree_->state_.page_manager_->Fetch(state.context_,
                     node->LeftChild());
    state.btree_->SetRootPage(new_root);
    state.btree_->state_.page_manager_->Del(state.context_, RootPage);
    return new_root;
}

// Traverses the tree, looking for the leaf with the specified |key|. Will
// split or merge nodes while descending.
// Returns the leaf page and the |parent| of the leaf (can be null if
// there is no parent).
Page *
BtreeUpdateAction::TraverseTree(Context *context, const btree_key_t *key, Page **parent) {
    Page *page = btree_->RootPage(context);
    BtreeNodeProxy *node = btree_->GetNodeFromPage(page);

    *parent = 0;

    // if the root page is empty with children then collapse it
    if (unlikely(node->Length() == 0 && !node->IsLeaf())) {
        page = CollapseRoot(*this, page);
        node = btree_->GetNodeFromPage(page);
    }

    int slot;

    // now walk down the tree
    while (!node->IsLeaf()) {
        // is a split required?
        if (node->RequiresSplit()) {
            page = SplitPage(page, *parent, key);
            node = btree_->GetNodeFromPage(page);
        }

        // get the child page
        Page *sibling = 0;
        Page *child_page = btree_->LowerBound(context, page, key, 0, &slot);
        BtreeNodeProxy *child_node = btree_->GetNodeFromPage(child_page);

        // We can merge this child with the RIGHT sibling iff...
        // 1. it's not the right-most slot (and therefore the right sibling has
        //      the same parent as the child)
        // 2. the child is a leaf!
        // 3. it's empty or has too few elements
        // 4. its right sibling is also empty
        if (unlikely(slot < (int)node->Length() - 1
                     && child_node->IsLeaf()
                     && child_node->RequiresMerge()
                     && child_node->RightSibling() != 0)) {
            sibling = btree_->state_.page_manager_->Fetch(context, child_node->RightSibling(),
                      PageManager::kOnlyFromCache);
            if (sibling != 0) {
                BtreeNodeProxy *sib_node = btree_->GetNodeFromPage(sibling);
                if (sib_node->RequiresMerge()) {
                    MergePage(*this, child_page, sibling);
                    // also remove the link to the sibling from the parent
                    node->Erase(slot + 1);
                    page->SetDirty(true);
                }
            }
        }

        // We can also merge this child with the LEFT sibling iff...
        // 1. it's not the left-most slot
        // 2. the child is a leaf!
        // 3. it's empty or has too few elements
        // 4. its left sibling is also empty
        else if (unlikely(slot > 0
                          && child_node->IsLeaf()
                          && child_node->RequiresMerge()
                          && child_node->LeftSibling() != 0)) {
            sibling = btree_->state_.page_manager_->Fetch(context, child_node->LeftSibling(),
                      PageManager::kOnlyFromCache);
            if (sibling != 0) {
                BtreeNodeProxy *sib_node = btree_->GetNodeFromPage(sibling);
                if (sib_node->RequiresMerge()) {
                    MergePage(*this, sibling, child_page);
                    // also remove the link to the sibling from the parent
                    node->Erase(slot);
                    page->SetDirty(true);
                    // continue traversal with the sibling
                    child_page = sibling;
                    child_node = sib_node;
                }
            }
        }

        *parent = page;

        // go down one level in the tree
        page = child_page;
        node = child_node;
    }

    return page;
}

Page *
BtreeUpdateAction::SplitPage(Page *old_page, Page *parent, const btree_key_t *key) {
    BtreeNodeProxy *old_node = btree_->GetNodeFromPage(old_page);

    /* allocate a new page and initialize it */
    Page *new_page = btree_->state_.page_manager_->Alloc(context_, Page::kTypeBindex);
    {
        BtreeNode *node = BtreeNode::FromPage(new_page);
        node->SetFlags(old_node->IsLeaf() ? BtreeNode::kLeafNode : 0);
    }
    BtreeNodeProxy *new_node = btree_->GetNodeFromPage(new_page);

    /* no parent page? then we're splitting the root page. allocate
     * a new root page */
    if (unlikely(!parent))
        parent = AllocateNewRoot(*this, old_page);

    Page *to_return = 0;
    ByteArray pivot_key_arena;
    btree_key_t pivot_key = {0};

    /* if the key is appended then don't split the page; simply allocate
     * a new page and insert the new key. */
    int pivot = 0;

    /* no append? then calculate the pivot key and perform the split */
    if (pivot != (int)old_node->Length()) {
        pivot = PivotPosition(*this, old_node, key);

        /* and store the pivot key for later */
        old_node->Key(pivot, &pivot_key_arena, &pivot_key);

        /* leaf page: uncouple all cursors */
        //if (old_node->IsLeaf())
        //    BtreeCursor::uncouple_all_cursors(context, old_page, pivot);
        /* internal page: fix the ptr_down of the new page
         * (it must point to the ptr of the pivot key) */
        //else
        new_node->SetLeftChild(old_node->RecordId(pivot));

        /* now move some of the key/rid-tuples to the new page */
        old_node->Split(new_node, pivot);

        // if the new key is >= the pivot key then continue with the right page,
        // otherwise continue with the left page
        to_return = btree_->CompareKeys((btree_key_t *)key, &pivot_key) >= 0
                    ? new_page
                    : old_page;
    }

    /* update the parent page */
    BtreeNodeProxy *parent_node = btree_->GetNodeFromPage(parent);
    uint64_t rid = new_page->Address();
    btree_record_t record = btree_make_record(&rid, sizeof(rid));
    int st = InsertInPage(parent, &pivot_key, &record);
    if (st)
        throw StorageException("insert error");

    /* new root page? then also set the cuild pointer */
    if (parent_node->Length() == 0)
        parent_node->SetLeftChild(old_page->Address());

    /* fix the double-linked list of pages, and mark the pages as dirty */
    if (old_node->RightSibling()) {
        Page *sib_page = btree_->state_.page_manager_->Fetch(context_,
                         old_node->RightSibling());
        BtreeNodeProxy *sib_node = btree_->GetNodeFromPage(sib_page);
        sib_node->SetLeftSibling(new_page->Address());
        sib_page->SetDirty(true);
    }
    new_node->SetLeftSibling(old_page->Address());
    new_node->SetRightSibling(old_node->RightSibling());
    old_node->SetRightSibling(new_page->Address());
    new_page->SetDirty(true);
    old_page->SetDirty(true);

    return to_return;
}

int
BtreeUpdateAction::InsertInPage(Page *page, btree_key_t *key,
                                btree_record_t *record,
                                bool force_prepend, bool force_append) {
    bool exists = false;

    BtreeNodeProxy *node = btree_->GetNodeFromPage(page);

    int flags = 0;
    if (force_prepend)
        flags |= BtreeNode::kInsertPrepend;
    if (force_append)
        flags |= BtreeNode::kInsertAppend;

    BtreeNode::InsertResult result = node->Insert(key, flags);

    uint32_t new_duplicate_id = 0;
    if (exists) {
        if (node->IsLeaf()) {
            // overwrite record blob
            node->SetRecord(result.slot_, record, duplicate_index_,
                            flags, &new_duplicate_id);

        } else {
            // overwrite record id
            assert(record->size_ == sizeof(uint64_t));
            node->SetRecordId(result.slot_, *(uint64_t *)record->data_);
        }
    }
    // key does not exist and has to be inserted or appended
    else {
        try {
            if (node->IsLeaf()) {
                // allocate record id
                node->SetRecord(result.slot_, record, duplicate_index_,
                                flags, &new_duplicate_id);

            } else {
                // set the internal record id
                assert(record->size_ == sizeof(uint64_t));
                node->SetRecordId(result.slot_, *(uint64_t *)record->data_);
            }
        }
        // In case of an error: undo the insert. This happens very rarely but
        // it's possible, i.e. if the BlobManager fails to allocate storage.
        catch (Exception &ex) {
            if (result.slot_ < (int)node->Length())
                node->Erase(result.slot_);
            throw ex;
        }
    }

    page->SetDirty(true);

    // if this update was triggered with a cursor (and this is a leaf node):
    // couple it to the inserted key
    // TODO only when performing an insert(), not an erase()!
    //if (cursor && node->IsLeaf())
    //    cursor->couple_to(page, result.slot, new_duplicate_id);

    return 0;
}


struct BtreeInsertAction : public BtreeUpdateAction {
    BtreeInsertAction(BtreeIndex *btree, Context *context,
                      btree_key_t *key, btree_record_t *record, uint32_t flags)
        : BtreeUpdateAction(btree, context, 0),
          key_(key),
          record_(record),
          flags_(flags) {}

    // This is the entry point for the actual insert operation
    int Run() {
        /*
         * append the key_? AppendOrPrependKey() will try to append or
         * prepend the key_; if this fails because the key_ is NOT the largest
         * (or smallest) key_ in the database or because the current page is
         * already full, it will remove the HINT_APPEND (or HINT_PREPEND)
         * flag and call Insert()
         */
        /*
                int st;
                if (hints.leaf_page_addr
                        && ISSETANY(hints.flags_, UPS_HINT_APPEND | UPS_HINT_PREPEND)) {
                    st = AppendOrPrependKey();
                    if (unlikely(st == UPS_LIMITS_REACHED))
                        st = Insert();
                } else {
                    st = Insert();
                }
        */
        return Insert();
    }

    int Insert() {
        // traverse the tree till a leaf is reached
        Page *parent;
        Page *page = TraverseTree(context_, key_, &parent);

        // We've reached the leaf; it's still possible that we have to
        // split the page, therefore this case has to be handled
        int st = InsertInPage(page, key_, record_);
        if (unlikely(st == BTREE_LIMITS_REACHED)) {
            page = SplitPage(page, parent, key_);
            return InsertInPage(page, key_, record_);
        }

        return st;
    }

    // the key_ that is inserted
    btree_key_t *key_;

    // the record_ that is inserted
    btree_record_t *record_;

    // flags_ of ups_db_insert()
    uint32_t flags_;

};

int BtreeIndex::Insert(Context *context, btree_key_t *key_, btree_record_t *record_, uint32_t flags_) {
    BtreeInsertAction bia(this, context, key_, record_, flags_);
    int st = bia.Run();
    return 0;
}



struct BtreeEraseAction : public BtreeUpdateAction {
    BtreeEraseAction(BtreeIndex *btree, Context *context,
                     btree_key_t *key, int duplicate_index, uint32_t /* flags (not used) */)
        : BtreeUpdateAction(btree, context, duplicate_index),
          key_(key) {
    }

    int Run() {
        // traverse the tree to the leaf, splitting/merging nodes as required
        Page *parent;
        Page *page = TraverseTree(context_, key_, &parent);
        BtreeNodeProxy *node = btree_->GetNodeFromPage(page);

        // we have reached the leaf; search the leaf for the key
        int slot = node->Find(key_);
        if (slot < 0) {
            return -1;
        }

        // remove the key from the leaf
        return RemoveEntry(page, parent, slot);
    }

    int RemoveEntry(Page *page, Page *parent, int slot) {
        BtreeNodeProxy *node = btree_->GetNodeFromPage(page);

        assert(slot >= 0);
        assert(slot < (int)node->Length());

        // delete the record, but only on leaf nodes! internal nodes don't have
        // records; they point to pages instead, and we do not want to delete
        // those.
        bool has_duplicates_left = false;
        if (node->IsLeaf()) {
            // only delete a duplicate?
            if (duplicate_index_ > 0)
                node->EraseRecord(slot, duplicate_index_ - 1, false,
                                   &has_duplicates_left);
            else
                node->EraseRecord(slot, 0, true, 0);
        }

        page->SetDirty(true);

        if (has_duplicates_left)
            return 0;

        // We've reached the leaf; it's still possible that we have to
        // split the page, therefore this case has to be handled
        try {
            node->Erase(slot);
        } catch (Exception &ex) {
            // Split the page in the middle. This will invalidate the |node| pointer
            // and the |slot| of the key, therefore restart the whole operation
            SplitPage(page, parent, key_);
            return Run();
        }

        return 0;
    }

    // the key that is retrieved
    btree_key_t *key_;
};

int
BtreeIndex::Erase(Context *context, btree_key_t *key, int duplicate_index, uint32_t flags) {
    BtreeEraseAction bea(this, context, key, duplicate_index, flags);
    return bea.Run();
}

struct BtreeFindAction {
    BtreeFindAction(BtreeIndex *btree, Context *context,
                    btree_key_t *key, ByteArray *key_arena,
                    btree_record_t *record, ByteArray *record_arena,
                    uint32_t flags)
        : btree_(btree), context_(context), key_(key),
          record_(record), flags_(flags), key_arena_(key_arena),
          record_arena_(record_arena) {
    }

    int Run() {
        Page *page = 0;
        int slot = -1;
        BtreeNodeProxy *node = 0;
        uint32_t is_approx_match = 0;

        if (slot == -1) {
            /* load the root page */
            page = btree_->RootPage(context_);

            /* now traverse the root to the leaf nodes till we find a leaf */
            node = btree_->GetNodeFromPage(page);
            while (!node->IsLeaf()) {
                page = btree_->LowerBound(context_, page, key_, PageManager::kReadOnly, 0);
                if (unlikely(!page)) {
                    return -1;
                }

                node = btree_->GetNodeFromPage(page);
            }

            /* check the leaf page for the key (shortcut w/o approx. matching) */
            if (flags_ == 0) {
                slot = node->Find(key_);
                if (unlikely(slot == -1)) {
                    return -1;
                }

                goto return_result;
            }

            /* check the leaf page for the key (long path w/ approx. matching),
             * then fall through */
            slot = Find(context_, page, key_, flags_, &is_approx_match);
        }

        if (unlikely(slot == -1)) {
            // find the left sibling
            if (node->LeftSibling() > 0) {
                page = btree_->state_.page_manager_->Fetch(context_, node->LeftSibling(), PageManager::kReadOnly);
                node = btree_->GetNodeFromPage(page);
                slot = node->Length() - 1;
                is_approx_match = BtreeKey::kLower;
            }
        } else if (unlikely(slot >= (int)node->Length())) {
            // find the right sibling
            if (node->RightSibling() > 0) {
                page = btree_->state_.page_manager_->Fetch(context_, node->RightSibling(),PageManager::kReadOnly);
                node = btree_->GetNodeFromPage(page);
                slot = 0;
                is_approx_match = BtreeKey::kGreater;
            } else
                slot = -1;
        }

        if (unlikely(slot < 0)) {
            return -1;
        }

        assert(node->IsLeaf());

return_result:

        // approx. match: patch the key flags
        key_->flags_ = is_approx_match;

        // no need to load the key if we have an exact match
        if (key_ && is_approx_match )
            node->Key(slot, key_arena_, key_);

        if (likely(record_ != 0))
            node->Record(slot, record_arena_, record_, flags_);

        return 0;
    }

    // Searches a leaf node for a key.
    //
    // !!!
    // only works with leaf nodes!!
    //
    // Returns the index of the key, or -1 if the key was not found, or
    // another negative status code value when an unexpected error occurred.
    int Find(Context *context, Page *page, btree_key_t *key, uint32_t flags, uint32_t *is_approx_match) {
        *is_approx_match = 0;

        BtreeNodeProxy *node = btree_->GetNodeFromPage(page);
        if (unlikely(node->Length() == 0))
            return -1;

        int cmp;
        int slot = node->LowerBound(key, 0, &cmp);

        if (cmp == 0)
            return slot;

        // approx. matching: smaller key is required
        /*
        if (ISSET(flags, UPS_FIND_LT_MATCH)) {
            if (cmp == 0 && ISSET(flags, UPS_FIND_GT_MATCH)) {
                *is_approx_match = BtreeKey::kLower;
                return slot + 1;
            }

            if (slot < 0 && ISSET(flags, UPS_FIND_GT_MATCH)) {
                *is_approx_match = BtreeKey::kGreater;
                return 0;
            }
            *is_approx_match = BtreeKey::kLower;
            return cmp <= 0 ? slot - 1 : slot;
        }

        // approx. matching: greater key is required
        if (ISSET(flags, UPS_FIND_GT_MATCH)) {
            *is_approx_match = BtreeKey::kGreater;
            return slot + 1;
        }*/

        return cmp ? -1 : slot;
    }

    // the current btree
    BtreeIndex *btree_;

    // The caller's Context
    Context *context_;

    // the key that is retrieved
    btree_key_t *key_;

    // the record that is retrieved
    btree_record_t *record_;

    // flags of ups_db_find()
    uint32_t flags_;

    // allocator for the key data
    ByteArray *key_arena_;

    // allocator for the record data
    ByteArray *record_arena_;
};

int
BtreeIndex::Find(Context *context, btree_key_t *key,
                 ByteArray *key_arena, btree_record_t *record,
                 ByteArray *record_arena, uint32_t flags) {
    BtreeFindAction bfa(this, context, key, key_arena, record, record_arena, flags);
    return bfa.Run();
}



}