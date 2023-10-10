#pragma once

#include "common/types/alias/concurrency.h"
#include "common/types/alias/containers.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "json.hpp"
#include "storage/index_def/index_def.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {

class TxnManager;

class TableCollectionEntry;

class IndexMeta {
public:
    explicit IndexMeta(SharedPtr<String> table_collection_dir, TableCollectionEntry *table_collection_entry)
        : table_collection_entry_(table_collection_entry), table_collection_dir_(table_collection_dir) {}

public:
    static EntryResult CreateNewEntry(IndexMeta *index_meta, SharedPtr<IndexDef> index_def, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult DropNewEntry(IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static void DeleteNewEntry(IndexMeta *index_meta, u64 txn_id, TxnManager *txn_mgr);

    static EntryResult GetEntry(IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String> ToString(IndexMeta *index_meta);

    static inline TableCollectionEntry *GetTableCollectionEntry(IndexMeta *index_meta) { return index_meta->table_collection_entry_; }

    static nlohmann::json Serialize(const IndexMeta *index_meta);

    static UniquePtr<IndexMeta> Deserialize(const nlohmann::json &index_meta_json, TableCollectionEntry *table_collection_entry);

public:
    RWMutex rw_locker_{};

    TableCollectionEntry *table_collection_entry_{};

    SharedPtr<String> table_collection_dir_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity