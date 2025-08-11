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

module infinity_core:physical_flush.impl;

import :physical_flush;

import :stl;
import :new_txn;
import :query_context;
import :table_def;
import :data_table;
import :wal_manager;
import :physical_operator_type;
import :operator_state;
import :logger;
import :bg_task;
import third_party;
import :status;
import :infinity_context;
import :background_process;
import :new_txn;
import :new_txn_manager;
import :kv_store;
import flush_statement;

namespace infinity {

void PhysicalFlush::Init(QueryContext *query_context) {}

bool PhysicalFlush::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    switch (flush_type_) {
        case FlushType::kCatalog:
            FlushCatalog(query_context, operator_state);
            break;
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

void PhysicalFlush::FlushCatalog(QueryContext *query_context, OperatorState *operator_state) {
    NewTxn *new_txn = query_context->GetNewTxn();
    NewTxnManager *new_txn_mgr = new_txn->txn_mgr();
    Status status = new_txn_mgr->kv_store()->Flush();
    if (!status.ok()) {
        RecoverableError(status);
    }
    LOG_TRACE("Flush catalog successfully");
    return;
}

void PhysicalFlush::FlushData(QueryContext *query_context, OperatorState *operator_state) {
    auto *wal_manager = query_context->storage()->wal_manager();
    if (wal_manager->IsCheckpointing()) {
        LOG_ERROR("There is a running checkpoint task, skip this checkpoint triggered by command");
        Status status = Status::Checkpointing();
        RecoverableError(status);
    }

    TxnTimeStamp max_commit_ts{};
    i64 wal_size{};
    std::tie(max_commit_ts, wal_size) = wal_manager->GetCommitState();
    LOG_TRACE(fmt::format("Construct checkpoint task with WAL size: {}, max_commit_ts: {}", wal_size, max_commit_ts));
    auto checkpoint_task = MakeShared<NewCheckpointTask>(wal_size);
    NewTxn *new_txn = query_context->GetNewTxn();
    checkpoint_task->new_txn_ = new_txn;

    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    bg_processor->Submit(checkpoint_task);
    checkpoint_task->Wait();
    return;
}

void PhysicalFlush::FlushLog(QueryContext *query_context, OperatorState *operator_state) {
    // Generate the result
    Status status = Status::NotSupport("Flush log");
    RecoverableError(status);
}

void PhysicalFlush::FlushBuffer(QueryContext *query_context, OperatorState *operator_state) {
    // Generate the result
    Status status = Status::NotSupport("Flush buffer");
    RecoverableError(status);
}

} // namespace infinity
