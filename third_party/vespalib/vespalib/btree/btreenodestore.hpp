// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "btreenodestore.h"
#include <vespalib/datastore/datastore.hpp>

namespace vespalib::btree {

template <typename EntryType>
void BTreeNodeBufferType<EntryType>::initialize_reserved_entries(void *buffer, EntryCount reserved_entries) {
    ParentType::initialize_reserved_entries(buffer, reserved_entries);
    EntryType *e = static_cast<EntryType *>(buffer);
    for (size_t j = reserved_entries; j != 0; --j) {
        e->freeze();
        ++e;
    }
}

template <typename EntryType>
void BTreeNodeBufferType<EntryType>::clean_hold(void *buffer, size_t offset, EntryCount num_entries, CleanContext) {
    EntryType *e = static_cast<EntryType *>(buffer) + offset;
    for (size_t j = num_entries; j != 0; --j) {
        e->cleanFrozen();
        ++e;
    }
}

template <typename KeyT, typename DataT, typename AggrT, size_t INTERNAL_SLOTS, size_t LEAF_SLOTS>
BTreeNodeStore<KeyT, DataT, AggrT, INTERNAL_SLOTS, LEAF_SLOTS>::BTreeNodeStore(vespalib::alloc::MemoryAllocator *allocator)
    : _store(), _internalNodeType(MIN_BUFFER_ARRAYS, RefType::offsetSize()), _leafNodeType(MIN_BUFFER_ARRAYS, RefType::offsetSize()) {
    _store.set_memory_allocator(allocator);
    _store.addType(&_internalNodeType);
    _store.addType(&_leafNodeType);
    _store.init_primary_buffers();
    _store.enableFreeLists();
}

template <typename KeyT, typename DataT, typename AggrT, size_t INTERNAL_SLOTS, size_t LEAF_SLOTS>
BTreeNodeStore<KeyT, DataT, AggrT, INTERNAL_SLOTS, LEAF_SLOTS>::~BTreeNodeStore() {
    _store.dropBuffers(); // Drop buffers before type handlers are dropped
}

} // namespace vespalib::btree

#define VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_INTERNALNODE(K, A, S)                                                                              \
    template class BufferType<BTreeInternalNode<K, A, S>, FrozenBtreeNode<BTreeInternalNode<K, A, S>>>
#define VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(K, V, A, S)                                                                               \
    template class BufferType<BTreeLeafNode<K, V, A, S>, FrozenBtreeNode<BTreeLeafNode<K, V, A, S>>>
