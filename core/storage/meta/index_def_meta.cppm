
module;

import stl;
import base_entry;
import parser;
import txn_manager;
import third_party;
import index_def;

export module index_def_meta;

namespace infinity {

//class TxnManager;
//class IndexDef;
class TableCollectionEntry;
//class BufferManager;

export class IndexDefMeta {
public:
    explicit IndexDefMeta(SharedPtr<String> index_name, TableCollectionEntry *table_collection_entry);

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

    static Json Serialize(const IndexDefMeta *index_def_meta);

    static UniquePtr<IndexDefMeta> Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_collection_entry);

public:
    RWMutex rw_locker_{};

    TableCollectionEntry *table_collection_entry_{};

    SharedPtr<String> index_name_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity