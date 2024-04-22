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
#include <thread>
#include <vector>

module catalog;

import stl;
import defer_op;

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
import segment_index_entry;
import log_file;

namespace infinity {

// TODO Consider letting it commit as a transaction.
Catalog::Catalog(SharedPtr<String> data_dir)
    : data_dir_(std::move(data_dir)), catalog_dir_(MakeShared<String>(*data_dir_ + "/" + String(CATALOG_FILE_DIR))), running_(true) {
    LocalFileSystem fs;
    if (!fs.Exists(*catalog_dir_)) {
        fs.CreateDirectory(*catalog_dir_);
    }
    mem_index_commit_thread_ = Thread([this] { MemIndexCommitLoop(); });
}

Catalog::~Catalog() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("Catalog MemIndexCommitLoop was stopped...");
        return;
    }
    mem_index_commit_thread_.join();
}

void Catalog::SetTxnMgr(TxnManager *txn_mgr) { txn_mgr_ = txn_mgr; }

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
Tuple<DBEntry *, Status>
Catalog::CreateDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    auto init_db_meta = [&]() { return DBMeta::NewDBMeta(data_dir_, MakeShared<String>(db_name)); };
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
    auto init_db_meta = [&]() { return DBMeta::NewDBMeta(this->DataDir(), db_name); };
    LOG_TRACE(fmt::format("Adding new database entry: {}", *db_name));
    auto *db_meta = db_meta_map_.GetMetaNoLock(*db_name, std::move(init_db_meta));
    db_meta->CreateEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(db_meta, db_meta->db_name(), txn_id, begin_ts); },
                               txn_id,
                               begin_ts);
}

void Catalog::DropDatabaseReplay(const String &db_name,
                                 std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts) {
    auto [db_meta, status] = db_meta_map_.GetExistMetaNoLock(db_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    db_meta->DropEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(db_meta, db_meta->db_name(), txn_id, begin_ts); },
                             txn_id,
                             begin_ts);
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

