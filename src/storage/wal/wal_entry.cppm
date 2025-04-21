// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include <cassert>
#include <typeinfo>

export module wal_entry;

import table_def;
import index_base;
import data_block;
import stl;
import statement_common;
import infinity_exception;
import internal_types;
import persistence_manager;
import column_def;
import global_resource_usage;

namespace infinity {

struct BlockEntry;
struct SegmentEntry;
struct ChunkIndexEntry;
enum class SegmentStatus;
class ChunkIndexMeta;
class BlockMeta;
class SegmentMeta;

export enum class WalCommandType : i8 {
    INVALID = 0,
    // -----------------------------
    // Catalog
    // -----------------------------
    CREATE_DATABASE = 1,
    DROP_DATABASE = 2,
    CREATE_TABLE = 3,
    DROP_TABLE = 4,
    ALTER_INFO = 5,
    CREATE_INDEX = 6,
    DROP_INDEX = 7,
    CREATE_DATABASE_V2 = 8,
    DROP_DATABASE_V2 = 9,
    CREATE_TABLE_V2 = 10,
    DROP_TABLE_V2 = 11,
    ALTER_INFO_V2 = 12,
    CREATE_INDEX_V2 = 13,
    DROP_INDEX_V2 = 14,

    // -----------------------------
    // Data
    // -----------------------------
    IMPORT = 20,
    APPEND = 21,
    DELETE = 22,
    IMPORT_V2 = 23,
    APPEND_V2 = 24,
    DELETE_V2 = 25,

    // -----------------------------
    // SEGMENT STATUS
    // -----------------------------
    SET_SEGMENT_STATUS_SEALED = 31,
    UPDATE_SEGMENT_BLOOM_FILTER_DATA = 32,
    SET_SEGMENT_STATUS_SEALED_V2 = 33,
    UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2 = 34,

    // -----------------------------
    // Alter
    // -----------------------------
    RENAME_TABLE = 40,
    ADD_COLUMNS = 41,
    DROP_COLUMNS = 42,
    RENAME_TABLE_V2 = 43,
    ADD_COLUMNS_V2 = 44,
    DROP_COLUMNS_V2 = 45,

    // -----------------------------
    // Flush
    // -----------------------------
    CHECKPOINT = 99,
    COMPACT = 100,
    CHECKPOINT_V2 = 104,
    COMPACT_V2 = 105,

    // -----------------------------
    // Other
    // -----------------------------
    OPTIMIZE = 101,
    DUMP_INDEX = 102,
    DUMMY = 103,
    OPTIMIZE_V2 = 106,
    DUMP_INDEX_V2 = 107,
};

export struct WalBlockInfo {
    BlockID block_id_{};
    u16 row_count_{};
    u16 row_capacity_{};
    Vector<Pair<u32, u64>> outline_infos_;
    Vector<String> paths_;
    AddrSerializer addr_serializer_;
    mutable SizeT pm_size_ = 0; // tmp for test. should delete when stable

    WalBlockInfo() = default;

    explicit WalBlockInfo(BlockEntry *block_entry);

    explicit WalBlockInfo(BlockMeta &block_meta);

    bool operator==(const WalBlockInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalBlockInfo ReadBufferAdv(const char *&ptr);

    String ToString() const;
};

export struct WalSegmentInfo {
    SegmentID segment_id_{};
    u64 column_count_{0};
    SizeT row_count_{0};
    SizeT actual_row_count_{0};
    SizeT row_capacity_{0};
    Vector<WalBlockInfo> block_infos_;

    WalSegmentInfo() = default;

    explicit WalSegmentInfo(SegmentEntry *segment_entry);

    explicit WalSegmentInfo(SegmentMeta &segment_meta, TxnTimeStamp begin_ts);

    bool operator==(const WalSegmentInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalSegmentInfo ReadBufferAdv(const char *&ptr);

    String ToString() const;
};

export struct WalChunkIndexInfo {
    ChunkID chunk_id_{};
    String base_name_{};
    Vector<String> paths_{};
    AddrSerializer addr_serializer_;
    mutable SizeT pm_size_; // tmp for test. should delete when stable
    RowID base_rowid_{};
    u32 row_count_{};
    TxnTimeStamp deprecate_ts_{};

    WalChunkIndexInfo() = default;

