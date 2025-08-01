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

module infinity_core:physical_source.impl;

import :physical_source;

import :stl;
import :query_context;
import :table_def;
import :data_table;

import :physical_operator_type;
import :operator_state;
import :data_block;
import :fragment_data;
import :status;
import :infinity_exception;
import :logger;
import :physical_source;

namespace infinity {

String ToString(SourceType source_type) {
    switch (source_type) {
        case SourceType::kInvalid: {
            return "Invalid";
        }
        case SourceType::kTable: {
            return "Table";
        }
        case SourceType::kLocalQueue: {
            return "LocalQueue";
        }
        case SourceType::kEmpty: {
            return "Empty";
        }
        case SourceType::kRemote: {
            return "Remote";
        }
    }
}

void PhysicalSource::Init(QueryContext* query_context) {}

bool PhysicalSource::Execute(QueryContext *, OperatorState *) { return true; }

// A true return value indicates the source op of the task is complete.
bool PhysicalSource::Execute(QueryContext *, SourceState *source_state) {
    switch (source_state->state_type_) {
        case SourceStateType::kInvalid: {
            String error_message = "Unsupported source state type.";
            UnrecoverableError(error_message);
            break;
        }
        case SourceStateType::kMatchTensorScan:
        case SourceStateType::kMatchSparseScan:
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
            Status status = Status::NotSupport("Not support source state type");
            RecoverableError(status);
        }
    }
    return true;
}

} // namespace infinity