bool Catalog::CheckTableConflict(const String &db_name,
                                 const String &table_name,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts,
                                 TableEntry *&table_entry) {
    auto [db_meta, status, r_lock] = db_meta_map_.GetExistMeta(db_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError("Check conflict is not for non-exist db meta");
    }
    DBEntry *db_entry = nullptr;
    bool conflict = db_meta->CheckConflict(std::move(r_lock), txn_id, begin_ts, db_entry);
    if (conflict) {
        return true;
    }
    return db_entry->CheckConflict(table_name, txn_id, begin_ts, table_entry);
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

Status Catalog::RemoveIndexEntry(TableIndexEntry *table_index_entry, TransactionID txn_id) {
    TableIndexMeta *table_index_meta = table_index_entry->table_index_meta();
    LOG_TRACE(fmt::format("Remove a index entry: {}", *table_index_entry->GetIndexName()));
    table_index_meta->DeleteEntry(txn_id);
    return Status::OK();
}

void Catalog::CommitCreateIndex(TxnIndexStore *txn_index_store, TxnTimeStamp commit_ts, bool is_replay) {
    auto *table_index_entry = txn_index_store->table_index_entry_;
    table_index_entry->CommitCreateIndex(txn_index_store, commit_ts, is_replay);
}

// void Catalog::RollbackPopulateIndex(TxnIndexStore *txn_index_store, Txn *txn) {
//     auto *table_index_entry = txn_index_store->table_index_entry_;
//     table_index_entry->RollbackPopulateIndex(txn_index_store, txn);
// }

void Catalog::Append(TableEntry *table_entry, TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {
    return table_entry->AppendData(txn_id, txn_store, commit_ts, buffer_mgr);
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

nlohmann::json Catalog::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;
    Vector<DBMeta *> databases;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["data_dir"] = *this->data_dir_;
        json_res["next_txn_id"] = this->next_txn_id_;
        json_res["full_ckp_commit_ts"] = this->full_ckp_commit_ts_;
        databases.reserve(this->db_meta_map().size());
        for (auto &db_meta : this->db_meta_map()) {
            databases.push_back(db_meta.second.get());
        }
    }

    for (auto &db_meta : databases) {
        json_res["databases"].emplace_back(db_meta->Serialize(max_commit_ts));
    }
    return json_res;
}

UniquePtr<Catalog> Catalog::NewCatalog(SharedPtr<String> data_dir, bool create_default_db) {
    auto catalog = MakeUnique<Catalog>(data_dir);
    if (create_default_db) {
        // db current dir is same level as catalog
        UniquePtr<DBMeta> db_meta = MakeUnique<DBMeta>(data_dir, MakeShared<String>("default"));
        SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(db_meta.get(), false, db_meta->data_dir(), db_meta->db_name(), 0, 0);
        // TODO commit ts == 0 is true??
        db_entry->commit_ts_ = 0;
        db_meta->db_entry_list().emplace_front(std::move(db_entry));

        catalog->db_meta_map()["default"] = std::move(db_meta);
    }
    return catalog;
}

UniquePtr<Catalog>
Catalog::LoadFromFiles(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos, BufferManager *buffer_mgr) {

    // 1. load json
    // 2. load entries
    LOG_INFO(fmt::format("Load base FULL catalog json from: {}", full_ckp_info.path_));
    auto catalog = Catalog::LoadFromFile(full_ckp_info, buffer_mgr);

    // Load catalogs delta checkpoints and merge.
    TxnTimeStamp max_commit_ts = 0;

    for (const auto &delta_ckp_info : delta_ckp_infos) {
        LOG_INFO(fmt::format("Load catalog DELTA entry binary from: {}", delta_ckp_info.path_));
        auto catalog_delta_entry = Catalog::LoadFromFileDelta(delta_ckp_info);
        max_commit_ts = std::max(max_commit_ts, catalog_delta_entry->commit_ts());
        catalog->ReplayDeltaEntry(std::move(catalog_delta_entry));
    }
    catalog->LoadFromEntryDelta(max_commit_ts, buffer_mgr);

    LOG_TRACE(fmt::format("Catalog Delta Op is done"));

    return catalog;
}

// called by Replay
UniquePtr<CatalogDeltaEntry> Catalog::LoadFromFileDelta(const DeltaCatalogFileInfo &delta_ckp_info) {
    const auto &catalog_path = delta_ckp_info.path_;

    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    i32 file_size = fs.GetFileSize(*catalog_file_handler);
    Vector<char> buf(file_size);
    fs.Read(*catalog_file_handler, buf.data(), file_size);
    fs.Close(*catalog_file_handler);
    char *ptr = buf.data();
    auto catalog_delta_entry = CatalogDeltaEntry::ReadAdv(ptr, file_size);
    if (catalog_delta_entry.get() == nullptr) {
        UnrecoverableError(fmt::format("Load catalog delta entry failed: {}", catalog_path));
    }
    i32 n_bytes = catalog_delta_entry->GetSizeInBytes();
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }
    return catalog_delta_entry;
}

