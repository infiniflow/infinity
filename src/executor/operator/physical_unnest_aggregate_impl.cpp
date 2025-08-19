// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:physical_unnest_aggregate.impl;

import :physical_unnest_aggregate;
import :query_context;
import :table_def;
import :data_table;
import :physical_operator_type;
import :operator_state;
import :expression_state;
import :expression_selector;
import :data_block;
import :logger;

import third_party;

import :infinity_exception;

namespace infinity {

void PhysicalUnnestAggregate::Init(QueryContext *query_context) {}

bool PhysicalUnnestAggregate::Execute(QueryContext *, OperatorState *operator_state) {
    OperatorState *prev_op_state = operator_state->prev_op_state_;
    if (prev_op_state->Complete()) {
        operator_state->SetComplete();
    }
    return true;
}

} // namespace infinity
