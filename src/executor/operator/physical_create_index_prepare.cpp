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

#include <tuple>
#include <vector>

module physical_create_index_prepare;

import stl;

import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;
import block_index;
import status;
import infinity_exception;
import index_base;
import index_file_worker;
import buffer_manager;
import buffer_handle;
import index_hnsw;
import default_values;
import base_table_ref;
import extra_ddl_info;
import wal_manager;
import infinity_context;
import new_txn;

namespace infinity {
PhysicalCreateIndexPrepare::PhysicalCreateIndexPrepare(u64 id,
                                                       SharedPtr<BaseTableRef> base_table_ref,
                                                       SharedPtr<IndexBase> index_definition,
                                                       ConflictType conflict_type,
                                                       SharedPtr<Vector<String>> output_names,
                                                       SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                       SharedPtr<Vector<LoadMeta>> load_metas,
                                                       bool prepare)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexPrepare, nullptr, nullptr, id, load_metas), base_table_ref_(base_table_ref),
      index_def_ptr_(index_definition), conflict_type_(conflict_type), output_names_(output_names), output_types_(output_types), prepare_(prepare) {}

void PhysicalCreateIndexPrepare::Init(QueryContext *query_context) {}

bool PhysicalCreateIndexPrepare::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    auto *table_info = base_table_ref_->table_info_.get();
    NewTxn *new_txn = query_context->GetNewTxn();

    Status status = new_txn->CreateIndex(*table_info->db_name_, *table_info->table_name_, index_def_ptr_, conflict_type_);
    if (!status.ok()) {
        operator_state->status_ = status;
        return false;
    }

    operator_state->SetComplete();
    return true;
}
} // namespace infinity