void Catalog::LoadFromEntryDelta(TxnTimeStamp max_commit_ts, BufferManager *buffer_mgr) {
    auto delta_entry = global_catalog_delta_entry_->PickFlushEntry(full_ckp_commit_ts_, max_commit_ts);

    auto &delta_ops = delta_entry->operations();
    for (auto &op : delta_ops) {
        auto type = op->GetType();
        LOG_TRACE(fmt::format("Catalog Delta Op is {}", op->ToString()));
        auto commit_ts = op->commit_ts_;
        auto txn_id = op->txn_id_;
        auto begin_ts = op->begin_ts_;
        MergeFlag merge_flag = op->merge_flag_;
        if (op->commit_ts_ < full_ckp_commit_ts_) {
            // Ignore the old txn
            continue;
        }
        switch (type) {

            // -----------------------------
            // Entry
            // -----------------------------
            case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
                auto add_db_entry_op = static_cast<AddDBEntryOp *>(op.get());
                const auto &db_name = add_db_entry_op->db_name_;
                const auto &db_entry_dir = add_db_entry_op->db_entry_dir_;
                if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kDeleteAndNew) {
                    this->DropDatabaseReplay(
                        *db_name,
                        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return DBEntry::ReplayDBEntry(db_meta, true, db_entry_dir, db_name, txn_id, begin_ts, commit_ts);
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                    this->CreateDatabaseReplay(
                        db_name,
                        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return DBEntry::ReplayDBEntry(db_meta, false, db_entry_dir, db_name, txn_id, begin_ts, commit_ts);
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kUpdate) {
                    UnrecoverableError("Update database entry is not supported.");
                }
                break;
            }
            case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
                auto add_table_entry_op = static_cast<AddTableEntryOp *>(op.get());
                const auto &db_name = add_table_entry_op->db_name_;
                const auto &table_name = add_table_entry_op->table_name_;
                const auto &table_entry_dir = add_table_entry_op->table_entry_dir_;
                auto column_defs = add_table_entry_op->column_defs_;
                auto entry_type = add_table_entry_op->table_entry_type_;
                auto row_count = add_table_entry_op->row_count_;
                SegmentID unsealed_id = add_table_entry_op->unsealed_id_;
                SegmentID next_segment_id = add_table_entry_op->next_segment_id_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kDeleteAndNew) {
                    // Actually the table entry replayed doesn't have full information cause index entry are lacked, but that is ok for now.
                    db_entry->DropTableReplay(
                        *table_name,
                        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableEntry::ReplayTableEntry(true,
                                                                table_meta,
                                                                table_entry_dir,
                                                                table_name,
                                                                column_defs,
                                                                entry_type,
                                                                txn_id,
                                                                begin_ts,
                                                                commit_ts,
                                                                row_count,
                                                                unsealed_id,
                                                                next_segment_id);
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                    db_entry->CreateTableReplay(
                        table_name,
                        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableEntry::ReplayTableEntry(false,
                                                                table_meta,
                                                                table_entry_dir,
                                                                table_name,
                                                                column_defs,
                                                                entry_type,
                                                                txn_id,
                                                                begin_ts,
                                                                commit_ts,
                                                                row_count,
                                                                unsealed_id,
                                                                next_segment_id);
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kUpdate) {
                    db_entry->UpdateTableReplay(
                        table_name,
                        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableEntry::ReplayTableEntry(false,
                                                                table_meta,
                                                                table_entry_dir,
                                                                table_name,
                                                                column_defs,
                                                                entry_type,
                                                                txn_id,
                                                                begin_ts,
                                                                commit_ts,
                                                                row_count,
                                                                unsealed_id,
                                                                next_segment_id);
                        },
                        txn_id,
                        begin_ts);
                }
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
                auto add_segment_entry_op = static_cast<AddSegmentEntryOp *>(op.get());
                const auto &db_name = add_segment_entry_op->db_name_;
                const auto &table_name = add_segment_entry_op->table_name_;
                auto segment_id = add_segment_entry_op->segment_id_;
                auto segment_status = add_segment_entry_op->status_;
                auto column_count = add_segment_entry_op->column_count_;
                auto row_count = add_segment_entry_op->row_count_;
                auto actual_row_count = add_segment_entry_op->actual_row_count_;
                auto row_capacity = add_segment_entry_op->row_capacity_;
                auto min_row_ts = add_segment_entry_op->min_row_ts_;
                auto max_row_ts = add_segment_entry_op->max_row_ts_;
                auto deprecate_ts = add_segment_entry_op->deprecate_ts_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                table_entry->AddSegmentReplay(
                    [&]() {
                        auto segment = SegmentEntry::NewReplaySegmentEntry(table_entry,
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
                        return segment;
                    },
                    segment_id);
                break;
            }
            case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                auto add_block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                const auto &db_name = add_block_entry_op->db_name_;
                const auto &table_name = add_block_entry_op->table_name_;
                auto segment_id = add_block_entry_op->segment_id_;
                auto block_id = add_block_entry_op->block_id_;
                auto row_count = add_block_entry_op->row_count_;
                auto row_capacity = add_block_entry_op->row_capacity_;
                auto min_row_ts = add_block_entry_op->min_row_ts_;
                auto max_row_ts = add_block_entry_op->max_row_ts_;
                auto check_point_ts = add_block_entry_op->checkpoint_ts_;
                auto check_point_row_count = add_block_entry_op->checkpoint_row_count_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                auto segment_entry = table_entry->segment_map_.at(segment_id).get();
                segment_entry->AddBlockReplay(BlockEntry::NewReplayBlockEntry(segment_entry,
                                                                              block_id,
                                                                              row_count,
                                                                              row_capacity,
                                                                              min_row_ts,
                                                                              max_row_ts,
                                                                              commit_ts,
                                                                              check_point_ts,
                                                                              check_point_row_count,
                                                                              buffer_mgr),
                                              block_id);
                break;
            }
            case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
                auto add_column_entry_op = static_cast<AddColumnEntryOp *>(op.get());
                const auto &db_name = add_column_entry_op->db_name_;
                const auto &table_name = add_column_entry_op->table_name_;
                auto segment_id = add_column_entry_op->segment_id_;
                auto block_id = add_column_entry_op->block_id_;
                auto column_id = add_column_entry_op->column_id_;
                i32 next_outline_idx = add_column_entry_op->next_outline_idx_;
                u64 last_chunk_offset = add_column_entry_op->last_chunk_offset_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                auto *segment_entry = table_entry->segment_map_.at(segment_id).get();
                auto *block_entry = segment_entry->GetBlockEntryByID(block_id).get();
                block_entry->AddColumnReplay(
                    BlockColumnEntry::NewReplayBlockColumnEntry(block_entry, column_id, buffer_mgr, next_outline_idx, last_chunk_offset, commit_ts),
                    column_id);
                break;
            }

            // -----------------------------
            // Index
            // -----------------------------
            case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
                auto add_table_index_entry_op = static_cast<AddTableIndexEntryOp *>(op.get());
                const auto &db_name = add_table_index_entry_op->db_name_;
                const auto &table_name = add_table_index_entry_op->table_name_;
                const auto &index_name = add_table_index_entry_op->index_name_;
                const auto &index_dir = add_table_index_entry_op->index_dir_;
                auto index_base = add_table_index_entry_op->index_base_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kDeleteAndNew) {
                    table_entry->DropIndexReplay(
                        *index_name,
                        [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            auto index_entry = TableIndexEntry::NewTableIndexEntry(nullptr, true, nullptr, index_meta, txn_id, begin_ts);
                            index_entry->commit_ts_.store(commit_ts);
                            return index_entry;
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                    table_entry->CreateIndexReplay(
                        index_name,
                        [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
                            return TableIndexEntry::ReplayTableIndexEntry(index_meta, false, index_base, index_dir, txn_id, begin_ts, commit_ts);
                        },
                        txn_id,
                        begin_ts);
                }
                if (merge_flag == MergeFlag::kUpdate) {
                    table_entry->UpdateIndexReplay(*index_name, txn_id, begin_ts, commit_ts);
                }
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                auto add_segment_index_entry_op = static_cast<AddSegmentIndexEntryOp *>(op.get());
                const auto &db_name = add_segment_index_entry_op->db_name_;
                const auto &table_name = add_segment_index_entry_op->table_name_;
                const auto &index_name = add_segment_index_entry_op->index_name_;
                auto segment_id = add_segment_index_entry_op->segment_id_;
                auto min_ts = add_segment_index_entry_op->min_ts_;
                auto max_ts = add_segment_index_entry_op->max_ts_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
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
                                                                                             commit_ts);
                    bool insert_ok = table_index_entry->index_by_segment().insert({segment_id, std::move(segment_index_entry)}).second;
                    if (!insert_ok) {
                        UnrecoverableError(fmt::format("Segment index {} is already in the catalog", segment_id));
                    }
                }
                break;
            }
            case CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY: {
                auto add_chunk_index_entry_op = static_cast<AddChunkIndexEntryOp *>(op.get());
                const auto &db_name = add_chunk_index_entry_op->db_name_;
                const auto &table_name = add_chunk_index_entry_op->table_name_;
                const auto &index_name = add_chunk_index_entry_op->index_name_;
                auto segment_id = add_chunk_index_entry_op->segment_id_;
                const auto &base_name = add_chunk_index_entry_op->base_name_;
                auto base_rowid = add_chunk_index_entry_op->base_rowid_;
                auto row_count = add_chunk_index_entry_op->row_count_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);

                if (auto iter = table_entry->segment_map_.find(segment_id); iter != table_entry->segment_map_.end()) {
                    auto *table_index_entry = table_entry->GetIndexReplay(*index_name, txn_id, begin_ts);
                    auto *segment_entry = iter->second.get();
                    if (segment_entry->status() == SegmentStatus::kDeprecated) {
                        UnrecoverableError(fmt::format("Segment {} is deprecated", segment_id));
                    }
                    auto iter2 = table_index_entry->index_by_segment().find(segment_id);
                    if (iter2 == table_index_entry->index_by_segment().end()) {
                        UnrecoverableError(fmt::format("Segment index {} is not found", segment_id));
                    }
                    auto *segment_index_entry = iter2->second.get();
                    segment_index_entry->AddChunkIndexEntryReplay(table_entry, base_name, base_rowid, row_count, buffer_mgr);
                }
                break;
            }
            case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
                auto *set_segment_status_sealed_op = static_cast<SetSegmentStatusSealedOp *>(op.get());
                const auto &db_name = set_segment_status_sealed_op->db_name_;
                const auto &table_name = set_segment_status_sealed_op->table_name_;
                auto segment_id = set_segment_status_sealed_op->segment_id_;
                const auto &segment_filter_binary = set_segment_status_sealed_op->segment_filter_binary_data_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                auto *segment_entry = table_entry->segment_map_.at(segment_id).get();
                segment_entry->SetSealed(); // ignore the return value.
                segment_entry->LoadFilterBinaryData(segment_filter_binary);
                break;
            }
            case CatalogDeltaOpType::SET_BLOCK_STATUS_SEALED: {
                auto *set_block_status_sealed_op = static_cast<SetBlockStatusSealedOp *>(op.get());
                const auto &db_name = set_block_status_sealed_op->db_name_;
                const auto &table_name = set_block_status_sealed_op->table_name_;
                auto segment_id = set_block_status_sealed_op->segment_id_;
                auto block_id = set_block_status_sealed_op->block_id_;
                const auto &block_filter_binary = set_block_status_sealed_op->block_filter_binary_data_;

                auto *db_entry = this->GetDatabaseReplay(*db_name, txn_id, begin_ts);
                auto *table_entry = db_entry->GetTableReplay(*table_name, txn_id, begin_ts);
                auto *segment_entry = table_entry->segment_map_.at(segment_id).get();
                auto *block_entry = segment_entry->GetBlockEntryByID(block_id).get();
                block_entry->LoadFilterBinaryData(block_filter_binary);
                break;
            }
            default:
                UnrecoverableError(fmt::format("Unknown catalog delta op type: {}", op->GetTypeStr()));
        }
    }
}

