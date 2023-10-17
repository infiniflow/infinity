#pragma once

#include "common/types/alias/concurrency.h"
#include "common/types/alias/containers.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "json.hpp"
#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {

class TxnManager;
class IndexDef;
class TableCollectionEntry;
class BufferManager;

class IndexDefMeta {
public:
    explicit IndexDefMeta(SharedPtr<String> index_name, TableCollectionEntry *table_collection_entry)
        : index_name_(std::move(index_name)), table_collection_entry_(table_collection_entry) {
        // Insert a dummy entry
        entry_list_.emplace_front(MakeUnique<BaseEntry>(EntryType::kDummy));
    }

public:
    static EntryResult CreateNewEntry(IndexDefMeta *index_def_meta,
                                      SharedPtr<IndexDef> index_def,
                                      ConflictType conflict_type,
                                      u64 txn_id,
                                      TxnTimeStamp begin_ts,
                                      TxnManager *txn_mgr);

    static EntryResult DropNewEntry(IndexDefMeta *index_def_meta, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static void DeleteNewEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnManager *txn_mgr);

    static EntryResult GetEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String> ToString(IndexDefMeta *index_def_meta);

    static inline TableCollectionEntry *GetTableCollectionEntry(IndexDefMeta *index_def_meta) { return index_def_meta->table_collection_entry_; }

    static nlohmann::json Serialize(const IndexDefMeta *index_def_meta);

    static UniquePtr<IndexDefMeta> Deserialize(const nlohmann::json &index_def_meta_json, TableCollectionEntry *table_collection_entry);

public:
    RWMutex rw_locker_{};

    TableCollectionEntry *table_collection_entry_{};

    SharedPtr<String> index_name_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity