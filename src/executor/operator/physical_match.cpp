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
import fulltext_score_result_heap;
import physical_index_scan;
import column_index_reader;
import filter_value_type_classification;
import common_analyzer;
import analyzer_pool;
import roaring_bitmap;
import segment_entry;
import knn_filter;
import highlighter;
import parse_fulltext_options;
import result_cache_manager;
import cached_match;
import filter_iterator;
import score_threshold_iterator;

namespace infinity {

void ASSERT_FLOAT_EQ(float bar, u32 i, float a, float b) {
    float diff_percent = std::abs(a - b) / std::max(std::abs(a), std::abs(b));
    if (diff_percent > bar) {
        OStringStream oss;
        oss << "result mismatch at " << i << " : a: " << a << ", b: " << b << ", diff_percent: " << diff_percent << std::endl;
        Status status = Status::SyntaxError("Debug Info: " + std::move(oss).str());
        RecoverableError(status);
    }
}

void ExecuteFTSearch(UniquePtr<DocIterator> &et_iter, FullTextScoreResultHeap &result_heap, u32 &blockmax_loop_cnt) {
    // et_iter is nullptr if fulltext index is present but there's no data
    if (et_iter == nullptr) {
        LOG_DEBUG(fmt::format("et_iter is nullptr"));
        return;
    }
    while (true) {
        ++blockmax_loop_cnt;
        bool ok = et_iter->Next();
        if (!ok) [[unlikely]] {
            break;
        }
        RowID id = et_iter->DocID();
        float et_score = et_iter->Score();
        if (SHOULD_LOG_DEBUG()) {
            OStringStream oss;
            et_iter->PrintTree(oss, "", true);
            String msg = fmt::format("Found candidate doc_id {} score {}\n", id.ToUint64(), et_score);
            msg += oss.str();
            LOG_DEBUG(msg);
        }
        if (result_heap.AddResult(et_score, id)) {
            // update threshold
            et_iter->UpdateScoreThreshold(result_heap.GetScoreThreshold());
        }
        if (blockmax_loop_cnt % 10 == 0) {
            LOG_DEBUG(fmt::format("ExecuteFTSearch has evaluated {} candidates", blockmax_loop_cnt));
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

    switch (early_term_algo_) {
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
    UniquePtr<DocIterator> et_iter;
    UniquePtr<DocIterator> doc_iterator;
    u32 ordinary_loop_cnt = 0;
    u32 blockmax_loop_cnt = 0;
    u32 ordinary_result_count = 0;
    u32 blockmax_result_count = 0;
    UniquePtr<float[]> ordinary_score_result;
    UniquePtr<RowID[]> ordinary_row_id_result;
    UniquePtr<float[]> blockmax_score_result;
    UniquePtr<RowID[]> blockmax_row_id_result;
    TimeDurationType ordinary_duration = {};
    TimeDurationType blockmax_duration = {};
    assert(common_query_filter_);
    full_text_query_context.query_tree_ = MakeUnique<FilterQueryNode>(common_query_filter_.get(), std::move(query_tree_));

    if (use_block_max_iter) {
        et_iter = query_builder.CreateSearch(full_text_query_context, early_term_algo_, minimum_should_match_option_);
        // et_iter is nullptr if fulltext index is present but there's no data
        if (et_iter != nullptr) {
            et_iter->UpdateScoreThreshold(std::max(begin_threshold_, score_threshold_));
            if (score_threshold_ > 0.0f) {
                auto new_et_iter = MakeUnique<ScoreThresholdIterator>(std::move(et_iter), score_threshold_);
                et_iter = std::move(new_et_iter);
            }
        }
    }
    if (use_ordinary_iter) {
        doc_iterator = query_builder.CreateSearch(full_text_query_context, EarlyTermAlgo::kNaive, minimum_should_match_option_);
        if (doc_iterator && score_threshold_ > 0.0f) {
            auto new_doc_iter = MakeUnique<ScoreThresholdIterator>(std::move(doc_iterator), score_threshold_);
            doc_iterator = std::move(new_doc_iter);
        }
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
        ExecuteFTSearch(et_iter, result_heap, blockmax_loop_cnt);
        result_heap.Sort();
        blockmax_result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto blockmax_end_ts = std::chrono::high_resolution_clock::now();
        blockmax_duration = blockmax_end_ts - blockmax_begin_ts;
#endif
    }
    if (use_ordinary_iter) {
        ordinary_score_result = MakeUniqueForOverwrite<float[]>(top_n_);
        ordinary_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n_);
        FullTextScoreResultHeap result_heap(top_n_, ordinary_score_result.get(), ordinary_row_id_result.get());
#ifdef INFINITY_DEBUG
        auto ordinary_begin_ts = std::chrono::high_resolution_clock::now();
#endif
        ExecuteFTSearch(doc_iterator, result_heap, ordinary_loop_cnt);
        result_heap.Sort();
        ordinary_result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto ordinary_end_ts = std::chrono::high_resolution_clock::now();
        ordinary_duration = ordinary_end_ts - ordinary_begin_ts;
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
        LOG_DEBUG(std::move(stat_info).str());
    }
    if (use_ordinary_iter and use_block_max_iter) {
        OStringStream compare_info;
        compare_info << "Compare ordinary and blockmax:\n";
        compare_info << "duration ratio: " << blockmax_duration.count() / ordinary_duration.count() << std::endl;
        compare_info << "loop count ratio: " << (static_cast<float>(blockmax_loop_cnt) / ordinary_loop_cnt) << std::endl;
        LOG_DEBUG(std::move(compare_info).str());
        if (ordinary_result_count != blockmax_result_count) {
            Status status = Status::SyntaxError("Debug Info: result count mismatch!");
            RecoverableError(status);
        }
        for (u32 i = 0; i < result_count; ++i) {
            ASSERT_FLOAT_EQ(1e-4, i, ordinary_score_result[i], blockmax_score_result[i]);
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
                ColumnVector column_vector = block_column_ptr->GetConstColumnVector(query_context->storage()->buffer_manager());
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
    ResultCacheManager *cache_mgr = query_context->storage()->result_cache_manager();
    if (cache_result_ && cache_mgr != nullptr) {
        AddCache(query_context, cache_mgr, output_data_blocks);
    }
    auto finish_output_time = std::chrono::high_resolution_clock::now();
    TimeDurationType output_duration = finish_output_time - begin_output_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Part 5: Output data time: {} ms", output_duration.count()));
    return true;
}
#pragma clang diagnostic pop

PhysicalMatch::PhysicalMatch(const u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchExpression> match_expr,
                             IndexReader index_reader,
                             UniquePtr<QueryNode> &&query_tree,
                             const float begin_threshold,
                             const EarlyTermAlgo early_term_algo,
                             const u32 top_n,
                             const SharedPtr<CommonQueryFilter> &common_query_filter,
                             MinimumShouldMatchOption &&minimum_should_match_option,
                             const f32 score_threshold,
                             const u64 match_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas,
                             const bool cache_result)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id, std::move(load_metas), cache_result), table_index_(match_table_index),
      base_table_ref_(std::move(base_table_ref)), match_expr_(std::move(match_expr)), index_reader_(std::move(index_reader)),
      query_tree_(std::move(query_tree)), begin_threshold_(begin_threshold), early_term_algo_(early_term_algo), top_n_(top_n),
      common_query_filter_(common_query_filter), minimum_should_match_option_(std::move(minimum_should_match_option)),
      score_threshold_(score_threshold) {}

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

void PhysicalMatch::AddCache(QueryContext *query_context, ResultCacheManager *cache_mgr, const Vector<UniquePtr<DataBlock>> &output_data_blocks) {
    Txn *txn = query_context->GetTxn();
    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    TxnTimeStamp query_ts = std::min(txn->BeginTS(), table_entry->max_commit_ts());
    Vector<UniquePtr<DataBlock>> data_blocks(output_data_blocks.size());
    for (SizeT i = 0; i < output_data_blocks.size(); ++i) {
        data_blocks[i] = output_data_blocks[i]->Clone();
    }
    auto cached_node = MakeUnique<CachedMatch>(query_ts, this);
    bool success = cache_mgr->AddCache(std::move(cached_node), std::move(data_blocks));
    if (!success) {
        LOG_WARN(fmt::format("Add cache failed for query: {}", txn->BeginTS()));
    } else {
        LOG_INFO(fmt::format("Add cache success for query: {}", txn->BeginTS()));
    }
}

} // namespace infinity
