// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "btreenodestore.hpp"
#include "btreenodeallocator.h"
#include "btreeroot.h"
#include "btreerootbase.h"
#include <vespalib/datastore/atomic_value_wrapper.h>
#include <vespalib/datastore/buffer_type.hpp>
#include <vespalib/datastore/datastore.h>

namespace vespalib::btree {

template class BTreeNodeStore<uint32_t, uint32_t, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS, BTreeDefaultTraits::LEAF_SLOTS>;
template class BTreeNodeStore<uint32_t, BTreeNoLeafData, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS, BTreeDefaultTraits::LEAF_SLOTS>;
template class BTreeNodeStore<uint32_t, int32_t, MinMaxAggregated, BTreeDefaultTraits::INTERNAL_SLOTS, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeBufferType<BTreeInternalNode<uint32_t, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS>>;
template class BTreeNodeBufferType<BTreeInternalNode<uint32_t, MinMaxAggregated, BTreeDefaultTraits::INTERNAL_SLOTS>>;

template class BTreeNodeBufferType<BTreeLeafNode<uint32_t, uint32_t, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>>;
template class BTreeNodeBufferType<BTreeLeafNode<uint32_t, BTreeNoLeafData, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>>;
template class BTreeNodeBufferType<BTreeLeafNode<uint32_t, int32_t, MinMaxAggregated, BTreeDefaultTraits::LEAF_SLOTS>>;

} // namespace vespalib::btree

namespace vespalib::datastore {

using namespace btree;

VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(uint32_t, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(uint32_t, MinMaxAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(uint64_t, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(uint64_t, MinMaxAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(AtomicEntryRef, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(EntryRef, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS);

VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint32_t, uint32_t, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint32_t, BTreeNoLeafData, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint32_t, int32_t, MinMaxAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint64_t, uint64_t, MinMaxAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint64_t, AtomicValueWrapper<uint64_t>, MinMaxAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(AtomicEntryRef, AtomicEntryRef, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(AtomicEntryRef, BTreeNoLeafData, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(EntryRef, BTreeNoLeafData, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(EntryRef, EntryRef, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint32_t, EntryRef, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(uint64_t, EntryRef, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);

} // namespace vespalib::datastore
