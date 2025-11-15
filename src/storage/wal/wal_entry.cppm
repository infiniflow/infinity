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

export module infinity_core:wal_entry;

import :table_def;
import :index_base;
import :data_block;
import :infinity_exception;

import std;

import column_def;
import global_resource_usage;
import statement_common;
import internal_types;

namespace infinity {

enum class SegmentStatus : u8;
class ChunkIndexMeta;
class ChunkIndexMetaInfo;
class BlockMeta;
export class SegmentMeta;
struct EraseBaseCache;

export enum class WalCommandType : i8 {
    INVALID = 0,
    // -----------------------------
    // Catalog
    // -----------------------------,
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
    IMPORT_V2 = 23,
    APPEND_V2 = 24,
    DELETE_V2 = 25,

    // -----------------------------
    // Alter
    // -----------------------------
    RENAME_TABLE_V2 = 43,
    ADD_COLUMNS_V2 = 44,
    DROP_COLUMNS_V2 = 45,
    ALTER_INDEX_V2 = 46,

    // -----------------------------
    // Flush
    // -----------------------------
    CHECKPOINT = 99,
    CHECKPOINT_V2 = 104,
    COMPACT_V2 = 105,

    // -----------------------------
    // Snapshot
    // -----------------------------
    CREATE_TABLE_SNAPSHOT = 110,
    RESTORE_TABLE_SNAPSHOT = 111,
    RESTORE_DATABASE_SNAPSHOT = 112,
    // -----------------------------
    // Other
    // -----------------------------
    DUMMY = 103,
    DUMP_INDEX_V2 = 107,
    CLEANUP = 108
};

export struct WalBlockInfo {
    BlockID block_id_{};
    u16 row_count_{};
    u16 row_capacity_{};
    std::vector<std::pair<u32, u64>> outline_infos_;

    WalBlockInfo() = default;

    explicit WalBlockInfo(BlockMeta &block_meta);

    bool operator==(const WalBlockInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalBlockInfo ReadBufferAdv(const char *&ptr);

    std::string ToString() const;
};

export struct WalSegmentInfo {
    SegmentID segment_id_{};
    u64 column_count_{0};
    size_t row_count_{0};
    size_t actual_row_count_{0};
    size_t row_capacity_{0};
    std::vector<WalBlockInfo> block_infos_;

    WalSegmentInfo() = default;

    explicit WalSegmentInfo(SegmentMeta &segment_meta, TxnTimeStamp begin_ts);

    bool operator==(const WalSegmentInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalSegmentInfo ReadBufferAdv(const char *&ptr);

    std::string ToString() const;
};

export struct WalSegmentInfoV2 {
    SegmentID segment_id_{};
    std::vector<BlockID> block_ids_;

    WalSegmentInfoV2() = default;

    explicit WalSegmentInfoV2(SegmentID segment_id, const std::vector<BlockID> &block_ids);

    bool operator==(const WalSegmentInfoV2 &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalSegmentInfoV2 ReadBufferAdv(const char *&ptr);

    std::string ToString() const;
};

export struct WalChunkIndexInfo {
    ChunkID chunk_id_{};
    std::string base_name_{};
    RowID base_rowid_{};
    u32 row_count_{};
    u32 term_count_{};
    u32 index_size_{};
    TxnTimeStamp deprecate_ts_{};

    WalChunkIndexInfo() = default;

    explicit WalChunkIndexInfo(ChunkIndexMeta &chunk_index_meta);
    explicit WalChunkIndexInfo(const ChunkIndexMetaInfo &chunk_index_meta_info, const ChunkID &chunk_id);

    bool operator==(const WalChunkIndexInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalChunkIndexInfo ReadBufferAdv(const char *&ptr);

    std::string ToString() const;
};

export struct WalSegmentIndexInfo {
    SegmentID segment_id_ = -1;
    std::vector<WalChunkIndexInfo> chunk_infos_;

    WalSegmentIndexInfo() = default;