UniquePtr<Catalog> Catalog::LoadFromFile(const FullCatalogFileInfo &full_ckp_info, BufferManager *buffer_mgr) {
    const auto &catalog_path = full_ckp_info.path_;

    LocalFileSystem fs;
    UniquePtr<FileHandler> catalog_file_handler = fs.OpenFile(catalog_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    SizeT file_size = fs.GetFileSize(*catalog_file_handler);
    String json_str(file_size, 0);
    SizeT n_bytes = catalog_file_handler->Read(json_str.data(), file_size);
    if (file_size != n_bytes) {
        RecoverableError(Status::CatalogCorrupted(catalog_path));
    }

    nlohmann::json catalog_json = nlohmann::json::parse(json_str);
    return Deserialize(catalog_json, buffer_mgr);
}

UniquePtr<Catalog> Catalog::Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(catalog_json["data_dir"]);

    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    auto catalog = MakeUnique<Catalog>(std::move(data_dir));
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->full_ckp_commit_ts_ = catalog_json["full_ckp_commit_ts"];
    if (catalog_json.contains("databases")) {
        for (const auto &db_json : catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->db_meta_map().emplace(*db_meta->db_name(), std::move(db_meta));
        }
    }
    return catalog;
}

void Catalog::SaveFullCatalog(TxnTimeStamp max_commit_ts, String &full_catalog_path) {
    full_catalog_path = fmt::format("{}/{}", *catalog_dir_, CatalogFile::FullCheckpoingFilename(max_commit_ts));
    String catalog_tmp_path = fmt::format("{}/{}", *catalog_dir_, CatalogFile::TempFullCheckpointFilename(max_commit_ts));

    // Serialize catalog to string
    full_ckp_commit_ts_ = max_commit_ts;
    nlohmann::json catalog_json = Serialize(max_commit_ts);
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
    catalog_file_handler->Rename(catalog_tmp_path, full_catalog_path);

    global_catalog_delta_entry_->InitFullCheckpointTs(max_commit_ts);

    LOG_INFO(fmt::format("Saved catalog to: {}", full_catalog_path));
}

