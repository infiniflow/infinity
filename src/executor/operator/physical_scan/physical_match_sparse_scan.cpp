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

#include <vector>

module physical_match_sparse_scan;

import stl;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_entry;
import match_sparse_expression;
import base_table_ref;
import data_type;
import common_query_filter;
import default_values;
import logical_type;

namespace infinity {

PhysicalMatchSparseScan::PhysicalMatchSparseScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchSparseExpression> match_sparse_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalScanBase(id, PhysicalOperatorType::kMatchSparseScan, nullptr, nullptr, base_table_ref, load_metas), table_index_(table_index),
      match_sparse_expr_(std::move(match_sparse_expression)), common_query_filter_(common_query_filter) {}

void PhysicalMatchSparseScan::Init() {
    //
}

SharedPtr<Vector<String>> PhysicalMatchSparseScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatchSparseScan::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(match_sparse_expr_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

bool PhysicalMatchSparseScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *match_sparse_scan_state = static_cast<MatchSparseScanOperatorState *>(operator_state);
    match_sparse_scan_state->SetComplete();
    return true;
}

} // namespace infinity
