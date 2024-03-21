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

#include <fstream>
#include <vector>

module catalog;

import stl;

import txn_manager;
import logger;
import third_party;
import status;
import infinity_exception;
import function_set;
import table_function;
import special_function;
import buffer_manager;

import local_file_system;
import file_system_type;
import file_system;
import table_def;
import table_entry_type;
import meta_info;
import index_base;
import txn_store;
import data_access_state;
import catalog_delta_entry;
import file_writer;
import extra_ddl_info;

import table_meta;
import table_index_meta;
import base_entry;
import block_entry;
import block_column_entry;
import fulltext_index_entry;
import segment_index_entry;

namespace infinity {

// TODO Consider letting it commit as a transaction.
Catalog::Catalog(SharedPtr<String> dir) : current_dir_(std::move(dir)) {}

void Catalog::SetTxnMgr(TxnManager *txn_mgr) { txn_mgr_ = txn_mgr; }

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
Tuple<DBEntry *, Status>
Catalog::CreateDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    auto init_db_meta = [&]() {
        // Not find the db and create new db meta
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto db_dir = MakeShared<String>(parent_path.string());
        // Physical wal log
        return DBMeta::NewDBMeta(db_dir, MakeShared<String>(db_name));
    };
    LOG_TRACE(fmt::format("Adding new database entry: {}", db_name));
    auto [db_meta, r_lock] = this->db_meta_map_.GetMeta(db_name, std::move(init_db_meta));
    return db_meta->CreateNewEntry(std::move(r_lock), txn_id, begin_ts, txn_mgr, conflict_type);
}

// do not only use this method to drop database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::DropDatabase instead
Tuple<SharedPtr<DBEntry>, Status>
Catalog::DropDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    auto [db_meta, status, r_lock] = db_meta_map_.GetExistMeta(db_name, conflict_type);
    if (db_meta == nullptr) {
        return {nullptr, status};
    }
    return db_meta->DropNewEntry(std::move(r_lock), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<DBEntry *, Status> Catalog::GetDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [db_meta, status, r_lock] = db_meta_map_.GetExistMeta(db_name, ConflictType::kError);
    if (db_meta == nullptr) {
        return {nullptr, status};
    }
    return db_meta->GetEntry(std::move(r_lock), txn_id, begin_ts);
}

Tuple<SharedPtr<DatabaseInfo>, Status> Catalog::GetDatabaseInfo(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [db_meta, status, r_lock] = db_meta_map_.GetExistMeta(db_name, ConflictType::kError);
    if (db_meta == nullptr) {
        return {nullptr, status};
    }

    return db_meta->GetDatabaseInfo(std::move(r_lock), txn_id, begin_ts);
}

