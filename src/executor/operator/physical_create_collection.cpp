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

module physical_create_collection;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import load_meta;
import internal_types;
import extra_ddl_info;
import data_type;
import wal_manager;
import infinity_context;
import status;
import infinity_exception;

namespace infinity {

PhysicalCreateCollection::PhysicalCreateCollection(SharedPtr<String> schema_name,
                                                   SharedPtr<String> collection_name,
                                                   ConflictType conflict_type,
                                                   SharedPtr<Vector<String>> output_names,
                                                   SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                   u64 table_index,
                                                   u64 id,
                                                   SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateCollection, nullptr, nullptr, id, load_metas), schema_name_(std::move(schema_name)),
      collection_name_(std::move(collection_name)), conflict_type_(conflict_type), table_index_(table_index), output_names_(std::move(output_names)),
      output_types_(std::move(output_types)) {}

void PhysicalCreateCollection::Init() {}

bool PhysicalCreateCollection::Execute(QueryContext *, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to flush on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    operator_state->SetComplete();
    return true;
}

} // namespace infinity
