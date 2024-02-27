// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <ctpl_stl.h>
#include <storage/invertedindex/common/vespa_alloc.h>
#include <vespalib/btree/btree.hpp>
#include <vespalib/btree/btreebuilder.hpp>
#include <vespalib/btree/btreeiterator.hpp>
#include <vespalib/btree/btreenode.hpp>
#include <vespalib/btree/btreenodeallocator.hpp>
#include <vespalib/btree/btreenodestore.hpp>
#include <vespalib/btree/btreeroot.hpp>
#include <vespalib/btree/btreerootbase.hpp>
#include <vespalib/btree/btreestore.hpp>
#include <vespalib/btree/btreetraits.h>
#include <vespalib/btree/noaggregated.h>
#include <vespalib/datastore/buffer_type.hpp>

#include <string.h>

module memory_indexer;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import index_segment_reader;
import posting_writer;
import data_block;

import column_vector;
import analyzer;
import analyzer_pool;
import term;
import column_inverter;
import invert_task;
import indexer;
import third_party;
import ring;

namespace infinity {

bool MemoryIndexer::KeyComp::operator()(const String &lhs, const String &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

MemoryIndexer::MemoryIndexer(u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool,
                             ThreadPool &thread_pool)
    : column_id_(column_id), index_config_(index_config), byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool), thread_pool_(thread_pool),
      ring_inverted_(10UL), ring_sorted_(10UL) {
    memory_allocator_ = MakeShared<vespalib::alloc::MemoryPoolAllocator>(GetPool());
    posting_store_ = MakeUnique<PostingTable>(memory_allocator_.get());
    SetAnalyzer();
}

MemoryIndexer::~MemoryIndexer() { Reset(); }

void MemoryIndexer::SetIndexMode(IndexMode index_mode) { index_mode_ = index_mode; }

void MemoryIndexer::SetAnalyzer() {
    String analyzer = index_config_.GetAnalyzer(column_id_);
    analyzer_ = AnalyzerPool::instance().Get(analyzer);
    jieba_specialize_ = analyzer.compare("chinese") == 0 ? true : false;
}

void MemoryIndexer::Insert(const ColumnVector &column_vector, u32 row_offset, u32 row_count, RowID row_id_begin) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_++;
    }
    u64 seq_inserted = seq_inserted_++;
    auto task = MakeShared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, row_id_begin);
    auto func = [this, &task](int id) {
        auto inverter = MakeShared<ColumnInverter>(*this);
        inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->row_id_begin_);
        this->ring_inverted_.Put(task->task_seq_, inverter);
    };
    thread_pool_.push(func);
}

void MemoryIndexer::Commit() {
    thread_pool_.push([this](int id) {
        Vector<SharedPtr<ColumnInverter>> inverters;
        u64 seq_commit = this->ring_inverted_.GetMulti(inverters);
        SizeT num = inverters.size();
        for (SizeT i = 1; i < num; i++) {
            inverters[0]->Merge(*inverters[i]);
        }
        inverters[0]->Sort();
        this->ring_sorted_.Put(seq_commit, inverters[0]);
        this->ring_sorted_.Iterate([](SharedPtr<ColumnInverter> &inverter) { inverter->GeneratePosting(); });
        {
            std::unique_lock<std::mutex> lock(mutex_);
            inflight_tasks_ -= num;
            if (inflight_tasks_ == 0) {
                cv_.notify_all();
            }
        }
    });
}

MemoryIndexer::PostingPtr MemoryIndexer::GetOrAddPosting(const TermKey &term) {
    MemoryIndexer::PostingTable::Iterator iter = posting_store_->find(term);
    if (iter.valid())
        return iter.getData();
    else {
        MemoryIndexer::PostingPtr posting =
            MakeShared<PostingWriter>(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        posting_store_->insert(iter, term, posting);
        return posting;
    }
}

void MemoryIndexer::ReclaimMemory() {
    if (posting_store_.get()) {
        posting_store_->getAllocator().freeze();

        GenerationHandler::generation_t generation = generation_handler_.getCurrentGeneration();
        posting_store_->getAllocator().assign_generation(generation);

        generation_handler_.incGeneration();
        GenerationHandler::generation_t oldest_gen = generation_handler_.get_oldest_used_generation();

        posting_store_->getAllocator().reclaim_memory(oldest_gen);
    }
}

void MemoryIndexer::Reset() {
    if (posting_store_.get()) {
        for (auto it = posting_store_->begin(); it.valid(); ++it) {
            // delete it.getData();
        }
        posting_store_->clear();
    }
    thread_pool_.stop(true);
    cv_.notify_all();
}

} // namespace infinity

namespace vespalib::btree {
using namespace infinity;

template class BTreeNodeDataWrap<MemoryIndexer::TermKey, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeT<MemoryIndexer::TermKey, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeNodeTT<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeInternalNode<MemoryIndexer::TermKey, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeLeafNode<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeStore<MemoryIndexer::TermKey,
                              MemoryIndexer::PostingPtr,
                              NoAggregated,
                              BTreeDefaultTraits::INTERNAL_SLOTS,
                              BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeIterator<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTree<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRootBase<MemoryIndexer::TermKey,
                             MemoryIndexer::PostingPtr,
                             NoAggregated,
                             BTreeDefaultTraits::INTERNAL_SLOTS,
                             BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeAllocator<MemoryIndexer::TermKey,
                                  MemoryIndexer::PostingPtr,
                                  NoAggregated,
                                  BTreeDefaultTraits::INTERNAL_SLOTS,
                                  BTreeDefaultTraits::LEAF_SLOTS>;

} // namespace vespalib::btree

namespace vespalib::datastore {

using namespace vespalib::btree;
using namespace infinity;

template class BufferType<
    BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, std::less<MemoryIndexer::TermKey>, BTreeDefaultTraits>>;
template class BufferType<BTreeKeyData<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr>>;

VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);

} // namespace vespalib::datastore
