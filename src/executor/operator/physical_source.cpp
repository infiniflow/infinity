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
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import data_block;
import fragment_data;

import infinity_exception;

module physical_source;

namespace infinity {

void PhysicalSource::Init() {}

void PhysicalSource::Execute(QueryContext *, OperatorState *) {}

void PhysicalSource::Execute(QueryContext *, SourceState *source_state) {
    switch (source_state->state_type_) {
        case SourceStateType::kInvalid: {
            Error<ExecutorException>("Unsupported source state type.");
            break;
        }
        case SourceStateType::kKnnScan:
        case SourceStateType::kTableScan:
        case SourceStateType::kEmpty: {
            break;
        }
        case SourceStateType::kQueue: {
            QueueSourceState *queue_source_state = static_cast<QueueSourceState *>(source_state);
            UniquePtr<DataBlock> data_block{};
            bool completed = queue_source_state->GetData(data_block);

            OperatorState *next_op_state = queue_source_state->next_op_state_;
            switch(next_op_state->operator_type_) {
                case PhysicalOperatorType::kMergeKnn: {
                    MergeKnnOperatorState* merge_knn_op_state = (MergeKnnOperatorState*)next_op_state;
                    merge_knn_op_state->input_data_block_ = Move(data_block);
                    merge_knn_op_state->input_complete_ = completed;
                    break;
                }
                default: {
                    Error<ExecutorException>("Not support operator type");
                    break;
                }
            }
            break;
        }
        default: {
            Error<NotImplementException>("Not support source state type");
        }
    }
}

bool PhysicalSource::ReadyToExec(SourceState *source_state) {

    bool result = true;
    if (source_state->state_type_ == SourceStateType::kQueue) {
        QueueSourceState *queue_source_state = static_cast<QueueSourceState *>(source_state);
        result = queue_source_state->source_queue_.Size() > 0;
    }
    return result;
}

} // namespace infinity
