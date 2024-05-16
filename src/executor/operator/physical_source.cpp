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

module physical_source;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import data_block;
import fragment_data;
import status;
import infinity_exception;

namespace infinity {

void PhysicalSource::Init() {}

bool PhysicalSource::Execute(QueryContext *, OperatorState *) { return true; }

// A true return value indicates the source op of the task is complete.
bool PhysicalSource::Execute(QueryContext *, SourceState *source_state) {
    switch (source_state->state_type_) {
        case SourceStateType::kInvalid: {
            UnrecoverableError("Unsupported source state type.");
            break;
        }
        case SourceStateType::kKnnScan:
        case SourceStateType::kTableScan:
        case SourceStateType::kIndexScan:
        case SourceStateType::kCompact: {
            return true;
        }
        case SourceStateType::kEmpty: {
            source_state->next_op_state_->empty_source_ = true;
            return true;
        }
        case SourceStateType::kQueue: {
            QueueSourceState *queue_source_state = static_cast<QueueSourceState *>(source_state);
            return queue_source_state->GetData();
        }
        default: {
            RecoverableError(Status::NotSupport("Not support source state type"));
        }
    }
    return true;
}

} // namespace infinity
