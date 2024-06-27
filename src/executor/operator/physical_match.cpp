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

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

module physical_match;

import stl;
import txn;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
import operator_state;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import base_expression;
import match_expression;
import default_values;
import infinity_exception;
import value;
import third_party;
import base_table_ref;
import block_index;
import load_meta;
import block_entry;
import block_column_entry;
import logical_type;
import search_options;
import status;
import index_defines;

import query_node;
import query_builder;
import doc_iterator;
import logger;
import analyzer;
import term;
import early_terminate_iterator;
import fulltext_score_result_heap;
import physical_index_scan;
import explain_logical_plan;
import column_index_reader;
import match_data;
import filter_value_type_classification;
import common_analyzer;
import analyzer_pool;
import bitmask;
import segment_entry;
import knn_filter;

namespace infinity {

template <typename QueryIteratorT>
class FilterIteratorBase : public QueryIteratorT {
protected:
    // real search iterator
    UniquePtr<QueryIteratorT> query_iterator_;

    // filter info
    const CommonQueryFilter *common_query_filter_;
    const SizeT filter_result_count_ = common_query_filter_->filter_result_count_;
    const Map<SegmentID, std::variant<Vector<u32>, Bitmask>> *filter_result_ptr_ = &common_query_filter_->filter_result_;
    const BaseExpression *secondary_index_filter_ = common_query_filter_->secondary_index_filter_qualified_.get();
    const Map<SegmentID, SegmentSnapshot> &segment_index = common_query_filter_->base_table_ref_->block_index_->segment_block_index_;

    const TxnTimeStamp begin_ts_ = common_query_filter_->begin_ts_;
    SegmentID current_segment_id_ = filter_result_ptr_->size() ? filter_result_ptr_->begin()->first : INVALID_SEGMENT_ID;
    mutable SegmentID cache_segment_id_ = INVALID_SEGMENT_ID;
    mutable SegmentOffset cache_segment_offset_ = 0;
    using SegEntryT = const SegmentEntry *;
    mutable SegEntryT cache_segment_entry_ = nullptr;
    mutable bool cache_need_check_delete_ = false;

    // -1: not decoded
    // 0: use Vector<u32>*
    // 1: use Bitmask*
    i8 decode_status_ = -1;
    const Vector<u32> *doc_id_list_ = nullptr;
    const Bitmask *doc_id_bitmask_ = nullptr;
    u32 pos_ = 0;

    RowID SelfBlockMinPossibleDocID() const { return RowID(current_segment_id_, 0); }

    RowID SelfBlockLastDocID() const {
        if (current_segment_id_ != cache_segment_id_) {
            cache_segment_id_ = current_segment_id_;
            const auto &segment_info = segment_index.at(cache_segment_id_);
            cache_segment_entry_ = segment_info.segment_entry_;
            cache_segment_offset_ = segment_info.segment_offset_;
            cache_need_check_delete_ = cache_segment_entry_->CheckAnyDelete(begin_ts_);
        }
        return RowID(current_segment_id_, cache_segment_offset_);
    }
    bool SelfBlockSkipTo(RowID doc_id) {
        if (current_segment_id_ == INVALID_SEGMENT_ID) {
            return false;
        }
        while (true) {
            if (const SegmentID segment_id = doc_id.segment_id_; segment_id > current_segment_id_) {
                if (const auto it = filter_result_ptr_->lower_bound(segment_id); it == filter_result_ptr_->end()) {
                    current_segment_id_ = INVALID_SEGMENT_ID;
                    return false;
                } else {
                    current_segment_id_ = it->first;
                    decode_status_ = -1;
                    doc_id_list_ = nullptr;
                    doc_id_bitmask_ = nullptr;
                    pos_ = 0;
                }
            }
            if (doc_id <= SelfBlockLastDocID()) {
                return true;
            }
            doc_id = RowID(current_segment_id_ + 1, 0);
        }
    }
    Tuple<bool, RowID> SelfSeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond) {
        assert(doc_id.segment_id_ == current_segment_id_);
        assert(doc_id_no_beyond.segment_id_ == current_segment_id_);
        if (current_segment_id_ != cache_segment_id_) {
            cache_segment_id_ = current_segment_id_;
            cache_segment_entry_ = segment_index.at(cache_segment_id_).segment_entry_;
            cache_segment_offset_ = cache_segment_entry_->row_count();
            cache_need_check_delete_ = cache_segment_entry_->CheckAnyDelete(begin_ts_);
        }
        const RowID seek_end = std::min(doc_id_no_beyond, SelfBlockLastDocID());
        while (true) {
            if (doc_id > seek_end) {
                return {false, INVALID_ROWID};
            }
            const auto [success, id] = SeekInBlockRangeInner(doc_id, seek_end);
            if (!success) {
                return {false, INVALID_ROWID};
            }
            if (cache_need_check_delete_) [[unlikely]] {
                SegmentOffset max_segment_offset = cache_segment_offset_;
                DeleteFilter delete_filter(cache_segment_entry_, common_query_filter_->begin_ts_, max_segment_offset);
                if (delete_filter(id.segment_offset_)) {
                    return {true, id};
                }
            } else [[likely]] {
                return {true, id};
            }
            doc_id = id + 1;
        }
    }

