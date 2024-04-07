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
#include <string>

module physical_match;

import stl;

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
import load_meta;
import block_entry;
import block_column_entry;
import logical_type;
import search_options;
import status;
import index_defines;
import search_driver;
import query_node;
import query_builder;
import doc_iterator;
import logger;
import analyzer_pool;
import analyzer;
import term;
import early_terminate_iterator;
import fulltext_score_result_heap;

void ASSERT_FLOAT_EQ6(float a, float b) {
    std::cerr << "a: " << a << " b: " << b << std::endl;
    float diff_abs = std::abs(a - b);
    float max_abs = std::max(std::abs(a), std::abs(b));
    float err_percent = diff_abs / max_abs;
    std::cerr << "diff_abs: " << diff_abs << " max_abs: " << max_abs << " err_percent: " << err_percent << std::endl;
    assert((err_percent < 1e-6));
}

namespace infinity {

void AnalyzeFunc(const String &analyzer_name, String &&text, TermList &output_terms) {
    UniquePtr<Analyzer> analyzer = AnalyzerPool::instance().Get(analyzer_name);
    Term input_term;
    input_term.text_ = std::move(text);
    analyzer->Analyze(input_term, output_terms);
}

bool ExecuteInnerHomebrewed(QueryContext *query_context,
                            OperatorState *operator_state,
                            SharedPtr<BaseTableRef> &base_table_ref_,
                            SharedPtr<MatchExpression> &match_expr_,
                            Vector<SharedPtr<DataType>> OutputTypes) {
    // 1. build QueryNode tree
    // 1.1 populate column2analyzer
    TransactionID txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    QueryBuilder query_builder(txn_id, begin_ts, base_table_ref_);
    const Map<String, String> &column2analyzer = query_builder.GetColumn2Analyzer();
    // 1.2 parse options into map, populate default_field
    SearchOptions search_ops(match_expr_->options_text_);
    const String &default_field = search_ops.options_["default_field"];
    // 1.3 build filter
    SearchDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = reinterpret_cast<void (*)()>(&AnalyzeFunc);
    UniquePtr<QueryNode> query_tree = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (!query_tree) {
        RecoverableError(Status::ParseMatchExprFailed(match_expr_->fields_, match_expr_->matching_text_));
    }
#ifdef INFINITY_DEBUG
    LOG_TRACE("Query tree created successfully:");
    query_tree->PrintTree(std::cerr);
#endif

    // 2 build DocIterator
    FullTextQueryContext full_text_query_context;
    full_text_query_context.query_tree_ = std::move(query_tree);
    UniquePtr<DocIterator> doc_iterator = query_builder.CreateSearch(full_text_query_context);
    u32 result_count = 0;
    UniquePtr<float[]> score_result;
    UniquePtr<RowID[]> row_id_result;
#ifdef INFINITY_DEBUG
    if (doc_iterator.get() != nullptr) {
        LOG_TRACE("DocIterator created successfully:");
        full_text_query_context.query_tree_->PrintTree(std::cerr);
    }
    UniquePtr<EarlyTerminateIterator> et_iter = query_builder.CreateEarlyTerminateSearch(full_text_query_context);
    UniquePtr<float[]> et_score_result;
    UniquePtr<RowID[]> et_row_id_result;
    u32 old_loop_cnt = 0;
    u32 et_loop_cnt = 0;
#endif

    // 3 full text search
    RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : (doc_iterator->PrepareFirstDoc(), doc_iterator->Doc());
    if (iter_row_id != INVALID_ROWID) [[likely]] {
        u32 top_n = 0;
        if (auto iter_n_option = search_ops.options_.find("topn"); iter_n_option != search_ops.options_.end()) {
            int top_n_option = std::stoi(iter_n_option->second);
            if (top_n_option <= 0) {
                RecoverableError(Status::SyntaxError("topn must be a positive integer"));
            }
            top_n = top_n_option;
        } else {
            top_n = DEFAULT_FULL_TEXT_OPTION_TOP_N;
        }
        score_result = MakeUniqueForOverwrite<float[]>(top_n);
        row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n);
        FullTextScoreResultHeap result_heap(top_n, score_result.get(), row_id_result.get());
#ifdef INFINITY_DEBUG
        et_score_result = MakeUniqueForOverwrite<float[]>(top_n);
        et_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n);
        FullTextScoreResultHeap et_result_heap(top_n, et_score_result.get(), et_row_id_result.get());
        auto old_begin_ts = std::chrono::high_resolution_clock::now();
#endif
        do {
            // call scorer
            float score = query_builder.Score(iter_row_id);
            result_heap.AddResult(score, iter_row_id);
            // get next row_id
            iter_row_id = doc_iterator->Next();
#ifdef INFINITY_DEBUG
            ++old_loop_cnt;
#endif
        } while (iter_row_id != INVALID_ROWID);
        result_heap.Sort();
        result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto old_end_ts = std::chrono::high_resolution_clock::now();
        auto old_duration = std::chrono::duration_cast<std::chrono::microseconds>(old_end_ts - old_begin_ts).count();
        auto et_begin_ts = std::chrono::high_resolution_clock::now();
        while (true) {
            ++et_loop_cnt;
            auto [id, et_score] = et_iter->BlockNextWithThreshold(et_result_heap.GetScoreThreshold());
            if (id == INVALID_ROWID) {
                break;
            }
            if (et_result_heap.AddResult(et_score, id)) {
                // update threshold
                et_iter->UpdateScoreThreshold(et_result_heap.GetScoreThreshold());
            }
        }
        et_result_heap.Sort();
        auto et_end_ts = std::chrono::high_resolution_clock::now();
        auto et_duration = std::chrono::duration_cast<std::chrono::microseconds>(et_end_ts - et_begin_ts).count();
        std::cerr << "old_duration: " << old_duration << " et_duration: " << et_duration << std::endl;
        std::cerr << "duration ratio: " << (float)et_duration / old_duration << std::endl;
        std::cerr << "old_loop_cnt: " << old_loop_cnt << " et_loop_cnt: " << et_loop_cnt << std::endl;
        assert((result_count == et_result_heap.GetResultSize()));
        for (u32 i = 0; i < result_count; ++i) {
            // assert((row_id_result[i] == et_row_id_result[i]));
            ASSERT_FLOAT_EQ6(score_result[i], et_score_result[i]);
        }
#endif
    }
    LOG_TRACE(fmt::format("Full text search result count: {}", result_count));

    // 4 populate result DataBlock
    // 4.1 prepare first output_data_block
    auto &output_data_blocks = operator_state->data_block_array_;
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
            RowID &row_id = row_id_result[output_id];
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            const BlockEntry *block_entry = base_table_ref_->block_index_->GetBlockEntry(segment_id, block_id);
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
    return true;
}

PhysicalMatch::PhysicalMatch(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchExpression> match_expr,
                             u64 match_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id, load_metas), table_index_(match_table_index),
      base_table_ref_(std::move(base_table_ref)), match_expr_(std::move(match_expr)) {}

PhysicalMatch::~PhysicalMatch() = default;

void PhysicalMatch::Init() {}

bool PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) {
    return ExecuteInnerHomebrewed(query_context, operator_state, base_table_ref_, match_expr_, std::move(*GetOutputTypes()));
}

SharedPtr<Vector<String>> PhysicalMatch::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_)
        result_names->emplace_back(name);
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatch::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_)
        result_types->emplace_back(type);
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
