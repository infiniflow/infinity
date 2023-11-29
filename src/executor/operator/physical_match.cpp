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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_collection_entry;
import query_context;
// import data_table;
import operator_state;
import db_entry;
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
import irs_index_entry;

module physical_match;

namespace infinity {

PhysicalMatch::PhysicalMatch(u64 id, TableCollectionEntry *table_entry_ptr, SharedPtr<MatchExpression> match_expr)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id), table_entry_ptr_(table_entry_ptr), match_expr_(match_expr) {}

PhysicalMatch::~PhysicalMatch() {}

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

void PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) {
    // Build irs::filter
    // 1.1 populate column2analyzer
    u64 txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    SharedPtr<IrsIndexEntry> irs_index_entry;
    Map<String, String> column2analyzer;
    TableCollectionEntry::GetFullTextAnalyzers(table_entry_ptr_, txn_id, begin_ts, irs_index_entry, column2analyzer);
    // 1.2 parse options into map, populate default_field
    SearchOptions search_ops(match_expr_->options_text_);
    String default_field = search_ops.options_["default_field"];
    // 1.3 build filter
    QueryDriver driver(column2analyzer, default_field);
    driver.analyze_func_ = AnalyzeFunc;
    int rc = driver.ParseSingleWithFields(match_expr_->fields_, match_expr_->matching_text_);
    if (rc != 0) {
        Error<ExecutorException>("QueryDriver::ParseSingleWithFields failed");
    }
    UniquePtr<irs::filter> flt = std::move(driver.result);

    // Search
    ScoredIds result;
    SharedPtr<IRSDataStore> &dataStore = irs_index_entry->irs_index_;
    rc = dataStore->Search(flt.get(), search_ops.options_, result);
    if (rc != 0) {
        Error<ExecutorException>("IRSDataStore::Search failed");
    }

    // populate result datablock
    // FIXME: what if result exceed DataBlock's capacity?
    SharedPtr<DataBlock> output_data_block = DataBlock::Make();
    Vector<SharedPtr<ColumnVector>> column_vectors;
    column_vectors.push_back(ColumnVector::Make(MakeShared<DataType>(LogicalType::kFloat)));
    column_vectors.push_back(ColumnVector::Make(MakeShared<DataType>(LogicalType::kRowID)));
    output_data_block->Init(column_vectors);
    for (ScoredId &scoredId : result) {
        Value v = Value::MakeFloat(scoredId.first);
        column_vectors[0]->AppendValue(v);
        column_vectors[1]->AppendWith(DocID2RowID(scoredId.second), 1);
    }
    output_data_block->Finalize();
    operator_state->data_block_ = output_data_block;

    operator_state->SetComplete();
}

SharedPtr<Vector<String>> PhysicalMatch::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    result_names->emplace_back(COLUMN_NAME_SCORE);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatch::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kFloat));
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
    String res = Format("{} Table: {}, {}", arrow_str, *(table_entry_ptr_->table_collection_name_), match_expr_->ToString());
    return res;
}

} // namespace infinity
