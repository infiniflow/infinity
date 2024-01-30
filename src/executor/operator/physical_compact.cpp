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

module physical_compact;

import stl;
import query_context;
import compact_segments_task;

namespace infinity {

void PhysicalCompact::Init() {}

bool PhysicalCompact::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *compact_op_state = static_cast<CompactOperatorState *>(operator_state);

    auto *txn = query_context->GetTxn();
    CompactSegmentsTask compact_task(table_ref_.get(), txn);
    compact_task.Execute();
    compact_op_state->SetComplete();

    compact_op_state->result_msg_ = MakeUnique<String>("Compacted.");
    return true;
}

} // namespace infinity
