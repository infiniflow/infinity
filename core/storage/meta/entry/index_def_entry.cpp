module;

import stl;
import base_entry;
import index_def;
import third_party;
import index_def_meta;

module index_def_entry;

namespace infinity {
IndexDefEntry::IndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIndexDef), index_def_meta_(index_def_meta), index_def_(index_def) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

Json IndexDefEntry::Serialize(const IndexDefEntry *index_def_entry) {
    Json json;
    json["begin_ts"] = index_def_entry->begin_ts_;
    json["commit_ts"] = index_def_entry->commit_ts_.load();
    json["txn_id"] = index_def_entry->txn_id_.load();
    json["delete"] = index_def_entry->deleted_;
    if (!index_def_entry->deleted_) {
        json["index_def"] = index_def_entry->index_def_->Serialize();
    }
    return json;
}

UniquePtr<IndexDefEntry> IndexDefEntry::Deserialize(const Json &index_def_entry_json, IndexDefMeta *index_def_meta) {
    u64 txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    bool deleted = index_def_entry_json["delete"];
    if (!deleted) {
        auto index_def = IndexDef::Deserialize(index_def_entry_json["index_def"]);
        auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts);
        index_def_entry->commit_ts_.store(commit_ts);
        index_def_entry->deleted_ = deleted;
        index_def_entry->index_def_meta_ = index_def_meta;
        index_def_entry->index_def_ = index_def;
        return index_def_entry;
    }
    auto index_def_entry = MakeUnique<IndexDefEntry>(nullptr, index_def_meta, txn_id, begin_ts);
    index_def_entry->commit_ts_.store(commit_ts);
    index_def_entry->deleted_ = deleted;
    index_def_entry->index_def_meta_ = index_def_meta;
    return index_def_entry;
}
} // namespace infinity
