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

module physical_flush;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import logger;
import bg_task;
import third_party;
import status;

namespace infinity {

void PhysicalFlush::Init() {}

bool PhysicalFlush::Execute(QueryContext *query_context, OperatorState *operator_state) {
    switch (flush_type_) {
        case FlushType::kData: {
            FlushData(query_context, operator_state);
            break;
        }
        case FlushType::kLog: {
            FlushLog(query_context, operator_state);
            break;
        }
        case FlushType::kBuffer: {
            FlushBuffer(query_context, operator_state);
            break;
        }
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalFlush::FlushData(QueryContext *query_context, OperatorState *operator_state) {
    // full checkpoint here
    auto force_ckp_task = MakeShared<ForceCheckpointTask>(query_context->GetTxn(), true /*is_full_checkpoint*/);
    auto *wal_mgr = query_context->storage()->wal_manager();
    if (!wal_mgr->TrySubmitCheckpointTask(force_ckp_task)) {
        LOG_TRACE(fmt::format("Skip {} checkpoint(manual) because there is already a full checkpoint task running.", "FULL"));
        return ;
    }
    force_ckp_task->Wait();
    LOG_TRACE("Flushed data");
}

void PhysicalFlush::FlushLog(QueryContext *query_context, OperatorState *operator_state) {
    // Generate the result
    Status status = Status::NotSupport("Flush log");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void PhysicalFlush::FlushBuffer(QueryContext *query_context, OperatorState *operator_state) {
    // Generate the result
    Status status = Status::NotSupport("Flush buffer");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

} // namespace infinity
