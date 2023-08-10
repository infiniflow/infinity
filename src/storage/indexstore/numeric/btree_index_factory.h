// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "btree_index.h"
#include "btree_node.h"
#include "btree_node_proxy.h"
#include "btree_keys.h"
#include "btree_records.h"
#include "common/types/data_type.h"

namespace infinity {

template<class NodeLayout, class Comparator>
struct BtreeIndexTraitsImpl : public BtreeIndexTraits {
    // Compares two keys
    // Returns -1, 0, +1 or higher positive values are the result of a
    // successful key comparison (0 if both keys match, -1 when
    // LHS < RHS key, +1 when LHS > RHS key).
    virtual int CompareKeys(btree_key_t *lhs,
                            btree_key_t *rhs) const {
        Comparator cmp;
        return cmp(lhs->data_, lhs->size_, rhs->data_, rhs->size_);
    }

    // Implementation of get_node_from_page()
    virtual BtreeNodeProxy *GetNodeFromPageImpl(Page *page) const {
        return new BtreeNodeProxyImpl<NodeLayout, Comparator>(page);
    }
};

#define PAX_INTERNAL_NODE(KeyList, Compare) \
            return (new BtreeIndexTraitsImpl                                  \
                      <PaxNodeImpl<KeyList, InternalRecordList>,              \
                      Compare >())

#define PAX_INTERNAL_NUMERIC(type) \
            PAX_INTERNAL_NODE(PodKeyWithPayLoadList<type>, NumericCompare<type> )


#define LEAF_NODE_IMPL(Impl, KeyList, Compare) \
        {                                                                       \
            switch (value_type) {                                               \
                case kBoolean:                                                  \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<uint8_t> >,          \
                              Compare >());                                     \
                case kTinyInt:                                                  \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<uint16_t> >,         \
                              Compare >());                                     \
                case kInteger:                                                  \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<uint32_t> >,         \
                              Compare >());                                     \
                case kBigInt:                                                   \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<uint64_t> >,         \
                              Compare >());                                     \
                case kFloat:                                                    \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<float> >,            \
                              Compare >());                                     \
                case kDouble:                                                   \
                    return (new BtreeIndexTraitsImpl                            \
                              <Impl<KeyList, PodRecordList<double> >,           \
                              Compare >());                                     \
                default:                                                        \
                    assert(!"shouldn't be here");                               \
                    return (0);                                                 \
            }                                                                   \
        }
#define PAX_LEAF_NUMERIC(type) \
        LEAF_NODE_IMPL(PaxNodeImpl, PodKeyWithPayLoadList<type>, NumericCompare<type> )

struct BtreeIndexFactory {
    static BtreeIndexTraits *Create(LogicalType key_type, LogicalType value_type, bool is_leaf) {
        switch(key_type) {
        case kBoolean:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(uint8_t);
            PAX_LEAF_NUMERIC(uint8_t);
        case kTinyInt:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(uint16_t);
            PAX_LEAF_NUMERIC(uint16_t);
        case kInteger:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(uint32_t);
            PAX_LEAF_NUMERIC(uint32_t);
        case kBigInt:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(uint64_t);
            PAX_LEAF_NUMERIC(uint64_t);
        case kFloat:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(float);
            PAX_LEAF_NUMERIC(float);
        case kDouble:
            if (!is_leaf)
                PAX_INTERNAL_NUMERIC(double);
            PAX_LEAF_NUMERIC(double);
        default:
            return nullptr;
        }
    }

};

}