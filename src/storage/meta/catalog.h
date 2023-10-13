//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <shared_mutex>

#include "storage/meta/entry/base_entry.h"
#include "storage/meta/db_meta.h"
#include "storage/io/file_system_type.h"
#include "json.hpp"

namespace infinity {

class Txn;
class TxnManager;
class FunctionSet;
class TableFunction;
class DBEntry;
class BufferManager;

struct NewCatalog {
public:
    explicit NewCatalog(SharedPtr<String> dir);

public:
    static EntryResult CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts);

    static void RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr);

    static Vector<DBEntry *> Databases(NewCatalog *catalog, u64 txn_id, TxnTimeStamp begin_ts);

#if 0

    void
    CreateTable(String schema_name, const SharedPtr<Table>& table_def, ConflictType conflict_type);

    void
    DeleteTable(String schema_name, String table_name, ConflictType conflict_type);

    void
    CreateCollection(String schema_name, String collection_name, ConflictType conflict_type);

    void
    DeleteCollection(String schema_name, String collection_name, ConflictType conflict_type);

    Vector<SharedPtr<BaseTable>>
    GetTables(String schema_name);

    // View related methods
    SharedPtr<View>
    GetViewByName(String schema_name, String view_name);

    SharedPtr<View>
    GetViewByNameNoExcept(String schema_name, String view_name) noexcept;

    void
    CreateView(String schema_name, const SharedPtr<View>& view, ConflictType conflict_type);

    void
    DeleteView(String schema_name, String view_name, ConflictType conflict_type);

    Vector<SharedPtr<View>>
    GetViews(String schema_name);
#endif

    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void DeleteFunctionSet(NewCatalog *catalog, String function_name);

    // Table Function related methods
    static SharedPtr<TableFunction> GetTableFunctionByName(NewCatalog *catalog, String function_name);

    static void AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function);

    static void DeleteTableFunction(NewCatalog *catalog, String function_name);

    static nlohmann::json Serialize(const NewCatalog *catalog);

    static void Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog);

    static UniquePtr<NewCatalog> LoadFromFile(const SharedPtr<DirEntry> &dir_entry, BufferManager *buffer_mgr);

    static void SaveAsFile(const NewCatalog *catalog_ptr, const String &dir, const String &file_name);

public:
    SharedPtr<String> current_dir_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    u64 next_txn_id_{};
    u64 catalog_version_{};
    RWMutex rw_locker_;

    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_;
    HashMap<String, SharedPtr<TableFunction>> table_functions_;
};

} // namespace infinity
