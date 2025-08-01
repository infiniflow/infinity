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

#include <sstream>
#include <vector>
#include <memory>

module infinity_core:admin_executor.impl;

import :admin_executor;
import :query_context;
import :query_result;
import :stl;
import :status;
import :log_file;
import :third_party;
import :logger;
import data_type;
import logical_type;
import column_def;
import :table_def;
import :data_table;
import :data_block;
import :value;
import :value_expression;
import :logical_node;
import :virtual_store;
import :wal_entry;
import :buffer_manager;
import :new_catalog;
import :memory_indexer;
import :config;
import :default_values;
import :infinity_context;
import :options;
import :cluster_manager;
import :utility;
import :peer_task;
import :infinity_exception;
import :node_info;
import :persistence_manager;
import admin_statement;

namespace infinity {

QueryResult AdminExecutor::Execute(QueryContext *query_context, const AdminStatement *admin_statement) {

    switch (admin_statement->admin_type_) {

        case AdminStmtType::kListLogFiles: {
            return ListLogFiles(query_context, admin_statement);
        }
        case AdminStmtType::kShowLogFile: {
            return ShowLogFile(query_context, admin_statement);
        }
        case AdminStmtType::kListLogIndexes: {
            return ListLogIndexes(query_context, admin_statement);
        }
        case AdminStmtType::kShowLogIndex: {
            return ShowLogIndex(query_context, admin_statement);
        }
        case AdminStmtType::kListDatabases: {
            return ListDatabases(query_context, admin_statement);
        }
        case AdminStmtType::kShowDatabase: {
            return ShowDatabase(query_context, admin_statement);
        }
        case AdminStmtType::kListTables: {
            return ListTables(query_context, admin_statement);
        }
        case AdminStmtType::kShowTable: {
            return ShowTable(query_context, admin_statement);
        }
        case AdminStmtType::kListSegments: {
            return ListSegments(query_context, admin_statement);
        }
        case AdminStmtType::kShowSegment: {
            return ShowSegment(query_context, admin_statement);
        }
        case AdminStmtType::kListBlocks: {
            return ListBlocks(query_context, admin_statement);
        }
        case AdminStmtType::kShowBlock: {
            return ShowBlock(query_context, admin_statement);
        }
        case AdminStmtType::kListColumns: {
            return ListColumns(query_context, admin_statement);
        }
        case AdminStmtType::kShowColumn: {
            return ShowColumn(query_context, admin_statement);
        }
        case AdminStmtType::kListIndexes: {
            return ListIndexes(query_context, admin_statement);
        }
        case AdminStmtType::kShowIndex: {
            return ShowIndex(query_context, admin_statement);
        }
        case AdminStmtType::kListIndexSegments: {
            return ListIndexSegments(query_context, admin_statement);
        }
        case AdminStmtType::kShowIndexSegment: {
            return ShowIndexSegment(query_context, admin_statement);
        }
        case AdminStmtType::kListConfigs: {
            return ListConfigs(query_context, admin_statement);
        }
        case AdminStmtType::kListVariables: {
            return ListVariables(query_context, admin_statement);
        }
        case AdminStmtType::kShowVariable: {
            return ShowVariable(query_context, admin_statement);
        }
        case AdminStmtType::kListNodes: {
            return ListNodes(query_context, admin_statement);
        }
        case AdminStmtType::kShowNode: {
            return ShowNode(query_context, admin_statement);
        }
        case AdminStmtType::kShowCurrentNode: {
            return ShowCurrentNode(query_context, admin_statement);
        }
        case AdminStmtType::kRemoveNode: {
            return RemoveNode(query_context, admin_statement);
        }
        case AdminStmtType::kSetRole: {
            return SetRole(query_context, admin_statement);
        }
        case AdminStmtType::kCreateSnapshot: {
            return CreateSnapshot(query_context, admin_statement);
        }
        case AdminStmtType::kListSnapshots: {
            return ListSnapshots(query_context, admin_statement);
        }
        case AdminStmtType::kShowSnapshot: {
            return ShowSnapshot(query_context, admin_statement);
        }
        case AdminStmtType::kDeleteSnapshot: {
            return DeleteSnapshot(query_context, admin_statement);
        }
        case AdminStmtType::kExportSnapshot: {
            return ExportSnapshot(query_context, admin_statement);
        }
        case AdminStmtType::kRecoverFromSnapshot: {
            return RecoverFromSnapshot(query_context, admin_statement);
        }
        case AdminStmtType::kInvalid: {
            QueryResult query_result;
            query_result.result_table_ = nullptr;
            query_result.status_ = Status::InvalidCommand(fmt::format("Invalid admin command"));
            return query_result;
        }
    }
}

QueryResult AdminExecutor::ListLogFiles(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "file_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "type", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_logs"), nullptr, column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    SizeT row_count = 0;

    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    {
        // index
        Value value = Value::MakeBigInt(row_count);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    }

    {
        // file_name
        Value value = Value::MakeVarchar(temp_wal_info->path_);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    }

    {
        // type
        Value value = Value::MakeVarchar("mutable");
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    }

    ++row_count;