// called by bg_task
bool Catalog::SaveDeltaCatalog(TxnTimeStamp max_commit_ts, String &delta_catalog_path) {
    delta_catalog_path = fmt::format("{}/{}", *catalog_dir_, CatalogFile::DeltaCheckpointFilename(max_commit_ts));

    // Check the SegmentEntry's for flush the data to disk.
    UniquePtr<CatalogDeltaEntry> flush_delta_entry = global_catalog_delta_entry_->PickFlushEntry(full_ckp_commit_ts_, max_commit_ts);

    DeferFn defer_fn([&]() { txn_mgr_->RemoveWaitFlushTxns(flush_delta_entry->txn_ids()); });

    if (flush_delta_entry->operations().empty()) {
        LOG_TRACE("Save delta catalog ops is empty. Skip flush.");
        return true;
    }
    LOG_INFO(fmt::format("Save delta catalog commit ts:{}, checkpoint max commit ts:{}.", flush_delta_entry->commit_ts(), max_commit_ts));

    for (auto &op : flush_delta_entry->operations()) {
        switch (op->GetType()) {
            case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                auto *block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush block entry: {}", block_entry_op->ToString()));
                block_entry_op->FlushDataToDisk(max_commit_ts);
                LOG_TRACE(fmt::format("Flush block entry done"));
                break;
            }
            case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                auto add_segment_index_entry_op = static_cast<AddSegmentIndexEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush segment index entry: {}", add_segment_index_entry_op->ToString()));
                add_segment_index_entry_op->FlushDataToDisk(max_commit_ts);
                LOG_TRACE(fmt::format("Flush segment index entry done"));
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
    // log for delta op debug
    //     std::stringstream ss;
    //     ss << "Save delta catalog ops: ";
    //     for (auto &op : flush_delta_entry->operations()) {
    //         ss << op->ToString() << ". txn id: " << op->txn_id_ << "\n";
    //     }
    //     LOG_INFO(ss.str());
    // }
    LOG_INFO(fmt::format("Save delta catalog to: {}, size: {}.", delta_catalog_path, act_size));

    return false;
}

void Catalog::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size) {
    global_catalog_delta_entry_->AddDeltaEntry(std::move(delta_entry), wal_size);
}

