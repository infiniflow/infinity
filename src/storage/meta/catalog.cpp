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

#include <cassert>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

module catalog;

import stl;
import defer_op;
import data_type;
import txn_manager;
import logger;
import third_party;
import status;
import infinity_exception;
import function_set;
import scalar_function_set;
import table_function;
import special_function;
import buffer_manager;
import column_def;
import virtual_store;
import table_def;
import table_entry_type;
import meta_info;
import index_base;
import txn_store;
import data_access_state;
import catalog_delta_entry;
import file_writer;
import extra_ddl_info;
import index_defines;
import infinity_context;
import create_index_info;
import persistence_manager;

import table_meta;
import table_index_meta;
import base_entry;
import block_entry;
import block_column_entry;
import segment_index_entry;
import chunk_index_entry;
import log_file;
import persist_result_handler;
import local_file_handle;
import admin_statement;
import global_resource_usage;
import snapshot_info;

import new_txn_manager;
import new_catalog;
import new_txn;
import txn_state;
import kv_store;
import config;

namespace infinity {

// TODO Consider letting it commit as a transaction.
Catalog::Catalog() : catalog_dir_(MakeShared<String>(CATALOG_FILE_DIR)), running_(true) {
    String abs_catalog_dir = Path(InfinityContext::instance().config()->DataDir()) / String(CATALOG_FILE_DIR);
    if (!VirtualStore::Exists(abs_catalog_dir)) {
        VirtualStore::MakeDirectory(abs_catalog_dir);
    }

    ResizeProfileHistory(DEFAULT_PROFILER_HISTORY_SIZE);

#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Catalog");
#endif
}

Catalog::~Catalog() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("Catalog MemIndexCommitLoop was stopped...");
        return;
    }

    if (mem_index_commit_thread_.get() != nullptr) {
        mem_index_commit_thread_->join();
        mem_index_commit_thread_.reset();
    }

#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Catalog");
#endif
}

// do not only use this method to create database
// it will not record database in transaction, so when you commit transaction
// it will lose operation
// use Txn::CreateDatabase instead
Tuple<DBEntry *, Status> Catalog::CreateDatabase(const SharedPtr<String> &db_name,
                                                 const SharedPtr<String> &comment,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr,
                                                 ConflictType conflict_type) {
    auto init_db_meta = [&]() { return DBMeta::NewDBMeta(db_name); };
    LOG_TRACE(fmt::format("Adding new database entry: {}", *db_name));
    auto [db_meta, r_lock] = this->db_meta_map_.GetMeta(*db_name, std::move(init_db_meta));
    return db_meta->CreateNewEntry(std::move(r_lock), comment, txn_id, begin_ts, txn_mgr, conflict_type);
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

void Catalog::CreateDatabaseReplay(
    const SharedPtr<String> &db_name,
    const SharedPtr<String> &comment,
    std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    TransactionID txn_id,
    TxnTimeStamp begin_ts) {

    auto init_db_meta = [&]() { return DBMeta::NewDBMeta(db_name); };
    LOG_TRACE(fmt::format("Adding new database entry: {}", *db_name));
    auto *db_meta = db_meta_map_.GetMetaNoLock(*db_name, std::move(init_db_meta));
    db_meta->CreateEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(db_meta, db_name, comment, txn_id, begin_ts); },
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
    Vector<DBEntry *> res;
    res.reserve(db_meta_map_.Size());

    {
        auto [_, db_meta_ptrs, meta_lock] = db_meta_map_.GetAllMetaGuard();
        for (const auto &db_meta_ptr : db_meta_ptrs) {
            auto [db_entry, status] = db_meta_ptr->GetEntryNolock(txn_id, begin_ts);
            if (status.ok()) {
                res.emplace_back(db_entry);
            }
        }
    }
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

    return db_entry->CreateTable(TableEntryType::kTableEntry,
                                 table_def->table_name(),
                                 table_def->table_comment(),
                                 table_def->columns(),
                                 txn_id,
                                 begin_ts,
                                 txn_mgr,
                                 conflict_type);
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

Status Catalog::GetTables(const String &db_name, Vector<TableDetail> &output_table_array, Txn *txn) {
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();
    // Check the db entries
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return status;
    }
    return db_entry->GetTablesDetail(txn, output_table_array);
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

Tuple<SharedPtr<TableSnapshotInfo>, Status> Catalog::GetTableSnapshot(const String &db_name, const String &table_name, Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Fail to get table {} from database: {}, error message: {}.", table_name, db_name, table_status.message()));
        return {nullptr, table_status};
    }
    return {table_entry->GetSnapshotInfo(txn_ptr), Status::OK()};
}

