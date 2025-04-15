// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module new_catalog;

import stl;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import third_party;
import logger;
import infinity_exception;
import default_values;
import mem_index;
import column_index_reader;
import data_type;
import parsed_expr;
import constant_expr;
import meta_key;
import catalog;
import catalog_delta_entry;
import db_meeta;
import table_def;
import table_meeta;
import segment_meta;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

NewCatalog::~NewCatalog() = default;

Status NewCatalog::Init(KVStore *kv_store) {
    auto kv_instance = kv_store->GetInstance();
    String db_string_id;
    Status status = kv_instance->Get(LATEST_DATABASE_ID.data(), db_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_DATABASE_ID.data(), "0");
    }
    String table_string_id;
    status = kv_instance->Get(LATEST_TABLE_ID.data(), table_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_TABLE_ID.data(), "0");
    }
    String index_string_id;
    status = kv_instance->Get(LATEST_INDEX_ID.data(), index_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_INDEX_ID.data(), "0");
    }
    status = kv_instance->Commit();
    if (!status.ok()) {
        UnrecoverableError("Can't initialize latest ID");
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalog(Config *config_ptr, const String &full_ckp_path, const Vector<String> &delta_ckp_path_array) {
    // Read full checkpoint file
    UniquePtr<nlohmann::json> full_ckp_json = Catalog::LoadFullCheckpointToJson(config_ptr, full_ckp_path);
    Status status = NewCatalog::Init(kv_store_);
    if (!status.ok()) {
        return status;
    }

    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();

    if (full_ckp_json->contains("databases")) {
        for (const auto &db_json : (*full_ckp_json)["databases"]) {
            status = TransformCatalogDatabase(db_json, kv_instance.get());
            if (!status.ok()) {
                return status;
            }
        }
    }

    // Read delta checkpoint files
    for (const String &delta_ckp_path : delta_ckp_path_array) {
        UniquePtr<CatalogDeltaEntry> catalog_delta_entry = Catalog::LoadFromFileDelta(delta_ckp_path);
    }

    status = kv_instance->Commit();
    if (!status.ok()) {
        return status;
    }
    // Rename the old meta filename
    return Status::OK();
}

Status NewCatalog::TransformCatalogDatabase(const nlohmann::json &db_meta_json, KVInstance *kv_instance) {
    String db_name = db_meta_json["db_name"];
    if (db_meta_json.contains("db_entries")) {

        TxnTimeStamp max_commit_ts = 0;

        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            TxnTimeStamp current_commit_ts = db_entry_json["commit_ts"];
            max_commit_ts = std::max(max_commit_ts, current_commit_ts);
        }

        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            if (max_commit_ts == db_entry_json["commit_ts"]) {
                if (db_entry_json["deleted"] == true) {
                    return Status::OK();
                }

                SharedPtr<String> db_comment = nullptr;
                if (db_entry_json.contains("db_comment")) {
                    db_comment = MakeShared<String>(db_entry_json["db_comment"]);
                } else {
                    db_comment = MakeShared<String>();
                }

                String db_id_str;
                Status status = IncrLatestID(kv_instance, db_id_str, LATEST_DATABASE_ID);
                if (!status.ok()) {
                    return status;
                }

                Optional<DBMeeta> db_meta;
                status = NewCatalog::AddNewDB(kv_instance, db_id_str, max_commit_ts, db_name, db_comment.get(), db_meta);
                if (!status.ok()) {
                    return status;
                }

                if (db_entry_json.contains("tables")) {
                    for (const auto &table_meta_json : db_entry_json["tables"]) {
                        Status status = TransformCatalogTable(db_meta, table_meta_json, db_name);
                        if (!status.ok()) {
                            return status;
                        }
                    }
                }
                break;
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogTable(Optional<DBMeeta> &db_meta, const nlohmann::json &table_meta_json, String const &db_name) {
    String table_name = table_meta_json["table_name"];
    if (table_meta_json.contains("table_entries")) {
        auto &kv_instance = db_meta->kv_instance();
        for (auto &table_entry_json : table_meta_json["table_entries"]) {
            bool deleted = table_entry_json["deleted"];
            if (deleted) {
                continue;
            }

            String table_id_str;
            Status status = IncrLatestID(&kv_instance, table_id_str, LATEST_DATABASE_ID);
            if (!status.ok()) {
                return status;
            }

            TxnTimeStamp table_begin_ts = table_entry_json["begin_ts"];
            TxnTimeStamp table_commit_ts = table_entry_json["commit_ts"];

            Vector<SharedPtr<ColumnDef>> columns;

            SharedPtr<String> table_entry_dir;
            table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);

            for (const auto &column_def_json : table_entry_json["column_definition"]) {
                SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
                i64 column_id = column_def_json["column_id"];
                String column_name = column_def_json["column_name"];

                std::set<ConstraintType> constraints;
                if (column_def_json.contains("constraints")) {
                    for (const auto &column_constraint : column_def_json["constraints"]) {
                        ConstraintType constraint = column_constraint;
                        constraints.emplace(constraint);
                    }
                }

                String comment;
                if (column_def_json.contains("column_comment")) {
                    comment = column_def_json["column_comment"];
                }

                SharedPtr<ParsedExpr> default_expr = nullptr;
                if (column_def_json.contains("default")) {
                    default_expr = ConstantExpr::Deserialize(column_def_json["default"]);
                }

                SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints, comment, default_expr);
                columns.emplace_back(column_def);
            }

            String table_comment;
            auto table_def = TableDef::Make(MakeShared<String>(db_name), MakeShared<String>(table_name), MakeShared<String>(table_comment), columns);
            Optional<TableMeeta> table_meta;
            status = AddNewTable(db_meta.value(), table_id_str, table_begin_ts, table_commit_ts, table_def, table_meta);
            if (!status.ok()) {
                return status;
            }

            SegmentID unsealed_segment_id = table_entry_json["unsealed_id"];
            table_meta->SetUnsealedSegmentID(unsealed_segment_id);

            if (table_entry_json.contains("segments")) {
                for (auto &segment_json : table_entry_json["segments"]) {
                    if (segment_json["deleted"]) {
                        continue;
                    }
                    status = TransformCatalogSegment(table_meta, segment_json);
                    if (!status.ok()) {
                        return status;
                    }
                }
            }

            if (table_entry_json.contains("table_indexes")) {
                for (auto &index_json : table_entry_json["table_indexes"]) {
                    status = TransformCatalogTableIndex(index_json, &kv_instance);
                    if (!status.ok()) {
                        return status;
                    }
                }
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogSegment(Optional<TableMeeta> &table_meta, const nlohmann::json &segment_entry_json) {
    // auto &kv_instance = table_meta->kv_instance();
    TxnTimeStamp segment_commit_ts = segment_entry_json["commit_ts"];

    Optional<SegmentMeta> segment_meta;
    Status status = NewCatalog::AddNewSegment1(table_meta.value(), segment_commit_ts, segment_meta);
    if (!status.ok()) {
        return status;
    }

    // if (segment_entry_json.contains("block_entries")) {
    //     for (const auto &block_entry_json : segment_entry_json["block_entries"]) {
    //         Status status = TransformCatalogBlock(block_entry_json, &kv_instance);
    //         if (!status.ok()) {
    //             return status;
    //         }
    //     }
    // }

    return Status::OK();
}
Status NewCatalog::TransformCatalogBlock(const nlohmann::json &block_entry_json, KVInstance *kv_instance) { return Status::OK(); }
Status NewCatalog::TransformCatalogBlockColumn(const nlohmann::json &block_column_entry_json, KVInstance *kv_instance) { return Status::OK(); }

Status NewCatalog::TransformCatalogTableIndex(const nlohmann::json &table_index_entry_json, KVInstance *kv_instance) { return Status::OK(); }
Status NewCatalog::TransformCatalogSegmentIndex(const nlohmann::json &segment_index_entry_json, KVInstance *kv_instance) { return Status::OK(); }
Status NewCatalog::TransformCatalogChunkIndex(const nlohmann::json &chunk_index_entry_json, KVInstance *kv_instance) { return Status::OK(); }

String NewCatalog::GetPathNameTail(const String &path) {
    SizeT delimiter_i = path.rfind('/');
    if (delimiter_i == String::npos) {
        return path;
    }
    return path.substr(delimiter_i + 1);
}

Status NewCatalog::LockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
        case LockType::kUnlocked: {
            table_memory_context->locker = LockType::kLocking;
            break;
        }
    }
    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::CommitLockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is locked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kLocking) {
        table_memory_context->locker = LockType::kLocked;
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in locking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::RollbackLockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is locked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kLocking) {
        table_memory_context_map_.erase(table_key);
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in locking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::UnlockTable(const String &table_key, TransactionID txn_id) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            table_memory_context->locker = LockType::kUnlocking;
            break;
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kUnlocked: {
            return Status::NotLocked(fmt::format("Table key: {} wasn't locked before", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
    }
    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::CommitUnlockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is being unlocked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kUnlocking) {
        table_memory_context_map_.erase(table_key);
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in unlocking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::RollbackUnlockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is being unlocked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kUnlocking) {
        table_memory_context->locker = LockType::kLocked;
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in unlocking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::ImmutateTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
        case LockType::kUnlocked: {
            table_memory_context->locker = LockType::kImmutable;
            break;
        }
    }

    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::MutateTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is immutated by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            table_memory_context_map_.erase(table_key);
            break;
        }
        case LockType::kUnlocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in unlocked state", table_key));
        }
    }

    return Status::OK();
}

Status NewCatalog::IncreaseTableWriteCount(const String &table_key) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locker != LockType::kUnlocked) {
        return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
    }

    ++table_memory_context->write_txn_num_;
    return Status::OK();
}

Status NewCatalog::DecreaseTableWriteCount(const String &table_key, SizeT count) {
    std::lock_guard lock(mtx_);

    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locker != LockType::kUnlocked) {
        UnrecoverableError(fmt::format("Table key: {} is already locked", table_key));
    }

    SizeT &write_txn_num = iter->second->write_txn_num_;
    if (write_txn_num >= count) {
        write_txn_num -= count;
    } else {
        UnrecoverableError(fmt::format("Attempt to reduce reference count {} by {}, of {}", write_txn_num, count, table_key));
    }

    if (write_txn_num == 0) {
        table_memory_context_map_.erase(table_key);
    }

    return Status::OK();
}

SizeT NewCatalog::GetTableWriteCount() const {
    std::lock_guard lock(mtx_);
    return table_memory_context_map_.size();
}

Status NewCatalog::AddBlockLock(String block_key) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>());
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key, TxnTimeStamp checkpoint_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>(checkpoint_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock) {
    block_lock = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(block_lock_mtx_);
        if (auto iter = block_lock_map_.find(block_key); iter != block_lock_map_.end()) {
            block_lock = iter->second;
        }
    }
    if (block_lock == nullptr) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::DropBlockLockByBlockKey(const String &block_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(block_lock_mtx_);
        delete_success = block_lock_map_.erase(block_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::AddMemIndex(String mem_index_key, SharedPtr<MemIndex> mem_index) {
    bool insert_success = false;
    HashMap<String, SharedPtr<MemIndex>>::iterator iter;
    {
        std::unique_lock lock(mem_index_mtx_);
        std::tie(iter, insert_success) = mem_index_map_.emplace(std::move(mem_index_key), std::move(mem_index));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetMemIndex(const String &mem_index_key, SharedPtr<MemIndex> &mem_index) {
    mem_index = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(mem_index_mtx_);
        if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
            mem_index = iter->second;
        }
    }
    if (mem_index == nullptr) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Status NewCatalog::DropMemIndexByMemIndexKey(const String &mem_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(mem_index_mtx_);
        delete_success = mem_index_map_.erase(mem_index_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Status NewCatalog::IncreaseTableReferenceCountForMemIndex(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        table_lock_for_mem_index_[table_key] = MakeShared<TableLockForMemIndex>();
    }

    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is dumping mem index", table_key));
    }

    ++table_lock_for_mem_index->append_count_;

    return Status::OK();
}

Status NewCatalog::DecreaseTableReferenceCountForMemIndex(const String &table_key, SizeT count) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return Status::OK();
    }

    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->dumping_mem_index_) {
        UnrecoverableError(fmt::format("Table key: {} is dumping mem index", table_key));
    }

    SizeT &append_count = table_lock_for_mem_index->append_count_;
    if (append_count >= count) {
        append_count -= count;
    } else {
        UnrecoverableError(fmt::format("Attempt to reduce reference count {} by {}, of {}", append_count, count, table_key));
    }

    if (append_count == 0) {
        table_lock_for_mem_index_.erase(table_key);
    }

    return Status::OK();
}