    Pair<bool, RowID> SeekInBlockRangeInner(RowID doc_id, const RowID doc_id_no_beyond) {
        assert(doc_id_no_beyond <= SelfBlockLastDocID());
        assert(doc_id.segment_id_ == current_segment_id_);
        assert(doc_id_no_beyond.segment_id_ == current_segment_id_);
        assert(doc_id.segment_offset_ <= doc_id_no_beyond.segment_offset_);
        const u32 seek_offset_start = doc_id.segment_offset_;
        const u32 seek_offset_end = doc_id_no_beyond.segment_offset_;
        if (decode_status_ == -1) [[unlikely]] {
            const std::variant<Vector<u32>, Bitmask> &doc_id_list_or_bitmask = filter_result_ptr_->at(current_segment_id_);
            decode_status_ = doc_id_list_or_bitmask.index();
            assert((decode_status_ == 0 || decode_status_ == 1));
            pos_ = 0;
            switch (decode_status_) {
                case 0: {
                    doc_id_list_ = &std::get<0>(doc_id_list_or_bitmask);
                    doc_id_bitmask_ = nullptr;
                    break;
                }
                case 1: {
                    doc_id_list_ = nullptr;
                    doc_id_bitmask_ = &std::get<1>(doc_id_list_or_bitmask);
                    break;
                }
                default: {
                    String error_message = "Error variant status!";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                    break;
                }
            }
        }
        switch (decode_status_) {
            case 0: {
                // use Vector<u32>*
                u32 pos = pos_;
                for (; pos < doc_id_list_->size(); ++pos) {
                    if ((*doc_id_list_)[pos] >= seek_offset_start) {
                        break;
                    }
                }
                pos_ = pos;
                if (pos < doc_id_list_->size()) {
                    if (const u32 offset_in_segment = (*doc_id_list_)[pos]; offset_in_segment <= seek_offset_end) {
                        return {true, RowID(current_segment_id_, offset_in_segment)};
                    }
                }
                break;
            }
            case 1: {
                // use Bitmask*
                if (pos_) {
                    // check previous result
                    if (pos_ > seek_offset_end) {
                        return {false, INVALID_ROWID};
                    }
                    if (pos_ >= seek_offset_start) {
                        return {true, RowID(current_segment_id_, pos_)};
                    }
                }
                const u64 *data_ptr = doc_id_bitmask_->GetData();
                if (data_ptr == nullptr) [[likely]] {
                    // all true
                    pos_ = seek_offset_start;
                    return {true, doc_id};
                }
                const u32 current_64_pos = seek_offset_start / 64;
                const u32 next_64_pos = (seek_offset_start + 63) / 64;
                const u32 total_64_end = (doc_id_bitmask_->count() + 63) / 64;
                if (const u64 data_start = data_ptr[current_64_pos]; current_64_pos != next_64_pos and data_start) {
                    u32 i = seek_offset_start - current_64_pos * 64;
                    for (; i < 64; ++i) {
                        if (data_start & (1ULL << i)) {
                            break;
                        }
                    }
                    if (i < 64) {
                        const u32 pos = current_64_pos * 64 + i;
                        pos_ = pos;
                        if (pos <= seek_offset_end) {
                            return {true, RowID(current_segment_id_, pos)};
                        }
                    }
                }
                u32 pos_64 = next_64_pos;
                for (; pos_64 < total_64_end; ++pos_64) {
                    if (const u64 data = data_ptr[pos_64]; data) {
                        u32 i = 0;
                        for (; i < 64; ++i) {
                            if (data & (1ULL << i)) {
                                break;
                            }
                        }
                        assert(i < 64);
                        const u32 pos = pos_64 * 64 + i;
                        pos_ = pos;
                        if (pos <= seek_offset_end) {
                            return {true, RowID(current_segment_id_, pos)};
                        }
                        break;
                    }
                }
                if (pos_64 == total_64_end) {
                    // no more valid doc_id
                    pos_ = std::numeric_limits<u32>::max();
                }
                break;
            }
            default: {
                String error_message = "Error variant status!";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                break;
            }
        }
        return {false, INVALID_ROWID};
    }

public:
    FilterIteratorBase(const CommonQueryFilter *common_query_filter, UniquePtr<QueryIteratorT> &&query_iterator)
        : query_iterator_(std::move(query_iterator)), common_query_filter_(common_query_filter) {}