    WalSegmentIndexInfo(SegmentID segment_id, const std::vector<WalChunkIndexInfo> &chunk_infos)
        : segment_id_(segment_id), chunk_infos_(chunk_infos) {}

    bool operator==(const WalSegmentIndexInfo &other) const;

    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteBufferAdv(char *&buf) const;

    static WalSegmentIndexInfo ReadBufferAdv(const char *&ptr);

    std::string ToString() const;
};

// WalCommandType -> std::string
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

    virtual std::string ToString() const = 0;
    virtual std::string CompactInfo() const = 0;
    virtual std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const = 0;

    // Read from a serialized version
    static std::shared_ptr<WalCmd> ReadAdv(const char *&ptr, i32 max_bytes);

    static std::string WalCommandTypeToString(WalCommandType type);

    WalCommandType type_{WalCommandType::INVALID};
};

export struct WalCmdDummy final : public WalCmd {
    WalCmdDummy() : WalCmd(WalCommandType::DUMMY) {}
    ~WalCmdDummy() override = default;

    bool operator==(const WalCmd &other) const final { return typeid(*this) == typeid(other); }
    [[nodiscard]] i32 GetSizeInBytes() const final { return 0; }
    void WriteAdv(char *&buf) const final {}

    std::string ToString() const final { return "Dummy"; }
    std::string CompactInfo() const final { return "Dummy"; }
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct WalCmdCreateDatabaseV2 final : public WalCmd {
    explicit WalCmdCreateDatabaseV2(const std::string &db_name, const std::string &db_id, const std::string &db_comment)
        : WalCmd(WalCommandType::CREATE_DATABASE_V2), db_name_(db_name), db_id_(db_id), db_comment_(db_comment) {}

    ~WalCmdCreateDatabaseV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string db_comment_{};
};

export struct WalCmdDropDatabaseV2 final : public WalCmd {
    explicit WalCmdDropDatabaseV2(const std::string &db_name, const std::string &db_id, TxnTimeStamp create_ts)
        : WalCmd(WalCommandType::DROP_DATABASE_V2), db_name_(db_name), db_id_(db_id), create_ts_(create_ts) {}

    ~WalCmdDropDatabaseV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    TxnTimeStamp create_ts_{};
};

export struct WalCmdCreateTableV2 final : public WalCmd {
    WalCmdCreateTableV2(const std::string &db_name, const std::string &db_id, const std::string &table_id, const std::shared_ptr<TableDef> &table_def)
        : WalCmd(WalCommandType::CREATE_TABLE_V2), db_name_(db_name), db_id_(db_id), table_id_(table_id), table_def_(table_def) {}

    ~WalCmdCreateTableV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_id_{};
    std::shared_ptr<TableDef> table_def_{};
};

export struct WalCmdDropTableV2 final : public WalCmd {
    WalCmdDropTableV2(const std::string &db_name,
                      const std::string &db_id,
                      const std::string &table_name,
                      const std::string &table_id,
                      TxnTimeStamp create_ts,
                      const std::string &table_key)
        : WalCmd(WalCommandType::DROP_TABLE_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          create_ts_(create_ts), table_key_(table_key) {}

    ~WalCmdDropTableV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    TxnTimeStamp create_ts_{};

    // Redundant but useful in commit phase.
    std::string table_key_{};
};

export struct WalCmdCreateIndexV2 final : public WalCmd {
    WalCmdCreateIndexV2(const std::string &db_name,
                        const std::string &db_id,
                        const std::string &table_name,
                        const std::string &table_id,
                        const std::string &index_id,
                        const std::shared_ptr<IndexBase> &index_base,
                        const std::string &table_key)
        : WalCmd(WalCommandType::CREATE_INDEX_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_id_(index_id), index_base_(index_base), table_key_(table_key) {}
    ~WalCmdCreateIndexV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string index_id_{};
    std::shared_ptr<IndexBase> index_base_{};
    std::vector<WalSegmentIndexInfo> segment_index_infos_;

    static WalCmdCreateIndexV2 ReadBufferAdv(const char *&ptr, i32 max_bytes);
    // Redundant but useful in commit phase.
    std::string table_key_{};
};

export struct WalRestoreIndexV2 final {
    WalRestoreIndexV2(const std::string &index_id,
                      const std::shared_ptr<IndexBase> &index_base,
                      const std::vector<WalSegmentIndexInfo> &segment_index_infos)
        : index_id_(index_id), index_base_(index_base), segment_index_infos_(segment_index_infos) {}
    WalRestoreIndexV2() = default;
    bool operator==(const WalRestoreIndexV2 &other) const;
    [[nodiscard]] i32 GetSizeInBytes() const;
    void WriteAdv(char *&buf) const;
    std::string ToString() const;
    std::string CompactInfo() const;

