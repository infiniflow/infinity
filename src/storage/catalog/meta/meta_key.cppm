// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module meta_key;

import stl;
import meta_type;
import third_party;
import default_values;

namespace infinity {

export struct MetaKey {
    MetaType type_{MetaType::kInvalid};

    explicit MetaKey(MetaType type) : type_(type) {}
    virtual ~MetaKey() = default;

    virtual String ToString() const = 0;
    virtual nlohmann::json ToJson() const = 0;
};
class ColumnDef;

export struct DBMetaKey final : public MetaKey {
    DBMetaKey(String db_id_str, String db_name) : MetaKey(MetaType::kDB), db_id_str_(std::move(db_id_str)), db_name_(std::move(db_name)) {}

    String db_id_str_{};
    String db_name_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct DBTagMetaKey final : public MetaKey {
    DBTagMetaKey(String db_id_str, String tag_name) : MetaKey(MetaType::kDBTag), db_id_str_(std::move(db_id_str)), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableMetaKey final : public MetaKey {
    TableMetaKey(String db_id_str, String table_id_str, const String &table_name)
        : MetaKey(MetaType::kTable), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), table_name_(std::move(table_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String table_name_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableColumnMetaKey final : public MetaKey {
    TableColumnMetaKey(String db_id_str, String table_id_str, String column_name)
        : MetaKey(MetaType::kTableColumn), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          column_name_(std::move(column_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String column_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableTagMetaKey final : public MetaKey {
    TableTagMetaKey(String db_id_str, String table_id_str, String tag_name)
        : MetaKey(MetaType::kTableTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentMetaKey final : public MetaKey {
    SegmentMetaKey(String db_id_str, String table_id_str, SegmentID segment_id)
        : MetaKey(MetaType::kSegment), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id) {}

    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentTagMetaKey final : public MetaKey {
    SegmentTagMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, String tag_name)
        : MetaKey(MetaType::kSegmentTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct BlockMetaKey final : public MetaKey {
    BlockMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id)
        : MetaKey(MetaType::kBlock), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          block_id_(block_id) {}

    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct BlockTagMetaKey final : public MetaKey {
    BlockTagMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, String tag_name)
        : MetaKey(MetaType::kBlockTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          block_id_(block_id), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ColumnMetaKey final : public MetaKey {
    ColumnMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, SharedPtr<ColumnDef> column_def);

    ~ColumnMetaKey() override;

    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    SharedPtr<ColumnDef> column_def_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableIndexMetaKey final : public MetaKey {
    TableIndexMetaKey(String db_id_str, String table_id_str, String index_id_str, String index_name)
        : MetaKey(MetaType::kTableIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), index_name_(std::move(index_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    String index_name_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableIndexTagMetaKey final : public MetaKey {
    TableIndexTagMetaKey(String db_id_str, String table_id_str, String index_id_str, String tag_name)
        : MetaKey(MetaType::kTableIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentIndexMetaKey final : public MetaKey {
    SegmentIndexMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id)
        : MetaKey(MetaType::kSegmentIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    SegmentID segment_id_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentIndexTagMetaKey final : public MetaKey {
    SegmentIndexTagMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id, String tag_name)
        : MetaKey(MetaType::kSegmentIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    SegmentID segment_id_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ChunkIndexMetaKey final : public MetaKey {
    ChunkIndexMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id, ChunkID chunk_id)
        : MetaKey(MetaType::kChunkIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), chunk_id_(chunk_id) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    TxnTimeStamp commit_ts_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ChunkIndexTagMetaKey final : public MetaKey {
    ChunkIndexTagMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id, ChunkID chunk_id, String tag_name)
        : MetaKey(MetaType::kChunkIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), chunk_id_(chunk_id), tag_name_(std::move(tag_name)) {}

    String db_id_str_{};
    String table_id_str_{};
    String index_id_str_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct PmPathMetaKey final : public MetaKey {
    PmPathMetaKey(String path_key) : MetaKey(MetaType::kPmPath), path_key_(std::move(path_key)) {}

    String path_key_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct PmObjectMetaKey final : public MetaKey {
    PmObjectMetaKey(String object_key) : MetaKey(MetaType::kPmObject), object_key_(std::move(object_key)) {}

    String object_key_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SystemTagMetaKey final : public MetaKey {
    SystemTagMetaKey(String tag_name) : MetaKey(MetaType::kSystemTag), tag_name_(std::move(tag_name)) {}

    String tag_name_{};
    String value_{};

    String ToString() const final;
    nlohmann::json ToJson() const final;
};

export SharedPtr<MetaKey> MetaParse(const String &key, const String &value);

} // namespace infinity