    // common
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override {
        os << prefix;
        os << (is_final ? "└──" : "├──");
        os << "FilterIterator (fake_doc_freq: " << filter_result_count_ << ") (secondary_index_filter: ";
        String filter_str;
        if (secondary_index_filter_) {
            ExplainLogicalPlan::Explain(secondary_index_filter_, filter_str);
        } else {
            filter_str = "None";
        }
        os << filter_str << ")\n";
        const String next_prefix = prefix + (is_final ? "    " : "│   ");
        query_iterator_->PrintTree(os, next_prefix, true);
    }
};

template <typename QueryIteratorT>
class FilterIterator;

template <>
class FilterIterator<DocIterator> final : public FilterIteratorBase<DocIterator> {
public:
    explicit FilterIterator(const CommonQueryFilter *common_query_filter, UniquePtr<DocIterator> &&query_iterator)
        : FilterIteratorBase(common_query_filter, std::move(query_iterator)) {
        query_iterator_->DoSeek(0);
        SelfDoSeek(0);
        DoSeek(0);
    }

    // DocIterator
    void DoSeek(RowID doc_id) override {
        while (true) {
            query_iterator_->Seek(doc_id);
            doc_id = query_iterator_->Doc();
            if (doc_id == INVALID_ROWID) {
                doc_id_ = INVALID_ROWID;
                return;
            }
            if (doc_id_ < doc_id) {
                SelfDoSeek(doc_id);
            }
            if (doc_id_ == doc_id || doc_id_ == INVALID_ROWID) {
                return;
            }
            doc_id = doc_id_;
        }
    }

    void SelfDoSeek(RowID doc_id) {
        for (RowID next_skip = std::max(doc_id, doc_id_ + 1);;) {
            if (!SelfBlockSkipTo(next_skip)) [[unlikely]] {
                doc_id_ = INVALID_ROWID;
                return;
            }
            next_skip = std::max(next_skip, SelfBlockMinPossibleDocID());
            assert((next_skip <= SelfBlockLastDocID()));
            const auto [success, id] = SelfSeekInBlockRange(next_skip, SelfBlockLastDocID());
            if (success) {
                doc_id_ = id;
                return;
            }
            next_skip = SelfBlockLastDocID() + 1;
        }
    }

    u32 GetDF() const override {
        String error_message = "Unreachable code!";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return 0;
    }
};

template <>
class FilterIterator<EarlyTerminateIterator> final : public FilterIteratorBase<EarlyTerminateIterator> {
    // block max info
    RowID common_block_min_possible_doc_id_{}; // not always exist
    RowID common_block_last_doc_id_{};

public:
    explicit FilterIterator(const CommonQueryFilter *common_query_filter, UniquePtr<EarlyTerminateIterator> &&query_iterator)
        : FilterIteratorBase(common_query_filter, std::move(query_iterator)) {
        doc_freq_ = std::numeric_limits<u32>::max();
    }
    void UpdateScoreThreshold(float threshold) override { query_iterator_->UpdateScoreThreshold(threshold); }

    bool NextShallow(RowID doc_id) override {
        assert(doc_id != INVALID_ROWID);
        while (true) {
            if (!query_iterator_->NextShallow(doc_id)) {
                common_block_min_possible_doc_id_ = INVALID_ROWID;
                common_block_last_doc_id_ = INVALID_ROWID;
                return false;
            }
            if (const RowID lowest_possible = query_iterator_->BlockMinPossibleDocID(); lowest_possible > doc_id) {
                doc_id = lowest_possible;
            }
            RowID common_block_last_doc_id = query_iterator_->BlockLastDocID();
            if (!SelfBlockSkipTo(doc_id)) {
                common_block_min_possible_doc_id_ = INVALID_ROWID;
                common_block_last_doc_id_ = INVALID_ROWID;
                return false;
            }
            if (const RowID lowest_possible = SelfBlockMinPossibleDocID(); lowest_possible > common_block_last_doc_id) {
                // continue loop
                doc_id = common_block_last_doc_id + 1;
                continue;
            } else if (lowest_possible > doc_id) {
                doc_id = lowest_possible;
            }
            common_block_last_doc_id = std::min(common_block_last_doc_id, SelfBlockLastDocID());
            common_block_min_possible_doc_id_ = doc_id;
            common_block_last_doc_id_ = common_block_last_doc_id;
            return true;
        }
    }