SizeT NewCatalog::GetTableReferenceCountForMemIndex(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return 0;
    }

    return iter->second->append_count_;
}

SizeT NewCatalog::GetTableReferenceCountForMemIndex() const {
    std::unique_lock lock(mem_index_mtx_);
    return table_lock_for_mem_index_.size();
}

Status NewCatalog::SetMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        table_lock_for_mem_index_[table_key] = MakeShared<TableLockForMemIndex>();
    }
    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->append_count_ > 0) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is appending mem index", table_key));
    }

    if (table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is dumping mem index", table_key));
    }
    table_lock_for_mem_index->dumping_mem_index_ = true;
    return Status::OK();
}

Status NewCatalog::UnsetMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        UnrecoverableError(fmt::format("Table key: {} isn't found in mem index dump", table_key));
    }
    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (!table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} isn't dumping mem index", table_key));
    }

    table_lock_for_mem_index->dumping_mem_index_ = false;
    if (table_lock_for_mem_index->append_count_ == 0) {
        table_lock_for_mem_index_.erase(table_key);
    } else {
        UnrecoverableError(fmt::format("Table key: {} is appending, why unset mem index dump", table_key));
    }

    return Status::OK();
}

bool NewCatalog::IsMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return false;
    }

    return iter->second->dumping_mem_index_;
}

