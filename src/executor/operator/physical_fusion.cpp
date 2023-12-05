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
import fusion_expression;
import load_meta;
import default_values;
import third_party;

module physical_fusion;

namespace infinity {

PhysicalFusion::PhysicalFusion(u64 id,
                               UniquePtr<PhysicalOperator> left,
                               UniquePtr<PhysicalOperator> right,
                               SharedPtr<FusionExpression> fusion_expr,
                               SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kFusion, Move(left), Move(right), id, load_metas), fusion_expr_(fusion_expr) {}

PhysicalFusion::~PhysicalFusion() {}

void PhysicalFusion::Init() {}

void PhysicalFusion::Execute(QueryContext *, OperatorState *) {}

SharedPtr<Vector<String>> PhysicalFusion::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    result_names->emplace_back(COLUMN_NAME_SCORE);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalFusion::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kFloat));
    return result_types;
}

String PhysicalFusion::ToString(i64 &space) const {
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> PhysicalFusion ";
    } else {
        arrow_str = "PhysicalFusion ";
    }
    String res = Format("{} {}", arrow_str, fusion_expr_->ToString());
    return res;
}

} // namespace infinity