    bool Next(RowID doc_id) override {
        bool ok = false;
        while(1) {
            ok = query_iterator_->Next(doc_id);
            if (!ok){
                break;
            }
            doc_id = query_iterator_->DocID();
            // check filter
            ok = SelfBlockSkipTo(doc_id);
            if (!ok) {
                break;
            }
            const auto [ok, id2] = SelfSeekInBlockRange(doc_id, SelfBlockLastDocID());
            if (!ok){
                break;
            }
            if (id2 == doc_id) {
                common_block_min_possible_doc_id_ = doc_id;
                common_block_last_doc_id_ = std::max(query_iterator_->BlockLastDocID(), SelfBlockLastDocID());
                doc_id_ = id2;
                return true;
            }
            assert(id2 > doc_id);
            doc_id = id2;
        }
        common_block_min_possible_doc_id_ = INVALID_ROWID;
        common_block_last_doc_id_ = INVALID_ROWID;
        doc_id_ = INVALID_ROWID;
        return false;
    }

    RowID BlockMinPossibleDocID() const override { return common_block_min_possible_doc_id_; }
    RowID BlockLastDocID() const override { return common_block_last_doc_id_; }
    bool BlockSkipTo(RowID doc_id, float threshold) override {
        while (true) {
            if (!query_iterator_->BlockSkipTo(doc_id, threshold)) {
                return false;
            }
            if (const RowID lowest_possible = query_iterator_->BlockMinPossibleDocID(); lowest_possible > doc_id) {
                doc_id = lowest_possible;
            }
            RowID common_block_last_doc_id = query_iterator_->BlockLastDocID();
            if (!SelfBlockSkipTo(doc_id)) {
                return false;
            }
            if (const RowID lowest_possible = SelfBlockMinPossibleDocID(); lowest_possible > common_block_last_doc_id) {
                // continue loop
                doc_id = common_block_last_doc_id + 1;
                continue;
            } else if (lowest_possible > doc_id) {
                doc_id = lowest_possible;
            }
            common_block_last_doc_id = std::min(common_block_last_doc_id, SelfBlockLastDocID());
            common_block_min_possible_doc_id_ = doc_id;
            common_block_last_doc_id_ = common_block_last_doc_id;
            return true;
        }
    }
    float BlockMaxBM25Score() override { return query_iterator_->BlockMaxBM25Score(); }
    float BM25Score() override { return query_iterator_->BM25Score(); }
    Pair<bool, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override {
        String error_message = "Unreachable code!";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return {false, INVALID_ROWID};
    }
    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond, const float threshold) override {
        const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
        while (true) {
            if (doc_id > block_end) [[unlikely]] {
                return {false, 0.0F, INVALID_ROWID};
            }
            // real query
            const auto result = query_iterator_->SeekInBlockRange(doc_id, block_end, threshold);
            const auto &[success1, score, id1] = result;
            if (!success1) {
                return {false, 0.0F, INVALID_ROWID};
            }
            doc_id = id1;
            // check filter
            const auto [success2, id2] = SelfSeekInBlockRange(doc_id, block_end);
            if (id2 == doc_id) {
                doc_id_ = doc_id;
                return result;
            }
            // need to update doc_id, restart from the first iterator
            doc_id = id2;
        }
    }
    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override {
        String error_message = "Unreachable code!";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return {false, INVALID_ROWID};
    }
    bool NotPartCheckExist(RowID doc_id) override {
        String error_message = "Unreachable code!";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
};

// use QueryNodeType::FILTER
struct FilterQueryNode final : public QueryNode {
    // search iterator
    UniquePtr<QueryNode> query_tree_;
    // filter info
    const CommonQueryFilter *common_query_filter_;
    const SizeT filter_result_count_ = common_query_filter_->filter_result_count_;
    const Map<SegmentID, std::variant<Vector<u32>, Bitmask>> *filter_result_ptr_ = &common_query_filter_->filter_result_;
    const BaseExpression *secondary_index_filter_ = common_query_filter_->secondary_index_filter_qualified_.get();

    explicit FilterQueryNode(const CommonQueryFilter *common_query_filter, UniquePtr<QueryNode> &&query_tree)
        : QueryNode(QueryNodeType::FILTER), query_tree_(std::move(query_tree)), common_query_filter_(common_query_filter) {}

    void FilterOptimizeQueryTree() override {
        auto new_query_tree = GetOptimizedQueryTree(std::move(query_tree_));
        query_tree_ = std::move(new_query_tree);
    }

    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override {
        auto search_iter = query_tree_->CreateSearch(table_entry, index_reader, scorer);
        if (!search_iter) {
            return nullptr;
        }
        return MakeUnique<FilterIterator<DocIterator>>(common_query_filter_, std::move(search_iter));
    }
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlgo early_term_algo) const override {
        auto search_iter = query_tree_->CreateEarlyTerminateSearch(table_entry, index_reader, scorer, early_term_algo);
        if (!search_iter) {
            return nullptr;
        }
        return MakeUnique<FilterIterator<EarlyTerminateIterator>>(common_query_filter_, std::move(search_iter));
    }
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override {
        os << prefix;
        os << (is_final ? "└──" : "├──");
        os << "Filter (secondary_index_filter: ";
        String filter_str;
        if (secondary_index_filter_) {
            ExplainLogicalPlan::Explain(secondary_index_filter_, filter_str);
        } else {
            filter_str = "None";
        }
        os << filter_str << ") (filter_result_count: " << filter_result_count_ << ")\n";
    }
};

