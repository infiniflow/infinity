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

namespace infinity {

void ASSERT_FLOAT_EQ6(u32 i, float a, float b) {
    float diff_percent = std::abs(a - b) / std::max(std::abs(a), std::abs(b));
    if (diff_percent >= 1e-6) {
        OStringStream oss;
        oss << "result mismatch at " << i << " : a: " << a << ", b: " << b << ", diff_percent: " << diff_percent << std::endl;
        UnrecoverableError(std::move(oss).str());
    }
}

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
    const String &block_max_option = search_ops.options_["block_max"];
    bool use_ordinary_iter = false;
    bool use_block_max_iter = false;
    if (block_max_option == "true") {
        use_block_max_iter = true;
    } else if (block_max_option == "false") {
        use_ordinary_iter = true;
    } else if (block_max_option == "compare" or block_max_option.empty()) {
        use_ordinary_iter = true;
        use_block_max_iter = true;
    } else {
        RecoverableError(Status::SyntaxError("block_max option must be empty, true, false or compare"));
    }
    // 1.3 build filter
    SearchDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = reinterpret_cast<void (*)()>(&AnalyzeFunc);
    UniquePtr<QueryNode> query_tree = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (!query_tree) {
        RecoverableError(Status::ParseMatchExprFailed(match_expr_->fields_, match_expr_->matching_text_));
    }
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "Query tree created successfully:\n";
        query_tree->PrintTree(oss);
        LOG_INFO(std::move(oss).str());
    }
#endif

    // 2 build query iterator
    // result
    u32 result_count = 0;
    const float *score_result = nullptr;
    const RowID *row_id_result = nullptr;
    // for comparison
    UniquePtr<DocIterator> doc_iterator;
    UniquePtr<EarlyTerminateIterator> et_iter;
    u32 ordinary_loop_cnt = 0;
    u32 blockmax_loop_cnt = 0;
    u32 ordinary_result_count = 0;
    u32 blockmax_result_count = 0;
    UniquePtr<float[]> ordinary_score_result;
    UniquePtr<RowID[]> ordinary_row_id_result;
    UniquePtr<float[]> blockmax_score_result;
    UniquePtr<RowID[]> blockmax_row_id_result;
    using TimeDurationType = std::chrono::high_resolution_clock::rep;
    TimeDurationType ordinary_duration = 0;
    TimeDurationType blockmax_duration = 0;
    FullTextQueryContext full_text_query_context;
    full_text_query_context.query_tree_ = std::move(query_tree);
    if (use_ordinary_iter) {
        doc_iterator = query_builder.CreateSearch(full_text_query_context);
#ifdef INFINITY_DEBUG
        if (doc_iterator.get() != nullptr) {
            OStringStream oss;
            oss << "DocIterator created successfully:\n";
            full_text_query_context.query_tree_->PrintTree(oss);
            LOG_INFO(std::move(oss).str());
        }
#endif
    }
    if (use_block_max_iter) {
        et_iter = query_builder.CreateEarlyTerminateSearch(full_text_query_context);
#ifdef INFINITY_DEBUG
        if (et_iter.get() != nullptr) {
            OStringStream oss;
            oss << "EarlyTerminateIterator created successfully:\n";
            full_text_query_context.query_tree_->PrintTree(oss);
            LOG_INFO(std::move(oss).str());
        }
#endif
    }

    // 3 full text search
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
    if (use_ordinary_iter) {
        RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : (doc_iterator->PrepareFirstDoc(), doc_iterator->Doc());
        if (iter_row_id != INVALID_ROWID) [[likely]] {
            ordinary_score_result = MakeUniqueForOverwrite<float[]>(top_n);
            ordinary_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n);
            FullTextScoreResultHeap result_heap(top_n, ordinary_score_result.get(), ordinary_row_id_result.get());
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
            ordinary_duration = std::chrono::duration_cast<std::chrono::microseconds>(ordinary_end_ts - ordinary_begin_ts).count();
#endif
        }
    }
    if (use_block_max_iter) {
        blockmax_score_result = MakeUniqueForOverwrite<float[]>(top_n);
        blockmax_row_id_result = MakeUniqueForOverwrite<RowID[]>(top_n);
        FullTextScoreResultHeap result_heap(top_n, blockmax_score_result.get(), blockmax_row_id_result.get());
#ifdef INFINITY_DEBUG
        auto blockmax_begin_ts = std::chrono::high_resolution_clock::now();
#endif
        if (et_iter) {
            while (true) {
                ++blockmax_loop_cnt;
                auto [id, et_score] = et_iter->BlockNextWithThreshold(result_heap.GetScoreThreshold());
                if (id == INVALID_ROWID) [[unlikely]] {
                    break;
                }
                if (result_heap.AddResult(et_score, id)) {
                    // update threshold
                    et_iter->UpdateScoreThreshold(result_heap.GetScoreThreshold());
                }
            }
        }
        result_heap.Sort();
        blockmax_result_count = result_heap.GetResultSize();
#ifdef INFINITY_DEBUG
        auto blockmax_end_ts = std::chrono::high_resolution_clock::now();
        blockmax_duration = std::chrono::duration_cast<std::chrono::microseconds>(blockmax_end_ts - blockmax_begin_ts).count();
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
#ifdef INFINITY_DEBUG
    if (use_ordinary_iter) {
        std::cerr << "ordinary_duration: " << ordinary_duration << std::endl;
        std::cerr << "ordinary_loop_cnt: " << ordinary_loop_cnt << std::endl;
    }
    if (use_block_max_iter) {
        std::cerr << "blockmax_duration: " << blockmax_duration << std::endl;
        std::cerr << "blockmax_loop_cnt: " << blockmax_loop_cnt << std::endl;
    }
    if (use_ordinary_iter and use_block_max_iter) {
        std::cerr << "duration ratio: " << (static_cast<float>(blockmax_duration) / ordinary_duration) << std::endl;
        std::cerr << "loop count ratio: " << (static_cast<float>(blockmax_loop_cnt) / ordinary_loop_cnt) << std::endl;
        if (ordinary_result_count != blockmax_result_count) {
            OStringStream oss;
            oss << "result count mismatch: ordinary: " << ordinary_result_count << ", blockmax: " << blockmax_result_count << '\n';
            UnrecoverableError(std::move(oss).str());
        }
        for (u32 i = 0; i < result_count; ++i) {
            ASSERT_FLOAT_EQ6(i, ordinary_score_result[i], blockmax_score_result[i]);
        }
    }
#endif
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
            const RowID &row_id = row_id_result[output_id];
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