void Catalog::CreateDatabaseReplay(const SharedPtr<String> &db_name,
                                   std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                   TransactionID txn_id,
                                   TxnTimeStamp begin_ts) {
    auto init_db_meta = [&]() {
        // Not find the db and create new db meta
        Path catalog_path(*this->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto db_dir = MakeShared<String>(parent_path.string());
        // Physical wal log
        return DBMeta::NewDBMeta(db_dir, db_name);
    };
    LOG_TRACE(fmt::format("Adding new database entry: {}", *db_name));
    auto *db_meta = db_meta_map_.GetMetaNoLock(*db_name, std::move(init_db_meta));
    db_meta->CreateEntryReplay(std::move(init_entry), txn_id, begin_ts);
}

void Catalog::DropDatabaseReplay(const String &db_name,
                                 std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts) {
    auto [db_meta, status] = db_meta_map_.GetExistMetaNoLock(db_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    db_meta->DropEntryReplay(std::move(init_entry), txn_id, begin_ts);
}

DBEntry *Catalog::GetDatabaseReplay(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [db_meta, status] = db_meta_map_.GetExistMetaNoLock(db_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return db_meta->GetEntryReplay(txn_id, begin_ts);
}

void Catalog::RemoveDBEntry(DBEntry *db_entry, TransactionID txn_id) {
    const auto &db_name = *db_entry->db_meta_->db_name();
    LOG_TRACE(fmt::format("Remove a database entry {}", db_name));
    db_entry->db_meta_->DeleteNewEntry(txn_id);
}

Vector<DBEntry *> Catalog::Databases(TransactionID txn_id, TxnTimeStamp begin_ts) {
    this->rw_locker().lock_shared();

    Vector<DBEntry *> res;
    res.reserve(this->db_meta_map().size());
    for (const auto &db_meta_pair : this->db_meta_map()) {
        DBMeta *db_meta = db_meta_pair.second.get();
        auto [db_entry, status] = db_meta->GetEntryNolock(txn_id, begin_ts);
        if (status.ok()) {
            res.emplace_back(db_entry);
        }
    }
    this->rw_locker().unlock_shared();
    return res;
}

Tuple<TableEntry *, Status> Catalog::CreateTable(const String &db_name,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 const SharedPtr<TableDef> &table_def,
                                                 ConflictType conflict_type,
                                                 TxnManager *txn_mgr) {
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry
        ->CreateTable(TableEntryType::kTableEntry, table_def->table_name(), table_def->columns(), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableEntry>, Status> Catalog::DropTableByName(const String &db_name,
                                                              const String &table_name,
                                                              ConflictType conflict_type,
                                                              TransactionID txn_id,
                                                              TxnTimeStamp begin_ts,
                                                              TxnManager *txn_mgr) {
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }
    return db_entry->DropTable(table_name, conflict_type, txn_id, begin_ts, txn_mgr);
}

Status Catalog::GetTables(const String &db_name, Vector<TableDetail> &output_table_array, TransactionID txn_id, TxnTimeStamp begin_ts) {
    // Check the db entries
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return status;
    }
    return db_entry->GetTablesDetail(txn_id, begin_ts, output_table_array);
}

Tuple<TableEntry *, Status> Catalog::GetTableByName(const String &db_name, const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    // Check the db entries
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry->GetTableCollection(table_name, txn_id, begin_ts);
}

Tuple<SharedPtr<TableInfo>, Status>
Catalog::GetTableInfo(const String &db_name, const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry->GetTableInfo(table_name, txn_id, begin_ts);
}

Status Catalog::RemoveTableEntry(TableEntry *table_entry, TransactionID txn_id) {
    TableMeta *table_meta = table_entry->GetTableMeta();
    LOG_TRACE(fmt::format("Remove a table/collection entry: {}", *table_entry->GetTableName()));
    table_meta->DeleteEntry(txn_id);

    return Status::OK();
}

Tuple<TableIndexEntry *, Status> Catalog::CreateIndex(TableEntry *table_entry,
                                                      const SharedPtr<IndexBase> &index_base,
                                                      ConflictType conflict_type,
                                                      TransactionID txn_id,
                                                      TxnTimeStamp begin_ts,
                                                      TxnManager *txn_mgr) {

    return table_entry->CreateIndex(index_base, conflict_type, txn_id, begin_ts, txn_mgr
                                    // , is_replay, replay_table_index_dir
    );
}

TableIndexEntry *Catalog::CreateIndexReplay(TableEntry *table_entry,
                                            const SharedPtr<IndexBase> &index_base,
                                            const SharedPtr<String> &index_entry_dir,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnTimeStamp commit_ts) {
    return table_entry->CreateIndexReplay(
        index_base->index_name_,
        [&](TableIndexMeta *index_meta, SharedPtr<String> index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
            return TableIndexEntry::ReplayTableIndexEntry(index_meta, index_base, index_entry_dir, txn_id, begin_ts, commit_ts, false);
        },
        txn_id,
        begin_ts);
}

Tuple<SharedPtr<TableIndexEntry>, Status> Catalog::DropIndex(const String &db_name,
                                                             const String &table_name,
                                                             const String &index_name,
                                                             ConflictType conflict_type,
                                                             TransactionID txn_id,
                                                             TxnTimeStamp begin_ts,
                                                             TxnManager *txn_mgr) {
    auto [table_entry, table_status] = GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        LOG_ERROR(fmt::format("Database: {}, Table: {} is invalid", db_name, table_name));
        return {nullptr, table_status};
    }

    return table_entry->DropIndex(index_name, conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status>
Catalog::GetIndexByName(const String &db_name, const String &table_name, const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }
    return table_entry->GetIndex(index_name, txn_id, begin_ts);
}

Tuple<SharedPtr<TableIndexInfo>, Status>
Catalog::GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }
    return table_entry->GetTableIndexInfo(index_name, txn_id, begin_ts);
}

Status Catalog::RemoveIndexEntry(const String &index_name, TableIndexEntry *table_index_entry, TransactionID txn_id) {
    const TableIndexMeta *table_index_meta = table_index_entry->table_index_meta();
    TableEntry *table_entry = table_index_meta->GetTableEntry();
    table_entry->RemoveIndexEntry(index_name, txn_id);
    return Status::OK();
}

void Catalog::CommitCreateIndex(TxnIndexStore *txn_index_store, TxnTimeStamp commit_ts, bool is_replay) {
    auto *table_index_entry = txn_index_store->table_index_entry_;
    table_index_entry->CommitCreateIndex(txn_index_store, commit_ts, is_replay);
}

void Catalog::RollbackCreateIndex(TxnIndexStore *txn_index_store) {
    auto *table_index_entry = txn_index_store->table_index_entry_;
    table_index_entry->RollbackCreateIndex(txn_index_store);
}

void Catalog::Append(TableEntry *table_entry, TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {
    return table_entry->Append(txn_id, txn_store, commit_ts, buffer_mgr);
}

void Catalog::RollbackAppend(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    return table_entry->RollbackAppend(txn_id, commit_ts, txn_store);
}

Status Catalog::Delete(TableEntry *table_entry, TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    return table_entry->Delete(txn_id, txn_store, commit_ts, delete_state);
}

Status Catalog::RollbackDelete(TableEntry *table_entry, TransactionID txn_id, DeleteState &append_state, BufferManager *buffer_mgr) {
    return table_entry->RollbackDelete(txn_id, append_state, buffer_mgr);
}

Status Catalog::CommitCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store) {
    return table_entry->CommitCompact(txn_id, commit_ts, compact_store);
}

Status Catalog::RollbackCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    return table_entry->RollbackCompact(txn_id, commit_ts, compact_store);
}