Status Catalog::ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info, Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [db_entry, status] = this->GetDatabase(table_snapshot_info->db_name_, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", table_snapshot_info->db_name_));
        return status;
    }

    return db_entry->ApplyTableSnapshot(table_snapshot_info, txn_ptr->TxnID(), txn_ptr->BeginTS());
}

Tuple<SharedPtr<TableInfo>, Status> Catalog::GetTableInfo(const String &db_name, const String &table_name, Txn *txn) {
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();
    auto [db_entry, status] = this->GetDatabase(db_name, txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", db_name));
        return {nullptr, status};
    }

    return db_entry->GetTableInfo(table_name, txn);
}

Tuple<SharedPtr<SegmentInfo>, Status> Catalog::GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id, Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }

    SharedPtr<SegmentInfo> segment_info = table_entry->GetSegmentInfo(segment_id, txn_ptr);
    if (segment_info == nullptr) {
        return {nullptr, Status::SegmentNotExist(segment_id)};
    }

    return {segment_info, Status::OK()};
}

Tuple<Vector<SharedPtr<SegmentInfo>>, Status> Catalog::GetSegmentsInfo(const String &db_name, const String &table_name, Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {Vector<SharedPtr<SegmentInfo>>(), table_status};
    }

    return {table_entry->GetSegmentsInfo(txn_ptr), Status::OK()};
}

Tuple<SharedPtr<BlockInfo>, Status>
Catalog::GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }

    auto segment_entry = table_entry->GetSegmentByID(segment_id, txn_ptr->BeginTS());
    if (!segment_entry) {
        return {nullptr, Status::SegmentNotExist(segment_id)};
    }

    auto block_info = segment_entry->GetBlockInfo(block_id, txn_ptr);
    if (!block_info) {
        return {nullptr, Status::BlockNotExist(block_id)};
    }

    return {block_info, Status::OK()};
}

Tuple<Vector<SharedPtr<BlockInfo>>, Status>
Catalog::GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id, Txn *txn_ptr) {

    Vector<SharedPtr<BlockInfo>> null_result;
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {null_result, table_status};
    }

    auto segment_entry = table_entry->GetSegmentByID(segment_id, txn_ptr->BeginTS());
    if (!segment_entry) {
        return {null_result, Status::SegmentNotExist(segment_id)};
    }

    return {segment_entry->GetBlocksInfo(txn_ptr), Status::OK()};
}

Tuple<SharedPtr<BlockColumnInfo>, Status> Catalog::GetBlockColumnInfo(const String &db_name,
                                                                      const String &table_name,
                                                                      SegmentID segment_id,
                                                                      BlockID block_id,
                                                                      ColumnID column_id,
                                                                      Txn *txn_ptr) {
    TransactionID txn_id = txn_ptr->TxnID();
    TxnTimeStamp begin_ts = txn_ptr->BeginTS();
    auto [table_entry, table_status] = this->GetTableByName(db_name, table_name, txn_id, begin_ts);
    if (!table_status.ok()) {
        return {nullptr, table_status};
    }

    auto segment_entry = table_entry->GetSegmentByID(segment_id, txn_ptr->BeginTS());
    if (!segment_entry) {
        return {nullptr, Status::SegmentNotExist(segment_id)};
    }

    return segment_entry->GetBlockColumnInfo(block_id, column_id, txn_ptr);
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

void Catalog::Append(TableEntry *table_entry,
                     TransactionID txn_id,
                     void *txn_store,
                     TxnTimeStamp commit_ts,
                     BufferManager *buffer_mgr,
                     bool is_replay) {
    return table_entry->AppendData(txn_id, txn_store, commit_ts, buffer_mgr, is_replay);
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
                            const HashMap<SegmentID, TxnSegmentStore> &segment_stores,
                            const DeleteState *delete_state) {
    return table_entry->CommitWrite(txn_id, commit_ts, segment_stores, delete_state);
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
        Status status = Status::FunctionNotFound(function_name);
        RecoverableError(status);
    }
    return catalog->function_sets_[function_name];
}

void Catalog::AddFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        String error_message = fmt::format("Trying to add duplicated function {} into catalog", name);
        UnrecoverableError(error_message);
    }
    catalog->function_sets_.emplace(name, function_set);
}

