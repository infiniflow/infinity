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

import physical_operator_type;
import operator_state;
import status;
import load_meta;
import extra_ddl_info;
import wal_manager;
import infinity_context;

module physical_create_table;

namespace infinity {

PhysicalCreateTable::PhysicalCreateTable(SharedPtr<String> schema_name,
                                         SharedPtr<TableDef> table_def_ptr,
                                         SharedPtr<Vector<String>> output_names,
                                         SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                         ConflictType conflict_type,
                                         u64 table_index,
                                         u64 id,
                                         SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, nullptr, nullptr, id, load_metas), table_def_ptr_(std::move(table_def_ptr)),
      schema_name_(std::move(schema_name)), table_index_(table_index), conflict_type_(conflict_type), output_names_(std::move(output_names)),
      output_types_(std::move(output_types)) {}

PhysicalCreateTable::PhysicalCreateTable(SharedPtr<String> schema_name,
                                         UniquePtr<PhysicalOperator> input,
                                         SharedPtr<Vector<String>> output_names,
                                         SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                         ConflictType conflict_type,
                                         u64 table_index,
                                         u64 id,
                                         SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, std::move(input), nullptr, id, load_metas), schema_name_(std::move(schema_name)),
      table_index_(table_index), conflict_type_(conflict_type), output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

void PhysicalCreateTable::Init() {}

bool PhysicalCreateTable::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to flush on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    auto txn = query_context->GetTxn();

    Status status = txn->CreateTable(*schema_name_, table_def_ptr_, conflict_type_);
    if (!status.ok()) {
        operator_state->status_ = status;
    }
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
