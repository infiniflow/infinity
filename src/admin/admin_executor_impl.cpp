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

module infinity_core:admin_executor.impl;

import :admin_executor;
import :query_context;
import :query_result;
import :status;
import :log_file;
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
import :kv_store;
import :kv_code;
import :rocksdb_merge_operator;

import std;
import third_party;

import data_type;
import logical_type;
import column_def;
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

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "file_name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "type", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_logs"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    size_t row_count = 0;

    std::string wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    if (!temp_wal_info.has_value()) {
        query_result.status_ = Status::NotFound("No active WAL");
        query_result.result_table_ = nullptr;
        return query_result;
    }

    {
        // index
        Value value = Value::MakeBigInt(row_count);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
    }

    {
        // file_name
        Value value = Value::MakeVarchar(temp_wal_info->path_);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
    }

    {
        // type
        Value value = Value::MakeVarchar("mutable");
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
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
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // file_name
            Value value = Value::MakeVarchar(wal_info.path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // type
            Value value = Value::MakeVarchar("immutable");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
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

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_log"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    std::string wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    if (!temp_wal_info.has_value()) {
        query_result.status_ = Status::NotFound("No active WAL");
        query_result.result_table_ = nullptr;
        return query_result;
    }

    i64 file_index = admin_statement->log_file_index_.value();
    std::string file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        auto wal_array_size = wal_infos.size();
        if (static_cast<size_t>(file_index) > wal_array_size) {
            query_result.status_ = Status::OutofBound(fmt::format("The wal index is not valid. Max is {}", wal_array_size));
            query_result.result_table_ = nullptr;
            return query_result;
        }
        file_path = wal_infos[file_index - 1].path_;
    }

    size_t file_size = VirtualStore::GetFileSize(file_path);
    std::unique_ptr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    std::vector<std::shared_ptr<WalEntry>> wal_entries = wal_iterator->GetAllEntries();
    bool is_wal_good = wal_iterator->IsGood();
    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(file_index));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(file_path);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("file_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(file_size));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("status");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(is_wal_good ? "good" : "bad");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("entry_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(wal_entries.size()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListLogIndexes(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    std::string wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    if (!temp_wal_info.has_value()) {
        query_result.status_ = Status::NotFound("No active WAL");
        query_result.result_table_ = nullptr;
        return query_result;
    }

    i64 file_index = admin_statement->log_file_index_.value();
    std::string file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        auto wal_array_size = wal_infos.size();
        if (static_cast<size_t>(file_index) > wal_array_size) {
            query_result.status_ = Status::OutofBound(fmt::format("The wal index is not valid. Max is {}", wal_array_size));
            return query_result;
        }
        file_path = wal_infos[file_index - 1].path_;
    }

    std::unique_ptr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    std::vector<std::shared_ptr<WalEntry>> wal_entries = wal_iterator->GetAllEntries();

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, bigint_type, "size", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, bigint_type, "txn_id", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bigint_type, "commit_ts", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, bigint_type, "command_count", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(5, varchar_type, "commands", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_logs"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        bigint_type,
        bigint_type,
        bigint_type,
        bigint_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    size_t row_count = 0;
    for (const auto &wal_entry : wal_entries) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(row_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // size
            Value value = Value::MakeBigInt(wal_entry->size_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // transaction_id
            Value value = Value::MakeBigInt(wal_entry->txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // commit_ts
            Value value = Value::MakeBigInt(wal_entry->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // commit_ts
            Value value = Value::MakeBigInt(wal_entry->cmds_.size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        {
            // Commands
            Value value = Value::MakeVarchar(wal_entry->CompactInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[5]);
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

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "type", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "command", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_log_entry_command"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    size_t row_count = 0;

    std::string wal_dir = query_context->storage()->wal_manager()->wal_dir();
    auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir);

    if (!temp_wal_info.has_value()) {
        query_result.status_ = Status::NotFound("No active WAL");
        query_result.result_table_ = nullptr;
        return query_result;
    }

    i64 file_index = admin_statement->log_file_index_.value();
    std::string file_path;
    if (file_index == 0) {
        file_path = temp_wal_info->path_;
    } else {
        auto wal_array_size = wal_infos.size();
        if (static_cast<size_t>(file_index) > wal_array_size) {
            query_result.status_ = Status::OutofBound(fmt::format("The wal index is not valid. Max is {}", wal_array_size));
            query_result.result_table_ = nullptr;
            return query_result;
        }
        file_path = wal_infos[file_index - 1].path_;
    }

    std::unique_ptr<WalEntryIterator> wal_iterator = WalEntryIterator::Make(file_path, false);
    i64 log_index_in_file = admin_statement->log_index_in_file_.value();
    std::shared_ptr<WalEntry> wal_entry = wal_iterator->GetEntryByIndex(log_index_in_file);
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
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // type
            Value value = Value::MakeVarchar(WalCmd::WalCommandTypeToString(wal_cmd->GetType()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // content
            Value value = Value::MakeVarchar(wal_cmd->CompactInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
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
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_databases"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct db_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
        std::string comment_;
    };

    std::vector<db_output_obj> db_output_objs;

    std::map<std::string, std::vector<std::pair<std::string, std::string>>> db_kvs_map;

    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        db_kvs_map[db_name].emplace_back(key_str, db_id);
        catalog_db_iter->Next();
    }

    for (const auto &[db_name, db_kvs] : db_kvs_map) {
        for (const auto &[db_key, db_value] : db_kvs) { // create -> drop -> create
            TxnTimeStamp db_commit_ts = std::stoull(Partition(db_key, '|').back());
            auto drop_iter = db->NewIterator(read_options);
            auto drop_db_name_prefix = fmt::format("{}{}", KeyEncode::kDropDbHeader, db_name);
            drop_iter->Seek(drop_db_name_prefix);
            TxnTimeStamp drop_ts{};
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_db_name_prefix)) {
                auto key_str = drop_iter->key().ToString();
                drop_ts = static_cast<TxnTimeStamp>(std::stoull(Partition(key_str, '/')[1]));
                if (drop_ts >= db_commit_ts) {
                    dropped = true;
                    break;
                }
                drop_iter->Next();
            }

            // Get comment from tag
            std::string comment;
            auto comment_key = KeyEncode::CatalogDbTagKey(db_value, "comment");
            auto comment_iter = db->NewIterator(read_options);
            comment_iter->Seek(comment_key);
            if (comment_iter->Valid() && comment_iter->key().ToString() == comment_key) {
                comment = comment_iter->value().ToString();
            }

            db_output_objs.emplace_back(db_name, std::stoull(db_value), dropped, comment);
        }
    }

    // size_t row_count = 0;
    auto all_db_cnt = db_output_objs.size();
    for (size_t i = 0; i < all_db_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // database name
            Value value = Value::MakeVarchar(db_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // database dir
            Value value = Value::MakeVarchar(fmt::format("db_{}", db_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(db_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // comment
            Value value = Value::MakeVarchar(db_output_objs[i].comment_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowDatabase(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_database"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct db_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
        std::string comment_;
    };

    std::vector<db_output_obj> db_output_objs;

    std::map<std::string, std::vector<std::pair<std::string, std::string>>> db_kvs_map;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();

    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        db_kvs_map[db_name].emplace_back(key_str, db_id);
        catalog_db_iter->Next();
    }

    for (const auto &[db_name, db_kvs] : db_kvs_map) {
        if (db_name != schema_name) {
            continue;
        }

        for (const auto &[db_key, db_value] : db_kvs) { // create -> drop -> create
            TxnTimeStamp db_commit_ts = std::stoull(Partition(db_key, '|').back());
            auto drop_iter = db->NewIterator(read_options);
            auto drop_db_name_prefix = fmt::format("{}{}", KeyEncode::kDropDbHeader, db_name);
            drop_iter->Seek(drop_db_name_prefix);
            TxnTimeStamp drop_ts{};
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_db_name_prefix)) {
                auto key_str = drop_iter->key().ToString();
                drop_ts = static_cast<TxnTimeStamp>(std::stoull(Partition(key_str, '/')[1]));
                if (drop_ts >= db_commit_ts) {
                    dropped = true;
                    break;
                }
                drop_iter->Next();
            }

            // Get comment from tag
            std::string comment;
            auto comment_key = KeyEncode::CatalogDbTagKey(db_value, "comment");
            auto comment_iter = db->NewIterator(read_options);
            comment_iter->Seek(comment_key);
            if (comment_iter->Valid() && comment_iter->key().ToString() == comment_key) {
                comment = comment_iter->value().ToString();
            }

            db_output_objs.emplace_back(db_name, std::stoull(db_value), dropped, comment);
        }
    }

    // size_t row_count = 0;
    auto all_db_cnt = db_output_objs.size();
    for (size_t i = 0; i < all_db_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // database name
            Value value = Value::MakeVarchar(db_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // database dir
            Value value = Value::MakeVarchar(fmt::format("db_{}", db_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(db_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // comment
            Value value = Value::MakeVarchar(db_output_objs[i].comment_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListTables(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_tables"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct table_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<table_output_obj> table_output_objs;

    std::map<std::string, std::vector<std::pair<std::string, std::string>>> table_kvs_map;

    // First get the db_id from the schema name
    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Now list all tables in this database
    auto catalog_table_iter = db->NewIterator(read_options);
    auto catalog_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str);
    catalog_table_iter->Seek(catalog_table_prefix);

    while (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        std::string key_str = catalog_table_iter->key().ToString();
        std::string table_id = catalog_table_iter->value().ToString();
        size_t start = catalog_table_prefix.size();
        size_t end = key_str.find('|', start);
        std::string table_name = key_str.substr(start, end - start);
        table_kvs_map[table_name].emplace_back(key_str, table_id);
        catalog_table_iter->Next();
    }

    for (const auto &[table_name, table_kvs] : table_kvs_map) {
        for (const auto &[table_key, table_value] : table_kvs) {
            TxnTimeStamp table_commit_ts = std::stoull(Partition(table_key, '|').back());

            // Check if table is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_table_prefix = fmt::format("drop|tbl|{}/", db_id_str);
            drop_iter->Seek(drop_table_prefix);
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_table_prefix)) {
                auto key_str = drop_iter->key().ToString();
                auto parts = Partition(key_str, '/');
                if (parts.size() >= 4 && parts[2] == table_name) {
                    TxnTimeStamp drop_ts = static_cast<TxnTimeStamp>(std::stoull(parts[3]));
                    if (drop_ts >= table_commit_ts) {
                        dropped = true;
                        break;
                    }
                }
                drop_iter->Next();
            }

            table_output_objs.emplace_back(table_name, std::stoull(table_value), dropped);
        }
    }

    auto all_table_cnt = table_output_objs.size();
    for (size_t i = 0; i < all_table_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // table name
            Value value = Value::MakeVarchar(table_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // table dir
            Value value = Value::MakeVarchar(fmt::format("tbl_{}", table_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(table_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowTable(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_table"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct table_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<table_output_obj> table_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Now get the specific table
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    while (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        std::string key_str = catalog_table_iter->key().ToString();
        std::string table_id = catalog_table_iter->value().ToString();
        TxnTimeStamp table_commit_ts = std::stoull(Partition(key_str, '|').back());

        // Check if table is dropped
        auto drop_iter = db->NewIterator(read_options);
        auto drop_table_prefix = fmt::format("drop|tbl|{}/", db_id_str);
        drop_iter->Seek(drop_table_prefix);
        bool dropped{};
        while (drop_iter->Valid() && drop_iter->key().starts_with(drop_table_prefix)) {
            auto drop_key_str = drop_iter->key().ToString();
            auto parts = Partition(drop_key_str, '/');
            if (parts.size() >= 4 && parts[2] == table_name) {
                TxnTimeStamp drop_ts = static_cast<TxnTimeStamp>(std::stoull(parts[3]));
                if (drop_ts >= table_commit_ts) {
                    dropped = true;
                    break;
                }
            }
            drop_iter->Next();
        }

        table_output_objs.emplace_back(table_name, std::stoull(table_id), dropped);
        catalog_table_iter->Next();
    }

    auto all_table_cnt = table_output_objs.size();
    for (size_t i = 0; i < all_table_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // table name
            Value value = Value::MakeVarchar(table_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // table dir
            Value value = Value::MakeVarchar(fmt::format("tbl_{}", table_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(table_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListSegments(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_segments"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct segment_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<segment_output_obj> segment_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Now list all segments in this table
    auto catalog_segment_prefix = KeyEncode::CatalogTableSegmentKeyPrefix(db_id_str, table_id_str);
    auto catalog_segment_iter = db->NewIterator(read_options);
    catalog_segment_iter->Seek(catalog_segment_prefix);

    while (catalog_segment_iter->Valid() && catalog_segment_iter->key().starts_with(catalog_segment_prefix)) {
        std::string key_str = catalog_segment_iter->key().ToString();
        // Format: catalog|seg|{db_id}|{table_id}|{segment_id}
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 5) {
            SegmentID segment_id = static_cast<SegmentID>(std::stoull(parts[4]));

            // Check if segment is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_key = KeyEncode::DropSegmentKey(db_id_str, table_id_str, segment_id);
            drop_iter->Seek(drop_key);
            bool dropped = drop_iter->Valid();

            segment_output_objs.emplace_back(fmt::format("seg_{}", segment_id), static_cast<size_t>(segment_id), dropped);
        }
        catalog_segment_iter->Next();
    }

    auto all_seg_cnt = segment_output_objs.size();
    for (size_t i = 0; i < all_seg_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // segment name
            Value value = Value::MakeVarchar(segment_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // segment dir
            Value value = Value::MakeVarchar(fmt::format("seg_{}", segment_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(segment_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowSegment(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_segment"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct segment_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<segment_output_obj> segment_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto segment_id = admin_statement->segment_index_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get the specific segment
    auto catalog_segment_key = KeyEncode::CatalogTableSegmentKey(db_id_str, table_id_str, static_cast<SegmentID>(segment_id));
    auto catalog_segment_iter = db->NewIterator(read_options);
    catalog_segment_iter->Seek(catalog_segment_key);

    if (catalog_segment_iter->Valid() && catalog_segment_iter->key().ToString() == catalog_segment_key) {
        // Check if segment is dropped
        auto drop_iter = db->NewIterator(read_options);
        auto drop_key = KeyEncode::DropSegmentKey(db_id_str, table_id_str, static_cast<SegmentID>(segment_id));
        drop_iter->Seek(drop_key);
        bool dropped = drop_iter->Valid();

        segment_output_objs.emplace_back(fmt::format("seg_{}", segment_id), static_cast<size_t>(segment_id), dropped);
    }

    auto all_seg_cnt = segment_output_objs.size();
    for (size_t i = 0; i < all_seg_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // segment name
            Value value = Value::MakeVarchar(segment_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // segment dir
            Value value = Value::MakeVarchar(fmt::format("seg_{}", segment_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(segment_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListBlocks(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_blocks"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct block_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<block_output_obj> block_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto segment_id = admin_statement->segment_index_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Now list all blocks in this segment
    auto catalog_block_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(db_id_str, table_id_str, static_cast<SegmentID>(segment_id));
    auto catalog_block_iter = db->NewIterator(read_options);
    catalog_block_iter->Seek(catalog_block_prefix);

    while (catalog_block_iter->Valid() && catalog_block_iter->key().starts_with(catalog_block_prefix)) {
        std::string key_str = catalog_block_iter->key().ToString();
        // Format: catalog|blk|{db_id}|{table_id}|{segment_id}|{block_id}
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 6) {
            BlockID block_id = static_cast<BlockID>(std::stoull(parts[5]));

            // Check if block is dropped (blocks are not dropped individually in current implementation)
            bool dropped = false;

            block_output_objs.emplace_back(fmt::format("blk_{}", block_id), static_cast<size_t>(block_id), dropped);
        }
        catalog_block_iter->Next();
    }

    auto all_block_cnt = block_output_objs.size();
    for (size_t i = 0; i < all_block_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // block name
            Value value = Value::MakeVarchar(block_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // block dir
            Value value = Value::MakeVarchar(fmt::format("blk_{}", block_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(block_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowBlock(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_block"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct block_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<block_output_obj> block_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto segment_id = admin_statement->segment_index_.value();
    auto block_id = admin_statement->block_index_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get the specific block
    auto catalog_block_key =
        KeyEncode::CatalogTableSegmentBlockKey(db_id_str, table_id_str, static_cast<SegmentID>(segment_id), static_cast<BlockID>(block_id));
    auto catalog_block_iter = db->NewIterator(read_options);
    catalog_block_iter->Seek(catalog_block_key);

    if (catalog_block_iter->Valid() && catalog_block_iter->key().ToString() == catalog_block_key) {
        // Check if block is dropped (blocks are not dropped individually in current implementation)
        bool dropped = false;

        block_output_objs.emplace_back(fmt::format("blk_{}", block_id), static_cast<size_t>(block_id), dropped);
    }

    auto all_block_cnt = block_output_objs.size();
    for (size_t i = 0; i < all_block_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // block name
            Value value = Value::MakeVarchar(block_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // block dir
            Value value = Value::MakeVarchar(fmt::format("blk_{}", block_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(block_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListColumns(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_columns"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct column_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<column_output_obj> column_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Now list all columns in this table
    // Note: In current implementation, columns are stored in table metadata, not as separate entries
    // For simplicity, we'll use the TableColumnPrefix to scan for column metadata
    auto catalog_column_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
    auto catalog_column_iter = db->NewIterator(read_options);
    catalog_column_iter->Seek(catalog_column_prefix);

    while (catalog_column_iter->Valid() && catalog_column_iter->key().starts_with(catalog_column_prefix)) {
        std::string key_str = catalog_column_iter->key().ToString();
        // Format: tbl|col|{db_id}|{table_id}|{column_name}|{ts}
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 5 && parts[0] == "tbl" && parts[1] == "col") {
            std::string column_name = parts[4];

            // Check if column is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_col_prefix = fmt::format("drop|tbl_col|{}/{}/", db_id_str, table_id_str);
            drop_iter->Seek(drop_col_prefix);
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_col_prefix)) {
                auto drop_key_str = drop_iter->key().ToString();
                auto drop_parts = Partition(drop_key_str, '/');
                if (drop_parts.size() >= 4 && drop_parts[3] == column_name) {
                    dropped = true;
                    break;
                }
                drop_iter->Next();
            }

            column_output_objs.emplace_back(column_name, column_output_objs.size(), dropped);
        }
        catalog_column_iter->Next();
    }

    auto all_col_cnt = column_output_objs.size();
    for (size_t i = 0; i < all_col_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // column name
            Value value = Value::MakeVarchar(column_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // column dir
            Value value = Value::MakeVarchar(fmt::format("col_{}", column_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(column_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowColumn(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_column"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct column_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<column_output_obj> column_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto column_id_target = admin_statement->column_index_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get all columns and find the one with matching column_id
    auto catalog_column_prefix = KeyEncode::TableColumnPrefix(db_id_str, table_id_str);
    auto catalog_column_iter = db->NewIterator(read_options);
    catalog_column_iter->Seek(catalog_column_prefix);

    while (catalog_column_iter->Valid() && catalog_column_iter->key().starts_with(catalog_column_prefix)) {
        std::string key_str = catalog_column_iter->key().ToString();
        std::string value_str = catalog_column_iter->value().ToString();
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 5 && parts[0] == "tbl" && parts[1] == "col") {
            std::string column_name = parts[4];

            // Parse column_id from the JSON value
            try {
                nlohmann::json column_json = nlohmann::json::parse(value_str);
                if (column_json.contains("column_id")) {
                    int64_t column_id = column_json["column_id"].get<int64_t>();
                    if (column_id == column_id_target) {
                        // Found the matching column
                        // Check if column is dropped
                        auto drop_iter = db->NewIterator(read_options);
                        auto drop_col_prefix = fmt::format("drop|tbl_col|{}/{}/", db_id_str, table_id_str);
                        drop_iter->Seek(drop_col_prefix);
                        bool dropped{};
                        while (drop_iter->Valid() && drop_iter->key().starts_with(drop_col_prefix)) {
                            auto drop_key_str = drop_iter->key().ToString();
                            auto drop_parts = Partition(drop_key_str, '/');
                            if (drop_parts.size() >= 4 && drop_parts[3] == column_name) {
                                dropped = true;
                                break;
                            }
                            drop_iter->Next();
                        }

                        column_output_objs.emplace_back(column_name, column_id, dropped);
                        break;
                    }
                }
            } catch (const std::exception &e) {
                // Skip invalid JSON entries
            }
        }
        catalog_column_iter->Next();
    }

    auto all_col_cnt = column_output_objs.size();
    for (size_t i = 0; i < all_col_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // column name
            Value value = Value::MakeVarchar(column_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // column dir
            Value value = Value::MakeVarchar(fmt::format("col_{}", column_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(column_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListIndexes(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_indexes"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct index_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<index_output_obj> index_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Now list all indexes in this table
    auto catalog_index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str, table_id_str);
    auto catalog_index_iter = db->NewIterator(read_options);
    catalog_index_iter->Seek(catalog_index_prefix);

    while (catalog_index_iter->Valid() && catalog_index_iter->key().starts_with(catalog_index_prefix)) {
        std::string key_str = catalog_index_iter->key().ToString();
        std::string index_id = catalog_index_iter->value().ToString();
        // Format: catalog|idx|{db_id}|{table_id}|{index_name}|{ts}
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 6 && parts[0] == "catalog" && parts[1] == "idx") {
            std::string index_name = parts[4];
            TxnTimeStamp index_commit_ts = std::stoull(parts[5]);

            // Check if index is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_idx_prefix = fmt::format("drop|idx|{}/{}/", db_id_str, table_id_str);
            drop_iter->Seek(drop_idx_prefix);
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_idx_prefix)) {
                auto drop_key_str = drop_iter->key().ToString();
                auto drop_parts = Partition(drop_key_str, '/');
                if (drop_parts.size() >= 5 && drop_parts[3] == index_name) {
                    TxnTimeStamp drop_ts = static_cast<TxnTimeStamp>(std::stoull(drop_parts[4]));
                    if (drop_ts >= index_commit_ts) {
                        dropped = true;
                        break;
                    }
                }
                drop_iter->Next();
            }

            index_output_objs.emplace_back(index_name, std::stoull(index_id), dropped);
        }
        catalog_index_iter->Next();
    }

    auto all_idx_cnt = index_output_objs.size();
    for (size_t i = 0; i < all_idx_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // index name
            Value value = Value::MakeVarchar(index_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // index dir
            Value value = Value::MakeVarchar(fmt::format("idx_{}", index_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(index_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowIndex(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_index"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct index_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<index_output_obj> index_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto index_name = admin_statement->index_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get the specific index
    auto catalog_index_prefix = KeyEncode::CatalogIndexPrefix(db_id_str, table_id_str, index_name);
    auto catalog_index_iter = db->NewIterator(read_options);
    catalog_index_iter->Seek(catalog_index_prefix);

    if (catalog_index_iter->Valid() && catalog_index_iter->key().starts_with(catalog_index_prefix)) {
        std::string key_str = catalog_index_iter->key().ToString();
        std::string index_id = catalog_index_iter->value().ToString();
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 6 && parts[0] == "catalog" && parts[1] == "idx" && parts[4] == index_name) {
            TxnTimeStamp index_commit_ts = std::stoull(parts[5]);

            // Check if index is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_idx_prefix = fmt::format("drop|idx|{}/{}/", db_id_str, table_id_str);
            drop_iter->Seek(drop_idx_prefix);
            bool dropped{};
            while (drop_iter->Valid() && drop_iter->key().starts_with(drop_idx_prefix)) {
                auto drop_key_str = drop_iter->key().ToString();
                auto drop_parts = Partition(drop_key_str, '/');
                if (drop_parts.size() >= 5 && drop_parts[3] == index_name) {
                    TxnTimeStamp drop_ts = static_cast<TxnTimeStamp>(std::stoull(drop_parts[4]));
                    if (drop_ts >= index_commit_ts) {
                        dropped = true;
                        break;
                    }
                }
                drop_iter->Next();
            }

            index_output_objs.emplace_back(index_name, std::stoull(index_id), dropped);
        }
    }

    auto all_idx_cnt = index_output_objs.size();
    for (size_t i = 0; i < all_idx_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // index name
            Value value = Value::MakeVarchar(index_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // index dir
            Value value = Value::MakeVarchar(fmt::format("idx_{}", index_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(index_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListIndexSegments(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_index_segments"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct segment_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<segment_output_obj> segment_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto index_name = admin_statement->index_name_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get index_id
    std::string index_id_str;
    auto catalog_index_prefix = KeyEncode::CatalogIndexPrefix(db_id_str, table_id_str, index_name);
    auto catalog_index_iter = db->NewIterator(read_options);
    catalog_index_iter->Seek(catalog_index_prefix);

    if (catalog_index_iter->Valid() && catalog_index_iter->key().starts_with(catalog_index_prefix)) {
        index_id_str = catalog_index_iter->value().ToString();
    }

    if (index_id_str.empty()) {
        return query_result;
    }

    // Now list all index segments in this index
    auto catalog_idx_seg_prefix = KeyEncode::CatalogIdxSegmentKeyPrefix(db_id_str, table_id_str, index_id_str);
    auto catalog_idx_seg_iter = db->NewIterator(read_options);
    catalog_idx_seg_iter->Seek(catalog_idx_seg_prefix);

    while (catalog_idx_seg_iter->Valid() && catalog_idx_seg_iter->key().starts_with(catalog_idx_seg_prefix)) {
        std::string key_str = catalog_idx_seg_iter->key().ToString();
        // Format: idx_seg|{db_id}|{table_id}|{index_id}|{segment_id}
        auto parts = Partition(key_str, '|');
        if (parts.size() >= 5 && parts[0] == "idx_seg") {
            SegmentID segment_id = static_cast<SegmentID>(std::stoull(parts[4]));

            // Check if index segment is dropped
            auto drop_iter = db->NewIterator(read_options);
            auto drop_key = KeyEncode::DropSegmentIndexKey(db_id_str, table_id_str, index_id_str, segment_id);
            drop_iter->Seek(drop_key);
            bool dropped = drop_iter->Valid();

            segment_output_objs.emplace_back(fmt::format("idx_seg_{}", segment_id), static_cast<size_t>(segment_id), dropped);
        }
        catalog_idx_seg_iter->Next();
    }

    auto all_seg_cnt = segment_output_objs.size();
    for (size_t i = 0; i < all_seg_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // segment name
            Value value = Value::MakeVarchar(segment_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // segment dir
            Value value = Value::MakeVarchar(fmt::format("idx_seg_{}", segment_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(segment_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowIndexSegment(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;

    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, bigint_type, "index", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "dir", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, bool_type, "dropped", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "comment", std::set<ConstraintType>()),
    };

    auto table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_index_segment"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
        bool_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    rocksdb::DB *db{};
    rocksdb::Options options;
    rocksdb::ReadOptions read_options;

    auto catalog_dir = InfinityContext::instance().config()->CatalogDir();

    rocksdb::TransactionDB::OpenForReadOnly(options, catalog_dir, &db);

    struct segment_output_obj {
        std::string name_;
        size_t id_;
        bool dropped_;
    };

    std::vector<segment_output_obj> segment_output_objs;

    auto schema_name = admin_statement->schema_name_.has_value() ? admin_statement->schema_name_.value() : query_context->schema_name();
    auto table_name = admin_statement->table_name_.value();
    auto index_name = admin_statement->index_name_.value();
    auto segment_id = admin_statement->segment_index_.value();

    // First get the db_id from the schema name
    std::string db_id_str;
    auto catalog_db_iter = db->NewIterator(read_options);
    catalog_db_iter->Seek(KeyEncode::kCatalogDbHeader);

    while (catalog_db_iter->Valid() && catalog_db_iter->key().starts_with(KeyEncode::kCatalogDbHeader)) {
        std::string key_str = catalog_db_iter->key().ToString();
        std::string db_id = catalog_db_iter->value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        std::string db_name = key_str.substr(start, end - start);
        if (db_name == schema_name) {
            db_id_str = db_id;
            break;
        }
        catalog_db_iter->Next();
    }

    if (db_id_str.empty()) {
        return query_result;
    }

    // Get table_id
    std::string table_id_str;
    auto catalog_table_prefix = KeyEncode::CatalogTablePrefix(db_id_str, table_name);
    auto catalog_table_iter = db->NewIterator(read_options);
    catalog_table_iter->Seek(catalog_table_prefix);

    if (catalog_table_iter->Valid() && catalog_table_iter->key().starts_with(catalog_table_prefix)) {
        table_id_str = catalog_table_iter->value().ToString();
    }

    if (table_id_str.empty()) {
        return query_result;
    }

    // Get index_id
    std::string index_id_str;
    auto catalog_index_prefix = KeyEncode::CatalogIndexPrefix(db_id_str, table_id_str, index_name);
    auto catalog_index_iter = db->NewIterator(read_options);
    catalog_index_iter->Seek(catalog_index_prefix);

    if (catalog_index_iter->Valid() && catalog_index_iter->key().starts_with(catalog_index_prefix)) {
        index_id_str = catalog_index_iter->value().ToString();
    }

    if (index_id_str.empty()) {
        return query_result;
    }

    // Get the specific index segment
    auto catalog_idx_seg_key = KeyEncode::CatalogIdxSegmentKey(db_id_str, table_id_str, index_id_str, static_cast<SegmentID>(segment_id));
    auto catalog_idx_seg_iter = db->NewIterator(read_options);
    catalog_idx_seg_iter->Seek(catalog_idx_seg_key);

    if (catalog_idx_seg_iter->Valid() && catalog_idx_seg_iter->key().ToString() == catalog_idx_seg_key) {
        // Check if index segment is dropped
        auto drop_iter = db->NewIterator(read_options);
        auto drop_key = KeyEncode::DropSegmentIndexKey(db_id_str, table_id_str, index_id_str, static_cast<SegmentID>(segment_id));
        drop_iter->Seek(drop_key);
        bool dropped = drop_iter->Valid();

        segment_output_objs.emplace_back(fmt::format("idx_seg_{}", segment_id), static_cast<size_t>(segment_id), dropped);
    }

    auto all_seg_cnt = segment_output_objs.size();
    for (size_t i = 0; i < all_seg_cnt; ++i) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // index
            Value value = Value::MakeBigInt(i);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // segment name
            Value value = Value::MakeVarchar(segment_output_objs[i].name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // segment dir
            Value value = Value::MakeVarchar(fmt::format("idx_seg_{}", segment_output_objs[i].id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // dropped
            Value value = Value::MakeBool(segment_output_objs[i].dropped_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        if ((i + 1) % output_block_ptr->capacity() == 0) {
            output_block_ptr->Finalize();
            query_result.result_table_->Append(std::move(output_block_ptr));
            output_block_ptr = nullptr;
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListConfigs(QueryContext *query_context, const AdminStatement *admin_statement) {
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "config_name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    };

    Config *global_config = query_context->global_config();
    QueryResult query_result;
    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("list_configs"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    std::vector<std::shared_ptr<DataType>> column_types{
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
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // option value
            Value value = Value::MakeVarchar(global_config->Version());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option description
            Value value = Value::MakeVarchar("Infinity version.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        { // option name
            Value value = Value::MakeVarchar(TIME_ZONE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            i64 time_zone_bias = global_config->TimeZoneBias();
            if (time_zone_bias >= 0) {
                Value value = Value::MakeVarchar(fmt::format("{}+{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            } else {
                Value value = Value::MakeVarchar(fmt::format("{}{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Time zone information.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CPU_LIMIT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CPULimit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("CPU number used by infinity executor.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(RECORD_RUNNING_QUERY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = global_config->RecordRunningQuery() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("To record running query");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SERVER_ADDRESS_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ServerAddress());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity server listen ip address");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(POSTGRES_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PostgresPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Postgres port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(HTTP_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->HTTPPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("HTTP port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLIENT_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ClientPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Thrift RPC port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CONNECTION_POOL_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ConnectionPoolSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Connection pool capacity.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerServerConnectionPoolSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer server connection pool capacity.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILENAME_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogFileName());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log file name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_TO_STDOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = global_config->LogToStdout() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("If log is also output to standard output");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_MAX_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileMaxSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Max log file size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_ROTATE_COUNT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileRotateCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log files rotation limitation");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_LEVEL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(LogLevel2Str(global_config->GetLogLevel()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log level");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    if (InfinityContext::instance().persistence_manager() != nullptr) {
        {
            {
                // option name
                Value value = Value::MakeVarchar(PERSISTENCE_DIR_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(global_config->PersistenceDir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Virtual filesystem directory");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
            }
        }

        {
            {
                // option name
                Value value = Value::MakeVarchar(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(std::to_string(global_config->PersistenceObjectSizeLimit()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Virtual file limitation");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
            }
        }
    } else {
        {
            {
                // option name
                Value value = Value::MakeVarchar(DATA_DIR_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(global_config->DataDir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Data directory");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
            }
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CATALOG_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->CatalogDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Catalog directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->SnapshotDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Snapshot directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Snapshots store directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLEANUP_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Cleanup period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(COMPACT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CompactInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Compact period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(OPTIMIZE_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->OptimizeIndexInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Optimize memory index period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(MEM_INDEX_CAPACITY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->MemIndexCapacity()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Real-time index building row capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->SnapshotDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Snapshot storage directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(BUFFER_MANAGER_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->BufferManagerSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Buffer manager memory size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(TEMP_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->TempDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Temporary data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->WALDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_COMPACT_THRESHOLD_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->WALCompactThreshold()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log compact triggering threshold");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CHECKPOINT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CheckpointInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Checkpoint period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_FLUSH_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(FlushOptionTypeToString(global_config->FlushMethodAtCommit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log flush method");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(RESOURCE_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ResourcePath());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity resource directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ListVariables(QueryContext *query_context, const AdminStatement *admin_statement) {
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "description", std::set<ConstraintType>()),
    };

    QueryResult query_result;
    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("list_configs"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    std::vector<std::shared_ptr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    {
        {
            // option name
            Value value = Value::MakeVarchar("role");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // option value
            Value value = Value::MakeVarchar(ToString(InfinityContext::instance().GetServerRole()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // option description
            Value value = Value::MakeVarchar("Current infinity server role");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::ShowVariable(QueryContext *query_context, const AdminStatement *admin_statement) {
    std::shared_ptr<DataType> varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    //    std::shared_ptr<DataType> integer_type = std::make_shared<DataType>(LogicalType::kBigInt);
    //    std::shared_ptr<DataType> double_type = std::make_shared<DataType>(LogicalType::kDouble);
    //    std::shared_ptr<DataType> bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, column_defs);

    QueryResult query_result;
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kResult);

    std::vector<std::shared_ptr<DataType>> output_column_types{
        varchar_type,
    };

    std::string variable_name = admin_statement->variable_name_.value();
    ToLower(variable_name);

    if (variable_name == "role") {
        output_block_ptr->Init(output_column_types);
        Value value = Value::MakeVarchar(ToString(InfinityContext::instance().GetServerRole()));
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
    } else {
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::NoSysVar(variable_name);
        return query_result;
    }
    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));

    return query_result;
}

QueryResult AdminExecutor::ListNodes(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    if (!InfinityContext::instance().IsClusterRole()) {
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::NotSupport("'ADMIN SHOW NODES' only works in cluster mode");
        return query_result;
    }

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "role", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(2, varchar_type, "status", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(3, varchar_type, "address", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(4, varchar_type, "last_update", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(5, bigint_type, "heartbeat", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("list_nodes"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, bigint_type};

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);
    size_t row_count = 0;

    std::vector<std::shared_ptr<NodeInfo>> server_nodes = InfinityContext::instance().cluster_manager()->ListNodes();
    for (const auto &server_node : server_nodes) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(column_types);
        }

        {
            // name
            Value value = Value::MakeVarchar(server_node->node_name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // role
            Value value = Value::MakeVarchar(ToString(server_node->node_role()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // status
            Value value = Value::MakeVarchar(ToString(server_node->node_status()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // address
            Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // last_update
            std::chrono::system_clock::duration time_duration = std::chrono::seconds(server_node->update_ts());
            std::chrono::time_point<std::chrono::system_clock> time_since_epoch = std::chrono::time_point<std::chrono::system_clock>(time_duration);
            std::time_t t_c = std::chrono::system_clock::to_time_t(time_since_epoch);
            //            std::stringstream ss;
            //            ss << std::put_time(std::localtime(&t_c), "%F %T"));
            Value value = Value::MakeVarchar(std::ctime(&t_c));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        {
            // heartbeat
            Value value = Value::MakeBigInt(server_node->heartbeat_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[5]);
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

QueryResult AdminExecutor::ShowNode(QueryContext *query_context, const AdminStatement *admin_statement) {
    QueryResult query_result;
    if (!InfinityContext::instance().IsClusterRole()) {
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::NotSupport("SHOW NODE only works in cluster mode");
        return query_result;
    }

    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_node"), nullptr, column_defs);
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    std::string node_name = admin_statement->node_name_.value();
    auto [status, server_node] = InfinityContext::instance().cluster_manager()->GetNodeInfoByName(node_name);
    if (!status.ok()) {
        query_result.result_table_ = nullptr;
        query_result.status_ = std::move(status);
        return query_result;
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(server_node->node_name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("role");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(ToString(server_node->node_role()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("status");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(ToString(server_node->node_status()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("address");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("entry_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            //            const std::chrono::system_clock::duration time_since_epoch = std::chrono::seconds(server_node->last_update_ts_);
            const std::time_t t_c = server_node->update_ts();
            Value value = Value::MakeVarchar(std::ctime(&t_c));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::RemoveNode(QueryContext *query_context, const AdminStatement *admin_statement) {
    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};

    std::string node_name = admin_statement->node_name_.value();
    QueryResult query_result;

    Status status = InfinityContext::instance().cluster_manager()->RemoveNodeInfo(node_name);

    if (status.ok()) {
        auto result_table_def_ptr =
            std::make_shared<TableDef>(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("Tables"), nullptr, column_defs);
        query_result.result_table_ = std::make_shared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    } else {
        query_result.status_ = status;
    }

    return query_result;
}

QueryResult AdminExecutor::ShowCurrentNode(QueryContext *query_context, const AdminStatement *admin_statement) {
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);

    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
        std::make_shared<ColumnDef>(0, varchar_type, "name", std::set<ConstraintType>()),
        std::make_shared<ColumnDef>(1, varchar_type, "value", std::set<ConstraintType>()),
    };

    std::shared_ptr<TableDef> table_def =
        TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("show_current_node"), nullptr, column_defs);

    QueryResult query_result;
    query_result.result_table_ = std::make_shared<DataTable>(table_def, TableType::kDataTable);

    std::vector<std::shared_ptr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(column_types);

    if (InfinityContext::instance().IsClusterRole()) {
        std::shared_ptr<NodeInfo> server_node = InfinityContext::instance().cluster_manager()->ThisNode();
        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("name");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(server_node->node_name());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }

        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("role");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_node->node_role()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }

        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("status");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_node->node_status()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }

        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("address");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(fmt::format("{}:{}", server_node->node_ip(), server_node->node_port()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }

        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("update_time");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                // const std::chrono::system_clock::duration time_since_epoch = std::chrono::seconds(server_node->last_update_ts_);
                const std::time_t t_c = server_node->update_ts();
                Value value = Value::MakeVarchar(std::ctime(&t_c));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }
    } else {
        NodeRole server_role = InfinityContext::instance().GetServerRole();
        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("role");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(server_role));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }

        {
            size_t column_id = 0;
            {
                Value value = Value::MakeVarchar("status");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                bool infinity_started = InfinityContext::instance().InfinityContextStarted();
                std::string infinity_status("started");
                if (!infinity_started && server_role != NodeRole::kAdmin) {
                    infinity_status = "starting";
                }
                Value value = Value::MakeVarchar(infinity_status);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
        }
    }

    output_block_ptr->Finalize();
    query_result.result_table_->Append(std::move(output_block_ptr));
    return query_result;
}

QueryResult AdminExecutor::SetRole(QueryContext *query_context, const AdminStatement *admin_statement) {
    // TODO: check if current role is same as the target role.
    //    std::vector<std::shared_ptr<ColumnDef>> column_defs = {
    //        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
    //
    // NodeRole target_node_role = admin_statement->node_role_.value();
    QueryResult query_result;
    // NodeRole current_node_role = InfinityContext::instance().GetServerRole();
    // if (current_node_role == target_node_role) {
    //     LOG_INFO(fmt::format("Infinity is already the role of {}", ToString(current_node_role)));
    //     auto result_table_def_ptr =
    //         std::make_shared<TableDef>(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("Tables"), nullptr, column_defs);
    //     query_result.result_table_ = std::make_shared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    //     query_result.status_ = Status::OK();
    //     return query_result;
    // }
    // Status status;
    // switch (target_node_role) {
    //     case NodeRole::kAdmin: {
    //         status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //         LOG_INFO("Start in ADMIN mode");
    //         break;
    //     }
    //     case NodeRole::kStandalone: {
    //         status = InfinityContext::instance().ChangeServerRole(NodeRole::kStandalone);
    //         LOG_INFO("Start in STANDALONE mode");
    //         break;
    //     }
    //     case NodeRole::kLeader: {
    //         std::string node_name = admin_statement->node_name_.value();
    //         switch (IdentifierValidation(node_name)) {
    //             case IdentifierValidationStatus::kOk:
    //                 break;
    //             case IdentifierValidationStatus::kEmpty: {
    //                 status = Status::EmptyColumnName();
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kExceedLimit: {
    //                 status = Status::ExceedColumnNameLength(node_name.length());
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kInvalidName: {
    //                 status = Status::InvalidColumnName(node_name);
    //                 break;
    //             }
    //         }
    //         if (!status.ok()) {
    //             query_result.status_ = status;
    //             return query_result;
    //         }
    //         status = InfinityContext::instance().ChangeServerRole(NodeRole::kLeader, false, node_name);
    //         if (!status.ok()) {
    //             LOG_INFO("Fail to change to LEADER role");
    //             if (status.code() == ErrorCode::kCantSwitchRole) {
    //                 // Just don't change the role
    //                 break;
    //             }
    //
    //             Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //             if (!restore_status.ok()) {
    //                 UnrecoverableError(fmt::format("Fail to change node role to LEADER, then fail to restore to ADMIN."));
    //             }
    //         } else {
    //             LOG_INFO("Start in LEADER role");
    //         }
    //         break;
    //     }
    //     case NodeRole::kFollower: {
    //         std::string node_name = admin_statement->node_name_.value();
    //         switch (IdentifierValidation(node_name)) {
    //             case IdentifierValidationStatus::kOk:
    //                 break;
    //             case IdentifierValidationStatus::kEmpty: {
    //                 status = Status::EmptyColumnName();
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kExceedLimit: {
    //                 status = Status::ExceedColumnNameLength(node_name.length());
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kInvalidName: {
    //                 status = Status::InvalidColumnName(node_name);
    //                 break;
    //             }
    //         }
    //         if (!status.ok()) {
    //             query_result.status_ = status;
    //             return query_result;
    //         }
    //
    //         std::string leader_address = admin_statement->leader_address_.value();
    //         std::string leader_ip;
    //         i64 leader_port;
    //         if (!ParseIPPort(leader_address, leader_ip, leader_port)) {
    //             query_result.status_ = Status::InvalidServerAddress(leader_address);
    //             return query_result;
    //         }
    //
    //         status = InfinityContext::instance().ChangeServerRole(NodeRole::kFollower, false, node_name, leader_ip, leader_port);
    //         if (!status.ok()) {
    //             if (status.code() != ErrorCode::kCantSwitchRole) {
    //                 LOG_INFO("Fail to change to FOLLOWER role");
    //                 Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //                 if (!restore_status.ok()) {
    //                     UnrecoverableError(fmt::format("Fail to change node role to FOLLOWER, then fail to restore to ADMIN."));
    //                 }
    //             }
    //         } else {
    //             LOG_INFO("Start in FOLLOWER role");
    //         }
    //         break;
    //     }
    //     case NodeRole::kLearner: {
    //         std::string node_name = admin_statement->node_name_.value();
    //         switch (IdentifierValidation(node_name)) {
    //             case IdentifierValidationStatus::kOk:
    //                 break;
    //             case IdentifierValidationStatus::kEmpty: {
    //                 status = Status::EmptyColumnName();
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kExceedLimit: {
    //                 status = Status::ExceedColumnNameLength(node_name.length());
    //                 break;
    //             }
    //             case IdentifierValidationStatus::kInvalidName: {
    //                 status = Status::InvalidColumnName(node_name);
    //                 break;
    //             }
    //         }
    //         if (!status.ok()) {
    //             query_result.status_ = status;
    //             return query_result;
    //         }
    //
    //         std::string leader_address = admin_statement->leader_address_.value();
    //         std::string leader_ip;
    //         i64 leader_port;
    //         if (!ParseIPPort(leader_address, leader_ip, leader_port)) {
    //             query_result.status_ = Status::InvalidServerAddress(leader_address);
    //             return query_result;
    //         }
    //
    //         status = InfinityContext::instance().ChangeServerRole(NodeRole::kLearner, false, node_name, leader_ip, leader_port);
    //         if (!status.ok()) {
    //             if (status.code() != ErrorCode::kCantSwitchRole) {
    //                 LOG_INFO("Fail to change to LEARNER role");
    //                 Status restore_status = InfinityContext::instance().ChangeServerRole(NodeRole::kAdmin);
    //                 if (!restore_status.ok()) {
    //                     UnrecoverableError(fmt::format("Fail to change node role to FOLLOWER, then fail to restore to ADMIN."));
    //                 }
    //             }
    //         } else {
    //             LOG_INFO("Start in LEARNER role");
    //         }
    //         break;
    //     }
    //     default: {
    //         UnrecoverableError("Unsupported role");
    //     }
    // }
    //
    // if (status.ok()) {
    //     auto result_table_def_ptr =
    //         std::make_shared<TableDef>(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("Tables"), nullptr, column_defs);
    //     query_result.result_table_ = std::make_shared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    // } else {
    //     query_result.status_ = status;
    // }

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
