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

module physical_create_index_prepare;

import stl;

import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;

import status;
import infinity_exception;
import index_base;
import index_file_worker;
import buffer_manager;
import buffer_handle;
import index_hnsw;
import default_values;
import txn_store;
import base_table_ref;
import extra_ddl_info;

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

void PhysicalCreateIndexPrepare::Init() {}

bool PhysicalCreateIndexPrepare::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *txn = query_context->GetTxn();
    auto *table_entry = base_table_ref_->table_entry_ptr_;
    auto [table_index_entry, status] = txn->CreateIndexDef(table_entry, index_def_ptr_, conflict_type_);
    if (!status.ok()) {
        operator_state->status_ = status;
    } else {
        auto [segment_index_entries, status] = txn->CreateIndexPrepare(table_index_entry, base_table_ref_.get(), prepare_);
        if (!status.ok()) {
            operator_state->status_ = status;
            return true;
        }
        for (auto *segment_index_entry : segment_index_entries) {
            base_table_ref_->index_index_->Insert(table_index_entry, segment_index_entry);
        }
        if (!prepare_) {
            auto status = txn->CreateIndexFinish(table_entry, table_index_entry);
            if (!status.ok()) {
                operator_state->status_ = status;
                return true;
            }
        }
    }
    operator_state->SetComplete();
    return true;
}
} // namespace infinity