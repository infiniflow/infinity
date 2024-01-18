// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "btree.h"

namespace vespalib::btree {

template <typename KeyT, typename DataT, typename AggrT, typename CompareT, typename TraitsT, class AggrCalcT>
BTree<KeyT, DataT, AggrT, CompareT, TraitsT, AggrCalcT>::BTree(std::shared_ptr<vespalib::alloc::MemoryAllocator> &allocator)
    : _alloc(allocator), _tree() {}

template <typename KeyT, typename DataT, typename AggrT, typename CompareT, typename TraitsT, class AggrCalcT>
BTree<KeyT, DataT, AggrT, CompareT, TraitsT, AggrCalcT>::~BTree() {
    clear();
    _alloc.freeze();
    _alloc.reclaim_all_memory();
}

} // namespace vespalib::btree
