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
import knn_result_handler;
import logger;
import physical_match_homebrewed;

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
