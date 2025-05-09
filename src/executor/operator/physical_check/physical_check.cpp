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

#include <map>
#include <string>

module physical_check;

import stl;
import txn;
import new_txn;
import query_context;

import profiler;
import operator_state;
import data_block;

import infinity_exception;
import table_entry_type;
import value_expression;
import logical_show;
import meta_info;

import value;
import table_def;
import data_table;
import third_party;
import index_base;
import index_hnsw;
import index_full_text;
import database_detail;
import default_values;
import defer_op;
import config;
import session;
import options;
import status;
import virtual_store;
import utility;
import buffer_manager;
import session_manager;
import compilation_config;
import logical_type;
import create_index_info;
import segment_index_entry;
import segment_iter;
import segment_entry;
import variables;
import default_values;
import catalog;
import txn_manager;
import wal_manager;
import logger;
import chunk_index_entry;
import memory_indexer;
import background_process;
import compaction_process;
import bg_task;
import buffer_obj;
import file_worker_type;
import system_info;
import wal_entry;
import catalog_delta_entry;
import memindex_tracer;
import persistence_manager;
import global_resource_usage;
import infinity_context;
import cleanup_scanner;
import obj_status;
import admin_statement;
import result_cache_manager;
import peer_task;
import node_info;
import txn_context;
import txn_state;
import snapshot_brief;
import command_statement;
import chunk_index_meta;
import new_txn_manager;
import kv_store;
import meta_tree;

namespace infinity {

void PhysicalCheck::Init(QueryContext *query_context) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (check_type_) {
        case CheckStmtType::kSystem: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("status");
            output_types_->emplace_back(varchar_type);
            break;
        }

        default: {
            Status status = Status::NotSupport("Not implemented check type");
            RecoverableError(status);
        }
    }
}

bool PhysicalCheck::Execute(QueryContext *query_context, OperatorState *operator_state) {
    CheckOperatorState *check_operator_state = (CheckOperatorState *)(operator_state);
    DeferFn defer_fn([&]() { check_operator_state->SetComplete(); });

    switch (check_type_) {
        case CheckStmtType::kSystem: {
            ExecuteCheckSystem(query_context, check_operator_state);
            break;
        }
        default: {
            String error_message = "Invalid chunk scan type";
            UnrecoverableError(error_message);
        }
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalCheck::ExecuteCheckSystem(QueryContext *query_context, CheckOperatorState *check_operator_state) {
    // Define output database detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    auto *new_catalog = query_context->storage()->new_catalog();
    auto meta_tree_ptr = new_catalog->MakeMetaTree();


    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("OK");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    // {
    //     SizeT column_id = 0;
    //     {
    //         Value value = Value::MakeVarchar("storage_directory");
    //         ValueExpression value_expr(value);
    //         value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //     }
    // }
    //
    // {
    //     SizeT column_id = 0;
    //     {
    //         Value value = Value::MakeVarchar("table_count");
    //         ValueExpression value_expr(value);
    //         value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //     }
    // }
    //
    // {
    //     SizeT column_id = 0;
    //     {
    //         Value value = Value::MakeVarchar("comment");
    //         ValueExpression value_expr(value);
    //         value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //     }
    // }

    output_block_ptr->Finalize();
    check_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

} // namespace infinity