void ASSERT_FLOAT_EQ(float bar, u32 i, float a, float b) {
    float diff_percent = std::abs(a - b) / std::max(std::abs(a), std::abs(b));
    if (diff_percent > bar) {
        OStringStream oss;
        oss << "result mismatch at " << i << " : a: " << a << ", b: " << b << ", diff_percent: " << diff_percent << std::endl;
        Status status = Status::SyntaxError("Debug Info: " + std::move(oss).str());
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
}

void ExecuteFTSearch(UniquePtr<EarlyTerminateIterator> &et_iter,
                     FullTextScoreResultHeap &result_heap,
                     u32 &blockmax_loop_cnt,
                     EarlyTermAlgo early_term_algo) {
    // et_iter is nullptr if fulltext index is present but there's no data
    if (et_iter == nullptr)
        return;
    switch (early_term_algo) {
        case EarlyTermAlgo::kBMM: {
            while (true) {
                auto [id, et_score] = et_iter->BlockNextWithThreshold(result_heap.GetScoreThreshold());
                if (id == INVALID_ROWID) [[unlikely]] {
                    break;
                }
                ++blockmax_loop_cnt;
                if (result_heap.AddResult(et_score, id)) {
                    // update threshold
                    if (const float new_threshold = result_heap.GetScoreThreshold(); new_threshold > 0.0f) {
                        et_iter->UpdateScoreThreshold(new_threshold);
                    }
                }
                if (blockmax_loop_cnt % 10 == 0) {
                    LOG_DEBUG(fmt::format("ExecuteFTSearch has evaluated {} candidates", blockmax_loop_cnt));
                }
            }
            break;
        }
        case EarlyTermAlgo::kBMW:
        default: {
            while (true) {
                ++blockmax_loop_cnt;
                bool ok = et_iter->Next();
                if (!ok) [[unlikely]] {
                    break;
                }
                RowID id = et_iter->DocID();
                float et_score = et_iter->BM25Score();
                if (result_heap.AddResult(et_score, id)) {
                    // update threshold
                    if (const float new_threshold = result_heap.GetScoreThreshold(); new_threshold > 0.0f) {
                        et_iter->UpdateScoreThreshold(new_threshold);
                    }
                }
                if (blockmax_loop_cnt % 10 == 0) {
                    LOG_DEBUG(fmt::format("ExecuteFTSearch has evaluated {} candidates", blockmax_loop_cnt));
                }
            }
        }
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable" 
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
bool PhysicalMatch::ExecuteInnerHomebrewed(QueryContext *query_context, OperatorState *operator_state) {
    using TimeDurationType = std::chrono::duration<float, std::milli>;
    auto execute_start_time = std::chrono::high_resolution_clock::now();
    // 1. build QueryNode tree
    // 1.1 populate column2analyzer
//    Txn *txn = query_context->GetTxn();
    QueryBuilder query_builder(base_table_ref_.get());
    query_builder.Init(index_reader_);
    auto finish_init_query_builder_time = std::chrono::high_resolution_clock::now();
    TimeDurationType query_builder_init_duration = finish_init_query_builder_time - execute_start_time;
    LOG_DEBUG(fmt::format("PhysicalMatch 0: Init QueryBuilder time: {} ms", query_builder_init_duration.count()));

    // 1.2 parse options into map, populate default_field
    bool use_ordinary_iter = false;
    bool use_block_max_iter = false;

    switch(early_term_algo_) {
        case EarlyTermAlgo::kBMM: {
            use_block_max_iter = true;
            break;
        }
        case EarlyTermAlgo::kNaive: {
            use_ordinary_iter = true;
            break;
        }
        case EarlyTermAlgo::kCompare: {
            use_ordinary_iter = true;
            use_block_max_iter = true;
            break;
        }
        case EarlyTermAlgo::kBMW:
        default: {
            use_block_max_iter = true;
            break;
        }
    }

    auto finish_parse_query_tree_time = std::chrono::high_resolution_clock::now();
    TimeDurationType parse_query_tree_duration = finish_parse_query_tree_time - finish_init_query_builder_time;
    LOG_DEBUG(fmt::format("PhysicalMatch 1: Parse QueryNode tree time: {} ms", parse_query_tree_duration.count()));

    // 2 build query iterator
    // result
    FullTextQueryContext full_text_query_context;
    u32 result_count = 0;
    const float *score_result = nullptr;
    const RowID *row_id_result = nullptr;
    // for comparison
    UniquePtr<EarlyTerminateIterator> et_iter;
    UniquePtr<DocIterator> doc_iterator;
    // run EarlyTerminateIterator again to avoid the effect of cache
    UniquePtr<EarlyTerminateIterator> et_iter_2;
    UniquePtr<EarlyTerminateIterator> et_iter_3;
    u32 ordinary_loop_cnt = 0;
    u32 blockmax_loop_cnt = 0;
    u32 blockmax_loop_cnt_2 = 0;
    u32 ordinary_result_count = 0;
    u32 blockmax_result_count = 0;
    u32 blockmax_result_count_2 = 0;
    UniquePtr<float[]> ordinary_score_result;
    UniquePtr<RowID[]> ordinary_row_id_result;
    UniquePtr<float[]> blockmax_score_result;
    UniquePtr<RowID[]> blockmax_row_id_result;
    UniquePtr<float[]> blockmax_score_result_2;
    UniquePtr<RowID[]> blockmax_row_id_result_2;
    TimeDurationType ordinary_duration = {};
    TimeDurationType blockmax_duration = {};
    TimeDurationType blockmax_duration_2 = {};
    TimeDurationType blockmax_duration_3 = {};
    assert(common_query_filter_);
    full_text_query_context.query_tree_ = MakeUnique<FilterQueryNode>(common_query_filter_.get(), std::move(query_tree_));

    if (use_block_max_iter) {
        et_iter = query_builder.CreateEarlyTerminateSearch(full_text_query_context, early_term_algo_);
        // et_iter is nullptr if fulltext index is present but there's no data
        if (et_iter != nullptr && begin_threshold_ > 0.0f)
            et_iter->UpdateScoreThreshold(begin_threshold_);
    }
    if (use_ordinary_iter) {
        doc_iterator = query_builder.CreateSearch(full_text_query_context);
    }
    if (use_block_max_iter and use_ordinary_iter) {
        et_iter_2 = query_builder.CreateEarlyTerminateSearch(full_text_query_context, early_term_algo_);
        et_iter_3 = query_builder.CreateEarlyTerminateSearch(full_text_query_context, early_term_algo_);
        if (et_iter_2 != nullptr && begin_threshold_ > 0.0f)
            et_iter_2->UpdateScoreThreshold(begin_threshold_);
        if (et_iter_3 != nullptr && begin_threshold_ > 0.0f)
            et_iter_3->UpdateScoreThreshold(begin_threshold_);
    }

    // 3 full text search

    auto finish_query_builder_time = std::chrono::high_resolution_clock::now();
    TimeDurationType query_builder_duration = finish_query_builder_time - finish_parse_query_tree_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Part 2: Build Query iterator time: {} ms", query_builder_duration.count()));
    if (use_block_max_iter) {
        blockmax_score_result = MakeUniqueForOverwrite<float[]>(top_n_);
        blockmax_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n_);
        FullTextScoreResultHeap result_heap(top_n_, blockmax_score_result.get(), blockmax_row_id_result.get());
#ifdef INFINITY_DEBUG
        auto blockmax_begin_ts = std::chrono::high_resolution_clock::now();
#endif
        ExecuteFTSearch(et_iter, result_heap, blockmax_loop_cnt, early_term_algo_);
        result_heap.Sort();
        blockmax_result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto blockmax_end_ts = std::chrono::high_resolution_clock::now();
        blockmax_duration = blockmax_end_ts - blockmax_begin_ts;
#endif
    }
    if (use_ordinary_iter) {
        RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : (doc_iterator->PrepareFirstDoc(), doc_iterator->Doc());
        if (iter_row_id != INVALID_ROWID) [[likely]] {
            ordinary_score_result = MakeUniqueForOverwrite<float[]>(top_n_);
            ordinary_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n_);
            FullTextScoreResultHeap result_heap(top_n_, ordinary_score_result.get(), ordinary_row_id_result.get());
#ifdef INFINITY_DEBUG
            auto ordinary_begin_ts = std::chrono::high_resolution_clock::now();
#endif
            do {
                ++ordinary_loop_cnt;
                // call scorer
                float score = query_builder.Score(iter_row_id);
                result_heap.AddResult(score, iter_row_id);
                // get next row_id
                iter_row_id = doc_iterator->Next();
            } while (iter_row_id != INVALID_ROWID);
            result_heap.Sort();
            ordinary_result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
            auto ordinary_end_ts = std::chrono::high_resolution_clock::now();
            ordinary_duration = ordinary_end_ts - ordinary_begin_ts;
#endif
        }
    }
    if (use_ordinary_iter and use_block_max_iter) {
        blockmax_score_result_2 = MakeUniqueForOverwrite<float[]>(top_n_);
        blockmax_row_id_result_2 = MakeUniqueForOverwrite<RowID[]>(top_n_);
        FullTextScoreResultHeap result_heap(top_n_, blockmax_score_result_2.get(), blockmax_row_id_result_2.get());
#ifdef INFINITY_DEBUG
        auto blockmax_begin_ts = std::chrono::high_resolution_clock::now();
#endif
        ExecuteFTSearch(et_iter_2, result_heap, blockmax_loop_cnt_2, early_term_algo_);
        result_heap.Sort();
        blockmax_result_count_2 = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto blockmax_end_ts = std::chrono::high_resolution_clock::now();
        blockmax_duration_2 = blockmax_end_ts - blockmax_begin_ts;
        {
            auto blockmax_score_result_3 = MakeUniqueForOverwrite<float[]>(top_n_);
            auto blockmax_row_id_result_3 = MakeUniqueForOverwrite<RowID[]>(top_n_);
            FullTextScoreResultHeap result_heap_3(top_n_, blockmax_score_result_3.get(), blockmax_row_id_result_3.get());
            auto blockmax_begin_ts_3 = std::chrono::high_resolution_clock::now();
            u32 blockmax_loop_cnt_3 = 0;
            ExecuteFTSearch(et_iter_3, result_heap_3, blockmax_loop_cnt_3, early_term_algo_);
            result_heap_3.Sort();
            auto blockmax_end_ts_3 = std::chrono::high_resolution_clock::now();
            if (blockmax_loop_cnt_3 != blockmax_loop_cnt_2) {
                assert(false);
            }
            assert(result_heap_3.GetResultSize() == result_heap.GetResultSize());
            for (u32 i = 0; i < top_n_; ++i) {
                assert(blockmax_score_result_2[i] == blockmax_score_result_3[i]);
                assert(blockmax_row_id_result_2[i] == blockmax_row_id_result_3[i]);
            }
            blockmax_duration_3 = blockmax_end_ts_3 - blockmax_begin_ts_3;
        }
#endif
    }
    if (use_block_max_iter) {
        result_count = blockmax_result_count;
        score_result = blockmax_score_result.get();
        row_id_result = blockmax_row_id_result.get();
    } else {
        result_count = ordinary_result_count;
        score_result = ordinary_score_result.get();
        row_id_result = ordinary_row_id_result.get();
    }
    auto finish_query_time = std::chrono::high_resolution_clock::now();
    TimeDurationType query_duration = finish_query_time - finish_query_builder_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Part 3: Full text search time: {} ms", query_duration.count()));
#ifdef INFINITY_DEBUG
    {
        OStringStream stat_info;
        stat_info << "Full text search stat:\n";
        if (use_block_max_iter) {
            stat_info << "blockmax_duration: " << blockmax_duration << std::endl;
            stat_info << "blockmax_loop_cnt: " << blockmax_loop_cnt << std::endl;
        }
        if (use_ordinary_iter) {
            stat_info << "ordinary_duration: " << ordinary_duration << std::endl;
            stat_info << "ordinary_loop_cnt: " << ordinary_loop_cnt << std::endl;
        }
        if (use_ordinary_iter and use_block_max_iter) {
            stat_info << "blockmax_duration_2: " << blockmax_duration_2 << std::endl;
            stat_info << "blockmax_duration_3: " << blockmax_duration_3 << std::endl;
            stat_info << "blockmax_loop_cnt_2: " << blockmax_loop_cnt_2 << std::endl;
        }
        LOG_DEBUG(std::move(stat_info).str());
    }
    if (use_ordinary_iter and use_block_max_iter) {
        OStringStream compare_info;
        compare_info << "Compare ordinary and blockmax:\n";
        compare_info << "duration ratio 1: " << blockmax_duration.count() / ordinary_duration.count() << std::endl;
        compare_info << "duration ratio 2: " << blockmax_duration_2.count() / ordinary_duration.count() << std::endl;
        compare_info << "duration ratio 3: " << blockmax_duration_3.count() / ordinary_duration.count() << std::endl;
        compare_info << "duration ratio 2/1: " << blockmax_duration_2.count() / blockmax_duration.count() << std::endl;
        compare_info << "duration ratio 3/2: " << blockmax_duration_3.count() / blockmax_duration_2.count() << std::endl;
        compare_info << "loop count ratio: " << (static_cast<float>(blockmax_loop_cnt) / ordinary_loop_cnt) << std::endl;
        LOG_DEBUG(std::move(compare_info).str());
        if (blockmax_result_count != blockmax_result_count_2 or ordinary_result_count != blockmax_result_count or
            blockmax_loop_cnt != blockmax_loop_cnt_2) {
            Status status = Status::SyntaxError("Debug Info: result count mismatch!");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        for (u32 i = 0; i < result_count; ++i) {
            ASSERT_FLOAT_EQ(1e-4, i, ordinary_score_result[i], blockmax_score_result[i]);
            ASSERT_FLOAT_EQ(0.0f, i, blockmax_score_result[i], blockmax_score_result_2[i]);
        }
    }
#endif
    LOG_DEBUG(fmt::format("Full text search result count: {}", result_count));
    auto begin_output_time = std::chrono::high_resolution_clock::now();
    TimeDurationType output_info_duration = begin_output_time - finish_query_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Part 4: Output stat info time: {} ms", output_info_duration.count()));
    // 4 populate result DataBlock
    // 4.1 prepare first output_data_block
    auto &output_data_blocks = operator_state->data_block_array_;
    auto OutputTypesPtr = GetOutputTypes();
    Vector<SharedPtr<DataType>> &OutputTypes = *OutputTypesPtr;
    auto append_data_block = [&]() {
        auto data_block = DataBlock::MakeUniquePtr();
        data_block->Init(OutputTypes);
        output_data_blocks.emplace_back(std::move(data_block));
    };
    append_data_block();
    // 4.2 output
    {
        Vector<SizeT> &column_ids = base_table_ref_->column_ids_;
        SizeT column_n = column_ids.size();
        u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
        u32 output_block_row_id = 0;
        DataBlock *output_block_ptr = output_data_blocks.back().get();
        for (u32 output_id = 0; output_id < result_count; ++output_id) {
            if (output_block_row_id == block_capacity) {
                output_block_ptr->Finalize();
                append_data_block();
                output_block_ptr = output_data_blocks.back().get();
                output_block_row_id = 0;
            }
            const RowID &row_id = row_id_result[output_id];
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            const BlockEntry *block_entry = base_table_ref_->block_index_->GetBlockEntry(segment_id, block_id);
            assert(block_entry != nullptr);
            SizeT column_id = 0;
            for (; column_id < column_n; ++column_id) {
                BlockColumnEntry *block_column_ptr = block_entry->GetColumnBlockEntry(column_ids[column_id]);
                ColumnVector column_vector = block_column_ptr->GetColumnVector(query_context->storage()->buffer_manager());
                output_block_ptr->column_vectors[column_id]->AppendWith(column_vector, block_offset, 1);
            }
            Value v = Value::MakeFloat(score_result[output_id]);
            output_block_ptr->column_vectors[column_id++]->AppendValue(v);
            output_block_ptr->column_vectors[column_id]->AppendWith(row_id, 1);
            ++output_block_row_id;
        }
        output_block_ptr->Finalize();
    }

    operator_state->SetComplete();
    auto finish_output_time = std::chrono::high_resolution_clock::now();
    TimeDurationType output_duration = finish_output_time - begin_output_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Part 5: Output data time: {} ms", output_duration.count()));
    return true;
}
#pragma clang diagnostic pop

PhysicalMatch::PhysicalMatch(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchExpression> match_expr,
                             IndexReader index_reader,
                             UniquePtr<QueryNode>&& query_tree,
                             float begin_threshold,
                             EarlyTermAlgo early_term_algo,
                             u32 top_n,
                             const SharedPtr<CommonQueryFilter> &common_query_filter,
                             u64 match_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id, load_metas), table_index_(match_table_index),
      base_table_ref_(std::move(base_table_ref)), match_expr_(std::move(match_expr)), index_reader_(index_reader), query_tree_(std::move(query_tree)),
      begin_threshold_(begin_threshold), early_term_algo_(early_term_algo), top_n_(top_n), common_query_filter_(common_query_filter) {}

PhysicalMatch::~PhysicalMatch() = default;

void PhysicalMatch::Init() {}

bool PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto start_time = std::chrono::high_resolution_clock::now();
    assert(common_query_filter_);
    {
        Txn *txn = query_context->GetTxn();
        bool try_result = common_query_filter_->TryFinishBuild(txn);
        auto finish_filter_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> filter_duration = finish_filter_time - start_time;
        LOG_DEBUG(fmt::format("PhysicalMatch Prepare: Filter time: {} ms", filter_duration.count()));
        if (!try_result) {
            // not ready, abort and wait for next time
            return true;
        }
    }
    bool return_value = ExecuteInnerHomebrewed(query_context, operator_state);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end_time - start_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Execute time: {} ms", duration.count()));
    return return_value;
}

SharedPtr<Vector<String>> PhysicalMatch::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatch::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kFloat));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

String PhysicalMatch::ToString(i64 &space) const {
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> PhysicalMatch ";
    } else {
        arrow_str = "PhysicalMatch ";
    }
    String res = fmt::format("{} Table: {}, {}", arrow_str, *(base_table_ref_->table_entry_ptr_->GetTableName()), match_expr_->ToString());
    return res;
}

} // namespace infinity
