
module;

import stl;
import parser;
import index_def;
import base_entry;
import third_party;

export module index_def_entry;

namespace infinity {

class IndexDefMeta;

export struct IndexDefEntry : public BaseEntry {
public:
    explicit IndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts);

public:
    static Json Serialize(const IndexDefEntry *index_def_entry);

    static UniquePtr<IndexDefEntry> Deserialize(const Json &index_def_entry_json, IndexDefMeta *index_def_meta);

public:
    RWMutex rw_locker_{};

    IndexDefMeta *index_def_meta_{};

    SharedPtr<IndexDef> index_def_{};
};
} // namespace infinity
