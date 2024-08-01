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

#include <vector>

module admin_executor;

import query_context;
import query_result;
import stl;
import status;
import log_file;
import third_party;
import logger;
import data_type;
import logical_type;
import column_def;
import table_def;
import data_table;
import data_block;
import value;
import value_expression;
import logical_node;
import local_file_system;
import wal_entry;

namespace infinity {

QueryResult AdminExecutor::Execute(QueryContext* query_context, const AdminStatement* admin_statement) {

    switch(admin_statement->admin_type_) {

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
        case AdminStmtType::kListCatalogs: {
            return ListCatalogs(query_context, admin_statement);
        }
        case AdminStmtType::kShowCatalog: {
            return ShowCatalog(query_context, admin_statement);
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
        case AdminStmtType::kInvalid: {
            QueryResult query_result;
            query_result.result_table_ = nullptr;
            query_result.status_ = Status::InvalidCommand(fmt::format("Invalid admin command"));
            return query_result;
        }
    }
}

QueryResult AdminExecutor::ListLogFiles(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "file_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "type", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_logs"), column_defs);
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

    ++ row_count;

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

        ++ row_count;
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

QueryResult AdminExecutor::ShowLogFile(QueryContext* query_context, const AdminStatement* admin_statement) {

    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_log"), column_defs);
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
    if(file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        file_path = wal_infos[file_index - 1].path_;
    }

    LocalFileSystem fs;
    SizeT file_size = fs.GetFileSizeByPath(file_path);
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
            Value value = Value::MakeVarchar(is_wal_good ? "good": "bad");
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

QueryResult AdminExecutor::ListLogIndexes(QueryContext* query_context, const AdminStatement* admin_statement) {
    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    i64 file_index = admin_statement->log_file_index_.value();
    String file_path;
    if(file_index == 0) {
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

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_logs"), column_defs);
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

        ++ row_count;
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

QueryResult AdminExecutor::ShowLogIndex(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "type", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "command", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_log_entry_command"), column_defs);
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
    if(file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        file_path = wal_infos[file_index - 1].path_;
    }

    UniquePtr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    i64 log_index_in_file = admin_statement->log_index_in_file_.value();
    SharedPtr<WalEntry> wal_entry = wal_iterator->GetEntryByIndex(log_index_in_file);
    if(wal_entry == nullptr) {
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

        ++ row_count;
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

Vector<SharedPtr<WalEntry>> AdminExecutor::GetAllCheckpointEntries(QueryContext* query_context, const AdminStatement* admin_statement) {
    String wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);
    if (!wal_infos.empty()) {
        std::sort(wal_infos.begin(), wal_infos.end(), [](const WalFileInfo &a, const WalFileInfo &b) {
            return a.max_commit_ts_ > b.max_commit_ts_;
        });
    }
    Vector<String> wal_list;
    if (temp_wal_info.has_value()) {
        wal_list.push_back(temp_wal_info->path_);
    }
    for (const auto &wal_info : wal_infos) {
        wal_list.push_back(wal_info.path_);
    }

    Vector<SharedPtr<WalEntry>> checkpoint_entries;
    if (wal_list.empty()) {
        return checkpoint_entries;
    }

    WalListIterator iterator(wal_list);
    while (iterator.HasNext()) {
        auto wal_entry_ptr = iterator.Next();
        for (auto &entry_cmd : wal_entry_ptr->cmds_) {
            if (entry_cmd->GetType() == WalCommandType::CHECKPOINT) {
                checkpoint_entries.push_back(wal_entry_ptr);
                break;
            }
        }
    }

    return checkpoint_entries;
}

QueryResult AdminExecutor::ListCatalogs(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;

    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, bool_type, "full_checkpoint", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(2, bigint_type, "max_commit_ts", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "path", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(4, varchar_type, "file", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_log_entry_command"), column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    if(checkpoint_entries.empty()) {
        return query_result;
    }

    // Prepare the output table
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        bool_type,
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    SizeT row_count = 0;

    for (const auto &checkpoint_entry : checkpoint_entries) {
        WalCmdCheckpoint* checkpoint_cmd = static_cast<WalCmdCheckpoint*>(checkpoint_entry->cmds_[0].get());

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
            // full_checkpoint
            Value value = Value::MakeBool(checkpoint_cmd->is_full_checkpoint_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // max_commit_ts
            Value value = Value::MakeBigInt(checkpoint_cmd->max_commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        {
            // catalog_path_
            Value value = Value::MakeVarchar(checkpoint_cmd->catalog_path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }

        {
            // catalog_name_
            Value value = Value::MakeVarchar(checkpoint_cmd->catalog_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++ row_count;
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

QueryResult AdminExecutor::ShowCatalog(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;

    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("show_catalog"), column_defs);
    query_result.result_table_ = MakeShared<DataTable>(table_def, TableType::kDataTable);

    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    Vector<SharedPtr<WalEntry>> checkpoint_entries = GetAllCheckpointEntries(query_context, admin_statement);
    if(checkpoint_entries.empty()) {
        return query_result;
    }

    i64 catalog_file_index = admin_statement->catalog_file_index_.value();
    WalCmdCheckpoint* checkpoint_cmd = static_cast<WalCmdCheckpoint*>(checkpoint_entries[catalog_file_index]->cmds_[0].get());

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("full_checkpoint");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(checkpoint_cmd->is_full_checkpoint_ ? "true" : "false");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("max_commit_ts");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(checkpoint_cmd->max_commit_ts_));
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
            Value value = Value::MakeVarchar(checkpoint_cmd->catalog_path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(checkpoint_cmd->catalog_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            LocalFileSystem fs;
            String file_path = fmt::format("{}/{}", checkpoint_cmd->catalog_path_, checkpoint_cmd->catalog_name_);
            SizeT file_size = fs.GetFileSizeByPath(file_path);
            Value value = Value::MakeVarchar(std::to_string(file_size));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListDatabases(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowDatabase(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListTables(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowTable(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListSegments(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowSegment(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListBlocks(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowBlock(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListColumns(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowColumn(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListIndexes(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowIndex(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ListIndexSegments(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

QueryResult AdminExecutor::ShowIndexSegment(QueryContext* query_context, const AdminStatement* admin_statement) {
    QueryResult query_result;
    query_result.result_table_ = nullptr;
    query_result.status_ = Status::NotSupport("Not support to handle admin statement");
    return query_result;
}

}