void Catalog::AppendToScalarFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (!catalog->function_sets_.contains(name)) {
        String error_message = fmt::format("Trying to append to non-existent function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    auto target_scalar_function_set = std::dynamic_pointer_cast<ScalarFunctionSet>(catalog->function_sets_[name]);
    if (!target_scalar_function_set) {
        String error_message = fmt::format("Trying to append to non-scalar function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    auto source_function_set = std::dynamic_pointer_cast<ScalarFunctionSet>(function_set);
    if (!source_function_set) {
        String error_message = fmt::format("Trying to append non-scalar function to scalar function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    for (const auto &function : source_function_set->GetAllScalarFunctions()) {
        target_scalar_function_set->AddFunction(function);
    }
}

void Catalog::AddSpecialFunction(Catalog *catalog, const SharedPtr<SpecialFunction> &special_function) {
    String name = special_function->name();
    StringToLower(name);
    if (catalog->special_functions_.contains(name)) {
        String error_message = fmt::format("Trying to add duplicated special function into catalog: {}", name);
        UnrecoverableError(error_message);
    }
    catalog->special_functions_.emplace(name, special_function);
    switch (special_function->special_type()) {
        case SpecialType::kRowID:
        case SpecialType::kDistance:
        case SpecialType::kDistanceFactors:
        case SpecialType::kSimilarity:
        case SpecialType::kSimilarityFactors:
        case SpecialType::kScore:
        case SpecialType::kScoreFactors:
        case SpecialType::kFilterFullText: {
            return;
        }
        case SpecialType::kCreateTs:
        case SpecialType::kDeleteTs: {
            break;
        }
    }
    auto special_column_def = MakeUnique<ColumnDef>(special_function->extra_idx(),
                                                    MakeShared<DataType>(special_function->data_type()),
                                                    special_function->name(),
                                                    std::set<ConstraintType>());
    catalog->special_columns_.emplace(name, std::move(special_column_def));
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
    TransactionID next_txn_id = this->next_txn_id_;
    json_res["next_txn_id"] = next_txn_id;
    json_res["full_ckp_commit_ts"] = this->full_ckp_commit_ts_;

    {
        auto [_, db_meta_ptrs, meta_lock] = db_meta_map_.GetAllMetaGuard();
        for (DBMeta *db_meta_ptr : db_meta_ptrs) {
            json_res["databases"].emplace_back(db_meta_ptr->Serialize(max_commit_ts));
        }
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        PersistResultHandler handler(pm);
        // Finalize current object to ensure PersistenceManager be in a consistent state
        PersistWriteResult result = pm->CurrentObjFinalize(true);
        handler.HandleWriteResult(result);

        // json_res["obj_addr_map"] = pm->Serialize();
    }
    return json_res;
}

UniquePtr<Catalog> Catalog::NewCatalog() {
    auto catalog = MakeUnique<Catalog>();
    return catalog;
}

// called by Replay
UniquePtr<CatalogDeltaEntry> Catalog::LoadFromFileDelta(const String &catalog_path, PersistenceManager *pm_ptr) {
    VirtualStore::AddRequestCount();
    if (!VirtualStore::Exists(catalog_path)) {
        std::filesystem::path filePath = catalog_path;
        String dst_file_name = filePath.filename();
        VirtualStore::DownloadObject(catalog_path, dst_file_name);
        VirtualStore::AddCacheMissCount();
    }

    auto [catalog_file_handle, status] = VirtualStore::Open(catalog_path, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("{}:{}", catalog_path, status.message()));
    }

    i64 file_size = catalog_file_handle->FileSize();
    Vector<char> buf(file_size);
    catalog_file_handle->Read(buf.data(), file_size);

    const char *ptr = buf.data();
    auto catalog_delta_entry = CatalogDeltaEntry::ReadAdv(ptr, file_size, pm_ptr);
    if (catalog_delta_entry.get() == nullptr) {
        String error_message = fmt::format("Load catalog delta entry failed: {}", catalog_path);
        UnrecoverableError(error_message);
    }
    i32 n_bytes = catalog_delta_entry->GetSizeInBytes();
    if (file_size != n_bytes && file_size != ptr - buf.data()) {
        Status status = Status::FileCorrupted(catalog_path);
        RecoverableError(status);
    }
    return catalog_delta_entry;
}

UniquePtr<nlohmann::json> Catalog::LoadFullCheckpointToJson(Config *config_ptr, const String &file_name) {
    const auto &catalog_path = Path(config_ptr->DataDir()) / file_name;
    String dst_dir = catalog_path.parent_path().string();
    String dst_file_name = catalog_path.filename().string();

    if (!VirtualStore::Exists(dst_dir)) {
        VirtualStore::MakeDirectory(dst_dir);
    }

    VirtualStore::AddRequestCount();
    if (!VirtualStore::Exists(catalog_path)) {
        VirtualStore::DownloadObject(catalog_path, dst_file_name);
        VirtualStore::AddCacheMissCount();
    }

    auto [catalog_file_handle, status] = VirtualStore::Open(catalog_path, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    i64 file_size = catalog_file_handle->FileSize();
    String json_str(file_size, 0);
    auto [n_bytes, status_read] = catalog_file_handle->Read(json_str.data(), file_size);
    if (!status.ok()) {
        RecoverableError(status_read);
    }
    if ((SizeT)file_size != n_bytes) {
        Status status = Status::FileCorrupted(catalog_path);
        RecoverableError(status);
    }

    return MakeUnique<nlohmann::json>(nlohmann::json::parse(json_str));
}

UniquePtr<Catalog> Catalog::LoadFullCheckpoint(const String &file_name) {
    UniquePtr<nlohmann::json> catalog_json = LoadFullCheckpointToJson(InfinityContext::instance().config(), file_name);
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    return Deserialize(*catalog_json, buffer_mgr);
}

UniquePtr<Catalog> Catalog::Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr) {
    // FIXME: new catalog need a scheduler, current we use nullptr to represent it.
    auto catalog = MakeUnique<Catalog>();
    catalog->next_txn_id_ = catalog_json["next_txn_id"];
    catalog->full_ckp_commit_ts_ = catalog_json["full_ckp_commit_ts"];
    if (catalog_json.contains("databases")) {
        for (const auto &db_json : catalog_json["databases"]) {
            UniquePtr<DBMeta> db_meta = DBMeta::Deserialize(db_json, buffer_mgr);
            catalog->db_meta_map_.AddNewMetaNoLock(*db_meta->db_name(), std::move(db_meta));
        }
    }
    if (catalog_json.contains("obj_addr_map")) {
        PersistenceManager *pm = InfinityContext::instance().persistence_manager();
        if (pm != nullptr) {
            // pm->Deserialize(catalog_json["obj_addr_map"]);
        }
    }
    return catalog;
}

void Catalog::SaveFullCatalog(TxnTimeStamp max_commit_ts, String &full_catalog_path, String &full_catalog_name) {
    full_catalog_path = *catalog_dir_;
    full_catalog_name = CatalogFile::FullCheckpointFilename(max_commit_ts);
    String full_path = Path(InfinityContext::instance().config()->DataDir()) / *catalog_dir_ / full_catalog_name;
    String catalog_tmp_path =
        Path(InfinityContext::instance().config()->DataDir()) / *catalog_dir_ / CatalogFile::TempFullCheckpointFilename(max_commit_ts);

    // Serialize catalog to string
    full_ckp_commit_ts_ = max_commit_ts;
    nlohmann::json catalog_json = Serialize(max_commit_ts);
    String catalog_str = catalog_json.dump();

    // Save catalog to tmp file.
    // FIXME: Temp implementation, will be replaced by async task.
    auto [catalog_file_handle, status] = VirtualStore::Open(catalog_tmp_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("{}: {}", catalog_tmp_path, status.message()));
    }

    status = catalog_file_handle->Append(catalog_str.data(), catalog_str.size());
    if (!status.ok()) {
        RecoverableError(status);
    }
    catalog_file_handle->Sync();

    // Rename temp file to regular catalog file
    VirtualStore::Rename(catalog_tmp_path, full_path);
    if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader or InfinityContext::instance().GetServerRole() == NodeRole::kStandalone) {
        VirtualStore::UploadObject(full_path, full_catalog_name);
    }

    global_catalog_delta_entry_->SetFullCheckpointTs(max_commit_ts);

    LOG_DEBUG(fmt::format("Saved catalog to: {}", full_path));
}

// called by bg_task
bool Catalog::SaveDeltaCatalog(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts, String &delta_catalog_path, String &delta_catalog_name) {
    // Pick the delta entry to flush and set the max commit ts.
    UniquePtr<CatalogDeltaEntry> flush_delta_entry = global_catalog_delta_entry_->PickFlushEntry(max_commit_ts);
    if (last_ckp_ts >= max_commit_ts) {
        return false;
    }

    delta_catalog_path = *catalog_dir_;
    delta_catalog_name = CatalogFile::DeltaCheckpointFilename(max_commit_ts);
    String delta_path = fmt::format("{}/{}/{}", InfinityContext::instance().config()->DataDir(), *catalog_dir_, delta_catalog_name);

    if (flush_delta_entry->commit_ts() != max_commit_ts) {
        String error_message = "Expect flush_delta_entry->commit_ts() == max_commit_ts";
        UnrecoverableError(error_message);
    }

    LOG_TRACE(fmt::format("Save delta catalog commit ts:{}.", max_commit_ts));

    for (auto &op : flush_delta_entry->operations()) {
        switch (op->GetType()) {
            case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
                auto *column_entry_op = static_cast<AddColumnEntryOp *>(op.get());
                LOG_TRACE(fmt::format("Flush column entry: {}", column_entry_op->ToString()));
                column_entry_op->FlushDataToDisk(max_commit_ts);
                LOG_TRACE(fmt::format("Flush column entry done"));
                break;
            }
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
                LOG_TRACE(fmt::format("Ignore delta op: {}", op->ToString()));
                break;
        }
    }

    // Finalize current object to ensure PersistenceManager be in a consistent state
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize(true);
        handler.HandleWriteResult(result);
    }

    // Save the global catalog delta entry to disk.
    auto exp_size = flush_delta_entry->GetSizeInBytes();
    Vector<char> buf(exp_size);
    char *ptr = buf.data();
    flush_delta_entry->WriteAdv(ptr);
    i32 act_size = ptr - buf.data();
    if (exp_size != act_size) {
        String error_message = fmt::format("Save delta catalog failed, exp_size: {}, act_size: {}", exp_size, act_size);
        UnrecoverableError(error_message);
    }

    auto [out_file_handle, write_status] = VirtualStore::Open(delta_path, FileAccessMode::kWrite);
    if (!write_status.ok()) {
        String error_message = fmt::format("Failed to open delta catalog file: {}", delta_path);
        UnrecoverableError(error_message);
    }

    out_file_handle->Append((reinterpret_cast<const char *>(buf.data())), act_size);
    out_file_handle->Sync();
    if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader or InfinityContext::instance().GetServerRole() == NodeRole::kStandalone) {
        VirtualStore::UploadObject(delta_path, delta_catalog_name);
    }
    // {
    // log for delta op debug
    //     std::stringstream ss;
    //     ss << "Save delta catalog ops: ";
    //     for (auto &op : flush_delta_entry->operations()) {
    //         ss << op->ToString() << ". txn id: " << op->txn_id_ << "\n";
    //     }
    //     LOG_INFO(ss.str());
    // }
    LOG_DEBUG(fmt::format("Save delta catalog to: {}, size: {}.", delta_path, act_size));

    return true;
}

