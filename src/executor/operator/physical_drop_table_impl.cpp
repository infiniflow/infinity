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

module infinity_core:physical_drop_table.impl;

import :physical_drop_table;

import :stl;
import :query_context;
import :table_def;
import :data_table;
import :result_cache_manager;
import :physical_operator_type;
import :operator_state;
import :status;
import :infinity_exception;
import logical_type;
import column_def;
import :wal_manager;
import :infinity_context;
import :new_txn;
import data_type;

namespace infinity {

void PhysicalDropTable::Init(QueryContext *query_context) {}

bool PhysicalDropTable::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    NewTxn *new_txn = query_context->GetNewTxn();
    Status status = new_txn->DropTable(*schema_name_, *table_name_, conflict_type_);
    if (!status.ok()) {
        operator_state->status_ = status;
    }

    if (ResultCacheManager *cache_mgr = query_context->storage()->result_cache_manager(); cache_mgr != nullptr) {
        cache_mgr->DropTable(*schema_name_, *table_name_);
    }

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};

    auto result_table_def_ptr = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
