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

#include <string>

module physical_match_homebrewed;

import stl;
import query_context;
import operator_state;
import data_block;
import column_vector;
import default_values;
import match_expression;
import infinity_exception;
import data_type;
import internal_types;
import value;
import third_party;
import base_table_ref;
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
import knn_result_handler;
import analyzer_pool;
import analyzer;
import logger;
import term;

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
    Map<String, String> &column2analyzer = query_builder.GetColumn2Analyzer();
    // 1.2 parse options into map, populate default_field
    SearchOptions search_ops(match_expr_->options_text_);
    String &default_field = search_ops.options_["default_field"];
    // 1.3 build filter
    SearchDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = reinterpret_cast<void (*)()>(&AnalyzeFunc);
    UniquePtr<QueryNode> query_tree = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (!query_tree) {
        RecoverableError(Status::ParseMatchExprFailed(match_expr_->fields_, match_expr_->matching_text_));
    }

    // 2 build DocIterator
    FullTextQueryContext full_text_query_context;
    full_text_query_context.query_tree_ = std::move(query_tree);
    UniquePtr<DocIterator> doc_iterator = query_builder.CreateSearch(full_text_query_context);
    u32 result_count = 0;
    UniquePtr<float[]> score_result;
    UniquePtr<RowID[]> row_id_result;

    // 3 full text search
    RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : doc_iterator->Doc();
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
        using ResultHandler = ReservoirResultHandler<CompareMin<float, RowID>>;
        ResultHandler result_handler(1, top_n, score_result.get(), row_id_result.get());
        result_handler.Begin();
        // prepare query_builder
        query_builder.LoadScorerColumnLength(iter_row_id);
        do {
            // call scorer
            float score = query_builder.Score(iter_row_id);
            result_handler.AddResult(0, score, iter_row_id);
            // get next row_id
            iter_row_id = doc_iterator->Next();
        } while (iter_row_id != INVALID_ROWID);
        result_handler.End();
        result_count = result_handler.GetSize(0);
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

} // namespace infinity
