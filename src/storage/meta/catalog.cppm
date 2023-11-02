//
// Created by jinhai on 23-6-4.
//

module;

import stl;
import base_entry;
import db_entry;
import db_meta;
import txn_manager;
import function;
import function_set;
import table_function;
import third_party;
import buffer_manager;

export module new_catalog;

namespace infinity {

export struct NewCatalog {
public:
    explicit NewCatalog(SharedPtr<String> dir, bool create_default_db = false);

public:
    static EntryResult CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts);

    static void RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr);

    static Vector<DBEntry *> Databases(NewCatalog *catalog, u64 txn_id, TxnTimeStamp begin_ts);

    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void DeleteFunctionSet(NewCatalog *catalog, String function_name);

    // Table Function related methods
    static SharedPtr<TableFunction> GetTableFunctionByName(NewCatalog *catalog, String function_name);

    static void AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function);

    static void DeleteTableFunction(NewCatalog *catalog, String function_name);

    static Json Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static void Deserialize(const Json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog);

    static UniquePtr<NewCatalog> LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr);

    static UniquePtr<NewCatalog> LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr);

    static String SaveAsFile(NewCatalog *catalog_ptr, const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    void MergeFrom(NewCatalog &other);

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
