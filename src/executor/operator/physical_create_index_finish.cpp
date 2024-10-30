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

import third_party;
import logger;
import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;
import index_base;
import infinity_exception;
import wal_manager;
import infinity_context;
import status;

module physical_create_index_finish;

namespace infinity {
PhysicalCreateIndexFinish::PhysicalCreateIndexFinish(u64 id,
                                                     UniquePtr<PhysicalOperator> left,
                                                     SharedPtr<String> db_name,
                                                     SharedPtr<String> table_name,
                                                     SharedPtr<IndexBase> index_base,
                                                     SharedPtr<Vector<String>> output_names,
                                                     SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                     SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexFinish, std::move(left), nullptr, id, load_metas), db_name_(db_name),
      table_name_(table_name), index_base_(index_base), output_names_(output_names), output_types_(output_types) {}

void PhysicalCreateIndexFinish::Init() {}

bool PhysicalCreateIndexFinish::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to flush on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    auto *txn = query_context->GetTxn();
    auto status = txn->CreateIndexFinish(*db_name_, *table_name_, index_base_);
    if (!status.ok()) {
        RecoverableError(status);
    }
    operator_state->SetComplete();
    return true;
}

} // namespace infinity