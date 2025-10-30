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

module infinity_core:physical_optimize.impl;

import :physical_optimize;
import :query_context;
import :operator_state;
import :infinity_exception;
import :table_def;
import :status;
import :logger;
import :base_table_ref;
import :wal_manager;
import :infinity_context;
import :new_txn;
import :optimization_process;
import :bg_task;

import std;
import third_party;

namespace infinity {

void PhysicalOptimize::Init(QueryContext *query_context) {}

bool PhysicalOptimize::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *optimize_operator_state = static_cast<OptimizeOperatorState *>(operator_state);
    auto *new_txn = query_context->GetNewTxn();

    auto optimize_task = std::make_shared<ManualOptimizeTask>(new_txn, db_name_, table_name_);
    auto *optimize_processor = InfinityContext::instance().storage()->optimization_processor();
    optimize_processor->Submit(optimize_task);
    optimize_task->Wait();

    optimize_operator_state->status_ = optimize_task->result_status_;
    optimize_operator_state->SetComplete();
    return true;
}

} // namespace infinity
