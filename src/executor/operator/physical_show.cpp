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

#include <string>
#include <map>

module physical_show;

import stl;
import txn;
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
import index_ivfflat;
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
import local_file_system;
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
import background_process;
import compaction_process;
import bg_task;
import buffer_obj;
import file_worker_type;
import system_info;

namespace infinity {

void PhysicalShow::Init() {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (scan_type_) {
        case ShowType::kShowDatabase: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowTable: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowIndex: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowIndexSegment: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowIndexChunk: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowDatabases: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("database");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowTables: {

            output_names_->reserve(8);
            output_types_->reserve(8);

            output_names_->emplace_back("database");
            output_names_->emplace_back("table");
            output_names_->emplace_back("type");
            output_names_->emplace_back("column_count");
            output_names_->emplace_back("block_count");
            output_names_->emplace_back("block_capacity");
            output_names_->emplace_back("segment_count");
            output_names_->emplace_back("segment_capacity");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowType::kShowColumn: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("column_name");
            output_names_->emplace_back("column_type");
            output_names_->emplace_back("constraint");
            output_names_->emplace_back("default");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowIndexes: {

            output_names_->reserve(7);
            output_types_->reserve(7);

            output_names_->emplace_back("index_name");
            output_names_->emplace_back("index_type");
            output_names_->emplace_back("column_id");
            output_names_->emplace_back("column_name");
            output_names_->emplace_back("path");
            output_names_->emplace_back("index segments");
            output_names_->emplace_back("other_parameters");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowConfigs: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("config_name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowProfiles: {

            output_names_->reserve(9);
            output_types_->reserve(9);

            output_names_->emplace_back("record_no");
            output_names_->emplace_back("command parsing");
            output_names_->emplace_back("logical plan building");
            output_names_->emplace_back("plan optimizing");
            output_names_->emplace_back("physical plan building");
            output_names_->emplace_back("pipeline building");
            output_names_->emplace_back("task building");
            output_names_->emplace_back("execution");
            output_names_->emplace_back("commit");
            output_names_->emplace_back("rollback");
            output_names_->emplace_back("total_cost");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowSegments: {
            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("id");
            output_names_->emplace_back("status");
            output_names_->emplace_back("size");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowSegment: {
            output_names_->reserve(10);
            output_types_->reserve(10);

            output_names_->emplace_back("id");
            output_names_->emplace_back("status");
            output_names_->emplace_back("dir");
            output_names_->emplace_back("size");
            output_names_->emplace_back("block_count");
            output_names_->emplace_back("row_capacity");
            output_names_->emplace_back("row_count");
            output_names_->emplace_back("actual_row_count");
            output_names_->emplace_back("room");
            output_names_->emplace_back("column_count");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }

        case ShowType::kShowBlocks: {
            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("id");
            output_names_->emplace_back("size");
            output_names_->emplace_back("row_count");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            break;
        }

        case ShowType::kShowBlock: {
            output_names_->reserve(8);
            output_types_->reserve(8);

            output_names_->emplace_back("id");
            output_names_->emplace_back("path");
            output_names_->emplace_back("size");
            output_names_->emplace_back("row_capacity");
            output_names_->emplace_back("row_count");
            output_names_->emplace_back("checkpoint_row_count");
            output_names_->emplace_back("column_count");
            output_names_->emplace_back("checkpoint_ts");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }

        case ShowType::kShowBlockColumn: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }

        case ShowType::kShowSessionVariable: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowGlobalVariable: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowSessionVariables: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowGlobalVariables: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowConfig: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowBuffer: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("path");
            output_names_->emplace_back("status");
            output_names_->emplace_back("size");
            output_names_->emplace_back("buffered_type");
            output_names_->emplace_back("type");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented show type");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
}

bool PhysicalShow::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ShowOperatorState *show_operator_state = (ShowOperatorState *)(operator_state);
    DeferFn defer_fn([&]() { show_operator_state->SetComplete(); });

    switch (scan_type_) {
        case ShowType::kShowDatabase: {
            ExecuteShowDatabase(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowTable: {
            ExecuteShowTable(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowIndex: {
            ExecuteShowIndex(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowIndexSegment: {
            ExecuteShowIndexSegment(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowIndexChunk: {
            ExecuteShowIndexChunk(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowDatabases: {
            ExecuteShowDatabases(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowTables: {
            ExecuteShowTables(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowColumn: {
            ExecuteShowColumns(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowIndexes: {
            ExecuteShowIndexes(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowConfigs: {
            ExecuteShowConfigs(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowProfiles: {
            ExecuteShowProfiles(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowSegments: {
            ExecuteShowSegments(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowSegment: {
            ExecuteShowSegmentDetail(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowBlocks: {
            ExecuteShowBlocks(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowBlock: {
            ExecuteShowBlockDetail(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowBlockColumn: {
            ExecuteShowBlockColumn(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowViews: {
            ExecuteShowViews(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowSessionVariable: {
            ExecuteShowSessionVariable(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowGlobalVariable: {
            ExecuteShowGlobalVariable(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowSessionVariables: {
            ExecuteShowSessionVariables(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowGlobalVariables: {
            ExecuteShowGlobalVariables(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowConfig: {
            ExecuteShowConfig(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowBuffer: {
            ExecuteShowBuffer(query_context, show_operator_state);
            break;
        }
        default: {
            String error_message = "Invalid chunk scan type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalShow::ExecuteShowDatabase(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output database detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    auto [database_info, status] = txn->GetDatabaseInfo(db_name_);

    if (!status.ok()) {
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database name to the 1 column
            const String *table_name = database_info->db_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            const String *db_dir = database_info->db_entry_dir_.get();
            Value value = Value::MakeVarchar(*db_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            Value value = Value::MakeVarchar(std::to_string(database_info->table_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowTable(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    auto [table_info, status] = txn->GetTableInfo(db_name_, object_name_);

    if (!status.ok()) {
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(db_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_name = table_info->table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_dir = table_info->table_entry_dir_.get();
            Value value = Value::MakeVarchar(*table_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->segment_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndex(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    auto [table_index_info, status] = txn->GetTableIndexInfo(db_name_, object_name_, index_name_.value());

    if (!status.ok()) {
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(db_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(object_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(index_name_.value());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_type");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_type_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_names");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_names_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_ids");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_ids_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("other_parameters");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_other_params_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_dir = table_index_info->index_entry_dir_.get();
            Value value = Value::MakeVarchar(*table_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_dir = table_index_info->index_entry_dir_.get();
            const auto &index_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*table_dir));
            Value value = Value::MakeVarchar(index_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_index_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_index_info->segment_index_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexSegment(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    auto [table_entry, status1] = txn->GetTableByName(db_name_, object_name_);
    if (!status1.ok()) {
        LOG_ERROR(status1.message());
        RecoverableError(status1);
        return;
    }

    auto [table_index_entry, status2] = txn->GetIndexByName(db_name_, object_name_, index_name_.value());
    if (!status2.ok()) {
        LOG_ERROR(status2.message());
        RecoverableError(status2);
        return;
    }

    Map<SegmentID, SharedPtr<SegmentIndexEntry>> segment_map = table_index_entry->GetIndexBySegmentSnapshot(table_entry, txn);
    auto iter = segment_map.find(segment_id_.value());
    if(iter == segment_map.end()) {
        show_operator_state->status_ = Status::SegmentNotExist(segment_id_.value());
        LOG_ERROR(show_operator_state->status_.message());
        RecoverableError(show_operator_state->status_);
    }

    SegmentIndexEntry* segment_index_entry = iter->second.get();
    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_id_.value()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*segment_index_entry->index_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_segment_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const auto &index_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*segment_index_entry->index_dir()));
            Value value = Value::MakeVarchar(index_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("chunk_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            IndexBase* index_base = table_index_entry->table_index_def().get();
            String index_type_name = IndexInfo::IndexTypeToString(index_base->index_type_);
            switch(index_base->index_type_) {
                case IndexType::kIVFFlat: {
                    Status status3 = Status::InvalidIndexName(index_type_name);
                    show_operator_state->status_ = status3;
                    LOG_ERROR(fmt::format("{} isn't implemented.", index_type_name));
                    RecoverableError(status3);
                    break;
                }
                case IndexType::kHnsw: {
                    auto [chunk_index_entries, _] = segment_index_entry->GetHnswIndexSnapshot();

                    Value value = Value::MakeVarchar(std::to_string(chunk_index_entries.size()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case IndexType::kFullText: {
                    auto [chunk_index_entries, _] = segment_index_entry->GetFullTextIndexSnapshot();

                    Value value = Value::MakeVarchar(std::to_string(chunk_index_entries.size()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case IndexType::kSecondary: {
                    auto [chunk_index_entries, _] = segment_index_entry->GetSecondaryIndexSnapshot();

                    Value value = Value::MakeVarchar(std::to_string(chunk_index_entries.size()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case IndexType::kInvalid: {
                    Status status3 = Status::InvalidIndexName(index_type_name);
                    LOG_ERROR(fmt::format("{} is invalid.", index_type_name));
                    RecoverableError(status3);
                    break;
                }
            }
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexChunk(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    auto [table_entry, status1] = txn->GetTableByName(db_name_, object_name_);
    if (!status1.ok()) {
        LOG_ERROR(status1.message());
        RecoverableError(status1);
        return;
    }

    auto [table_index_entry, status2] = txn->GetIndexByName(db_name_, object_name_, index_name_.value());
    if (!status2.ok()) {
        LOG_ERROR(status2.message());
        RecoverableError(status2);
        return;
    }

    Map<SegmentID, SharedPtr<SegmentIndexEntry>> segment_map = table_index_entry->GetIndexBySegmentSnapshot(table_entry, txn);
    auto iter = segment_map.find(segment_id_.value());
    if(iter == segment_map.end()) {
        show_operator_state->status_ = Status::SegmentNotExist(segment_id_.value());
        LOG_ERROR(show_operator_state->status_.message());
        RecoverableError(show_operator_state->status_);
    }

    SegmentIndexEntry* segment_index_entry = iter->second.get();
    IndexBase* index_base = table_index_entry->table_index_def().get();
    String index_type_name = IndexInfo::IndexTypeToString(index_base->index_type_);

    Vector<SharedPtr<ChunkIndexEntry>> chunk_indexes;
    switch(index_base->index_type_) {
        case IndexType::kIVFFlat: {
            Status status3 = Status::InvalidIndexName(index_type_name);
            show_operator_state->status_ = status3;
            LOG_ERROR(fmt::format("{} isn't implemented.", index_type_name));
            RecoverableError(status3);
            break;
        }
        case IndexType::kHnsw: {
            auto [chunk_index_entries, _] = segment_index_entry->GetHnswIndexSnapshot();
            chunk_indexes = chunk_index_entries;
            break;
        }
        case IndexType::kFullText: {
            auto [chunk_index_entries, _] = segment_index_entry->GetFullTextIndexSnapshot();
            chunk_indexes = chunk_index_entries;
            break;
        }
        case IndexType::kSecondary: {
            auto [chunk_index_entries, _] = segment_index_entry->GetSecondaryIndexSnapshot();
            chunk_indexes = chunk_index_entries;
            break;
        }
        case IndexType::kInvalid: {
            Status status3 = Status::InvalidIndexName(index_type_name);
            LOG_ERROR(fmt::format("{} is invalid.", index_type_name));
            RecoverableError(status3);
            break;
        }
    }

    ChunkID chunk_id = chunk_id_.value();
    if(chunk_id >= chunk_indexes.size()) {
        show_operator_state->status_ = Status::ChunkNotExist(chunk_id);
        LOG_ERROR(show_operator_state->status_.message());
        RecoverableError(show_operator_state->status_);
    }

    ChunkIndexEntry* chunk_index_entry = chunk_indexes[chunk_id].get();

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("file_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(chunk_index_entry->base_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("start_row");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(chunk_index_entry->base_rowid_.ToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(chunk_index_entry->row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("deprecate_timestamp");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(chunk_index_entry->deprecate_ts_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<DatabaseDetail> databases_detail = txn->ListDatabases();

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type};
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    for (auto &database_detail : databases_detail) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = database_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowTables(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<TableDetail> table_collections_detail;
    Status status = txn->GetTables(db_name_, table_collections_detail);

    if (!status.ok()) {
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    for (auto &table_detail : table_collections_detail) {
        // Initialize the output data block
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_detail.table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableEntryType table_type = table_detail.table_entry_type_;
        {
            // Append base table type to the 2 column
            const String &base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            switch (table_type) {
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.column_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableEntryType::kCollectionEntry: {
                    // TODO: column count need to be given for table.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
            }
        }

        ++column_id;
        {
            // Append block count the 4 column
            switch (table_type) {
                case TableEntryType::kTableEntry:
                case TableEntryType::kCollectionEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.block_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    String error_message = "Invalid table type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
        }

        ++column_id;
        {
            // Append block capacity the 5 column
            switch (table_type) {
                case TableEntryType::kCollectionEntry:
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.block_capacity_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    String error_message = "Invalid table type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 6 column
            switch (table_type) {
                case TableEntryType::kCollectionEntry:
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.segment_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    String error_message = "Invalid table type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
        }

        ++column_id;
        {
            // Append segment capacity the 7 column
            SizeT default_row_size = table_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowViews(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<ViewDetail> views_detail;
    Status status = txn->GetViews(db_name_, views_detail);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        LOG_ERROR(status.message());
        RecoverableError(status);
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    for (auto &view_detail : views_detail) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = view_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = view_detail.view_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append base table type to the 2 column
            Value value = Value::MakeBigInt(static_cast<i64>(view_detail.column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "profile_no", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "parser", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "logical_plan", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "optimizer", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(4, varchar_type, "physical_plan", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(5, varchar_type, "pipeline_build", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(6, varchar_type, "task_build", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(7, varchar_type, "execution", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(8, varchar_type, "commit", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(9, varchar_type, "rollback", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(10, varchar_type, "total_cost", std::set<ConstraintType>()),
    };

    auto catalog = txn->GetCatalog();
    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("profiles"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type};
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    auto records = catalog->GetProfileRecords();
    for (SizeT i = 0; i < records.size(); ++i) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        // Output record no
        ValueExpression record_no_expr(Value::MakeVarchar(fmt::format("{}", i)));
        record_no_expr.AppendToChunk(output_block_ptr->column_vectors[0]);

        // Output each query phase
        i64 total_cost{};
        SizeT column_count = column_defs.size();
        for (SizeT j = 0; j < column_count - 2; ++j) {
            i64 this_time = records[i]->ElapsedAt(j);
            total_cost += this_time;

            NanoSeconds duration(this_time);
            ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(duration)));
            phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors[j + 1]);
        }

        // Output total query duration
        NanoSeconds total_duration(total_cost);
        ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(total_duration)));
        phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors.back());

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

/**
 * @brief Execute Show table details statement (i.e. show t1)
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "constraint", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "default", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Views"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
        varchar_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    SizeT column_count = table_entry->ColumnCount();
    for (SizeT input_column_id = 0; input_column_id < column_count; ++input_column_id) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        const ColumnDef *column = table_entry->GetColumnDefByID(input_column_id);

        SizeT output_column_idx = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column type to the second column, if the column type is embedded type, append the embedded type
            String column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column constraint to the third column
            String column_constraint;
            for (auto &constraint : column->constraints_) {
                column_constraint += " " + ConstrainTypeToString(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column default value to the fourth column
            String column_default = column->default_expr_->ToString();
            Value value = Value::MakeVarchar(column_default);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowSegments(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    for (auto &[_, segment_entry] : table_entry->segment_map()) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        SizeT column_id = 0;
        {
            Value value = Value::MakeBigInt(segment_entry->segment_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(SegmentEntry::SegmentStatusToString(segment_entry->status()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const auto &seg_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*segment_entry->segment_dir()));
            Value value = Value::MakeVarchar(seg_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowSegmentDetail(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    Vector<SharedPtr<DataType>> column_types{bigint_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             bigint_type,
                                             bigint_type,
                                             bigint_type,
                                             bigint_type,
                                             bigint_type,
                                             bigint_type};
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    if (auto segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts); segment_entry) {

        SizeT column_id = 0;
        {
            Value value = Value::MakeBigInt(segment_entry->segment_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(SegmentEntry::SegmentStatusToString(segment_entry->status()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*segment_entry->segment_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const auto &seg_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*segment_entry->segment_dir()));
            Value value = Value::MakeVarchar(seg_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->block_entries().size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->row_capacity());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->row_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->actual_row_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->Room());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(segment_entry->column_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

    } else {
        Status status = Status::SegmentNotExist(*segment_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBlocks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        bigint_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    auto segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts);
    if (!segment_entry) {
        Status status = Status::SegmentNotExist(*segment_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }
    auto block_entry_iter = BlockEntryIter(segment_entry.get());
    for (auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        SizeT column_id = 0;
        {
            Value value = Value::MakeBigInt(block_entry->block_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const auto &blk_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*block_entry->base_dir()));
            Value value = Value::MakeVarchar(blk_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(block_entry->row_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowBlockDetail(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{bigint_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};
    output_block_ptr->Init(column_types);

    auto segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts);
    if (!segment_entry) {
        Status status = Status::SegmentNotExist(*segment_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto block_entry = segment_entry->GetBlockEntryByID(*block_id_);
    if (!block_entry) {
        Status status = Status::BlockNotExist(*block_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    SizeT column_id = 0;
    {
        Value value = Value::MakeBigInt(block_entry->block_id());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeVarchar(*block_entry->base_dir());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        const auto &blk_size = Utility::FormatByteSize(LocalFileSystem::GetFolderSizeByPath(*block_entry->base_dir()));
        Value value = Value::MakeVarchar(blk_size);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeBigInt(block_entry->row_capacity());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeBigInt(block_entry->row_count());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeBigInt(block_entry->checkpoint_row_count());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeBigInt(block_entry->columns().size());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    ++column_id;
    {
        Value value = Value::MakeBigInt(block_entry->checkpoint_ts());
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBlockColumn(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    SizeT column_count = table_entry->ColumnCount();
    SizeT table_column_id = *column_id_;

    if (table_column_id >= column_count) {
        Status status = Status::ColumnNotExist(fmt::format("index {}", table_column_id));
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts);
    if (!segment_entry) {
        Status status = Status::SegmentNotExist(*segment_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto block_entry = segment_entry->GetBlockEntryByID(*block_id_);
    if (!block_entry) {
        Status status = Status::BlockNotExist(*block_id_);
        LOG_ERROR(status.message());
        RecoverableError(status);
        return;
    }

    auto column_block_entry = block_entry->GetColumnBlockEntry(table_column_id);
    if (!column_block_entry) {
        String error_message = fmt::format("Attempt to get column {} from block {}", table_column_id, *block_id_);
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };
    output_block_ptr->Init(column_types);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const ColumnDef *column_def = table_entry->GetColumnDefByID(table_column_id);
            Value value = Value::MakeVarchar(column_def->name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_column_id));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("data_type");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(column_block_entry->column_type()->ToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(column_block_entry->FilePath());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    for (u32 layer_n = 0; layer_n < 2; ++layer_n) {
        SizeT outline_count = column_block_entry->OutlineBufferCount(layer_n);
        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar(fmt::format("extra_file_group_{}_count", layer_n));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(std::to_string(outline_count));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar(fmt::format("extra_file_group_{}_name", layer_n));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                String outline_storage;
                for (SizeT idx = 0; idx < outline_count; ++idx) {
                    outline_storage += *(column_block_entry->OutlineFilename(layer_n, idx));
                    outline_storage += ";";
                }

                Value value = Value::MakeVarchar(outline_storage);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

// Execute show system table
void PhysicalShow::ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "config_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    };

    Config *global_config = query_context->global_config();

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("configs"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    // Config
    {
        {
            Value value = Value::MakeVarchar(VERSION_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // option value
            Value value = Value::MakeVarchar(global_config->Version());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option description
            Value value = Value::MakeVarchar("Infinity version.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        { // option name
            Value value = Value::MakeVarchar(TIME_ZONE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            i64 time_zone_bias = global_config->TimeZoneBias();
            if (time_zone_bias >= 0) {
                Value value = Value::MakeVarchar(fmt::format("{}+{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            } else {
                Value value = Value::MakeVarchar(fmt::format("{}{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Time zone information.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CPU_LIMIT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CPULimit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("CPU number used by infinity executor.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SERVER_ADDRESS_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ServerAddress());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity server listen ip address");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(POSTGRES_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PostgresPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Postgres port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(HTTP_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->HTTPPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("HTTP port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLIENT_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ClientPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Thrift RPC port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CONNECTION_POOL_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ConnectionPoolSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Connection pool capacity.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILENAME_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogFileName());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log file name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_TO_STDOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = global_config->LogToStdout() ? Value::MakeVarchar("True") : Value::MakeVarchar("False");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("If log is also output to standard output");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_MAX_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileMaxSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Max log file size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_ROTATE_COUNT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileRotateCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log files rotation limitation");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_LEVEL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(LogLevel2Str(global_config->GetLogLevel()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log level");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(DATA_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->DataDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLEANUP_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Cleanup period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(COMPACT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CompactInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Compact period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(OPTIMIZE_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->OptimizeIndexInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Optimize memory index period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(MEM_INDEX_CAPACITY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->MemIndexCapacity()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Real-time index building row capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(BUFFER_MANAGER_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->BufferManagerSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Buffer manager memory size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(TEMP_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->TempDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Temporary data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->WALDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_COMPACT_THRESHOLD_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->WALCompactThreshold()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log compact triggering threshold");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(FULL_CHECKPOINT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->FullCheckpointInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Full checkpoint period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(DELTA_CHECKPOINT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->DeltaCheckpointInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Delta checkpoint period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->DeltaCheckpointThreshold()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Delta checkpoint triggering threshold");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_FLUSH_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(FlushOptionTypeToString(global_config->FlushMethodAtCommit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log flush method");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(RESOURCE_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ResourcePath());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity resource directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    auto [table_entry, table_status] = txn->GetTableByName(db_name_, object_name_);
    if (!table_status.ok()) {
        show_operator_state->status_ = table_status;
        //        Error<UnrecoverableException>(table_status.message());
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    Vector<SharedPtr<ColumnDef>> column_defs = {MakeShared<ColumnDef>(0, varchar_type, "index_name", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(1, varchar_type, "method_type", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(2, bigint_type, "column_id", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(3, varchar_type, "column_name", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(4, varchar_type, "path", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(5, varchar_type, "index_segment", std::set<ConstraintType>()),
                                                MakeShared<ColumnDef>(6, varchar_type, "other_parameters", std::set<ConstraintType>())};

    auto table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Views"), column_defs);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, bigint_type, varchar_type, varchar_type, varchar_type, varchar_type};
    SizeT row_count = 0;
    output_block_ptr->Init(column_types);

    {
        auto map_guard = table_entry->IndexMetaMap();
        for (const auto &[index_name, index_meta] : *map_guard) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(column_types);
            }

            auto [table_index_entry, status] = index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
            if (!status.ok()) {
                // Index isn't found.
                continue;
            }

            const IndexBase *index_base = table_index_entry->index_base();
            String index_column_name = index_base->column_name();
            u64 index_column_id = table_entry->GetColumnIdByName(index_column_name);
            SizeT column_id = 0;
            {
                // Append index name to the first column
                Value value = Value::MakeVarchar(index_name);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index method type to the second column
                Value value = Value::MakeVarchar(IndexInfo::IndexTypeToString(index_base->index_type_));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column id
                Value value = Value::MakeBigInt(index_column_id);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column names to the third column
                String column_names;
                SizeT idx = 0;
                for (auto &column_name : index_base->column_names_) {
                    column_names += column_name;
                    if (idx < index_base->column_names_.size() - 1) {
                        column_names += ",";
                    }
                    idx++;
                }
                Value value = Value::MakeVarchar(column_names);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index path
                Value value = Value::MakeVarchar(*table_index_entry->index_dir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append Index segment
                SizeT segment_count = table_entry->segment_map().size();
                SizeT index_segment_count = table_index_entry->index_by_segment().size();
                String result_value = fmt::format("{}/{}", index_segment_count, segment_count);
                Value value = Value::MakeVarchar(result_value);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index other parameters to the fourth column
                String other_parameters = index_base->BuildOtherParamsString();
                Value value = Value::MakeVarchar(other_parameters);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            if (++row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                show_operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowViewDetail(QueryContext *query_context,
                                         const SharedPtr<Vector<SharedPtr<DataType>>> &view_column_types,
                                         const SharedPtr<Vector<String>> &view_column_names) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Views"), output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SizeT column_count = view_column_types->size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_column_names->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column
            String column_type = view_column_types->at(idx)->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowSessionVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> integer_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    SessionVariable session_var = VarUtil::GetSessionVarByName(object_name_);
    BaseSession *session_ptr = query_context->current_session();
    switch (session_var) {
        case SessionVariable::kQueryCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kTotalCommitCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->committed_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kTotalRollbackCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->rollbacked_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kConnectedTime: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kEnableProfile: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                bool_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBool(query_context->is_enable_profiling());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(object_name_);
            LOG_ERROR(operator_state->status_.message());
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowSessionVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "variable_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("configs"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    BaseSession *session_ptr = query_context->current_session();
    for (auto &session_var_pair : VarUtil::session_name_map_) {
        const String &var_name = session_var_pair.first;
        SessionVariable session_var = session_var_pair.second;

        switch (session_var) {
            case SessionVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Accomplished query count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Committed count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Rollbacked transaction count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kConnectedTime: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Connected timestamp of this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kEnableProfile: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    bool enable_profile = query_context->is_enable_profiling();
                    String enable_profile_condition = enable_profile ? "true" : "false";
                    Value value = Value::MakeVarchar(enable_profile_condition);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Enable profile");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                LOG_ERROR(operator_state->status_.message());
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowGlobalVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> integer_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    GlobalVariable global_var = VarUtil::GetGlobalVarByName(object_name_);
    switch (global_var) {
        case GlobalVariable::kQueryCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(query_context->session_manager()->total_query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSessionCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            SessionManager *session_manager = query_context->session_manager();
            u64 session_count = session_manager->GetSessionCount();
            Value value = Value::MakeBigInt(session_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBufferPoolUsage: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 memory_limit = buffer_manager->memory_limit();
            u64 memory_usage = buffer_manager->memory_usage();
            Value value = Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSchedulePolicy: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar("round robin");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kDeltaLogCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Catalog *catalog_ptr = query_context->storage()->catalog();
            Value value = Value::MakeBigInt(catalog_ptr->GetDeltaLogCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kNextTxnID: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Catalog *catalog_ptr = query_context->storage()->catalog();
            Value value = Value::MakeBigInt(catalog_ptr->next_txn_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBufferedObjectCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            Value value = Value::MakeBigInt(buffer_manager->BufferedObjectCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kUnusedBufferObjectCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            Value value = Value::MakeBigInt(buffer_manager->WaitingGCObjectCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kActiveTxnCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnManager *txn_manager = query_context->storage()->txn_manager();
            Value value = Value::MakeBigInt(txn_manager->ActiveTxnCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kCurrentTs: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnManager *txn_manager = query_context->storage()->txn_manager();
            Value value = Value::MakeBigInt(txn_manager->CurrentTS());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kTotalCommitCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnManager *txn_manager = query_context->storage()->txn_manager();
            Value value = Value::MakeBigInt(txn_manager->total_committed_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kTotalRollbackCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnManager *txn_manager = query_context->storage()->txn_manager();
            Value value = Value::MakeBigInt(txn_manager->total_rollbacked_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kActiveWALFilename: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            WalManager *wal_manager = query_context->storage()->wal_manager();
            Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kProfileRecordCapacity: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Catalog *catalog_ptr = query_context->storage()->catalog();
            Value value = Value::MakeBigInt(catalog_ptr->ProfileHistorySize());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBackgroundTaskCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            SizeT running_task_count = bg_processor->RunningTaskCount();
            Value value = Value::MakeBigInt(running_task_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kRunningBGTask: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            Value value = Value::MakeVarchar(BGTaskTypeToString(bg_processor->RunningTaskType()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kRunningCompactTask: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
            Value value = Value::MakeBigInt(compaction_processor->RunningTaskCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSystemMemoryUsage: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            i64 memory_usage = SystemInfo::MemoryUsage();
            Value value = Value::MakeBigInt(memory_usage);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(object_name_);
            LOG_ERROR(operator_state->status_.message());
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowGlobalVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "variable_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("configs"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &global_var_pair : VarUtil::global_name_map_) {
        const String &var_name = global_var_pair.first;
        GlobalVariable global_var_enum = global_var_pair.second;
        switch (global_var_enum) {
            case GlobalVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(query_context->session_manager()->total_query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Query count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSessionCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    SessionManager *session_manager = query_context->session_manager();
                    u64 session_count = session_manager->GetSessionCount();
                    Value value = Value::MakeVarchar(std::to_string(session_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Session count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBufferPoolUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    u64 memory_limit = buffer_manager->memory_limit();
                    u64 memory_usage = buffer_manager->memory_usage();
                    Value value =
                        Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffer manager usage");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSchedulePolicy: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar("round robin");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Task scheduling policy");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kDeltaLogCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Catalog *catalog_ptr = query_context->storage()->catalog();
                    Value value = Value::MakeVarchar(std::to_string(catalog_ptr->GetDeltaLogCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Catalog delta log count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kNextTxnID: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Catalog *catalog_ptr = query_context->storage()->catalog();
                    Value value = Value::MakeVarchar(std::to_string(catalog_ptr->next_txn_id()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Next transaction id of system");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBufferedObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    Value value = Value::MakeVarchar(std::to_string(buffer_manager->BufferedObjectCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffered object count in buffer manager");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kUnusedBufferObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    Value value = Value::MakeVarchar(std::to_string(buffer_manager->WaitingGCObjectCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Unused object in buffer manager waiting for garbage collection");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kActiveTxnCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    TxnManager *txn_manager = query_context->storage()->txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(txn_manager->ActiveTxnCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Active transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kCurrentTs: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    TxnManager *txn_manager = query_context->storage()->txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(txn_manager->CurrentTS()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current timestamp");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    TxnManager *txn_manager = query_context->storage()->txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(txn_manager->total_committed_txn_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global committed transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    TxnManager *txn_manager = query_context->storage()->txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(txn_manager->total_rollbacked_txn_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global rollbacked transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kActiveWALFilename: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    WalManager *wal_manager = query_context->storage()->wal_manager();
                    Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Write ahead log filename");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kProfileRecordCapacity: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Catalog *catalog_ptr = query_context->storage()->catalog();
                    Value value = Value::MakeVarchar(std::to_string(catalog_ptr->ProfileHistorySize()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Profile record history capacity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBackgroundTaskCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    SizeT running_task_count = bg_processor->RunningTaskCount();
                    Value value = Value::MakeVarchar(std::to_string(running_task_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Background tasks count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kRunningBGTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    Value value = Value::MakeVarchar(BGTaskTypeToString(bg_processor->RunningTaskType()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kRunningCompactTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
                    Value value = Value::MakeVarchar(std::to_string(compaction_processor->RunningTaskCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSystemMemoryUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    i64 memory_usage = SystemInfo::MemoryUsage();
                    Value value = Value::MakeVarchar(std::to_string(memory_usage));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Infinity system memory usage.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                LOG_ERROR(operator_state->status_.message());
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowConfig(QueryContext *query_context, ShowOperatorState *operator_state) {
    Config *global_config = query_context->global_config();
    auto [base_option, status] = global_config->GetConfigByName(object_name_);
    if (!status.ok()) {
        operator_state->status_ = status;
        return;
    }
    if (object_name_ == "time_zone_bias") {
        operator_state->status_ = Status::InvalidConfig(fmt::format("Option: {} doesn't exist.", object_name_));
        return;
    }

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    auto double_type = MakeShared<DataType>(LogicalType::kDouble);
    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);

    switch (base_option->data_type_) {
        case BaseOptionDataType::kInteger: {
            Vector<SharedPtr<DataType>> output_column_types{
                bigint_type,
            };
            output_block_ptr->Init(output_column_types);

            IntegerOption *integer_option = static_cast<IntegerOption *>(base_option);
            Value value = Value::MakeBigInt(integer_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kFloat: {
            Vector<SharedPtr<DataType>> output_column_types{
                double_type,
            };
            output_block_ptr->Init(output_column_types);

            FloatOption *float_option = static_cast<FloatOption *>(base_option);
            Value value = Value::MakeDouble(float_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kString: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            StringOption *string_option = static_cast<StringOption *>(base_option);
            String value_str;
            if (object_name_ == "time_zone") {
                auto [time_zone_bias, _] = global_config->GetConfigByName("time_zone_bias");

                IntegerOption *time_zone_bias_int = static_cast<IntegerOption *>(time_zone_bias);
                if (time_zone_bias_int->value_ >= 0) {
                    value_str = fmt::format("{}+{}", string_option->value_, time_zone_bias_int->value_);
                } else {
                    value_str = fmt::format("{}-{}", string_option->value_, time_zone_bias_int->value_);
                }
            } else {
                value_str = string_option->value_;
            }
            Value value = Value::MakeVarchar(value_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kBoolean: {
            Vector<SharedPtr<DataType>> output_column_types{
                bool_type,
            };
            output_block_ptr->Init(output_column_types);

            BooleanOption *boolean_option = static_cast<BooleanOption *>(base_option);
            Value value = Value::MakeBool(boolean_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kLogLevel: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            LogLevelOption *loglevel_option = static_cast<LogLevelOption *>(base_option);
            Value value = Value::MakeVarchar(LogLevel2Str(loglevel_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kFlush: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            FlushOption *flush_option = static_cast<FlushOption *>(base_option);
            Value value = Value::MakeVarchar(FlushOptionTypeToString(flush_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            String error_message = "Invalid option data type.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBuffer(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "path", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "status", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, bigint_type, "size", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "buffered_type", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(4, varchar_type, "type", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_buffer"), column_defs);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    SizeT row_count = 0;

    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    Vector<BufferObjectInfo> buffer_object_info_array = buffer_manager->GetBufferObjectsInfo();
    for(const auto& buffer_object_info: buffer_object_info_array) {

        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // path
            Value value = Value::MakeVarchar(buffer_object_info.object_path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // status
            Value value = Value::MakeVarchar(BufferStatusToString(buffer_object_info.buffered_status_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // size
            i64 buffer_object_size = static_cast<i64>(buffer_object_info.object_size_);
            Value value = Value::MakeBigInt(buffer_object_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
        {
            // buffered type
            Value value = Value::MakeVarchar(BufferTypeToString(buffer_object_info.buffered_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }
        {
            // type
            Value value = Value::MakeVarchar(FileWorkerType2Str(buffer_object_info.file_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++ row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }

    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return ;
}

} // namespace infinity
