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
import parser;
import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;
import catalog;
import index_def;
import status;
import infinity_exception;
import index_base;
import index_file_worker;
import segment_iter;
import buffer_manager;
import buffer_handle;
import index_hnsw;
import default_values;
import txn_store;

import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;

module physical_create_index_prepare;

namespace infinity {
PhysicalCreateIndexPrepare::PhysicalCreateIndexPrepare(u64 id,
                                                       SharedPtr<String> schema_name,
                                                       SharedPtr<String> table_name,
                                                       SharedPtr<IndexDef> index_definition,
                                                       ConflictType conflict_type,
                                                       SharedPtr<Vector<String>> output_names,
                                                       SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                       SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexPrepare, nullptr, nullptr, id, load_metas), schema_name_(schema_name),
      table_name_(table_name), index_def_ptr_(index_definition), conflict_type_(conflict_type), output_names_(output_names),
      output_types_(output_types) {}

void PhysicalCreateIndexPrepare::Init() {}

bool PhysicalCreateIndexPrepare::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *txn = query_context->GetTxn();
    Status status = txn->CreateIndex(*schema_name_, *table_name_, index_def_ptr_, conflict_type_, true);
    if (!status.ok()) {
        operator_state->error_message_ = Move(status.msg_);
    }
    operator_state->SetComplete();
    return true;
}
} // namespace infinity