    std::string index_id_{};
    std::shared_ptr<IndexBase> index_base_{};
    std::vector<WalSegmentIndexInfo> segment_index_infos_;

    static WalRestoreIndexV2 ReadBufferAdv(const char *&ptr, i32 max_bytes);
    void WriteBufferAdv(char *&buf) const;
};

export struct WalCmdDropIndexV2 final : public WalCmd {
    WalCmdDropIndexV2(const std::string &db_name,
                      const std::string &db_id,
                      const std::string &table_name,
                      const std::string &table_id,
                      const std::string &index_name,
                      const std::string &index_id,
                      TxnTimeStamp create_ts,
                      const std::string &index_key)
        : WalCmd(WalCommandType::DROP_INDEX_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_name_(index_name), index_id_(index_id), create_ts_(create_ts), index_key_(index_key) {}
    ~WalCmdDropIndexV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string index_name_{};
    std::string index_id_{};
    TxnTimeStamp create_ts_{};

    // Redundant but useful in commit phase.
    std::string index_key_{};
};

export struct WalCmdImportV2 final : public WalCmd {
    WalCmdImportV2(const std::string &db_name,
                   const std::string &db_id,
                   const std::string &table_name,
                   const std::string &table_id,
                   const WalSegmentInfo &segment_info)
        : WalCmd(WalCommandType::IMPORT_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          segment_info_(segment_info) {}
    ~WalCmdImportV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    WalSegmentInfo segment_info_;
};

export struct WalCmdAppendV2 final : public WalCmd {
    WalCmdAppendV2(const std::string &db_name,
                   const std::string &db_id,
                   const std::string &table_name,
                   const std::string &table_id,
                   const std::vector<std::pair<RowID, u64>> &row_ranges,
                   const std::shared_ptr<DataBlock> &block)
        : WalCmd(WalCommandType::APPEND_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id), row_ranges_(row_ranges),
          block_(block) {}

    ~WalCmdAppendV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::vector<std::pair<RowID, u64>> row_ranges_{};
    std::shared_ptr<DataBlock> block_;
};

export struct WalCmdDeleteV2 final : public WalCmd {
    WalCmdDeleteV2(const std::string &db_name,
                   const std::string &db_id,
                   const std::string &table_name,
                   const std::string &table_id,
                   const std::vector<RowID> &row_ids)
        : WalCmd(WalCommandType::DELETE_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id), row_ids_(row_ids) {}

    ~WalCmdDeleteV2() override = default;

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::vector<RowID> row_ids_{};
};

export struct WalCmdCheckpoint final : public WalCmd {
    WalCmdCheckpoint(i64 max_commit_ts, const std::string &catalog_path, const std::string &catalog_name)
        : WalCmd(WalCommandType::CHECKPOINT), max_commit_ts_(max_commit_ts), catalog_path_(catalog_path), catalog_name_(catalog_name) {
        assert(!std::filesystem::path(catalog_path_).is_absolute());
    }

    ~WalCmdCheckpoint() override = default;