    for (const auto &wal_info : wal_infos) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(row_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // file_name
            Value value = Value::MakeVarchar(wal_info.path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // type
            Value value = Value::MakeVarchar("immutable");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        ++row_count;
        if (row_count % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowLogFile(QueryContext *query_context, const AdminStatement *admin_statement) {

    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_log"), nullptr, column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    i64 file_index = admin_statement->log_file_index_.value();
    String file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        file_path = wal_infos[file_index - 1].path_;
    }

    SizeT file_size = VirtualStore::GetFileSize(file_path);
    UniquePtr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    Vector<SharedPtr<WalEntry>> wal_entries = wal_iterator->GetAllEntries();
    bool is_wal_good = wal_iterator->IsGood();
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(file_index));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(file_path);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("file_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(file_size));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("status");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(is_wal_good ? "good" : "bad");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("entry_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(wal_entries.size()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListLogIndexes(QueryContext *query_context, const AdminStatement *admin_statement) {
    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    i64 file_index = admin_statement->log_file_index_.value();
    String file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        file_path = wal_infos[file_index - 1].path_;
    }

    UniquePtr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    Vector<SharedPtr<WalEntry>> wal_entries = wal_iterator->GetAllEntries();

    QueryResult query_result;
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, bigint_type, "size", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, bigint_type, "txn_id", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(3, bigint_type, "commit_ts", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(4, bigint_type, "command_count", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(5, varchar_type, "commands", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_logs"), nullptr, column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        bigint_type,
        bigint_type,
        bigint_type,
        bigint_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    SizeT row_count = 0;
    for (const auto &wal_entry : wal_entries) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(row_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // size
            Value value = Value::MakeBigInt(wal_entry->size_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // transaction_id
            Value value = Value::MakeBigInt(wal_entry->txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        {
            // commit_ts
            Value value = Value::MakeBigInt(wal_entry->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }

        {
            // commit_ts
            Value value = Value::MakeBigInt(wal_entry->cmds_.size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        {
            // Commands
            Value value = Value::MakeVarchar(wal_entry->CompactInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
        }

        ++row_count;
        if (row_count % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowLogIndex(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "type", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "command", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def =
        TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_log_entry_command"), nullptr, column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    SizeT row_count = 0;

    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    i64 file_index = admin_statement->log_file_index_.value();
    String file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        file_path = wal_infos[file_index - 1].path_;
    }

    UniquePtr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    i64 log_index_in_file = admin_statement->log_index_in_file_.value();
    SharedPtr<WalEntry> wal_entry = wal_iterator->GetEntryByIndex(log_index_in_file);
    if (wal_entry == nullptr) {
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::NoWALEntryFound(file_path, log_index_in_file);
        return query_result;
    }

    for (const auto &wal_cmd : wal_entry->cmds_) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(row_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // type
            Value value = Value::MakeVarchar(WalCmd::WalCommandTypeToString(wal_cmd->GetType()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // content
            Value value = Value::MakeVarchar(wal_cmd->CompactInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        ++row_count;
        if (row_count % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListDatabases(QueryContext *query_context, const AdminStatement *admin_statement) {
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_catalog"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    auto [_, meta_ptrs, meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //    for (auto &meta_ptr : meta_ptrs) {
    //        DBMeta *db_meta = static_cast<DBMeta *>(meta_ptr);
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // database name
    //            Value value = Value::MakeVarchar(*db_meta->db_name());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowDatabase(QueryContext *query_context, const AdminStatement *admin_statement) {

    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, bool_type, "deleted", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, varchar_type, "encode", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_catalog"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{bigint_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bool_type,
    //    varchar_type};
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    auto [_, meta_ptrs, meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // database name
    //            Value value = Value::MakeVarchar(*db_entry->db_name_ptr());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // dir
    //            String dir_str = db_entry->db_entry_dir() == nullptr ? "" : *db_entry->db_entry_dir();
    //            Value value = Value::MakeVarchar(dir_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(db_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(db_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(db_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // delete
    //            Value value = Value::MakeBool(db_entry->deleted_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(db_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListTables(QueryContext *query_context, const AdminStatement *admin_statement) {

    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "db_entry_dir", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        varchar_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //    for (auto &table_meta_ptr : table_meta_ptrs) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // table name
    //            Value value = Value::MakeVarchar(table_meta_ptr->table_name());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // db_entry dir
    //            Value value = Value::MakeVarchar(*table_meta_ptr->db_entry_dir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowTable(QueryContext *query_context, const AdminStatement *admin_statement) {

    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, bool_type, "deleted", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(8, bigint_type, "column_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(9, bigint_type, "row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(10, bigint_type, "segment_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(11, bigint_type, "unsealed_segment", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(12, bigint_type, "next_segment", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bool_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // database name
    //            Value value = Value::MakeVarchar(*table_entry->GetTableName());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // dir
    //            String dir_str = table_entry->TableEntryDir() == nullptr ? "" : *table_entry->TableEntryDir();
    //            Value value = Value::MakeVarchar(dir_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(table_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(table_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(table_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // delete
    //            Value value = Value::MakeBool(table_entry->deleted_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(table_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        {
    //            // column count
    //            Value value = Value::MakeBigInt(table_entry->ColumnCount());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[8]);
    //        }
    //
    //        {
    //            // row count
    //            Value value = Value::MakeBigInt(table_entry->row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[9]);
    //        }
    //
    //        {
    //            // segment count
    //            Value value = Value::MakeBigInt(table_entry->segment_map().size());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[10]);
    //        }
    //
    //        {
    //            // unsealed_segment
    //            Value value = Value::MakeBigInt(table_entry->unsealed_id());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[11]);
    //        }
    //
    //        {
    //            // next segment id
    //            Value value = Value::MakeBigInt(table_entry->next_segment_id());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[12]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListSegments(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "segment_id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bool_type, "deleted", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, bigint_type, "row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(8, bigint_type, "row_capacity", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(9, bigint_type, "column_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(10, bigint_type, "actual_row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(11, bigint_type, "checkpoint_row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(12, bigint_type, "min_row_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(13, bigint_type, "max_row_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(14, bigint_type, "first_delete_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(15, bigint_type, "deprecate_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(16, bigint_type, "block_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(17, varchar_type, "status", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bool_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        varchar_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    Map<SegmentID, SharedPtr<SegmentEntry>> &segment_map = current_table_entry->segment_map();
    //    for (auto &[segment_id, segment_entry] : segment_map) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // segment_id
    //            Value value = Value::MakeBigInt(segment_id);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // dir
    //            String dir_str = segment_entry->segment_dir() == nullptr ? "" : *segment_entry->segment_dir();
    //            Value value = Value::MakeVarchar(dir_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(segment_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(segment_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(segment_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // delete
    //            Value value = Value::MakeBool(segment_entry->deleted_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(segment_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // row count
    //            Value value = Value::MakeBigInt(segment_entry->row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        {
    //            // row capacity
    //            Value value = Value::MakeBigInt(segment_entry->row_capacity());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[8]);
    //        }
    //
    //        {
    //            // column count
    //            Value value = Value::MakeBigInt(segment_entry->column_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[9]);
    //        }
    //
    //        {
    //            // actual_row_count
    //            Value value = Value::MakeBigInt(segment_entry->actual_row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[10]);
    //        }
    //
    //        {
    //            // checkpoint_row_count
    //            Value value = Value::MakeBigInt(segment_entry->checkpoint_row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[11]);
    //        }
    //
    //        {
    //            // min_row_ts
    //            Value value = Value::MakeBigInt(segment_entry->min_row_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[12]);
    //        }
    //
    //        {
    //            // max_row_ts
    //            Value value = Value::MakeBigInt(segment_entry->max_row_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[13]);
    //        }
    //
    //        {
    //            // first_delete_ts
    //            Value value = Value::MakeBigInt(segment_entry->first_delete_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[14]);
    //        }
    //
    //        {
    //            // deprecate_ts
    //            Value value = Value::MakeBigInt(segment_entry->deprecate_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[15]);
    //        }
    //
    //        {
    //            // block_count
    //            Value value = Value::MakeBigInt(segment_entry->block_entries().size());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[16]);
    //        }
    //
    //        {
    //            // status
    //            Value value = Value::MakeVarchar(SegmentEntry::SegmentStatusToString(segment_entry->status()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[17]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowSegment(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListBlocks(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "block_id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bool_type, "deleted", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, bigint_type, "row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(8, bigint_type, "row_capacity", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(9, bigint_type, "checkpoint_row_count", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(10, bigint_type, "min_row_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(11, bigint_type, "max_row_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(12, bigint_type, "checkpoint_ts", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bool_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    Map<SegmentID, SharedPtr<SegmentEntry>> &segment_map = current_table_entry->segment_map();
    //
    //    SegmentID segment_id = admin_statement->segment_index_.value();
    //    auto segment_it = segment_map.find(segment_id);
    //    if (segment_it == segment_map.end()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::SegmentNotExist(segment_id);
    //        return query_result;
    //    }
    //
    //    SegmentEntry *segment_entry = segment_it->second.get();
    //    Vector<SharedPtr<BlockEntry>> &block_entries = segment_entry->block_entries();
    //
    //    row_count = 0;
    //    for (const auto &block_entry : block_entries) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // block_id
    //            Value value = Value::MakeBigInt(block_entry->block_id());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // block dir
    //            String dir_str = block_entry->block_dir() == nullptr ? "" : *block_entry->block_dir();
    //            Value value = Value::MakeVarchar(dir_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(block_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(block_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(block_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // delete
    //            Value value = Value::MakeBool(block_entry->deleted_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(block_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // row count
    //            Value value = Value::MakeBigInt(block_entry->row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        {
    //            // row capacity
    //            Value value = Value::MakeBigInt(block_entry->row_capacity());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[8]);
    //        }
    //
    //        {
    //            // checkpoint_row_count
    //            Value value = Value::MakeBigInt(block_entry->checkpoint_row_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[9]);
    //        }
    //
    //        {
    //            // min_row_ts
    //            Value value = Value::MakeBigInt(block_entry->min_row_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[10]);
    //        }
    //
    //        {
    //            // max_row_ts
    //            Value value = Value::MakeBigInt(block_entry->max_row_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[11]);
    //        }
    //
    //        {
    //            // checkpoint_ts
    //            Value value = Value::MakeBigInt(block_entry->checkpoint_ts());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[12]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowBlock(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListColumns(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "column_id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, varchar_type, "file_name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, bigint_type, "outline_count", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        varchar_type,
    //        varchar_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    Map<SegmentID, SharedPtr<SegmentEntry>> &segment_map = current_table_entry->segment_map();
    //
    //    SegmentID segment_id = admin_statement->segment_index_.value();
    //    auto segment_it = segment_map.find(segment_id);
    //    if (segment_it == segment_map.end()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::SegmentNotExist(segment_id);
    //        return query_result;
    //    }
    //
    //    SegmentEntry *segment_entry = segment_it->second.get();
    //    Vector<SharedPtr<BlockEntry>> &block_entries = segment_entry->block_entries();
    //
    //    BlockID block_id = admin_statement->block_index_.value();
    //    row_count = 0;
    //    BlockEntry *current_block_entry = nullptr;
    //    for (const auto &block_entry : block_entries) {
    //        if (row_count == block_id) {
    //            current_block_entry = block_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    for (const auto &block_column_entry : current_block_entry->columns()) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // column_id
    //            Value value = Value::MakeBigInt(block_column_entry->column_id());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(block_column_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(block_column_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(block_column_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // filename
    //            Value value = Value::MakeVarchar(*block_column_entry->filename());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(block_column_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // outline buffer count
    //            Value value = Value::MakeBigInt(block_column_entry->OutlineBufferCount());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowColumn(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "type", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, varchar_type, "constraints", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        varchar_type,
    //        varchar_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    const Vector<SharedPtr<ColumnDef>> &table_column_defs = current_table_entry->column_defs();
    //    for (const SharedPtr<ColumnDef> &column_def : table_column_defs) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // column_id
    //            Value value = Value::MakeBigInt(column_def->id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // name
    //            Value value = Value::MakeVarchar(column_def->name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // type
    //            Value value = Value::MakeVarchar(column_def->column_type_->ToString());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // constraint
    //            std::stringstream ss;
    //            for (auto &constraint : column_def->constraints_) {
    //                ss << " " << ConstrainTypeToString(constraint);
    //            }
    //
    //            Value value = Value::MakeVarchar(std::move(ss).str());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListIndexes(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "count", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [meta_map, locker] = current_table_entry->IndexMetaMap();
    //    row_count = 0;
    //    for (const auto &table_index_pair : meta_map) {
    //        TableIndexMeta *table_index_meta = table_index_pair.second.get();
    //
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // name
    //            Value value = Value::MakeVarchar(*table_index_meta->index_name());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // index entry count
    //            Value value = Value::MakeBigInt(table_index_meta->EntryCount());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowIndex(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "index_dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bool_type, "deleted", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, bigint_type, "segment_update_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(8, varchar_type, "index_info", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(9, bigint_type, "segment_count", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        bool_type,
    //        varchar_type,
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    u64 table_index_meta_index = admin_statement->index_meta_index_.value();
    //    auto [index_meta_map, locker] = current_table_entry->IndexMetaMap();
    //    if (table_index_meta_index >= index_meta_map.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_index_meta_index, index_meta_map.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableIndexMeta *table_index_meta = nullptr;
    //    for (const auto &table_index_pair : index_meta_map) {
    //        if (row_count == table_index_meta_index) {
    //            table_index_meta = table_index_pair.second.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    row_count = 0;
    //    List<SharedPtr<TableIndexEntry>> table_index_entry_list = table_index_meta->GetAllEntries();
    //    for (const SharedPtr<TableIndexEntry> &table_index_entry : table_index_entry_list) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // index
    //            Value value = Value::MakeBigInt(row_count);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // index dir
    //            Value value = Value::MakeVarchar(*table_index_entry->index_dir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(table_index_entry->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(table_index_entry->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(table_index_entry->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // delete
    //            Value value = Value::MakeBool(table_index_entry->deleted_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(table_index_entry->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // segment update timestamp
    //            Value value = Value::MakeBigInt(table_index_entry->GetFulltexSegmentUpdateTs());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        {
    //            // index info
    //            Value value = Value::MakeVarchar(table_index_entry->index_base()->ToString());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[8]);
    //        }
    //
    //        {
    //            // segment count
    //            Value value = Value::MakeBigInt(table_index_entry->index_by_segment().size());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[9]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListIndexSegments(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    QueryResult query_result;
    //
    //    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, bigint_type, "segment_id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "index_dir", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, bigint_type, "transaction", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, bigint_type, "begin_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, bigint_type, "commit_ts", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, varchar_type, "encode", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(6, bigint_type, "next_chunk_id", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(7, bigint_type, "chunk_count", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_tables"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //        bigint_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    //    if (checkpoint_entries.empty()) {
    //        return query_result;
    //    }
    //
    //    auto [catalog, status] = LoadCatalogFiles(query_context, admin_statement, checkpoint_entries);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = status;
    //        return query_result;
    //    }
    //
    //    auto [_, db_meta_ptrs, db_meta_lock] = catalog->db_meta_map_.GetAllMetaGuard();
    //
    //    u64 database_meta_index = admin_statement->database_meta_index_.value();
    //    if (database_meta_index >= db_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_meta_index, db_meta_ptrs.size());
    //        return query_result;
    //    }
    //    DBMeta *db_meta = db_meta_ptrs[database_meta_index];
    //    List<SharedPtr<DBEntry>> entry_list = db_meta->GetAllEntries();
    //
    //    u64 database_entry_index = admin_statement->database_entry_index_.value();
    //    if (database_entry_index >= entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidDatabaseIndex(database_entry_index, entry_list.size());
    //        return query_result;
    //    }
    //
    //    SizeT row_count = 0;
    //    DBEntry *current_db_entry = nullptr;
    //    for (const SharedPtr<DBEntry> &db_entry : entry_list) {
    //        if (row_count == database_entry_index) {
    //            current_db_entry = db_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    auto [table_names, table_meta_ptrs, table_meta_lock] = current_db_entry->GetAllTableMetas();
    //
    //    u64 table_meta_index = admin_statement->table_meta_index_.value();
    //    if (table_meta_index >= table_meta_ptrs.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_meta_index, table_meta_ptrs.size());
    //        return query_result;
    //    }
    //
    //    TableMeta *table_meta = table_meta_ptrs[table_meta_index];
    //    List<SharedPtr<TableEntry>> table_entry_list = table_meta->GetAllEntries();
    //
    //    u64 table_entry_index = admin_statement->table_entry_index_.value();
    //    if (table_entry_index >= table_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_entry_index, table_entry_list.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableEntry *current_table_entry = nullptr;
    //    for (const SharedPtr<TableEntry> &table_entry : table_entry_list) {
    //        if (row_count == table_entry_index) {
    //            current_table_entry = table_entry.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    u64 table_index_meta_index = admin_statement->index_meta_index_.value();
    //    auto [index_meta_map, locker] = current_table_entry->IndexMetaMap();
    //    if (table_index_meta_index >= index_meta_map.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_index_meta_index, index_meta_map.size());
    //        return query_result;
    //    }
    //
    //    row_count = 0;
    //    TableIndexMeta *table_index_meta = nullptr;
    //    for (const auto &table_index_pair : index_meta_map) {
    //        if (row_count == table_index_meta_index) {
    //            table_index_meta = table_index_pair.second.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    u64 table_index_entry_index = admin_statement->index_entry_index_.value();
    //    List<SharedPtr<TableIndexEntry>> table_index_entry_list = table_index_meta->GetAllEntries();
    //    if (table_index_entry_index >= table_index_entry_list.size()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::InvalidTableIndex(table_index_meta_index, index_meta_map.size());
    //        return query_result;
    //    }
    //    row_count = 0;
    //    TableIndexEntry *table_index_entry = nullptr;
    //    for (const SharedPtr<TableIndexEntry> &table_index_entry_elem : table_index_entry_list) {
    //        if (row_count == table_index_entry_index) {
    //            table_index_entry = table_index_entry_elem.get();
    //        }
    //        ++row_count;
    //    }
    //
    //    Map<SegmentID, SharedPtr<SegmentIndexEntry>> &segment_indexes = table_index_entry->index_by_segment();
    //    for (auto &segment_index_pair : segment_indexes) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //        SegmentIndexEntry *segment_index_ptr = segment_index_pair.second.get();
    //        {
    //            // segment_id
    //            Value value = Value::MakeBigInt(segment_index_pair.first);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // index dir
    //            Value value = Value::MakeVarchar(*segment_index_ptr->index_dir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // transaction
    //            Value value = Value::MakeBigInt(segment_index_ptr->txn_id_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // begin_ts
    //            Value value = Value::MakeBigInt(segment_index_ptr->begin_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // commit_ts
    //            Value value = Value::MakeBigInt(segment_index_ptr->commit_ts_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // encode
    //            Value value = Value::MakeVarchar(segment_index_ptr->encode());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        {
    //            // next chunk id
    //            Value value = Value::MakeBigInt(segment_index_ptr->next_chunk_id());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
    //        }
    //
    //        {
    //            // chunk index count
    //            Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
    //            SharedPtr<MemoryIndexer> memory_indexer;
    //            segment_index_ptr->GetChunkIndexEntries(chunk_index_entries, memory_indexer, query_context->GetTxn());
    //            Value value = Value::MakeBigInt(chunk_index_entries.size());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[7]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowIndexSegment(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListConfigs(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, varchar_type, "config_name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    //    };
    //
    //    Config *global_config = query_context->global_config();
    //    QueryResult query_result;
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_configs"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    // create data block for output state
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    Vector<SharedPtr<DataType>> column_types{
    //        varchar_type,
    //        varchar_type,
    //        varchar_type,
    //    };
    //
    //    output_block_ptr->Init(column_types);
    //
    //    // Config
    //    {
    //        {
    //            Value value = Value::MakeVarchar(VERSION_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // option value
    //            Value value = Value::MakeVarchar(global_config->Version());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option description
    //            Value value = Value::MakeVarchar("Infinity version.");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        { // option name
    //            Value value = Value::MakeVarchar(TIME_ZONE_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            i64 time_zone_bias = global_config->TimeZoneBias();
    //            if (time_zone_bias >= 0) {
    //                Value value = Value::MakeVarchar(fmt::format("{}+{}", global_config->TimeZone(), time_zone_bias));
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //            } else {
    //                Value value = Value::MakeVarchar(fmt::format("{}{}", global_config->TimeZone(), time_zone_bias));
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //            }
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Time zone information.");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CPU_LIMIT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->CPULimit()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("CPU number used by infinity executor.");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(RECORD_RUNNING_QUERY_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = global_config->RecordRunningQuery() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("To record running query");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(SERVER_ADDRESS_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->ServerAddress());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Infinity server listen ip address");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(POSTGRES_PORT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->PostgresPort()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Postgres port");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(HTTP_PORT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->HTTPPort()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("HTTP port");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CLIENT_PORT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->ClientPort()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Thrift RPC port");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CONNECTION_POOL_SIZE_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->ConnectionPoolSize()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Connection pool capacity.");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->PeerServerConnectionPoolSize()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Peer server connection pool capacity.");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_FILENAME_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->LogFileName());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Log file name");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->LogDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Log directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_TO_STDOUT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = global_config->LogToStdout() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("If log is also output to standard output");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_FILE_MAX_SIZE_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileMaxSize()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Max log file size");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_FILE_ROTATE_COUNT_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileRotateCount()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Log files rotation limitation");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(LOG_LEVEL_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(LogLevel2Str(global_config->GetLogLevel()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Log level");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    if (InfinityContext::instance().persistence_manager() != nullptr) {
    //        {
    //            {
    //                // option name
    //                Value value = Value::MakeVarchar(PERSISTENCE_DIR_OPTION_NAME);
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar(global_config->PersistenceDir());
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar("Virtual filesystem directory");
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //            }
    //        }
    //
    //        {
    //            {
    //                // option name
    //                Value value = Value::MakeVarchar(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME);
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar(std::to_string(global_config->PersistenceObjectSizeLimit()));
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar("Virtual file limitation");
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //            }
    //        }
    //    } else {
    //        {
    //            {
    //                // option name
    //                Value value = Value::MakeVarchar(DATA_DIR_OPTION_NAME);
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar(global_config->DataDir());
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //            }
    //            {
    //                // option name type
    //                Value value = Value::MakeVarchar("Data directory");
    //                ValueExpression value_expr(value);
    //                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //            }
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CATALOG_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->CatalogDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Catalog directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->SnapshotDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Snapshot directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Snapshots store directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CLEANUP_INTERVAL_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Cleanup period interval");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(COMPACT_INTERVAL_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->CompactInterval()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Compact period interval");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(OPTIMIZE_INTERVAL_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->OptimizeIndexInterval()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Optimize memory index period interval");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(MEM_INDEX_CAPACITY_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->MemIndexCapacity()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Real-time index building row capacity");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->SnapshotDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Snapshot storage directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(BUFFER_MANAGER_SIZE_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->BufferManagerSize()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Buffer manager memory size");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(TEMP_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->TempDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Temporary data directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(WAL_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->WALDir());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Write ahead log data directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(WAL_COMPACT_THRESHOLD_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->WALCompactThreshold()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Write ahead log compact triggering threshold");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(CHECKPOINT_INTERVAL_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(std::to_string(global_config->CheckpointInterval()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Checkpoint period interval");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(WAL_FLUSH_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(FlushOptionTypeToString(global_config->FlushMethodAtCommit()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Write ahead log flush method");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar(RESOURCE_DIR_OPTION_NAME);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar(global_config->ResourcePath());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option name type
    //            Value value = Value::MakeVarchar("Infinity resource directory");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListVariables(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    //    };
    //
    //    QueryResult query_result;
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_configs"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    // create data block for output state
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    Vector<SharedPtr<DataType>> column_types{
    //        varchar_type,
    //        varchar_type,
    //        varchar_type,
    //    };
    //
    //    output_block_ptr->Init(column_types);
    //
    //    {
    //        {
    //            // option name
    //            Value value = Value::MakeVarchar("role");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // option value
    //            Value value = Value::MakeVarchar(ToString(InfinityContext::instance().GetServerRole()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // option description
    //            Value value = Value::MakeVarchar("Current infinity server role");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowVariable(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    //    SharedPtr<DataType> integer_type = MakeShared<DataType>(LogicalType::kBigInt);
    //    //    SharedPtr<DataType> double_type = MakeShared<DataType>(LogicalType::kDouble);
    //    //    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, column_defs);
    //
    //    QueryResult query_result;
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kResult);
    //
    //    Vector<SharedPtr<DataType>> output_column_types{
    //        varchar_type,
    //    };
    //
    //    String variable_name = admin_statement->variable_name_.value();
    //    ToLower(variable_name);
    //
    //    if (variable_name == "role") {
    //        output_block_ptr->Init(output_column_types);
    //        Value value = Value::MakeVarchar(ToString(InfinityContext::instance().GetServerRole()));
    //        ValueExpression value_expr(value);
    //        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //    } else {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::NoSysVar(variable_name);
    //        return query_result;
    //    }
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListNodes(QueryContext *query_context, const AdminStatement *admin_statement) {

    //    QueryResult query_result;
    //    if (!InfinityContext::instance().IsClusterRole()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::NotSupport("'ADMIN SHOW NODES' only works in cluster mode");
    //        return query_result;
    //    }
    //
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "role", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(2, varchar_type, "status", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(3, varchar_type, "address", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(4, varchar_type, "last_update", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(5, bigint_type, "heartbeat", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("list_nodes"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, bigint_type};
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //    SizeT row_count = 0;
    //
    //    Vector<SharedPtr<NodeInfo>> server_nodes = InfinityContext::instance().cluster_manager()->ListNodes();
    //    for (const auto &server_node : server_nodes) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(column_types);
    //        }
    //
    //        {
    //            // name
    //            Value value = Value::MakeVarchar(server_node->node_name());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //
    //        {
    //            // role
    //            Value value = Value::MakeVarchar(ToString(server_node->node_role()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //
    //        {
    //            // status
    //            Value value = Value::MakeVarchar(ToString(server_node->node_status()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //
    //        {
    //            // address
    //            Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //
    //        {
    //            // last_update
    //            std::chrono::system_clock::duration time_duration = std::chrono::seconds(server_node->update_ts());
    //            std::chrono::time_point<std::chrono::system_clock> time_since_epoch =
    //            std::chrono::time_point<std::chrono::system_clock>(time_duration); std::time_t t_c =
    //            std::chrono::system_clock::to_time_t(time_since_epoch);
    //            //            std::stringstream ss;
    //            //            ss << std::put_time(std::localtime(&t_c), "%F %T"));
    //            Value value = Value::MakeVarchar(std::ctime(&t_c));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        {
    //            // heartbeat
    //            Value value = Value::MakeBigInt(server_node->heartbeat_count());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
    //        }
    //
    //        ++row_count;
    //        if (row_count % output_block_ptr->capacity() == 0) {
    //            output_block_ptr->Finalize();
    //            query_result.result_table_->Append(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowNode(QueryContext *query_context, const AdminStatement *admin_statement) {

    //    QueryResult query_result;
    //    if (!InfinityContext::instance().IsClusterRole()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = Status::NotSupport("SHOW NODE only works in cluster mode");
    //        return query_result;
    //    }
    //
    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
    //        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    //    };
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_node"), nullptr, column_defs);
    //    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        varchar_type,
    //        varchar_type,
    //    };
    //
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(column_types);
    //
    //    String node_name = admin_statement->node_name_.value();
    //    auto [status, server_node] = InfinityContext::instance().cluster_manager()->GetNodeInfoByName(node_name);
    //    if (!status.ok()) {
    //        query_result.result_table_ = nullptr;
    //        query_result.status_ = std::move(status);
    //        return query_result;
    //    }
    //
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("name");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            Value value = Value::MakeVarchar(server_node->node_name());
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }
    //
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("role");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            Value value = Value::MakeVarchar(ToString(server_node->node_role()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }
    //
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("status");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            Value value = Value::MakeVarchar(ToString(server_node->node_status()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }
    //
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("address");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }
    //
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("entry_count");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            //            const std::chrono::system_clock::duration time_since_epoch = std::chrono::seconds(server_node->last_update_ts_);
    //            const std::time_t t_c = server_node->update_ts();
    //            Value value = Value::MakeVarchar(std::ctime(&t_c));
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }
    //
    //    output_block_ptr->Finalize();
    //    query_result.result_table_->Append(std::move(output_block_ptr));
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::RemoveNode(QueryContext *query_context, const AdminStatement *admin_statement) {
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
    //
    //    String node_name = admin_statement->node_name_.value();
    //    QueryResult query_result;
    //
    //    Status status = InfinityContext::instance().cluster_manager()->RemoveNodeInfo(node_name);
    //
    //    if (status.ok()) {
    //        auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
    //        query_result.result_table_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    //    } else {
    //        query_result.status_ = status;
    //    }
    //
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowCurrentNode(QueryContext *query_context, const AdminStatement *admin_statement) {

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_current_node"), nullptr, column_defs);

    QueryResult query_result;
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    if (InfinityContext::instance().IsClusterRole()) {
        SharedPtr<NodeInfo> server_node = InfinityContext::instance().cluster_manager()->ThisNode();
        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("name");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(server_node->node_name());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("role");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_node->node_role()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("status");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_node->node_status()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("address");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("update_time");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                // const std::chrono::system_clock::duration time_since_epoch = std::chrono::seconds(server_node->last_update_ts_);
                const std::time_t t_c = server_node->update_ts();
                Value value = Value::MakeVarchar(std::ctime(&t_c));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }
    } else {
        NodeRole server_role = InfinityContext::instance().GetServerRole();
        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("role");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_role));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }

        {
            SizeT column_id = 0;
            {
                Value value = Value::MakeVarchar("status");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                bool infinity_started = InfinityContext::instance().InfinityContextStarted();
                String infinity_status("started");
                if (!infinity_started && server_role != NodeRole::kAdmin) {
                    infinity_status = "starting";
                }
                Value value = Value::MakeVarchar(infinity_status);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::SetRole(QueryContext *query_context, const AdminStatement *admin_statement) {
    // TODO: check if current role is same as the target role.
    //    Vector<SharedPtr<ColumnDef>> column_defs = {
    //        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
    //
    //    NodeRole target_node_role = admin_statement->node_role_.value();
    //    QueryResult query_result;
    //    NodeRole current_node_role = InfinityContext::instance().GetServerRole();
    //    if (current_node_role == target_node_role) {
    //        LOG_INFO(fmt::format("Infinity is already the role of {}", ToString(current_node_role)));
    //        auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
    //        query_result.result_table_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    //        query_result.status_ = Status::OK();
    //        return query_result;
    //    }
    //    Status status;
    //    switch (target_node_role) {
    //        case NodeRole::kAdmin: {
    //            status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //            LOG_INFO("Start in ADMIN mode");
    //            break;
    //        }
    //        case NodeRole::kStandalone: {
    //            status = InfinityContext::instance().ChangeServerRole(NodeRole::kStandalone);
    //            LOG_INFO("Start in STANDALONE mode");
    //            break;
    //        }
    //        case NodeRole::kLeader: {
    //            String node_name = admin_statement->node_name_.value();
    //            switch (IdentifierValidation(node_name)) {
    //                case IdentifierValidationStatus::kOk:
    //                    break;
    //                case IdentifierValidationStatus::kEmpty: {
    //                    status = Status::EmptyColumnName();
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kExceedLimit: {
    //                    status = Status::ExceedColumnNameLength(node_name.length());
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kInvalidName: {
    //                    status = Status::InvalidColumnName(node_name);
    //                    break;
    //                }
    //            }
    //            if (!status.ok()) {
    //                query_result.status_ = status;
    //                return query_result;
    //            }
    //            status = InfinityContext::instance().ChangeServerRole(NodeRole::kLeader, false, node_name);
    //            if (!status.ok()) {
    //                LOG_INFO("Fail to change to LEADER role");
    //                if (status.code() == ErrorCode::kCantSwitchRole) {
    //                    // Just don't change the role
    //                    break;
    //                }
    //
    //                Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //                if (!restore_status.ok()) {
    //                    UnrecoverableError(fmt::format("Fail to change node role to LEADER, then fail to restore to ADMIN."));
    //                }
    //            } else {
    //                LOG_INFO("Start in LEADER role");
    //            }
    //            break;
    //        }
    //        case NodeRole::kFollower: {
    //            String node_name = admin_statement->node_name_.value();
    //            switch (IdentifierValidation(node_name)) {
    //                case IdentifierValidationStatus::kOk:
    //                    break;
    //                case IdentifierValidationStatus::kEmpty: {
    //                    status = Status::EmptyColumnName();
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kExceedLimit: {
    //                    status = Status::ExceedColumnNameLength(node_name.length());
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kInvalidName: {
    //                    status = Status::InvalidColumnName(node_name);
    //                    break;
    //                }
    //            }
    //            if (!status.ok()) {
    //                query_result.status_ = status;
    //                return query_result;
    //            }
    //
    //            String leader_address = admin_statement->leader_address_.value();
    //            String leader_ip;
    //            i64 leader_port;
    //            if (!ParseIPPort(leader_address, leader_ip, leader_port)) {
    //                query_result.status_ = Status::InvalidServerAddress(leader_address);
    //                return query_result;
    //            }
    //
    //            status = InfinityContext::instance().ChangeServerRole(NodeRole::kFollower, false, node_name, leader_ip, leader_port);
    //            if (!status.ok()) {
    //                if (status.code() != ErrorCode::kCantSwitchRole) {
    //                    LOG_INFO("Fail to change to FOLLOWER role");
    //                    Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //                    if (!restore_status.ok()) {
    //                        UnrecoverableError(fmt::format("Fail to change node role to FOLLOWER, then fail to restore to ADMIN."));
    //                    }
    //                }
    //            } else {
    //                LOG_INFO("Start in FOLLOWER role");
    //            }
    //            break;
    //        }
    //        case NodeRole::kLearner: {
    //            String node_name = admin_statement->node_name_.value();
    //            switch (IdentifierValidation(node_name)) {
    //                case IdentifierValidationStatus::kOk:
    //                    break;
    //                case IdentifierValidationStatus::kEmpty: {
    //                    status = Status::EmptyColumnName();
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kExceedLimit: {
    //                    status = Status::ExceedColumnNameLength(node_name.length());
    //                    break;
    //                }
    //                case IdentifierValidationStatus::kInvalidName: {
    //                    status = Status::InvalidColumnName(node_name);
    //                    break;
    //                }
    //            }
    //            if (!status.ok()) {
    //                query_result.status_ = status;
    //                return query_result;
    //            }
    //
    //            String leader_address = admin_statement->leader_address_.value();
    //            String leader_ip;
    //            i64 leader_port;
    //            if (!ParseIPPort(leader_address, leader_ip, leader_port)) {
    //                query_result.status_ = Status::InvalidServerAddress(leader_address);
    //                return query_result;
    //            }
    //
    //            status = InfinityContext::instance().ChangeServerRole(NodeRole::kLearner, false, node_name, leader_ip, leader_port);
    //            if (!status.ok()) {
    //                if (status.code() != ErrorCode::kCantSwitchRole) {
    //                    LOG_INFO("Fail to change to LEARNER role");
    //                    Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //                    if (!restore_status.ok()) {
    //                        UnrecoverableError(fmt::format("Fail to change node role to FOLLOWER, then fail to restore to ADMIN."));
    //                    }
    //                }
    //            } else {
    //                LOG_INFO("Start in LEARNER role");
    //            }
    //            break;
    //        }
    //        default: {
    //            UnrecoverableError("Unsupported role");
    //        }
    //    }
    //
    //    if (status.ok()) {
    //        auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
    //        query_result.result_table_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    //    } else {
    //        query_result.status_ = status;
    //    }
    //
    //    return query_result;
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::CreateSnapshot(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: create snapshot"));
    return query_result;
}

QueryResult AdminExecutor::ListSnapshots(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: list snapshots"));
    return query_result;
}

QueryResult AdminExecutor::ShowSnapshot(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: show snapshot"));
    return query_result;
}

QueryResult AdminExecutor::DeleteSnapshot(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: delete snapshot"));
    return query_result;
}

QueryResult AdminExecutor::ExportSnapshot(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: export snapshot"));
    return query_result;
}

QueryResult AdminExecutor::RecoverFromSnapshot(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport(fmt::format("Not support: recover from snapshot"));
    return query_result;
}

} // namespace infinity
