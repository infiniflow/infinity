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

module physical_merge_tensor_maxsim;

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

PhysicalMergeTensorMaxSim::PhysicalMergeTensorMaxSim(const u64 id,
                                                     UniquePtr<PhysicalOperator> left,
                                                     const u64 maxsim_table_index,
                                                     SharedPtr<BaseTableRef> base_table_ref,
                                                     SharedPtr<TensorMaxSimExpression> tensor_maxsim_expression,
                                                     const u32 topn,
                                                     SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMergeTensorMaxSim, std::move(left), nullptr, id, load_metas), table_index_(maxsim_table_index),
      base_table_ref_(std::move(base_table_ref)), tensor_maxsim_expr_(std::move(tensor_maxsim_expression)), topn_(topn) {}

void PhysicalMergeTensorMaxSim::Init() { left()->Init(); }

SizeT PhysicalMergeTensorMaxSim::TaskletCount() {
    UnrecoverableError("Not Expected: TaskletCount of PhysicalMergeTensorMaxSim?");
    return 0;
}

bool PhysicalMergeTensorMaxSim::Execute(QueryContext *query_context, OperatorState *operator_state) {
    UnrecoverableError("WIP");
    return false;
}

bool PhysicalMergeTensorMaxSim::ExecuteInner(QueryContext *query_context, OperatorState *operator_state) {
    UnrecoverableError("WIP");
    return false;
}

} // namespace infinity
