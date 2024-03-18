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

#include "analysis/token_attributes.hpp"
#include "search/filter.hpp"
#include <string>

module physical_match;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
// import data_table;
import operator_state;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import base_expression;
import match_expression;
import default_values;
import infinity_exception;
import iresearch_datastore;
import value;
import third_party;
import iresearch_analyzer;
import base_table_ref;
import load_meta;
import fulltext_index_entry;
import block_entry;
import block_column_entry;
import logical_type;
import search_options;
import query_driver;
import status;
import index_defines;
import search_driver;
import query_node;

namespace infinity {

PhysicalMatch::PhysicalMatch(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchExpression> match_expr,
                             u64 match_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id, load_metas), table_index_(match_table_index),
      base_table_ref_(std::move(base_table_ref)), match_expr_(std::move(match_expr)) {}

PhysicalMatch::~PhysicalMatch() = default;

void PhysicalMatch::Init() {}

static void AnalyzeFunc(const std::string &analyzer_name, const std::string &text, std::vector<std::string> &terms) {
    UniquePtr<IRSAnalyzer> analyzer = AnalyzerPool::instance().Get(analyzer_name);
    // refers to https://github.com/infiniflow/iresearch/blob/master/tests/analysis/jieba_analyzer_tests.cpp
    analyzer->reset(text);
    auto *term = irs::get<irs::term_attribute>(*analyzer);
    while (analyzer->next()) {
        auto term_value = std::string(irs::ViewCast<char>(term->value).data(), term->value.size());
        terms.push_back(term_value);
    }
}

bool PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) { return ExecuteInner(query_context, operator_state); }

bool PhysicalMatch::ExecuteInnerHomebrewed(QueryContext *query_context, OperatorState *operator_state) {
    // 1.1 populate column2analyzer
    TransactionID txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    SharedPtr<FulltextIndexEntry> fulltext_index_entry;
    Map<String, String> column2analyzer;
    base_table_ref_->table_entry_ptr_->GetFulltextAnalyzers(txn_id, begin_ts, fulltext_index_entry, column2analyzer);
    // 1.2 parse options into map, populate default_field
    SearchOptions search_ops(match_expr_->options_text_);
    String default_field = search_ops.options_["default_field"];
    // 1.3 build filter
    SearchDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = AnalyzeFunc;
    auto result = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (!result) {
        RecoverableError(Status::ParseMatchExprFailed(match_expr_->fields_, match_expr_->matching_text_));
    }
    // TODO
    return true;
}

bool PhysicalMatch::ExecuteInner(QueryContext *query_context, OperatorState *operator_state) {
    // 1 build irs::filter
    // 1.1 populate column2analyzer
    TransactionID txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    SharedPtr<FulltextIndexEntry> fulltext_index_entry;
    Map<String, String> column2analyzer;
    base_table_ref_->table_entry_ptr_->GetFulltextAnalyzers(txn_id, begin_ts, fulltext_index_entry, column2analyzer);
    // 1.2 parse options into map, populate default_field
    SearchOptions search_ops(match_expr_->options_text_);
    String default_field = search_ops.options_["default_field"];
    // 1.3 build filter
    QueryDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = AnalyzeFunc;
    int rc = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (rc != 0) {
        RecoverableError(Status::ParseMatchExprFailed(match_expr_->fields_, match_expr_->matching_text_));
    }
    UniquePtr<irs::filter> flt = std::move(driver.result_);

    // 2 full text search
    ScoredIds result;
    if (fulltext_index_entry == nullptr) {
        RecoverableError(Status::FTSIndexNotExist(*base_table_ref_->table_entry_ptr_->GetTableName()));
    }
    UniquePtr<IRSDataStore> &dataStore = fulltext_index_entry->irs_index_;
    if (dataStore == nullptr) {
        RecoverableError(Status::FTSIndexNotExist(*base_table_ref_->table_entry_ptr_->GetTableName()));
    }
    rc = dataStore->Search(flt.get(), search_ops.options_, result);
    if (rc != 0) {
        RecoverableError(Status::UnknownFTSFault());
    }

    // 3 populate result datablock
    // FIXME: what if result exceed DataBlock's capacity?
    // 3.1 initialize output datablock
    Vector<SizeT> &column_ids = base_table_ref_->column_ids_;
    SizeT column_n = column_ids.size();
    UniquePtr<DataBlock> output_data_block = DataBlock::MakeUniquePtr();
    output_data_block->Init(*GetOutputTypes());

    for (ScoredId &scoredId : result) {
        // 3.2 enrich columns needed by later operators
        RowID row_id = DocID2RowID(scoredId.second);
        u32 segment_id = row_id.segment_id_;
        u32 segment_offset = row_id.segment_offset_;
        u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
        u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

        const BlockEntry *block_entry = base_table_ref_->block_index_->GetBlockEntry(segment_id, block_id);

        SizeT column_id = 0;
        for (; column_id < column_n; ++column_id) {
            BlockColumnEntry *block_column_ptr = block_entry->GetColumnBlockEntry(column_ids[column_id]);
            ColumnVector column_vector = block_column_ptr->GetColumnVector(query_context->storage()->buffer_manager());
            output_data_block->column_vectors[column_id]->AppendWith(column_vector, block_offset, 1);
        }

        // 3.3 add hiden columns: score, row_id
        Value v = Value::MakeFloat(scoredId.first);
        output_data_block->column_vectors[column_id++]->AppendValue(v);
        output_data_block->column_vectors[column_id]->AppendWith(row_id, 1);
    }
    output_data_block->Finalize();
    operator_state->data_block_array_.emplace_back(std::move(output_data_block));

    operator_state->SetComplete();
    return true;
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
