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

module infinity_core:physical_drop_collection.impl;

import :physical_drop_collection;

import :stl;
import :query_context;
import :table_def;
import :data_table;

import :physical_operator_type;
import :operator_state;
import :wal_manager;
import :infinity_context;
import :status;

namespace infinity {

void PhysicalDropCollection::Init(QueryContext* query_context) {}

bool PhysicalDropCollection::Execute(QueryContext *, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    operator_state->SetComplete();
    return true;
}

} // namespace infinity