Status Catalog::CommitWrite(TableEntry *table_entry,
                            TransactionID txn_id,
                            TxnTimeStamp commit_ts,
                            const HashMap<SegmentID, TxnSegmentStore> &segment_stores) {
    return table_entry->CommitWrite(txn_id, commit_ts, segment_stores);
}

Status Catalog::RollbackWrite(TableEntry *table_entry, TxnTimeStamp commit_ts, const Vector<TxnSegmentStore> &segment_stores) {
    return table_entry->RollbackWrite(commit_ts, segment_stores);
}

SegmentID Catalog::GetNextSegmentID(TableEntry *table_entry) { return table_entry->GetNextSegmentID(); }

void Catalog::AddSegment(TableEntry *table_entry, SharedPtr<SegmentEntry> &segment_entry) {
    table_entry->segment_map_.emplace(segment_entry->segment_id(), std::move(segment_entry));
    // ATTENTION: focusing on the segment id
    table_entry->next_segment_id_++;
}

SharedPtr<FunctionSet> Catalog::GetFunctionSetByName(Catalog *catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if (!catalog->function_sets_.contains(function_name)) {
        RecoverableError(Status::FunctionNotFound(function_name));
    }
    return catalog->function_sets_[function_name];
}

void Catalog::AddFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        UnrecoverableError(fmt::format("Trying to add duplicated function table_name into catalog: {}", name));
    }
    catalog->function_sets_.emplace(name, function_set);
}

void Catalog::AddSpecialFunction(Catalog *catalog, const SharedPtr<SpecialFunction> &special_function) {
    String name = special_function->name();
    StringToLower(name);
    if (catalog->special_functions_.contains(name)) {
        UnrecoverableError(fmt::format("Trying to add duplicated special function into catalog: {}", name));
    }
    catalog->special_functions_.emplace(name, special_function);
}

