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

module infinity_core:physical_check.impl;

import :physical_check;

import :stl;
import :new_txn;
import :query_context;
import :third_party;

import :profiler;
import :operator_state;
import :data_block;

import :infinity_exception;
import :value_expression;

import :defer_op;
import :new_catalog;
import :status;
import :value;
import logical_type;
import :meta_tree;
import :db_meeta;
import check_statement;
import data_type;

namespace infinity {

void PhysicalCheck::Init(QueryContext *query_context) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (check_type_) {
        case CheckStmtType::kSystem: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("mismatch_data");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case CheckStmtType::kTable: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("mismatch_data");
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
        case CheckStmtType::kTable: {
            ExecuteCheckTable(query_context, check_operator_state);
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

    auto data_mismatch_entries = meta_tree_ptr->CheckMetaDataMapping(CheckStmtType::kSystem, None);

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type};

    output_block_ptr->Init(column_types);

    SizeT row_count = 0;
    for (const auto &data_mismatch_entry : data_mismatch_entries) {
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            check_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
            row_count = 0;
        }
        Value value = Value::MakeVarchar(data_mismatch_entry);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        row_count++;
    }
    output_block_ptr->Finalize();
    check_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalCheck::ExecuteCheckTable(QueryContext *query_context, CheckOperatorState *check_operator_state) {
    // Define output database detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    auto *new_catalog = query_context->storage()->new_catalog();
    auto meta_tree_ptr = new_catalog->MakeMetaTree();

    auto *new_txn = query_context->GetNewTxn();
    auto schema_name = schema_name_.value();
    auto table_name = table_name_.value();
    // if (!table_name.empty() && schema_name.empty()) {
    //     schema_name = "default_db";
    // }

    Optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = new_txn->GetDBMeta(schema_name, db_meta, db_create_ts);

    if (!status.ok()) {
        output_names_->reserve(1);
        output_types_->reserve(1);
        output_names_->clear();
        output_types_->clear();
        output_names_->emplace_back("check_fail_message");
        output_types_->emplace_back(varchar_type);

        UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
        Vector<SharedPtr<DataType>> column_types{varchar_type};

        output_block_ptr->Init(column_types);
        String message;
        if (status.code() == ErrorCode::kDBNotExist) {
            message = fmt::format("Database: {} is not exists", schema_name);
        } else {
            message = fmt::format("{}", status.message());
        }
        Value value = Value::MakeVarchar(message);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        output_block_ptr->Finalize();
        check_operator_state->output_.emplace_back(std::move(output_block_ptr));
        return;
    }

    auto db_id_str = db_meta->db_id_str();
    String table_id_str;
    String table_key;
    TxnTimeStamp create_table_ts;
    status = db_meta->GetTableID(table_name, table_key, table_id_str, create_table_ts);

    if (!status.ok()) {
        output_names_->reserve(1);
        output_types_->reserve(1);
        output_names_->clear();
        output_types_->clear();
        output_names_->emplace_back("check_fail_message");
        output_types_->emplace_back(varchar_type);

        UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
        Vector<SharedPtr<DataType>> column_types{varchar_type};

        output_block_ptr->Init(column_types);
        String message;
        if (status.code() == ErrorCode::kTableNotExist) {
            message = fmt::format("Table: {}.{} is not exists", schema_name, table_name);
        } else {
            message = fmt::format("{}", status.message());
        }
        Value value = Value::MakeVarchar(message);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        output_block_ptr->Finalize();
        check_operator_state->output_.emplace_back(std::move(output_block_ptr));
        return;
    }

    auto db_table_str = fmt::format("db_{}/tbl_{}", db_id_str, table_id_str);
    auto data_mismatch_entries = meta_tree_ptr->CheckMetaDataMapping(CheckStmtType::kTable, db_table_str);

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(column_types);

    {
        for (const auto &data_mismatch_entry : data_mismatch_entries) {
            Value value = Value::MakeVarchar(data_mismatch_entry);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
    }

    output_block_ptr->Finalize();
    check_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

} // namespace infinity
