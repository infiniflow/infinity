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
import parser;
import column_vector;
import analyzer;
import analyzer_pool;
import term;
import sequential_column_inverter;
import parallel_column_inverter;
import invert_task;
import commit_task;
import task_executor;
import memory_posting;
import indexer;
import third_party;
import index_builder;

namespace infinity {

int IndexInverter(Runnable &worker) {
    worker.Run();
    return 1;
}

int IndexCommiter(Runnable &worker) {
    worker.Run();
    return 1;
}

bool MemoryIndexer::KeyComp::operator()(const String &lhs, const String &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

// bool MemoryIndexer::KeyComp::operator()(const TermKey &lhs, const TermKey &rhs) const { return lhs < rhs; }

MemoryIndexer::MemoryIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool)
    : indexer_(indexer), column_id_(column_id), index_config_(index_config), byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool),
      num_inverters_(1), max_inverters_(4) {
    memory_allocator_ = MakeShared<vespalib::alloc::MemoryPoolAllocator>(GetPool());
    SetAnalyzer();
    inverter_ = MakeUnique<SequentialColumnInverter>(this);
    invert_executor_ = SequencedTaskExecutor::Create(IndexInverter, index_config_.GetIndexingParallelism(), index_config_.GetIndexingParallelism());
    commit_executor_ = SequencedTaskExecutor::Create(IndexCommiter, 1, 1);
}

MemoryIndexer::~MemoryIndexer() { Reset(); }

void MemoryIndexer::SetIndexMode(IndexMode index_mode) {
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

void MemoryIndexer::SetAnalyzer() {
    String analyzer = index_config_.GetAnalyzer(column_id_);
    analyzer_ = AnalyzerPool::instance().Get(analyzer);
    jieba_specialize_ = analyzer.compare("chinese") == 0 ? true : false;
}

void MemoryIndexer::Insert(RowID row_id, String &data) {
    SequentialColumnInverter inverter(this);
    inverter.InvertColumn(RowID2DocID(row_id), data);
    inverter.Commit();
}

void MemoryIndexer::Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) {
    auto task = MakeUnique<InvertTask>(inverter_.get(), column_vector, row_ids);
    invert_executor_->Execute(0, std::move(task));
}

void MemoryIndexer::SwitchActiveInverter() {
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
    inverter_ = MakeUnique<SequentialColumnInverter>(this);
    ++num_inverters_;
}

void MemoryIndexer::Commit() {
    auto task = MakeUnique<CommitTask>(inverter_.get());
    commit_executor_->Execute(0, std::move(task));
    SwitchActiveInverter();
}

bool MemoryIndexer::NeedDump() { return indexer_->NeedDump(); }

MemoryIndexer::PostingPtr MemoryIndexer::GetOrAddPosting(const TermKey &term) {
    MemoryIndexer::PostingTable::Iterator iter = posting_store_->find(term);
    if (iter.valid())
        return iter.getData();
    else {
        // MemoryIndexer::PostingPtr posting = new MemoryPosting<false>(GetPool(), index_config_.GetPostingFormatOption());
        // MemoryIndexer::PostingPtr posting = new PostingWriter(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        MemoryIndexer::PostingPtr posting =
            MakeShared<PostingWriter>(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        posting_store_->insert(iter, term, posting);
        return posting;
    }
}

MemoryIndexer::RTPostingPtr MemoryIndexer::GetOrAddRTPosting(const TermKey &term) {
    MemoryIndexer::RTPostingTable::Iterator iter = rt_posting_store_->find(term);
    if (iter.valid())
        return iter.getData();
    else {
        // MemoryIndexer::RTPostingPtr posting = new MemoryPosting<true>(GetPool(), index_config_.GetPostingFormatOption());
        MemoryIndexer::RTPostingPtr posting = MakeShared<MemoryPosting<true>>(GetPool(), index_config_.GetPostingFormatOption());
        rt_posting_store_->insert(iter, term, posting);
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
    if (rt_posting_store_.get()) {
        for (auto it = rt_posting_store_->begin(); it.valid(); ++it) {
            // delete it.getData();
        }
        rt_posting_store_->clear();
    }
}

void MemoryIndexer::Dump(IndexBuilder &index_builder) {}

} // namespace infinity

namespace vespalib::btree {
using namespace infinity;

template class BTreeNodeDataWrap<MemoryIndexer::TermKey, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeT<MemoryIndexer::TermKey, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeNodeTT<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeTT<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeInternalNode<MemoryIndexer::TermKey, NoAggregated, BTreeDefaultTraits::INTERNAL_SLOTS>;

template class BTreeLeafNode<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeLeafNode<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeStore<MemoryIndexer::TermKey,
                              MemoryIndexer::PostingPtr,
                              NoAggregated,
                              BTreeDefaultTraits::INTERNAL_SLOTS,
                              BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeStore<MemoryIndexer::TermKey,
                              MemoryIndexer::RTPostingPtr,
                              NoAggregated,
                              BTreeDefaultTraits::INTERNAL_SLOTS,
                              BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeIterator<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeIterator<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTree<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTree<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, const MemoryIndexer::KeyComp, BTreeDefaultTraits>;

template class BTreeRootBase<MemoryIndexer::TermKey,
                             MemoryIndexer::PostingPtr,
                             NoAggregated,
                             BTreeDefaultTraits::INTERNAL_SLOTS,
                             BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeRootBase<MemoryIndexer::TermKey,
                             MemoryIndexer::RTPostingPtr,
                             NoAggregated,
                             BTreeDefaultTraits::INTERNAL_SLOTS,
                             BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeAllocator<MemoryIndexer::TermKey,
                                  MemoryIndexer::PostingPtr,
                                  NoAggregated,
                                  BTreeDefaultTraits::INTERNAL_SLOTS,
                                  BTreeDefaultTraits::LEAF_SLOTS>;

template class BTreeNodeAllocator<MemoryIndexer::TermKey,
                                  MemoryIndexer::RTPostingPtr,
                                  NoAggregated,
                                  BTreeDefaultTraits::INTERNAL_SLOTS,
                                  BTreeDefaultTraits::LEAF_SLOTS>;

} // namespace vespalib::btree

namespace vespalib::datastore {

using namespace vespalib::btree;
using namespace infinity;

template class BufferType<
    BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, std::less<MemoryIndexer::TermKey>, BTreeDefaultTraits>>;
template class BufferType<
    BTreeRoot<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, std::less<MemoryIndexer::TermKey>, BTreeDefaultTraits>>;
template class BufferType<BTreeKeyData<MemoryIndexer::TermKey, MemoryIndexer::PostingPtr>>;
template class BufferType<BTreeKeyData<MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr>>;

VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(MemoryIndexer::TermKey, MemoryIndexer::PostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);
VESPALIB_DATASTORE_INSTANTIATE_BUFFERTYPE_LEAFNODE(MemoryIndexer::TermKey, MemoryIndexer::RTPostingPtr, NoAggregated, BTreeDefaultTraits::LEAF_SLOTS);

} // namespace vespalib::datastore