Tuple<SpecialFunction *, Status> Catalog::GetSpecialFunctionByNameNoExcept(Catalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->special_functions_.contains(function_name)) {
        return {nullptr, Status::SpecialFunctionNotFound()};
    }
    return {catalog->special_functions_[function_name].get(), Status::OK()};
}

nlohmann::json Catalog::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<DBMeta *> databases;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["current_dir"] = *this->current_dir_;
        json_res["next_txn_id"] = this->next_txn_id_;
        json_res["full_ckp_commit_ts"] = this->full_ckp_commit_ts_;
        json_res["catalog_version"] = this->catalog_version_;
        databases.reserve(this->db_meta_map().size());
        for (auto &db_meta : this->db_meta_map()) {
            databases.push_back(db_meta.second.get());
        }
    }

    for (auto &db_meta : databases) {
        json_res["databases"].emplace_back(db_meta->Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<Catalog> Catalog::NewCatalog(SharedPtr<String> dir, bool create_default_db) {
    auto catalog = MakeUnique<Catalog>(dir);
    if (create_default_db) {
        // db current dir is same level as catalog
        Path catalog_path(*catalog->current_dir_);
        Path parent_path = catalog_path.parent_path();
        auto data_dir = MakeShared<String>(parent_path.string());
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(db_meta.get(), false, db_meta->data_dir(), db_meta->db_name(), 0, 0);
        // TODO commit ts == 0 is true??
        db_entry->commit_ts_ = 0;
        db_meta->db_entry_list().emplace_front(std::move(db_entry));

        catalog->db_meta_map()["default"] = std::move(db_meta);
    }
    return catalog;
}

UniquePtr<Catalog> Catalog::LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr) {
    auto catalog = MakeUnique<Catalog>(nullptr);
    if (catalog_paths.empty()) {
        UnrecoverableError(fmt::format("Catalog path is empty"));
    }
    // 1. load json
    // 2. load entries
    LOG_INFO(fmt::format("Load base FULL catalog json from: {}", catalog_paths[0]));
    catalog = Catalog::LoadFromFile(catalog_paths[0], buffer_mgr);

    // Load catalogs delta checkpoints and merge.
    for (SizeT i = 1; i < catalog_paths.size(); i++) {
        LOG_INFO(fmt::format("Load catalog DELTA entry binary from: {}", catalog_paths[i]));
        Catalog::LoadFromEntry(catalog.get(), catalog_paths[i], buffer_mgr);
    }

    LOG_TRACE(fmt::format("Catalog Delta Op is done"));

    return catalog;
}

// called by Replay
void Catalog::LoadFromEntry(Catalog *catalog, const String &catalog_path, BufferManager *buffer_mgr) {
    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    i32 file_size = fs.GetFileSize(*catalog_file_handler);
    Vector<char> buf(file_size);
    fs.Read(*catalog_file_handler, buf.data(), file_size);
    fs.Close(*catalog_file_handler);
    char *ptr = buf.data();
    auto catalog_delta_entry = CatalogDeltaEntry::ReadAdv(ptr, file_size);
    i32 n_bytes = catalog_delta_entry->GetSizeInBytes();
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }

    //    auto global_commit_ts = catalog_delta_entry->commit_ts();
    //    auto global_txn_id = catalog_delta_entry->txn_id();
    auto &operations = catalog_delta_entry->operations();
    for (auto &op : operations) {
        auto type = op->GetType();
        LOG_TRACE(fmt::format("Catalog Delta Op is {}", op->ToString()));
        auto commit_ts = op->commit_ts();
        auto txn_id = op->txn_id();
        auto begin_ts = op->begin_ts();
        auto is_delete = op->is_delete();
        if (op->commit_ts_ < catalog->full_ckp_commit_ts_) {
            // Ignore the old txn
            continue;
        }
        switch (type) {

            // -----------------------------
            // Entry
            // -----------------------------
            case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
                auto add_db_entry_op = static_cast<AddDBEntryOp *>(op.get());
                const auto &db_name = add_db_entry_op->db_name();
                const auto &db_entry_dir = add_db_entry_op->db_entry_dir();
                if (!is_delete) {
                    catalog->CreateDatabaseReplay(
                        db_name,
                        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return DBEntry::ReplayDBEntry(db_meta, db_entry_dir, db_name, txn_id, begin_ts, commit_ts, false);
                        },
                        txn_id,
                        begin_ts);
                } else {
                    catalog->DropDatabaseReplay(
                        *db_name,
                        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return DBEntry::ReplayDBEntry(db_meta, db_entry_dir, db_name, txn_id, begin_ts, commit_ts, true);
                        },
                        txn_id,
                        begin_ts);
                }
                break;
            }
            case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
                auto add_table_entry_op = static_cast<AddTableEntryOp *>(op.get());
                const auto &db_name = add_table_entry_op->db_name();
                const auto &table_name = add_table_entry_op->table_name();
                const auto &table_entry_dir = add_table_entry_op->table_entry_dir();
                auto column_defs = add_table_entry_op->column_defs();
                auto entry_type = add_table_entry_op->table_entry_type();
                auto row_count = add_table_entry_op->row_count();
                SegmentID unsealed_id = add_table_entry_op->unsealed_id();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                if (!is_delete) {
                    db_entry->CreateTableReplay(
                        table_name,
                        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableEntry::ReplayTableEntry(table_meta,
                                                                table_entry_dir,
                                                                table_name,
                                                                column_defs,
                                                                entry_type,
                                                                txn_id,
                                                                begin_ts,
                                                                commit_ts,
                                                                false,
                                                                row_count,
                                                                unsealed_id);
                        },
                        [&](TableEntry *table_entry) {
                            table_entry->row_count_ = row_count;
                            table_entry->unsealed_id_ = unsealed_id;
                        },
                        txn_id,
                        begin_ts);
                } else {
                    db_entry->DropTableReplay(
                        *table_name,
                        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableEntry::ReplayTableEntry(table_meta,
                                                                table_entry_dir,
                                                                table_name,
                                                                column_defs,
                                                                entry_type,
                                                                txn_id,
                                                                begin_ts,
                                                                commit_ts,
                                                                true,
                                                                row_count,
                                                                unsealed_id);
                        },
                        txn_id,
                        begin_ts);
                }
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
                auto add_segment_entry_op = static_cast<AddSegmentEntryOp *>(op.get());
                const auto &db_name = add_segment_entry_op->db_name();
                const auto &table_name = add_segment_entry_op->table_name();
                auto segment_id = add_segment_entry_op->segment_id();
                auto segment_status = add_segment_entry_op->status();
                auto column_count = add_segment_entry_op->column_count();
                auto row_count = add_segment_entry_op->row_count();
                auto actual_row_count = add_segment_entry_op->actual_row_count();
                auto row_capacity = add_segment_entry_op->row_capacity();
                auto min_row_ts = add_segment_entry_op->min_row_ts();
                auto max_row_ts = add_segment_entry_op->max_row_ts();
                auto deprecate_ts = add_segment_entry_op->deprecate_ts();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                table_entry->AddSegmentReplay(
                    [&]() {
                        return SegmentEntry::NewReplayCatalogSegmentEntry(table_entry,
                                                                          segment_id,
                                                                          segment_status,
                                                                          column_count,
                                                                          row_count,
                                                                          actual_row_count,
                                                                          row_capacity,
                                                                          min_row_ts,
                                                                          max_row_ts,
                                                                          commit_ts,
                                                                          deprecate_ts,
                                                                          begin_ts,
                                                                          txn_id);
                    },
                    [&](SegmentEntry *segment) {
                        segment->UpdateSegmentInfo(segment_status, row_count, min_row_ts, max_row_ts, commit_ts, begin_ts, txn_id);
                    },
                    segment_id);
                break;
            }
            case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                auto add_block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                const auto &db_name = add_block_entry_op->db_name();
                const auto &table_name = add_block_entry_op->table_name();
                auto segment_id = add_block_entry_op->segment_id();
                auto block_id = add_block_entry_op->block_id();
                auto row_count = add_block_entry_op->row_count();
                auto row_capacity = add_block_entry_op->row_capacity();
                auto min_row_ts = add_block_entry_op->min_row_ts();
                auto max_row_ts = add_block_entry_op->max_row_ts();
                auto check_point_ts = add_block_entry_op->checkpoint_ts();
                auto check_point_row_count = add_block_entry_op->checkpoint_row_count();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                auto segment_entry = table_entry->segment_map_.at(segment_id).get();
                segment_entry->AddBlockReplay(
                    [&]() {
                        return BlockEntry::NewReplayCatalogBlockEntry(segment_entry,
                                                                      block_id,
                                                                      row_count,
                                                                      row_capacity,
                                                                      min_row_ts,
                                                                      max_row_ts,
                                                                      check_point_ts,
                                                                      check_point_row_count,
                                                                      buffer_mgr);
                    },
                    [&](BlockEntry *block) { block->UpdateBlockInfo(row_count, max_row_ts, check_point_ts, check_point_row_count); },
                    block_id);
                break;
            }
            case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
                auto add_column_entry_op = static_cast<AddColumnEntryOp *>(op.get());
                const auto &db_name = add_column_entry_op->db_name();
                const auto &table_name = add_column_entry_op->table_name();
                auto segment_id = add_column_entry_op->segment_id();
                auto block_id = add_column_entry_op->block_id();
                auto column_id = add_column_entry_op->column_id();
                i32 next_outline_idx = add_column_entry_op->next_outline_idx();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                auto *segment_entry = table_entry->segment_map_.at(segment_id).get();
                auto *block_entry = segment_entry->GetBlockEntryByID(block_id);
                block_entry->AddColumnReplay(
                    [&]() { return BlockColumnEntry::NewReplayBlockColumnEntry(block_entry, column_id, buffer_mgr, next_outline_idx); },
                    [&](BlockColumnEntry *column) { column->UpdateColumnInfo(next_outline_idx, buffer_mgr); },
                    column_id);
                break;
            }

            // -----------------------------
            // Index
            // -----------------------------
            case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
                auto add_table_index_entry_op = static_cast<AddTableIndexEntryOp *>(op.get());
                const auto &db_name = add_table_index_entry_op->db_name();
                const auto &table_name = add_table_index_entry_op->table_name();
                const auto &index_name = add_table_index_entry_op->index_name();
                const auto &index_dir = add_table_index_entry_op->index_dir();
                auto index_base = add_table_index_entry_op->index_base();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                if (!is_delete) {
                    table_entry->CreateIndexReplay(
                        index_name,
                        [&](TableIndexMeta *index_meta, const SharedPtr<String> &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableIndexEntry::ReplayTableIndexEntry(index_meta, index_base, index_dir, txn_id, begin_ts, commit_ts, false);
                        },
                        txn_id,
                        begin_ts);
                } else {
                    table_entry->DropIndexReplay(
                        *index_name,
                        [&](TableIndexMeta *index_meta, const SharedPtr<String> &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableIndexEntry::ReplayTableIndexEntry(index_meta, index_base, index_dir, txn_id, begin_ts, commit_ts, true);
                        },
                        txn_id,
                        begin_ts);
                }
                break;
            }
            case CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY: {
                auto add_fulltext_index_entry_op = static_cast<AddFulltextIndexEntryOp *>(op.get());
                const auto &db_name = add_fulltext_index_entry_op->db_name();
                const auto &table_name = add_fulltext_index_entry_op->table_name();
                const auto &index_name = add_fulltext_index_entry_op->index_name();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                auto *table_index_entry = table_entry->GetIndexReplay(*index_name, txn_id, begin_ts);

                auto fulltext_index_entry =
                    FulltextIndexEntry::NewReplayFulltextIndexEntry(table_index_entry, txn_id, begin_ts, commit_ts, is_delete);
                table_index_entry->fulltext_index_entry().swap(fulltext_index_entry);
                if (fulltext_index_entry.get() != nullptr) {
                    UnrecoverableError(fmt::format("Fulltext index {} is already in the catalog", *index_name));
                }
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                auto add_segment_index_entry_op = static_cast<AddSegmentIndexEntryOp *>(op.get());
                const auto &db_name = add_segment_index_entry_op->db_name();
                const auto &table_name = add_segment_index_entry_op->table_name();
                const auto &index_name = add_segment_index_entry_op->index_name();
                auto segment_id = add_segment_index_entry_op->segment_id();
                auto min_ts = add_segment_index_entry_op->min_ts();
                auto max_ts = add_segment_index_entry_op->max_ts();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                if (auto iter = table_entry->segment_map_.find(segment_id); iter != table_entry->segment_map_.end()) {
                    auto *table_index_entry = table_entry->GetIndexReplay(*index_name, txn_id, begin_ts);
                    auto *segment_entry = iter->second.get();
                    if (segment_entry->status() == SegmentStatus::kDeprecated) {
                        UnrecoverableError(fmt::format("Segment {} is deprecated", segment_id));
                    }
                    auto segment_index_entry = SegmentIndexEntry::NewReplaySegmentIndexEntry(table_index_entry,
                                                                                             table_entry,
                                                                                             segment_id,
                                                                                             buffer_mgr,
                                                                                             min_ts,
                                                                                             max_ts,
                                                                                             txn_id,
                                                                                             begin_ts,
                                                                                             commit_ts,
                                                                                             is_delete);
                    bool insert_ok = table_index_entry->index_by_segment().insert({segment_id, std::move(segment_index_entry)}).second;
                    if (!insert_ok) {
                        UnrecoverableError(fmt::format("Segment index {} is already in the catalog", segment_id));
                    }
                }
                break;
            }

            // -----------------------------
            // Segment Status
            // -----------------------------
            case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
                // case: unsealed segment become full in append operation
                auto set_segment_status_sealed_op = static_cast<SetSegmentStatusSealedOp *>(op.get());
                const auto &db_name = set_segment_status_sealed_op->db_name();
                const auto &table_name = set_segment_status_sealed_op->table_name();
                auto segment_id = set_segment_status_sealed_op->segment_id();
                auto const &segment_filter_binary = set_segment_status_sealed_op->segment_filter_binary_data();
                auto const &block_filter_binary = set_segment_status_sealed_op->block_filter_binary_data();

                auto *db_entry = catalog->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                auto segment_entry = table_entry->segment_map_.at(segment_id).get();
                segment_entry->SetSealed();
                segment_entry->LoadFilterBinaryData(segment_filter_binary, block_filter_binary);
                break;
            }

            default:
                UnrecoverableError(fmt::format("Unknown catalog delta op type: {}", op->GetTypeStr()));
        }
    }
}

