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

#include <memory>

import stl;
import query_context;
import base_expression;
import load_meta;
import physical_operator_type;
import value_expression;
import physical_limit;
import operator_state;

module physical_merge_limit;

namespace infinity {

PhysicalMergeLimit::PhysicalMergeLimit(u64 id,
                                       UniquePtr<PhysicalOperator> left,
                                       SharedPtr<BaseExpression> limit_expr,
                                       SharedPtr<BaseExpression> offset_expr,
                                       SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMergeLimit, Move(left), nullptr, id, load_metas), limit_expr_(Move(limit_expr)),
      offset_expr_(Move(offset_expr)) {
    i64 offset = 0;
    i64 limit = (static_pointer_cast<ValueExpression>(limit_expr_))->GetValue().value_.big_int;

    if (offset_expr_ != nullptr) {
        offset = (static_pointer_cast<ValueExpression>(offset_expr_))->GetValue().value_.big_int;
    }
    counter_ = MakeUnique<UnSyncCounter>(offset, limit);
}

void PhysicalMergeLimit::Init() {}

bool PhysicalMergeLimit::Execute(QueryContext *query_context, OperatorState *operator_state) {
    MergeLimitOperatorState *limit_op_state = (MergeLimitOperatorState *)operator_state;

    if (limit_op_state->input_data_blocks_.empty()) {
        return false;
    }
    auto result = PhysicalLimit::Execute(query_context, limit_op_state->input_data_blocks_, limit_op_state->data_block_array_, counter_.get());

    if (counter_->IsLimitOver() || limit_op_state->input_complete_) {
        limit_op_state->input_complete_ = true;
        limit_op_state->SetComplete();
    }
    limit_op_state->input_data_blocks_.clear();
    return result;
}

} // namespace infinity