    explicit WalChunkIndexInfo(ChunkIndexEntry *chunk_index_entry);

    explicit WalChunkIndexInfo(ChunkIndexMeta &chunk_index_meta);

    bool operator==(const WalChunkIndexInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalChunkIndexInfo ReadBufferAdv(const char *&ptr);

    String ToString() const;
};

export struct WalSegmentIndexInfo {
    SegmentID segment_id_ = -1;
    Vector<WalChunkIndexInfo> chunk_infos_;

    WalSegmentIndexInfo() = default;

    WalSegmentIndexInfo(SegmentID segment_id, Vector<WalChunkIndexInfo> chunk_infos)
        : segment_id_(segment_id), chunk_infos_(std::move(chunk_infos)) {}

    bool operator==(const WalSegmentIndexInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalSegmentIndexInfo ReadBufferAdv(const char *&ptr);

    String ToString() const;
};

// WalCommandType -> String
export struct WalCmd {
    WalCmd(WalCommandType type) : type_(type) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("WalCmd");
#endif
    }

    virtual ~WalCmd() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("WalCmd");
#endif
    }

    auto GetType() const -> WalCommandType { return type_; }

    virtual auto operator==(const WalCmd &other) const -> bool { return typeid(*this) == typeid(other); }
    auto operator!=(const WalCmd &other) const -> bool { return !(*this == other); }
    // Estimated serialized size in bytes
    [[nodiscard]] virtual i32 GetSizeInBytes() const = 0;
    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const = 0;

    virtual String ToString() const = 0;
    virtual String CompactInfo() const = 0;

    // Read from a serialized version
    static SharedPtr<WalCmd> ReadAdv(const char *&ptr, i32 max_bytes);

    static String WalCommandTypeToString(WalCommandType type);

    WalCommandType type_{WalCommandType::INVALID};
};

export struct WalCmdDummy final : public WalCmd {
    WalCmdDummy() : WalCmd(WalCommandType::DUMMY) {}

    bool operator==(const WalCmd &other) const final { return typeid(*this) == typeid(other); }
    [[nodiscard]] i32 GetSizeInBytes() const final { return 0; }
    void WriteAdv(char *&buf) const final {}
    String ToString() const final { return "Dummy"; }
    String CompactInfo() const final { return "Dummy"; }
};

export struct WalCmdCreateDatabase final : public WalCmd {
    explicit WalCmdCreateDatabase(String db_name, String db_dir_tail, String db_comment)
        : WalCmd(WalCommandType::CREATE_DATABASE), db_name_(std::move(db_name)), db_dir_tail_(std::move(db_dir_tail)),
          db_comment_(std::move(db_comment)) {
        assert(!std::filesystem::path(db_dir_tail_).is_absolute());
    }