UniquePtr<Catalog> Catalog::LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr) {
    UniquePtr<Catalog> catalog = nullptr;
    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    SizeT file_size = fs.GetFileSize(*catalog_file_handler);
    String json_str(file_size, 0);
    SizeT n_bytes = catalog_file_handler->Read(json_str.data(), file_size);
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }

    nlohmann::json catalog_json = nlohmann::json::parse(json_str);
    Deserialize(catalog_json, buffer_mgr, catalog);
    return catalog;
}

void Catalog::Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr, UniquePtr<Catalog> &catalog) {
    SharedPtr<String> current_dir = MakeShared<String>(catalog_json["current_dir"]);

    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    catalog = MakeUnique<Catalog>(current_dir);
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->full_ckp_commit_ts_ = catalog_json["full_ckp_commit_ts"];
    catalog->catalog_version_ = catalog_json["catalog_version"];
    if (catalog_json.contains("databases")) {
        for (const auto &db_json : catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->db_meta_map().emplace(*db_meta->db_name(), std::move(db_meta));
        }
    }
}

UniquePtr<String> Catalog::SaveFullCatalog(const String &catalog_dir, TxnTimeStamp max_commit_ts) {
    UniquePtr<String> catalog_path_ptr = MakeUnique<String>(fmt::format("{}/META_CATALOG.{}.json", catalog_dir, max_commit_ts));
    String catalog_tmp_path = String(fmt::format("{}/_META_CATALOG.{}.json", catalog_dir, max_commit_ts));

    // Serialize catalog to string
    full_ckp_commit_ts_ = max_commit_ts;
    nlohmann::json catalog_json = Serialize(max_commit_ts, true);
    String catalog_str = catalog_json.dump();

    // Save catalog to tmp file.
    // FIXME: Temp implementation, will be replaced by async task.
    LocalFileSystem fs;

    u8 fileflags = FileFlags::WRITE_FLAG;

    if (!fs.Exists(catalog_tmp_path)) {
        fileflags |= FileFlags::CREATE_FLAG;
    }

    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_tmp_path, fileflags, FileLockType::kWriteLock);

    // TODO: Save as a temp filename, then rename it to the real filename.
    SizeT n_bytes = catalog_file_handler->Write(catalog_str.data(), catalog_str.size());
    if (n_bytes != catalog_str.size()) {
        LOG_ERROR(fmt::format("Saving catalog file failed: {}", catalog_tmp_path));
        RecoverableError(Status::CatalogCorrupted(catalog_tmp_path));
    }
    catalog_file_handler->Sync();
    catalog_file_handler->Close();

    // Rename temp file to regular catalog file
    catalog_file_handler->Rename(catalog_tmp_path, *catalog_path_ptr);

    LOG_INFO(fmt::format("Saved catalog to: {}", *catalog_path_ptr));
    return catalog_path_ptr;
}

