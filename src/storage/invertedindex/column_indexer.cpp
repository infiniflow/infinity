module;

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
import parser;
import column_vector;
import analyzer;
import analyzer_pool;
import term;
import column_inverter;
import invert_task;
import commit_task;
import task_executor;
import memory_posting;
import indexer;
import third_party;

module column_indexer;

namespace infinity {

int IndexInverter(Runnable &worker) {
    worker.Run();
    return 1;
}

int IndexCommiter(Runnable &worker) {
    worker.Run();
    return 1;
}

bool ColumnIndexer::KeyComp::operator()(const String &lhs, const String &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

// bool ColumnIndexer::KeyComp::operator()(const TermKey &lhs, const TermKey &rhs) const { return lhs < rhs; }

ColumnIndexer::ColumnIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool)
    : indexer_(indexer), column_id_(column_id), index_config_(index_config), byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool),
      num_inverters_(1), max_inverters_(2) {
    memory_allocator_ = MakeShared<vespalib::alloc::MemoryPoolAllocator>(GetPool());
    SetAnalyzer();
    inverter_ = MakeUnique<ColumnInverter>(this);
    invert_executor_ = SequencedTaskExecutor::Create(IndexInverter, 1, 1);
    commit_executor_ = SequencedTaskExecutor::Create(IndexCommiter, 1, 1);
}

ColumnIndexer::~ColumnIndexer() {
    if (posting_store_.get()) {
        for (auto it = posting_store_->begin(); it.valid(); ++it) {
            delete it.getData();
        }
        posting_store_->clear();
    }
    if (rt_posting_store_.get()) {
        for (auto it = rt_posting_store_->begin(); it.valid(); ++it) {
            delete it.getData();
        }
        rt_posting_store_->clear();
    }
}

void ColumnIndexer::SetIndexMode(IndexMode index_mode) {
    index_mode_ = index_mode;
    switch (index_mode_) {
        case REAL_TIME:
            rt_posting_store_ = MakeUnique<RTPostingTable>(memory_allocator_.get());
        case NEAR_REAL_TIME:
            posting_store_ = MakeUnique<PostingTable>(memory_allocator_.get());
        default: {
        }
    }
}

void ColumnIndexer::SetAnalyzer() {
    String analyzer = index_config_.GetAnalyzer(column_id_);
    analyzer_ = AnalyzerPool::instance().Get(analyzer);
    jieba_specialize_ = analyzer.compare("chinese") == 0 ? true : false;
}

void ColumnIndexer::Insert(RowID row_id, String &data) {
    ColumnInverter inverter(this);
    inverter.InvertColumn(RowID2DocID(row_id), data);
    inverter.Commit();
}

void ColumnIndexer::Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) {
    auto task = MakeUnique<InvertTask>(inverter_.get(), column_vector, row_ids);
    invert_executor_->Execute(0, std::move(task));
}

void ColumnIndexer::SwitchActiveInverter() {
    inflight_inverters_.emplace_back(std::move(inverter_));
    while (!inflight_inverters_.empty() && inflight_inverters_.front()->ZeroRefCount()) {
        free_inverters_.emplace_back(std::move(inflight_inverters_.front()));
        inflight_inverters_.pop_front();
    }
    if (!free_inverters_.empty()) {
        inverter_ = std::move(free_inverters_.back());
        free_inverters_.pop_back();
        return;
    }
    if (num_inverters_ >= max_inverters_) {
        // assert(!inflight_inverters_.empty());
        inverter_ = std::move(inflight_inverters_.front());
        inflight_inverters_.pop_front();
        inverter_->WaitForZeroRefCount();
        return;
    }
    inverter_ = MakeUnique<ColumnInverter>(this);
    ++num_inverters_;
}

void ColumnIndexer::Commit() {
    auto task = MakeUnique<CommitTask>(inverter_.get());
    commit_executor_->Execute(0, std::move(task));
    SwitchActiveInverter();
}

bool ColumnIndexer::NeedDump() { return byte_slice_pool_->GetUsedBytes() >= index_config_.GetMemoryQuota(); }

void ColumnIndexer::Dump() { indexer_->Flush(); }

void ColumnIndexer::Flush() {}

ColumnIndexer::PostingPtr ColumnIndexer::GetOrAddPosting(const TermKey &term) {
    ColumnIndexer::PostingTable::Iterator iter = posting_store_->find(term);
    if (iter.valid())
        return iter.getData();
    else {
        // ColumnIndexer::PostingPtr posting = new MemoryPosting<false>(GetPool(), index_config_.GetPostingFormatOption());
        ColumnIndexer::PostingPtr posting = new PostingWriter(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        posting_store_->insert(iter, term, posting);
        return posting;
    }
}

ColumnIndexer::RTPostingPtr ColumnIndexer::GetOrAddRTPosting(const TermKey &term) {
    ColumnIndexer::RTPostingTable::Iterator iter = rt_posting_store_->find(term);
    if (iter.valid())
        return iter.getData();
    else {
        ColumnIndexer::RTPostingPtr posting = new MemoryPosting<true>(GetPool(), index_config_.GetPostingFormatOption());
        rt_posting_store_->insert(iter, term, posting);
        return posting;
    }
}

} // namespace infinity

namespace vespalib::btree {
using namespace infinity;

template class BTreeNodeDataWrap<ColumnIndexer::TermKey, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeT<ColumnIndexer::TermKey, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeNodeTT<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeTT<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeInternalNode<ColumnIndexer::TermKey, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeLeafNode<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeLeafNode<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeStore<ColumnIndexer::TermKey,
                              ColumnIndexer::PostingPtr,
                              NoAggregated,
                              BTreeDefaultTraits::INTERNAL_SLOTS,
                              BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeStore<ColumnIndexer::TermKey,
                              ColumnIndexer::RTPostingPtr,
                              NoAggregated,
                              BTreeDefaultTraits::INTERNAL_SLOTS,
                              BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeIterator<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeIterator<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTree<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTree<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRoot<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRoot<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, const ColumnIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRootBase<ColumnIndexer::TermKey,
                             ColumnIndexer::PostingPtr,
                             NoAggregated,
                             BTreeDefaultTraits::INTERNAL_SLOTS,
                             BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeRootBase<ColumnIndexer::TermKey,
                             ColumnIndexer::RTPostingPtr,
                             NoAggregated,
                             BTreeDefaultTraits::INTERNAL_SLOTS,
                             BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeAllocator<ColumnIndexer::TermKey,
                                  ColumnIndexer::PostingPtr,
                                  NoAggregated,
                                  BTreeDefaultTraits::INTERNAL_SLOTS,
                                  BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeAllocator<ColumnIndexer::TermKey,
                                  ColumnIndexer::RTPostingPtr,
                                  NoAggregated,
                                  BTreeDefaultTraits::INTERNAL_SLOTS,
                                  BTreeDefaultTraits::LEAF_SLOTS>;

} // namespace vespalib::btree

namespace vespalib::datastore {

using namespace vespalib::btree;
using namespace infinity;

template class BufferType<
    BTreeRoot<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, std::less<ColumnIndexer::TermKey>, BTreeDefaultTraits>>;
template class BufferType<
    BTreeRoot<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, std::less<ColumnIndexer::TermKey>, BTreeDefaultTraits>>;
template class BufferType<BTreeKeyData<ColumnIndexer::TermKey, ColumnIndexer::PostingPtr>>;
template class BufferType<BTreeKeyData<ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr>>;

VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(ColumnIndexer::TermKey, ColumnIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(ColumnIndexer::TermKey, ColumnIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);

} // namespace vespalib::datastore