void Catalog::ReplayDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry) { global_catalog_delta_entry_->ReplayDeltaEntry(std::move(delta_entry)); }

void Catalog::PickCleanup(CleanupScanner *scanner) { db_meta_map_.PickCleanup(scanner); }

void Catalog::MemIndexCommit() {
    auto db_meta_map_guard = db_meta_map_.GetMetaMap();
    for (auto &[_, db_meta] : *db_meta_map_guard) {
        auto [db_entry, status] = db_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            db_entry->MemIndexCommit();
        }
    }
}

void Catalog::MemIndexCommitLoop() {
    auto prev_time = std::chrono::system_clock::now();
    while (running_.load()) {
        auto cur_time = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(cur_time - prev_time).count() < 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        prev_time = cur_time;
        MemIndexCommit();
    }
}

void Catalog::MemIndexRecover(BufferManager *buffer_manager) {
    auto db_meta_map_guard = db_meta_map_.GetMetaMap();
    for (auto &[_, db_meta] : *db_meta_map_guard) {
        auto [db_entry, status] = db_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            db_entry->MemIndexRecover(buffer_manager);
        }
    }
}

Tuple<TxnTimeStamp, i64> Catalog::GetCheckpointState() const { return global_catalog_delta_entry_->GetCheckpointState(); }

void Catalog::InitDeltaEntry(TxnTimeStamp max_commit_ts) { global_catalog_delta_entry_->InitMaxCommitTS(max_commit_ts); }

} // namespace infinity