// called by bg_task
bool Catalog::SaveDeltaCatalog(const String &delta_catalog_path, TxnTimeStamp max_commit_ts) {
    LOG_INFO("SAVING DELTA CATALOG");
    LOG_INFO(fmt::format("Save delta catalog commit ts:{}, checkpoint max commit ts:{}.", global_catalog_delta_entry_->commit_ts(), max_commit_ts));

    // Check the SegmentEntry's for flush the data to disk.
    UniquePtr<CatalogDeltaEntry> flush_delta_entry = global_catalog_delta_entry_->PickFlushEntry(max_commit_ts);

    if (flush_delta_entry->operations().empty()) {
        LOG_INFO("Save delta catalog ops is empty. Skip flush.");
        return true;
    }
    HashSet<TransactionID> flushed_unique_txn_ids;
    for (auto &op : flush_delta_entry->operations()) {
        flushed_unique_txn_ids.insert(op->txn_id());

        switch (op->GetType()) {
            case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                auto *block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush block entry: {}", block_entry_op->ToString()));
                block_entry_op->FlushDataToDisk(max_commit_ts);
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                auto add_segment_index_entry_op = static_cast<AddSegmentIndexEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush segment index entry: {}", add_segment_index_entry_op->ToString()));
                add_segment_index_entry_op->Flush(max_commit_ts);
                break;
            }
            default:
                break;
        }
    }

    // Save the global catalog delta entry to disk.
    auto exp_size = flush_delta_entry->GetSizeInBytes();
    Vector<char> buf(exp_size);
    char *ptr = buf.data();
    flush_delta_entry->WriteAdv(ptr);
    i32 act_size = ptr - buf.data();
    if (exp_size != act_size) {
        UnrecoverableError(fmt::format("Save delta catalog failed, exp_size: {}, act_size: {}", exp_size, act_size));
    }

    std::ofstream outfile;
    outfile.open(delta_catalog_path, std::ios::binary);
    outfile.write((reinterpret_cast<const char *>(buf.data())), act_size);
    outfile.close();

    // {
    //     std::stringstream ss;
    //     ss << "Save delta catalog ops: ";
    //     for (auto &op : flush_delta_entry->operations()) {
    //         ss << op->ToString() << ". txn id: " << op->txn_id() << "\n";
    //     }
    //     LOG_INFO(ss.str());
    // }
    LOG_INFO(fmt::format("Save delta catalog to: {}, size: {}.", delta_catalog_path, act_size));

    txn_mgr_->RemoveWaitFlushTxns(Vector<TransactionID>(flushed_unique_txn_ids.begin(), flushed_unique_txn_ids.end()));

    return false;
}

void Catalog::PickCleanup(CleanupScanner *scanner) { db_meta_map_.PickCleanup(scanner); }
} // namespace infinity
