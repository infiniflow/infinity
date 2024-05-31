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

module physical_merge_match_sparse;

import stl;
import txn;
import query_context;
import physical_operator_type;
import operator_state;
import logger;
import status;
import infinity_exception;
import buffer_manager;
import third_party;
import default_values;
import data_block;
import value;
import column_vector;

namespace infinity {

PhysicalMergeMatchSparse::PhysicalMergeMatchSparse(u64 id,
                                                   UniquePtr<PhysicalOperator> left,
                                                   u64 table_index,
                                                   SharedPtr<BaseTableRef> base_table_ref,
                                                   SharedPtr<MatchSparseExpression> match_sparse_expr,
                                                   SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMergeMatchSparse, std::move(left), nullptr, id, load_metas), table_index_(table_index),
      base_table_ref_(std::move(base_table_ref)), match_sparse_expr_(std::move(match_sparse_expr)) {}

void PhysicalMergeMatchSparse::Init() {
    //
}

SizeT PhysicalMergeMatchSparse::TaskletCount() {
    String error_message = "Not Expected: TaskletCount of PhysicalMergeMatchSparse?";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return 0;
}

bool PhysicalMergeMatchSparse::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto* merge_match_sparse_state = static_cast<MergeMatchSparseOperatorState *>(operator_state);
    UnrecoverableError("Not implemented");
    merge_match_sparse_state->SetComplete();
    return true;
}

} // namespace infinity
