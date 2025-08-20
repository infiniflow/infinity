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

export module infinity_core:meta_key;

import :meta_type;
import :default_values;

import std;
import third_party;

import column_def;

namespace infinity {

export struct MetaKey {
    MetaType type_{MetaType::kInvalid};

    explicit MetaKey(MetaType type) : type_(type) {}
    virtual ~MetaKey() = default;

    virtual std::string ToString() const = 0;
    virtual nlohmann::json ToJson() const = 0;
};
// class ColumnDef;

export struct DBMetaKey final : public MetaKey {
    DBMetaKey(std::string db_id_str, std::string db_name) : MetaKey(MetaType::kDB), db_id_str_(std::move(db_id_str)), db_name_(std::move(db_name)) {}

    DBMetaKey(std::string db_id_str, std::string db_name, TxnTimeStamp commit_ts)
        : MetaKey(MetaType::kDB), db_id_str_(std::move(db_id_str)), db_name_(std::move(db_name)), commit_ts_(commit_ts) {}

    std::string db_id_str_{};
    std::string db_name_{};
    TxnTimeStamp commit_ts_{}; // Commit timestamp of "create database"

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct DBTagMetaKey final : public MetaKey {
    DBTagMetaKey(std::string db_id_str, std::string tag_name)
        : MetaKey(MetaType::kDBTag), db_id_str_(std::move(db_id_str)), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableMetaKey final : public MetaKey {
    TableMetaKey(std::string db_id_str, std::string table_id_str, const std::string &table_name)
        : MetaKey(MetaType::kTable), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), table_name_(std::move(table_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string table_name_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableNameMetaKey final : public MetaKey {
    TableNameMetaKey(std::string db_id_str, std::string table_id_str, const std::string &table_name)
        : MetaKey(MetaType::kTableName), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          table_name_(std::move(table_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string table_name_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableColumnMetaKey final : public MetaKey {
    TableColumnMetaKey(std::string db_id_str, std::string table_id_str, std::string column_name)
        : MetaKey(MetaType::kTableColumn), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          column_name_(std::move(column_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string column_name_{};
    TxnTimeStamp commit_ts_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableTagMetaKey final : public MetaKey {
    TableTagMetaKey(std::string db_id_str, std::string table_id_str, std::string tag_name)
        : MetaKey(MetaType::kTableTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentMetaKey final : public MetaKey {
    SegmentMetaKey(std::string db_id_str, std::string table_id_str, SegmentID segment_id)
        : MetaKey(MetaType::kSegment), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentTagMetaKey final : public MetaKey {
    SegmentTagMetaKey(std::string db_id_str, std::string table_id_str, SegmentID segment_id, std::string tag_name)
        : MetaKey(MetaType::kSegmentTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct BlockMetaKey final : public MetaKey {
    BlockMetaKey(std::string db_id_str, std::string table_id_str, SegmentID segment_id, BlockID block_id)
        : MetaKey(MetaType::kBlock), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          block_id_(block_id) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct BlockTagMetaKey final : public MetaKey {
    BlockTagMetaKey(std::string db_id_str, std::string table_id_str, SegmentID segment_id, BlockID block_id, std::string tag_name)
        : MetaKey(MetaType::kBlockTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          block_id_(block_id), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ColumnMetaKey final : public MetaKey {
    ColumnMetaKey(std::string db_id_str, std::string table_id_str, SegmentID segment_id, BlockID block_id, std::shared_ptr<ColumnDef> column_def);

    ~ColumnMetaKey() override;

    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    std::shared_ptr<ColumnDef> column_def_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableIndexMetaKey final : public MetaKey {
    TableIndexMetaKey(std::string db_id_str, std::string table_id_str, std::string index_id_str, std::string index_name)
        : MetaKey(MetaType::kTableIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), index_name_(std::move(index_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    std::string index_name_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct TableIndexTagMetaKey final : public MetaKey {
    TableIndexTagMetaKey(std::string db_id_str, std::string table_id_str, std::string index_id_str, std::string tag_name)
        : MetaKey(MetaType::kTableIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentIndexMetaKey final : public MetaKey {
    SegmentIndexMetaKey(std::string db_id_str, std::string table_id_str, std::string index_id_str, SegmentID segment_id)
        : MetaKey(MetaType::kSegmentIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    SegmentID segment_id_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SegmentIndexTagMetaKey final : public MetaKey {
    SegmentIndexTagMetaKey(std::string db_id_str, std::string table_id_str, std::string index_id_str, SegmentID segment_id, std::string tag_name)
        : MetaKey(MetaType::kSegmentIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    SegmentID segment_id_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ChunkIndexMetaKey final : public MetaKey {
    ChunkIndexMetaKey(std::string db_id_str, std::string table_id_str, std::string index_id_str, SegmentID segment_id, ChunkID chunk_id)
        : MetaKey(MetaType::kChunkIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), chunk_id_(chunk_id) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    TxnTimeStamp commit_ts_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct ChunkIndexTagMetaKey final : public MetaKey {
    ChunkIndexTagMetaKey(std::string db_id_str,
                         std::string table_id_str,
                         std::string index_id_str,
                         SegmentID segment_id,
                         ChunkID chunk_id,
                         std::string tag_name)
        : MetaKey(MetaType::kChunkIndexTag), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), chunk_id_(chunk_id), tag_name_(std::move(tag_name)) {}

    std::string db_id_str_{};
    std::string table_id_str_{};
    std::string index_id_str_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct PmObjectMetaKey final : public MetaKey {
    PmObjectMetaKey(std::string path_key) : MetaKey(MetaType::kPmObject), path_key_(std::move(path_key)) {}

    std::string path_key_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct PmStatMetaKey final : public MetaKey {
    PmStatMetaKey(std::string object_key) : MetaKey(MetaType::kPmStat), object_key_(std::move(object_key)) {}

    std::string object_key_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct DropMetaKey final : public MetaKey {
    DropMetaKey(std::string scope, std::string object_key) : MetaKey(MetaType::kDrop), scope_(std::move(scope)), object_key_(std::move(object_key)) {}

    std::string scope_{};
    std::string object_key_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export struct SystemTagMetaKey final : public MetaKey {
    SystemTagMetaKey(std::string tag_name) : MetaKey(MetaType::kSystemTag), tag_name_(std::move(tag_name)) {}

    std::string tag_name_{};
    std::string value_{};

    std::string ToString() const final;
    nlohmann::json ToJson() const final;
};

export std::shared_ptr<MetaKey> MetaParse(const std::string &key, const std::string &value);

} // namespace infinity