Status NewCatalog::AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache) {
    bool insert_success = false;
    HashMap<String, SharedPtr<TableIndexReaderCache>>::iterator iter;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        std::tie(iter, insert_success) = ft_index_cache_map_.emplace(std::move(ft_index_cache_key), std::move(ft_index_cache));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache) {
    ft_index_cache = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(ft_index_cache_mtx_);
        if (auto iter = ft_index_cache_map_.find(ft_index_cache_key); iter != ft_index_cache_map_.end()) {
            ft_index_cache = iter->second;
        }
    }
    if (ft_index_cache == nullptr) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        delete_success = ft_index_cache_map_.erase(ft_index_cache_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::AddSegmentIndexFtInfo(const String &segment_index_key, SharedPtr<SegmentIndexFtInfo> segment_index_ft_info) {
    bool insert_success = false;
    HashMap<String, SharedPtr<SegmentIndexFtInfo>>::iterator iter;
    {
        std::unique_lock lock(segment_index_ft_info_mtx_);
        std::tie(iter, insert_success) = segment_index_ft_info_map_.emplace(std::move(segment_index_key), std::move(segment_index_ft_info));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentIndexFtInfo(const String &segment_index_key, SharedPtr<SegmentIndexFtInfo> &segment_index_ft_info) {
    segment_index_ft_info = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(segment_index_ft_info_mtx_);
        if (auto iter = segment_index_ft_info_map_.find(segment_index_key); iter != segment_index_ft_info_map_.end()) {
            segment_index_ft_info = iter->second;
        }
    }
    if (segment_index_ft_info == nullptr) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} not found", segment_index_key));
    }
    return Status::OK();
}

Status NewCatalog::DropSegmentIndexFtInfoByKey(const String &segment_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(segment_index_ft_info_mtx_);
        delete_success = segment_index_ft_info_map_.erase(segment_index_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} not found", segment_index_key));
    }
    return Status::OK();
}

Status NewCatalog::AddSegmentUpdateTS(const String &segment_update_ts_key, SharedPtr<SegmentUpdateTS> segment_update_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<SegmentUpdateTS>>::iterator iter;
    {
        std::unique_lock lock(segment_update_ts_mtx_);
        std::tie(iter, insert_success) = segment_update_ts_map_.emplace(std::move(segment_update_ts_key), std::move(segment_update_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentUpdateTS(const String &segment_update_ts_key, SharedPtr<SegmentUpdateTS> &segment_update_ts) {
    segment_update_ts = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(segment_update_ts_mtx_);
        if (auto iter = segment_update_ts_map_.find(segment_update_ts_key); iter != segment_update_ts_map_.end()) {
            segment_update_ts = iter->second;
        }
    }
    if (segment_update_ts == nullptr) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} not found", segment_update_ts_key));
    }
    return Status::OK();
}

Status NewCatalog::DropSegmentUpdateTSByKey(const String &segment_update_ts_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(segment_update_ts_mtx_);
        delete_success = segment_update_ts_map_.erase(segment_update_ts_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} not found", segment_update_ts_key));
    }
    return Status::OK();
}

void NewCatalog::AddCleanedMeta(TxnTimeStamp ts, UniquePtr<MetaKey> meta) {
    std::unique_lock lock(cleaned_meta_mtx_);

    cleaned_meta_.emplace(ts, std::move(meta));
}

void NewCatalog::GetCleanedMeta(TxnTimeStamp ts, Vector<UniquePtr<MetaKey>> &metas) {
    std::unique_lock lock(cleaned_meta_mtx_);

    auto iter = cleaned_meta_.lower_bound(ts);
    for (auto it = cleaned_meta_.begin(); it != iter; ++it) {
        metas.push_back(std::move(it->second));
    }
    cleaned_meta_.erase(cleaned_meta_.begin(), iter);
}

} // namespace infinity