void Catalog::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry) { global_catalog_delta_entry_->AddDeltaEntry(std::move(delta_entry)); }

void Catalog::PickCleanup(CleanupScanner *scanner) { db_meta_map_.PickCleanup(scanner); }

void Catalog::InitCompactionAlg(TxnTimeStamp system_start_ts) {
    TransactionID txn_id = 0; // fake txn id
    Vector<DBEntry *> db_entries = this->Databases(txn_id, system_start_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, system_start_ts);
        for (auto *table_entry : table_entries) {
            table_entry->InitCompactionAlg(system_start_ts);
        }
    }
}

void Catalog::MemIndexCommit() {
    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    NewTxn *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>("mem index commit"), TransactionType::kNormal);

    Status status = NewCatalog::MemIndexCommit(new_txn);
    if (status.ok()) {
        Status commit_status = new_txn_mgr->CommitTxn(new_txn);
        if (!commit_status.ok()) {
            LOG_ERROR(fmt::format("Commit MemIndex commit failed: {}", commit_status.message()));
        }
    } else {
        LOG_ERROR(fmt::format("MemIndex commit failed: {}", status.message()));
        Status rollback_status = new_txn_mgr->RollBackTxn(new_txn);
        if (!rollback_status.ok()) {
            UnrecoverableError(rollback_status.message());
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

void Catalog::MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts) {
    all_memindex_recover_segment_.clear();
    auto db_meta_map_guard = db_meta_map_.GetMetaMap();
    for (auto &[_, db_meta] : *db_meta_map_guard) {
        auto [db_entry, status] = db_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            db_entry->MemIndexRecover(buffer_manager, ts);
        }
    }
    for (auto &segment_index_entry : all_memindex_recover_segment_) {
        segment_index_entry->MemIndexWaitInflightTasks();
    }
    all_memindex_recover_segment_.clear();
}

void Catalog::StartMemoryIndexCommit() {
    mem_index_commit_thread_ = MakeUnique<Thread>([this] { MemIndexCommitLoop(); });
}

SizeT Catalog::GetDeltaLogCount() const { return global_catalog_delta_entry_->OpSize(); }

Vector<CatalogDeltaOpBrief> Catalog::GetDeltaLogBriefs() const { return global_catalog_delta_entry_->GetOperationBriefs(); }

} // namespace infinity
