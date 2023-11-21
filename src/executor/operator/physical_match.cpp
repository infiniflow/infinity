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

import stl;
#include <string>
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
import third_party;

module physical_match;

namespace infinity {

PhysicalMatch::PhysicalMatch(u64 id, TableCollectionEntry *table_entry_ptr, SharedPtr<MatchExpression> match_expr)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id), table_entry_ptr_(table_entry_ptr), match_expr_(match_expr) {}

PhysicalMatch::~PhysicalMatch() {}

void PhysicalMatch::Init() {}

void PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) {}

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