    virtual bool operator==(const WalCmd &other) const final;
    virtual i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    i64 max_commit_ts_{};
    std::string catalog_path_{};
    std::string catalog_name_{};
};

export struct WalCmdCheckpointV2 final : public WalCmd {
    WalCmdCheckpointV2(i64 max_commit_ts) : WalCmd(WalCommandType::CHECKPOINT_V2), max_commit_ts_(max_commit_ts) {}

    ~WalCmdCheckpointV2() override = default;

    virtual bool operator==(const WalCmd &other) const final;
    virtual i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    i64 max_commit_ts_{};
};

export struct WalCmdCompactV2 final : public WalCmd {

    WalCmdCompactV2(const std::string &db_name,
                    const std::string &db_id,
                    const std::string &table_name,
                    const std::string &table_id,
                    const std::vector<std::string> &index_names,
                    const std::vector<std::string> &index_ids,
                    const std::vector<WalSegmentInfo> &new_segment_infos,
                    const std::vector<SegmentID> &deprecated_segment_ids)

        : WalCmd(WalCommandType::COMPACT_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_names_(index_names), index_ids_(index_ids), new_segment_infos_(new_segment_infos), deprecated_segment_ids_(deprecated_segment_ids) {}

    ~WalCmdCompactV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    const std::string db_name_{};
    const std::string db_id_{};
    const std::string table_name_{};
    const std::string table_id_{};
    std::vector<std::string> index_names_{};
    std::vector<std::string> index_ids_{};
    std::vector<WalSegmentInfo> new_segment_infos_{};
    const std::vector<SegmentID> deprecated_segment_ids_{};
};

export struct WalCmdAlterIndexV2 final : public WalCmd {
    WalCmdAlterIndexV2(const std::string &db_name,
                       const std::string &db_id,
                       const std::string &table_name,
                       const std::string &table_id,
                       const std::string &index_name,
                       const std::string &index_id,
                       std::vector<std::unique_ptr<InitParameter>> &&params)
        : WalCmd(WalCommandType::ALTER_INDEX_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_name_(index_name), index_id_(index_id), params_(std::move(params)) {}

    ~WalCmdAlterIndexV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string index_name_{};
    std::string index_id_{};
    std::vector<std::unique_ptr<InitParameter>> params_{};
};

export enum class DumpIndexCause { kImport, kCompact, kCreateIndex, kOptimizeIndex, kReplayCreateIndex, kDumpMemIndex, kInvalid };

export struct WalCmdDumpIndexV2 final : public WalCmd {
    WalCmdDumpIndexV2(const std::string &db_name,
                      const std::string &db_id,
                      const std::string &table_name,
                      const std::string &table_id,
                      const std::string &index_name,
                      const std::string &index_id,
                      SegmentID segment_id,
                      const std::string &table_key)
        : WalCmd(WalCommandType::DUMP_INDEX_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_name_(index_name), index_id_(index_id), segment_id_(segment_id), table_key_(table_key) {}

    WalCmdDumpIndexV2(const std::string &db_name,
                      const std::string &db_id,
                      const std::string &table_name,
                      const std::string &table_id,
                      const std::string &index_name,
                      const std::string &index_id,
                      SegmentID segment_id,
                      const std::vector<WalChunkIndexInfo> &chunk_infos,
                      const std::vector<ChunkID> &deprecate_ids,
                      const std::string &table_key)
        : WalCmd(WalCommandType::DUMP_INDEX_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          index_name_(index_name), index_id_(index_id), segment_id_(segment_id), chunk_infos_(chunk_infos), deprecate_ids_(deprecate_ids),
          table_key_(table_key) {}

    ~WalCmdDumpIndexV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string index_name_{};
    std::string index_id_{};
    SegmentID segment_id_{};
    std::vector<WalChunkIndexInfo> chunk_infos_{};
    std::vector<ChunkID> deprecate_ids_{};

    // Redudant but useful in commit phase.
    std::string table_key_{};
    DumpIndexCause dump_cause_{DumpIndexCause::kInvalid};
};

export struct WalCmdRenameTableV2 : public WalCmd {
    WalCmdRenameTableV2(const std::string &db_name,
                        const std::string &db_id,
                        const std::string &table_name,
                        const std::string &table_id,
                        const std::string &new_table_name,
                        const std::string &old_table_key)
        : WalCmd(WalCommandType::RENAME_TABLE_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          new_table_name_(new_table_name), old_table_key_(old_table_key) {}
    ~WalCmdRenameTableV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string new_table_name_{};

    // Redundant but useful in commit phase.
    std::string old_table_key_{};
};

export struct WalCmdAddColumnsV2 : public WalCmd {

    WalCmdAddColumnsV2(const std::string &db_name,
                       const std::string &db_id,
                       const std::string &table_name,
                       const std::string &table_id,
                       const std::vector<u32> &column_idx_list,
                       const std::vector<std::shared_ptr<ColumnDef>> &column_defs,
                       const std::string &table_key)

        : WalCmd(WalCommandType::ADD_COLUMNS_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          column_idx_list_(column_idx_list), column_defs_(column_defs), table_key_(table_key) {}
    ~WalCmdAddColumnsV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::vector<u32> column_idx_list_{};
    std::vector<std::shared_ptr<ColumnDef>> column_defs_{};

    // Redundant but usefule commit phase.
    std::string table_key_{};
};

export struct WalCmdDropColumnsV2 : public WalCmd {
    WalCmdDropColumnsV2(const std::string &db_name,
                        const std::string &db_id,
                        const std::string &table_name,
                        const std::string &table_id,
                        const std::vector<std::string> &column_names,
                        const std::vector<ColumnID> &column_ids,
                        const std::string &table_key,
                        const std::vector<std::string> &column_keys)
        : WalCmd(WalCommandType::DROP_COLUMNS_V2), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          column_names_(column_names), column_ids_(column_ids), table_key_(table_key), column_keys_(column_keys) {}
    ~WalCmdDropColumnsV2() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::vector<std::string> column_names_{};
    std::vector<ColumnID> column_ids_{};

    // Redundant but useful in commit phase.
    std::string table_key_{};
    std::vector<std::string> column_keys_{};
};

export struct WalCmdCleanup : public WalCmd {
    WalCmdCleanup(i64 timestamp) : WalCmd(WalCommandType::CLEANUP), timestamp_(timestamp) {};
    ~WalCmdCleanup() override = default;

    bool operator==(const WalCmd &other) const final;
    i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    i64 timestamp_{};
};

export struct WalCmdCreateTableSnapshot : public WalCmd {
    WalCmdCreateTableSnapshot(const std::string &db_name, const std::string &table_name, const std::string &snapshot_name, TxnTimeStamp max_commit_ts)
        : WalCmd(WalCommandType::CREATE_TABLE_SNAPSHOT), db_name_(db_name), table_name_(table_name), snapshot_name_(snapshot_name),
          max_commit_ts_(max_commit_ts) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    static WalCmdCreateTableSnapshot ReadBufferAdv(const char *&ptr, i32 max_bytes);

    std::string db_name_{};
    std::string table_name_{};
    std::string snapshot_name_{};
    TxnTimeStamp max_commit_ts_{};
};

export struct WalCmdRestoreTableSnapshot : public WalCmd {
    WalCmdRestoreTableSnapshot(const std::string &db_name,
                               const std::string &db_id,
                               const std::string &table_name,
                               const std::string &table_id,
                               const std::string &snapshot_name,
                               std::shared_ptr<TableDef> table_def_,
                               const std::vector<WalSegmentInfoV2> &segment_infos,
                               const std::vector<WalCmdCreateIndexV2> &index_cmds,
                               const std::vector<std::string> &files)
        : WalCmd(WalCommandType::RESTORE_TABLE_SNAPSHOT), db_name_(db_name), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          snapshot_name_(snapshot_name), table_def_(table_def_), files_(files), segment_infos_(segment_infos), index_cmds_(index_cmds) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    static WalCmdRestoreTableSnapshot ReadBufferAdv(const char *&ptr, i32 max_bytes);

    std::string db_name_{};
    std::string db_id_{};
    std::string table_name_{};
    std::string table_id_{};
    std::string snapshot_name_{};
    std::shared_ptr<TableDef> table_def_{};
    std::vector<std::string> files_;
    std::vector<WalSegmentInfoV2> segment_infos_; // eache segment has a vector of block ids(assuming same column count)
    std::vector<WalCmdCreateIndexV2> index_cmds_; // index commands to restore indexes
};

export struct WalCmdRestoreDatabaseSnapshot : public WalCmd {
    WalCmdRestoreDatabaseSnapshot(const std::string &db_name,
                                  const std::string &db_id_str,
                                  const std::string &db_comment,
                                  const std::vector<WalCmdRestoreTableSnapshot> &restore_table_wal_cmds)
        : WalCmd(WalCommandType::RESTORE_DATABASE_SNAPSHOT), db_name_(db_name), db_id_str_(db_id_str), db_comment_(db_comment),
          restore_table_wal_cmds_(restore_table_wal_cmds) {}

    bool operator==(const WalCmd &other) const final;
    [[nodiscard]] i32 GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;

    std::string ToString() const final;
    std::string CompactInfo() const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string db_comment_{};

    std::vector<WalCmdRestoreTableSnapshot> restore_table_wal_cmds_{};
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
    static std::shared_ptr<WalEntry> ReadAdv(const char *&ptr, i32 max_bytes);

    std::vector<std::shared_ptr<WalCmd>> cmds_{};

    // Return if the entry is a checkpoint.
    [[nodiscard]] bool IsCheckPoint(WalCmdCheckpoint *&checkpoint_cmd) const;
    [[nodiscard]] bool IsCheckPoint(WalCmdCheckpointV2 *&checkpoint_cmd) const;

    // Return if the entry is either a checkpoint or create snapshot (returns base WalCmd pointer)
    [[nodiscard]] bool IsCheckPointOrSnapshot(WalCmd *&cmd) const;

    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] std::string CompactInfo() const;
};

// Forward and backward iterator of WAL entries in a given WAL file
export class WalEntryIterator {
public:
    static std::unique_ptr<WalEntryIterator> Make(const std::string &wal_path, bool is_backward);

    WalEntryIterator(std::vector<char> &&buf, size_t wal_size, bool is_backward)
        : is_backward_(is_backward), off_(0), buf_(std::move(buf)), wal_size_(wal_size) {
        if (is_backward_) {
            off_ = buf_.size();
        }
    }

    [[nodiscard]] bool HasNext() { return (is_backward_ && off_ > 0) || (!is_backward_ && off_ < buf_.size()); }

    std::shared_ptr<WalEntry> Next();

    [[nodiscard]] i64 GetOffset() { return off_; };

    std::shared_ptr<WalEntry> GetEntryByIndex(i64 index);
    std::vector<std::shared_ptr<WalEntry>> GetAllEntries();
    bool IsGood() const;

private:
    bool is_backward_{};
    size_t off_{}; // offset of last returned entry if is_backward, otherwise offset of the entry needs to be returned
    std::vector<char> buf_{};
    size_t wal_size_{};
};

// Backward iterator of WAL entries in given WAL files
export class WalListIterator {
public:
    explicit WalListIterator(const std::vector<std::string> &wal_list);

    [[nodiscard]] bool HasNext();

    [[nodiscard]] std::shared_ptr<WalEntry> Next();

private:
    // Locate the latest checkpoint entry, and purge bad entries after it.
    void PurgeBadEntriesAfterLatestCheckpoint();
    std::list<std::string> wal_list_{};
    std::unique_ptr<WalEntryIterator> iter_{};
};

} // namespace infinity
