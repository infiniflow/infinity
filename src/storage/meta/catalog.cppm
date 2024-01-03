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

export module catalog;
export import :db_meta;
export import :db_entry;
export import :table_meta;
export import :table_entry;
export import :view_meta;
export import :view_entry;
export import :segment_entry;
export import :block_entry;
export import :block_column_entry;
export import :table_index_meta;
export import :table_index_entry;
export import :column_index_entry;
export import :segment_column_index_entry;
export import :irs_index_entry;
export import :base_entry;
import :base_meta;

import stl;
import parser;

import table_def;
import function;
import function_set;
import table_function;
import special_function;
import third_party;
import buffer_manager;
import profiler;
import status;
import default_values;
import table_detail;
import index_def;
import txn_store;
import data_access_state;

namespace infinity {

class ProfileHistory {
private:
    Mutex lock_{};
    Vector<SharedPtr<QueryProfiler>> queue{};
    SizeT front{};
    SizeT rear{};
    SizeT max_size{};

public:
    explicit ProfileHistory(SizeT size) {
        max_size = size + 1;
        queue.resize(max_size);
        front = 0;
        rear = 0;
    }

    void Enqueue(SharedPtr<QueryProfiler> &&profiler) {
        UniqueLock<Mutex> lk(lock_);
        if ((rear + 1) % max_size == front) {
            return;
        }
        queue[rear] = profiler;
        rear = (rear + 1) % max_size;
    }

    QueryProfiler *GetElement(SizeT index) {
        UniqueLock<Mutex> lk(lock_);

        // FIXME: Bug, unsigned integer: index will always >= 0
        if (index < 0 || index >= (rear - front + max_size) % max_size) {
            return nullptr;
        }
        SizeT actualIndex = (front + index) % max_size;
        return queue[actualIndex].get();
    }

    Vector<SharedPtr<QueryProfiler>> GetElements() {
        Vector<SharedPtr<QueryProfiler>> elements;
        elements.reserve(max_size);

        UniqueLock<Mutex> lk(lock_);
        for (SizeT i = 0; i < queue.size(); ++i) {
            if (queue[i].get() != nullptr) {
                elements.push_back(queue[i]);
            }
        }
        return elements;
    }
};

export struct NewCatalog {
public:
    explicit NewCatalog(SharedPtr<String> dir, bool create_default_db = false);

public:
    // Database related functions
    Tuple<DBEntry *, Status>
    CreateDatabase(const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type = ConflictType::kError);

    Tuple<DBEntry *, Status> DropDatabase(const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<DBEntry *, Status> GetDatabase(const String &db_name, u64 txn_id, TxnTimeStamp begin_ts);

    void RemoveDBEntry(const String &db_name, u64 txn_id, TxnManager *txn_mgr);

    // List databases
    Vector<DBEntry *> Databases(u64 txn_id, TxnTimeStamp begin_ts);

    // Table related functions
    Tuple<TableEntry *, Status> CreateTable(const String &db_name,
                                            u64 txn_id,
                                            TxnTimeStamp begin_ts,
                                            const SharedPtr<TableDef> &table_def,
                                            ConflictType conflict_type,
                                            TxnManager *txn_mgr);

    Tuple<TableEntry *, Status> DropTableByName(const String &db_name,
                                                const String &table_name,
                                                ConflictType conflict_type,
                                                u64 txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnManager *txn_mgr);

    Status GetTables(const String &db_name, Vector<TableDetail> &output_table_array, u64 txn_id, TxnTimeStamp begin_ts);

    Tuple<TableEntry *, Status> GetTableByName(const String &db_name, const String &table_name, u64 txn_id, TxnTimeStamp begin_ts);

    static Status RemoveTableEntry(TableEntry *table_entry, u64 txn_id, TxnManager *txn_mgr);

    // Index Related methods
    Tuple<TableEntry *, TableIndexEntry *, Status> CreateIndex(const String &db_name,
                                                               const String &table_name,
                                                               const SharedPtr<IndexDef> &index_def,
                                                               ConflictType conflict_type,
                                                               u64 txn_id,
                                                               TxnTimeStamp begin_ts,
                                                               TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status> DropIndex(const String &db_name,
                                               const String &table_name,
                                               const String &index_name,
                                               ConflictType conflict_type,
                                               u64 txn_id,
                                               TxnTimeStamp begin_ts,
                                               TxnManager *txn_mgr);

    static void
    CreateIndexFile(TableEntry *table_entry, void *txn_store, TableIndexEntry *table_index_entry, TxnTimeStamp begin_ts, BufferManager *buffer_mgr);

    static Status RemoveIndexEntry(const String &index_name, TableIndexEntry *table_index_entry, u64 txn_id, TxnManager *txn_mgr);

    static void CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_);

    // Append related functions
    static void Append(TableEntry *table_entry, u64 txn_id, void *txn_store, BufferManager *buffer_mgr);

    static void CommitAppend(TableEntry *table_entry, u64 txn_id, TxnTimeStamp commit_ts, const AppendState *append_state_ptr);

    static void RollbackAppend(TableEntry *table_entry, u64 txn_id, TxnTimeStamp commit_ts, void *txn_store);

    static Status Delete(TableEntry *table_entry, u64 txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state);

    static void CommitDelete(TableEntry *table_entry, u64 txn_id, TxnTimeStamp commit_ts, const DeleteState &append_state);

    static Status RollbackDelete(TableEntry *table_entry, u64 txn_id, DeleteState &append_state, BufferManager *buffer_mgr);

    static Status ImportSegment(TableEntry *table_entry, TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment);

    static u32 GetNextSegmentID(TableEntry *table_entry);

    static u32 GetMaxSegmentID(const TableEntry *table_entry);

    static void ImportSegment(TableEntry* table_entry, u32 segment_id, SharedPtr<SegmentEntry>& segment_entry);

    static void IncreaseTableRowCount(TableEntry* table_entry, u64 increased_row_count);
public:
    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void DeleteFunctionSet(NewCatalog *catalog, String function_name);

    // Table Function related methods
    static SharedPtr<TableFunction> GetTableFunctionByName(NewCatalog *catalog, String function_name);

    static void AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function);

    static void AddSpecialFunction(NewCatalog *catalog, const SharedPtr<SpecialFunction> &special_function);

    static SharedPtr<SpecialFunction> GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, String function_name);

    static void DeleteTableFunction(NewCatalog *catalog, String function_name);

public:
    // Serialization and Deserialization
    static Json Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static void Deserialize(const Json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog);

    static UniquePtr<NewCatalog> LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr);

    static UniquePtr<NewCatalog> LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr);

    static String SaveAsFile(NewCatalog *catalog_ptr, const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    void MergeFrom(NewCatalog &other);

public:
    // Profile related methods

    void AppendProfilerRecord(SharedPtr<QueryProfiler> profiler) { history.Enqueue(Move(profiler)); }

    const QueryProfiler *GetProfilerRecord(SizeT index) { return history.GetElement(index); }

    const Vector<SharedPtr<QueryProfiler>> GetProfilerRecords() { return history.GetElements(); }

public:
    SharedPtr<String> current_dir_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    u64 next_txn_id_{};
    u64 catalog_version_{};
    RWMutex rw_locker_{};

    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_{};
    HashMap<String, SharedPtr<TableFunction>> table_functions_{};
    HashMap<String, SharedPtr<SpecialFunction>> special_functions_{};

    ProfileHistory history{DEFAULT_PROFILER_HISTORY_SIZE};
};

} // namespace infinity
