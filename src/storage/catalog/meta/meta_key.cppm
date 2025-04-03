// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

namespace infinity {

export struct MetaKey {
    enum class Type {
        kDB,
        kTable,
        kSegment,
        kBlock,
        kColumn,
        kTableIndex,
        kSegmentIndex,
        kChunkIndex,
    } type_;

    MetaKey(Type type) : type_(type) {}
    virtual ~MetaKey() = default;
};
class ColumnDef;

export struct DBMetaKey : public MetaKey {
    DBMetaKey(String db_id_str) : MetaKey(Type::kDB), db_id_str_(std::move(db_id_str)) {}

    String db_id_str_;
};

export struct TableMetaKey : public MetaKey {
    TableMetaKey(String db_id_str, String table_id_str)
        : MetaKey(Type::kTable), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)) {}

    String db_id_str_;
    String table_id_str_;
};

export struct SegmentMetaKey : public MetaKey {
    SegmentMetaKey(String db_id_str, String table_id_str, SegmentID segment_id)
        : MetaKey(Type::kSegment), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id) {}

    String db_id_str_;
    String table_id_str_;
    SegmentID segment_id_;
};

export struct BlockMetaKey : public MetaKey {
    BlockMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id)
        : MetaKey(Type::kBlock), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
          block_id_(block_id) {}

    String db_id_str_;
    String table_id_str_;
    SegmentID segment_id_;
    BlockID block_id_;
};

export struct ColumnMetaKey : public MetaKey {
    ColumnMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, SharedPtr<ColumnDef> column_def);

    ~ColumnMetaKey() override;

    String db_id_str_;
    String table_id_str_;
    SegmentID segment_id_;
    BlockID block_id_;
    SharedPtr<ColumnDef> column_def_;
};

export struct TableIndexMetaKey : public MetaKey {
    TableIndexMetaKey(String db_id_str, String table_id_str, String index_id_str)
        : MetaKey(Type::kTableIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)) {}

    String db_id_str_;
    String table_id_str_;
    String index_id_str_;
};

export struct SegmentIndexMetaKey : public MetaKey {
    SegmentIndexMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id)
        : MetaKey(Type::kSegmentIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id) {}

    String db_id_str_;
    String table_id_str_;
    String index_id_str_;
    SegmentID segment_id_;
};

export struct ChunkIndexMetaKey : public MetaKey {
    ChunkIndexMetaKey(String db_id_str, String table_id_str, String index_id_str, SegmentID segment_id, ChunkID chunk_id)
        : MetaKey(Type::kChunkIndex), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)),
          index_id_str_(std::move(index_id_str)), segment_id_(segment_id), chunk_id_(chunk_id) {}

    String db_id_str_;
    String table_id_str_;
    String index_id_str_;
    SegmentID segment_id_;
    ChunkID chunk_id_;
};

} // namespace infinity