    bool operator==(const WalCmd &other) const final {
        const auto *other_cmd = dynamic_cast<const WalCmdCreateDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(db_dir_tail_, other_cmd->db_dir_tail_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_dir_tail_{};
    String db_comment_{};
};

export struct WalCmdCreateDatabaseV2 final : public WalCmd {
    explicit WalCmdCreateDatabaseV2(String db_name, String db_id, String db_comment)
        : WalCmd(WalCommandType::CREATE_DATABASE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), db_comment_(std::move(db_comment)) {}

    bool operator==(const WalCmd &other) const final {
        const auto *other_cmd = dynamic_cast<const WalCmdCreateDatabaseV2 *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(db_id_, other_cmd->db_id_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String db_comment_{};
};

export struct WalCmdDropDatabase final : public WalCmd {
    explicit WalCmdDropDatabase(String db_name) : WalCmd(WalCommandType::DROP_DATABASE), db_name_(std::move(db_name)) {}

    bool operator==(const WalCmd &other) const final {
        auto other_cmd = dynamic_cast<const WalCmdDropDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
};

export struct WalCmdDropDatabaseV2 final : public WalCmd {
    explicit WalCmdDropDatabaseV2(String db_name, String db_id)
        : WalCmd(WalCommandType::DROP_DATABASE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)) {}

    bool operator==(const WalCmd &other) const final {
        auto other_cmd = dynamic_cast<const WalCmdDropDatabaseV2 *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(db_id_, other_cmd->db_id_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
};

export struct WalCmdCreateTable final : public WalCmd {
    WalCmdCreateTable(String db_name, String table_dir_tail, const SharedPtr<TableDef> &table_def)
        : WalCmd(WalCommandType::CREATE_TABLE), db_name_(std::move(db_name)), table_dir_tail_(std::move(table_dir_tail)), table_def_(table_def) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_dir_tail_{};
    SharedPtr<TableDef> table_def_{};
};

export struct WalCmdCreateTableV2 final : public WalCmd {
    WalCmdCreateTableV2(String db_name, String db_id, String table_id, const SharedPtr<TableDef> &table_def)
        : WalCmd(WalCommandType::CREATE_TABLE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_id_(std::move(table_id)),
          table_def_(table_def) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_id_{};
    SharedPtr<TableDef> table_def_{};
};

export struct WalCmdDropTable final : public WalCmd {
    WalCmdDropTable(const String &db_name, const String &table_name)
        : WalCmd(WalCommandType::DROP_TABLE), db_name_(db_name), table_name_(table_name) {}

    bool operator==(const WalCmd &other) const final {
        auto other_cmd = dynamic_cast<const WalCmdDropTable *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(table_name_, other_cmd->table_name_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
};

export struct WalCmdDropTableV2 final : public WalCmd {
    WalCmdDropTableV2(const String &db_name, const String &db_id, const String &table_name, const String &table_id)
        : WalCmd(WalCommandType::DROP_TABLE_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id) {}

    bool operator==(const WalCmd &other) const final {
        auto other_cmd = dynamic_cast<const WalCmdDropTableV2 *>(&other);
        return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(db_id_, other_cmd->db_id_) &&
               IsEqual(table_name_, other_cmd->table_name_) && IsEqual(table_id_, other_cmd->table_id_);
    }
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
};

export struct WalCmdCreateIndex final : public WalCmd {
    WalCmdCreateIndex(String db_name, String table_name, String index_dir_tail_, SharedPtr<IndexBase> index_base)
        : WalCmd(WalCommandType::CREATE_INDEX), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_dir_tail_(std::move(index_dir_tail_)), index_base_(std::move(index_base)) {
        assert(!std::filesystem::path(index_dir_tail_).is_absolute());
    }

    WalCmdCreateIndex(String db_name, String table_name, SharedPtr<IndexBase> index_base)
        : WalCmd(WalCommandType::CREATE_INDEX), db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_base_(std::move(index_base)) {
    }

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String index_dir_tail_{};
    SharedPtr<IndexBase> index_base_{};
    Vector<WalSegmentIndexInfo> segment_index_infos_;
};

export struct WalCmdCreateIndexV2 final : public WalCmd {
    WalCmdCreateIndexV2(String db_name, String db_id, String table_name, String table_id, String index_id, SharedPtr<IndexBase> index_base)
        : WalCmd(WalCommandType::CREATE_INDEX_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), index_id_(std::move(index_id)), index_base_(std::move(index_base)) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    String index_id_{};
    SharedPtr<IndexBase> index_base_{};
    Vector<WalSegmentIndexInfo> segment_index_infos_;

    // Only used in commit phase. Don't need to write to WAL.
    String table_key_{};
};

export struct WalCmdDropIndex final : public WalCmd {
    WalCmdDropIndex(String db_name, String table_name, String index_name)
        : WalCmd(WalCommandType::DROP_INDEX), db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String index_name_{};
};

export struct WalCmdDropIndexV2 final : public WalCmd {
    WalCmdDropIndexV2(String db_name, String db_id, String table_name, String table_id, String index_name, String index_id)
        : WalCmd(WalCommandType::DROP_INDEX_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), index_name_(std::move(index_name)), index_id_(std::move(index_id)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    String index_name_{};
    String index_id_{};

    // Only used in commit phase. Don't need to write to WAL.
    String index_key_{};
};

export struct WalCmdImport final : public WalCmd {
    WalCmdImport(String db_name, String table_name, WalSegmentInfo &&segment_info)
        : WalCmd(WalCommandType::IMPORT), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_info_(std::move(segment_info)) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    WalSegmentInfo segment_info_;
};

export struct WalCmdImportV2 final : public WalCmd {
    WalCmdImportV2(String db_name, String db_id, String table_name, String table_id, WalSegmentInfo &&segment_info)
        : WalCmd(WalCommandType::IMPORT_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), segment_info_(std::move(segment_info)) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    WalSegmentInfo segment_info_;
};

export struct WalCmdAppend final : public WalCmd {
    WalCmdAppend(String db_name, String table_name, const SharedPtr<DataBlock> &block)
        : WalCmd(WalCommandType::APPEND), db_name_(std::move(db_name)), table_name_(std::move(table_name)), block_(block) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    SharedPtr<DataBlock> block_{};
};

export struct WalCmdAppendV2 final : public WalCmd {
    WalCmdAppendV2(String db_name,
                   String db_id,
                   String table_name,
                   String table_id,
                   Vector<Pair<RowID, u64>> row_ranges,
                   const SharedPtr<DataBlock> &block)
        : WalCmd(WalCommandType::APPEND_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), row_ranges_(std::move(row_ranges)), block_(block) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    Vector<Pair<RowID, u64>> row_ranges_{};
    SharedPtr<DataBlock> block_{};
};

export struct WalCmdDelete final : public WalCmd {
    WalCmdDelete(String db_name, String table_name, const Vector<RowID> &row_ids)
        : WalCmd(WalCommandType::DELETE), db_name_(std::move(db_name)), table_name_(std::move(table_name)), row_ids_(row_ids) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    Vector<RowID> row_ids_{};
};

export struct WalCmdDeleteV2 final : public WalCmd {
    WalCmdDeleteV2(String db_name, String db_id, String table_name, String table_id, const Vector<RowID> &row_ids)
        : WalCmd(WalCommandType::DELETE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), row_ids_(row_ids) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    Vector<RowID> row_ids_{};
};

// used when append op turn an old unsealed segment full and sealed
// will always have necessary minmax filter
// may have user-defined bloom filter
export struct WalCmdSetSegmentStatusSealed final : public WalCmd {
    WalCmdSetSegmentStatusSealed(String db_name,
                                 String table_name,
                                 SegmentID segment_id,
                                 String segment_filter_binary_data,
                                 Vector<Pair<BlockID, String>> block_filter_binary_data)
        : WalCmd(WalCommandType::SET_SEGMENT_STATUS_SEALED), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          segment_id_(segment_id), segment_filter_binary_data_(std::move(segment_filter_binary_data)),
          block_filter_binary_data_(std::move(block_filter_binary_data)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    static WalCmdSetSegmentStatusSealed ReadBufferAdv(const char *&ptr);

    const String db_name_{};
    const String table_name_{};
    const SegmentID segment_id_{};
    const String segment_filter_binary_data_{};
    const Vector<Pair<BlockID, String>> block_filter_binary_data_{};
};

export struct WalCmdSetSegmentStatusSealedV2 final : public WalCmd {
    WalCmdSetSegmentStatusSealedV2(String db_name,
                                   String db_id,
                                   String table_name,
                                   String table_id,
                                   SegmentID segment_id,
                                   String segment_filter_binary_data,
                                   Vector<Pair<BlockID, String>> block_filter_binary_data)
        : WalCmd(WalCommandType::SET_SEGMENT_STATUS_SEALED_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)),
          table_name_(std::move(table_name)), table_id_(std::move(table_id)), segment_id_(segment_id),
          segment_filter_binary_data_(std::move(segment_filter_binary_data)), block_filter_binary_data_(std::move(block_filter_binary_data)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    static WalCmdSetSegmentStatusSealedV2 ReadBufferAdv(const char *&ptr);

    const String db_name_{};
    const String db_id_{};
    const String table_name_{};
    const String table_id_{};
    const SegmentID segment_id_{};
    const String segment_filter_binary_data_{};
    const Vector<Pair<BlockID, String>> block_filter_binary_data_{};
};

// used when user-defined bloom filter need to be updated
export struct WalCmdUpdateSegmentBloomFilterData final : public WalCmd {
    WalCmdUpdateSegmentBloomFilterData(String db_name,
                                       String table_name,
                                       SegmentID segment_id,
                                       String segment_filter_binary_data,
                                       Vector<Pair<BlockID, String>> block_filter_binary_data)
        : WalCmd(WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          segment_id_(segment_id), segment_filter_binary_data_(std::move(segment_filter_binary_data)),
          block_filter_binary_data_(std::move(block_filter_binary_data)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    static WalCmdUpdateSegmentBloomFilterData ReadBufferAdv(const char *&ptr);

    const String db_name_{};
    const String table_name_{};
    const SegmentID segment_id_{};
    const String segment_filter_binary_data_{};
    const Vector<Pair<BlockID, String>> block_filter_binary_data_{};
};

// used when user-defined bloom filter need to be updated
export struct WalCmdUpdateSegmentBloomFilterDataV2 final : public WalCmd {
    WalCmdUpdateSegmentBloomFilterDataV2(String db_name,
                                         String db_id,
                                         String table_name,
                                         String table_id,
                                         SegmentID segment_id,
                                         String segment_filter_binary_data,
                                         Vector<Pair<BlockID, String>> block_filter_binary_data)
        : WalCmd(WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)),
          table_name_(std::move(table_name)), table_id_(std::move(table_id)), segment_id_(segment_id),
          segment_filter_binary_data_(std::move(segment_filter_binary_data)), block_filter_binary_data_(std::move(block_filter_binary_data)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    static WalCmdUpdateSegmentBloomFilterDataV2 ReadBufferAdv(const char *&ptr);

    const String db_name_{};
    const String db_id_{};
    const String table_name_{};
    const String table_id_{};
    const SegmentID segment_id_{};
    const String segment_filter_binary_data_{};
    const Vector<Pair<BlockID, String>> block_filter_binary_data_{};
};

export struct WalCmdCheckpoint final : public WalCmd {
    WalCmdCheckpoint(i64 max_commit_ts, bool is_full_checkpoint, String catalog_path, String catalog_name)
        : WalCmd(WalCommandType::CHECKPOINT), max_commit_ts_(max_commit_ts), is_full_checkpoint_(is_full_checkpoint), catalog_path_(catalog_path),
          catalog_name_(catalog_name) {
        assert(!std::filesystem::path(catalog_path_).is_absolute());
    }
    virtual bool operator==(const WalCmd &other) const final;
    virtual i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    i64 max_commit_ts_{};
    bool is_full_checkpoint_;
    String catalog_path_{};
    String catalog_name_{};
};

export struct WalCmdCheckpointV2 final : public WalCmd {
    WalCmdCheckpointV2(i64 max_commit_ts) : WalCmd(WalCommandType::CHECKPOINT_V2), max_commit_ts_(max_commit_ts) {}
    virtual bool operator==(const WalCmd &other) const final;
    virtual i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    i64 max_commit_ts_{};
};

export struct WalCmdCompact final : public WalCmd {
    WalCmdCompact(String db_name, String table_name, Vector<WalSegmentInfo> new_segment_infos, Vector<SegmentID> deprecated_segment_ids)
        : WalCmd(WalCommandType::COMPACT), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          new_segment_infos_(std::move(new_segment_infos)), deprecated_segment_ids_(std::move(deprecated_segment_ids)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    const String db_name_{};
    const String table_name_{};
    Vector<WalSegmentInfo> new_segment_infos_{};
    const Vector<SegmentID> deprecated_segment_ids_{};

    // Used in commit phase
    String db_id_str_;
    String table_id_str_;
    String table_key_;
};

export struct WalCmdCompactV2 final : public WalCmd {
    WalCmdCompactV2(String db_name,
                    String db_id,
                    String table_name,
                    String table_id,
                    Vector<WalSegmentInfo> new_segment_infos,
                    Vector<SegmentID> deprecated_segment_ids)
        : WalCmd(WalCommandType::COMPACT_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), new_segment_infos_(std::move(new_segment_infos)),
          deprecated_segment_ids_(std::move(deprecated_segment_ids)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    const String db_name_{};
    const String db_id_;
    const String table_name_{};
    const String table_id_;
    Vector<WalSegmentInfo> new_segment_infos_{};
    const Vector<SegmentID> deprecated_segment_ids_{};
};

export struct WalCmdOptimize final : public WalCmd {
    WalCmdOptimize(String db_name, String table_name, String index_name, Vector<UniquePtr<InitParameter>> params)
        : WalCmd(WalCommandType::OPTIMIZE), db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)),
          params_(std::move(params)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String index_name_{};
    Vector<UniquePtr<InitParameter>> params_{};
};

export struct WalCmdOptimizeV2 final : public WalCmd {
    WalCmdOptimizeV2(String db_name,
                     String db_id,
                     String table_name,
                     String table_id,
                     String index_name,
                     String index_id,
                     Vector<UniquePtr<InitParameter>> params)
        : WalCmd(WalCommandType::OPTIMIZE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), index_name_(std::move(index_name)), index_id_(std::move(index_id)), params_(std::move(params)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    String index_name_{};
    String index_id_{};
    Vector<UniquePtr<InitParameter>> params_{};
};

enum class DumpIndexCause { kImport, kCompact, kCreateIndex, kOptimizeIndex, kReplayCreateIndex, kDumpMemIndex, kInvalid };

export struct WalCmdDumpIndex final : public WalCmd {
    WalCmdDumpIndex(String db_name, String table_name, String index_name, SegmentID segment_id)
        : WalCmd(WalCommandType::DUMP_INDEX), db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)),
          segment_id_(segment_id) {}

    WalCmdDumpIndex(String db_name,
                    String table_name,
                    String index_name,
                    SegmentID segment_id,
                    Vector<WalChunkIndexInfo> chunk_infos,
                    Vector<ChunkID> deprecate_ids)
        : WalCmd(WalCommandType::DUMP_INDEX), db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)),
          segment_id_(segment_id), chunk_infos_(std::move(chunk_infos)), deprecate_ids_(std::move(deprecate_ids)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String index_name_{};
    SegmentID segment_id_{};
    Vector<WalChunkIndexInfo> chunk_infos_{};
    Vector<ChunkID> deprecate_ids_{};
};

export struct WalCmdDumpIndexV2 final : public WalCmd {
    WalCmdDumpIndexV2(String db_name, String db_id, String table_name, String table_id, String index_name, String index_id, SegmentID segment_id)
        : WalCmd(WalCommandType::DUMP_INDEX_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), index_name_(std::move(index_name)), index_id_(std::move(index_id)), segment_id_(segment_id) {}

    WalCmdDumpIndexV2(String db_name,
                      String db_id,
                      String table_name,
                      String table_id,
                      String index_name,
                      String index_id,
                      SegmentID segment_id,
                      Vector<WalChunkIndexInfo> chunk_infos,
                      Vector<ChunkID> deprecate_ids)
        : WalCmd(WalCommandType::DUMP_INDEX_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), index_name_(std::move(index_name)), index_id_(std::move(index_id)), segment_id_(segment_id),
          chunk_infos_(std::move(chunk_infos)), deprecate_ids_(std::move(deprecate_ids)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    String index_name_{};
    String index_id_{};
    SegmentID segment_id_{};
    Vector<WalChunkIndexInfo> chunk_infos_{};
    Vector<ChunkID> deprecate_ids_{};

    // Only used in commit phase. Don't need to write to WAL.
    String table_key_{};
    bool clear_mem_index_{};
    DumpIndexCause dump_cause_{DumpIndexCause::kInvalid};
};

export struct WalCmdRenameTable : public WalCmd {
    WalCmdRenameTable(String db_name, String table_name, String new_table_name)
        : WalCmd(WalCommandType::RENAME_TABLE), db_name_(db_name), table_name_(table_name), new_table_name_(new_table_name) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String new_table_name_{};
};

export struct WalCmdRenameTableV2 : public WalCmd {
    WalCmdRenameTableV2(String db_name, String db_id, String table_name, String table_id, String new_table_name)
        : WalCmd(WalCommandType::RENAME_TABLE_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(table_name),
          table_id_(std::move(table_id)), new_table_name_(std::move(new_table_name)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    String new_table_name_{};

    // Only used in commit phase. Don't need to write to WAL.
    String old_table_key_{};
};

export struct WalCmdAddColumns : public WalCmd {
    WalCmdAddColumns(String db_name, String table_name, Vector<SharedPtr<ColumnDef>> column_defs)
        : WalCmd(WalCommandType::ADD_COLUMNS), db_name_(db_name), table_name_(table_name), column_defs_(std::move(column_defs)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
};

export struct WalCmdAddColumnsV2 : public WalCmd {
    WalCmdAddColumnsV2(String db_name, String db_id, String table_name, String table_id, Vector<SharedPtr<ColumnDef>> column_defs)
        : WalCmd(WalCommandType::ADD_COLUMNS_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), column_defs_(std::move(column_defs)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    Vector<SharedPtr<ColumnDef>> column_defs_{};

    // Only used in commit phase. Don't need to write to WAL.
    String table_key_{};
};

export struct WalCmdDropColumns : public WalCmd {
    WalCmdDropColumns(String db_name, String table_name, Vector<String> column_names)
        : WalCmd(WalCommandType::DROP_COLUMNS), db_name_(db_name), table_name_(table_name), column_names_(std::move(column_names)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String table_name_{};
    String table_key_{};
    Vector<String> column_names_{};

    // Used in commit phase
    Vector<ColumnID> column_ids_{};
};

export struct WalCmdDropColumnsV2 : public WalCmd {
    WalCmdDropColumnsV2(String db_name, String db_id, String table_name, String table_id, Vector<String> column_names, Vector<ColumnID> column_ids)
        : WalCmd(WalCommandType::DROP_COLUMNS_V2), db_name_(std::move(db_name)), db_id_(std::move(db_id)), table_name_(std::move(table_name)),
          table_id_(std::move(table_id)), column_names_(std::move(column_names)), column_ids_(std::move(column_ids)) {}

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    String ToString() const final;
    String CompactInfo() const final;

    String db_name_{};
    String db_id_{};
    String table_name_{};
    String table_id_{};
    Vector<String> column_names_{};
    Vector<ColumnID> column_ids_{};

    // Only used in commit phase. Don't need to write to WAL.
    String table_key_{};
};

export struct WalEntryHeader {
    i32 size_{}; // size of header + payload + 4 bytes pad. There's 4 bytes pad just after the payload storing
    // the same value to assist backward iterating.
    u32 checksum_{}; // crc32 of the entry, including the header and the
    // payload. User shall populate it before writing to wal.
    i64 txn_id_{};             // txn id of the entry
    TxnTimeStamp commit_ts_{}; // commit timestamp of the txn
};

export struct WalEntry : WalEntryHeader {
    WalEntry() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("WalEntry");
#endif
    }

    ~WalEntry() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("WalEntry");
#endif
    }

    bool operator==(const WalEntry &other) const;

    bool operator!=(const WalEntry &other) const;

    // Estimated serialized size in bytes, ensured be no less than Write
    // requires, allowed be larger.
    [[nodiscard]] i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<WalEntry> ReadAdv(const char *&ptr, i32 max_bytes);

    Vector<SharedPtr<WalCmd>> cmds_{};

    // Return if the entry is a full checkpoint or delta checkpoint.
    [[nodiscard]] bool IsCheckPoint(WalCmdCheckpoint *&checkpoint_cmd) const;
    [[nodiscard]] bool IsCheckPoint(WalCmdCheckpointV2 *&checkpoint_cmd) const;

    [[nodiscard]] String ToString() const;
    [[nodiscard]] String CompactInfo() const;
};

// Forward and backward iterator of WAL entries in a given WAL file
export class WalEntryIterator {
public:
    static UniquePtr<WalEntryIterator> Make(const String &wal_path, bool is_backward);

    WalEntryIterator(Vector<char> &&buf, SizeT wal_size, bool is_backward)
        : is_backward_(is_backward), off_(0), buf_(std::move(buf)), wal_size_(wal_size) {
        if (is_backward_) {
            off_ = buf_.size();
        }
    }

    [[nodiscard]] bool HasNext() { return (is_backward_ && off_ > 0) || (!is_backward_ && off_ < buf_.size()); }

    SharedPtr<WalEntry> Next();

    [[nodiscard]] i64 GetOffset() { return off_; };

    SharedPtr<WalEntry> GetEntryByIndex(i64 index);
    Vector<SharedPtr<WalEntry>> GetAllEntries();
    bool IsGood() const;

private:
    bool is_backward_{};
    SizeT off_{}; // offset of last returned entry if is_backward, otherwise offset of the entry needs to be returned
    Vector<char> buf_{};
    SizeT wal_size_{};
};

// Backward iterator of WAL entries in given WAL files
export class WalListIterator {
public:
    explicit WalListIterator(const Vector<String> &wal_list);

    [[nodiscard]] bool HasNext();

    [[nodiscard]] SharedPtr<WalEntry> Next();

private:
    // Locate the latest full checkpoint entry, and purge bad entries after it.
    void PurgeBadEntriesAfterLatestCheckpoint();
    List<String> wal_list_{};
    UniquePtr<WalEntryIterator> iter_{};
};

} // namespace infinity
