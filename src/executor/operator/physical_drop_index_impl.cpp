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

module infinity_core:physical_drop_index.impl;

import :physical_drop_index;
import :query_context;
import :operator_state;
import :table_def;
import :data_table;
import :status;
import :infinity_exception;
import :wal_manager;
import :infinity_context;
import :status;
import :new_txn;

import data_type;
import logical_type;
import column_def;

namespace infinity {

void PhysicalDropIndex::Init(QueryContext *query_context) {}

bool PhysicalDropIndex::Execute(QueryContext *query_context, OperatorState *operator_state) {
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
    Status status = new_txn->DropIndexByName(*schema_name_, *table_name_, *index_name_, conflict_type_);

    if (!status.ok()) {
        operator_state->status_ = status;
    }

    // Generate the result
    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};

    auto result_table_def_ptr =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("Tables"), nullptr, column_defs);
    output_ = std::make_shared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
