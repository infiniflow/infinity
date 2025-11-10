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

export module infinity_core:meta_tree;

import :meta_type;
import :meta_key;
import :infinity_type;

import std.compat;
import third_party;

import internal_types;
import check_statement;

namespace infinity {

class Storage;
class SystemCache;
class DbCache;
class TableCache;
class SegmentCache;
class TableIndexCache;

export struct MetaObject {
    MetaObject(MetaType type, const std::shared_ptr<MetaKey> &meta_key) : type_(type), meta_key_(meta_key) {}
    virtual ~MetaObject() = default;
    virtual nlohmann::json ToJson() const = 0;

    MetaType type_{MetaType::kInvalid};
    std::shared_ptr<MetaKey> meta_key_{nullptr};
};

export struct MetaDBObject final : public MetaObject {
    MetaDBObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kDB, meta_key) {}
    nlohmann::json ToJson() const final;

    std::shared_ptr<DbCache> RestoreDbCache(Storage *storage_ptr) const;

    std::map<std::string, std::shared_ptr<MetaObject>> table_map_;
    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
};

export struct MetaTableObject final : public MetaObject {
    MetaTableObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kTable, meta_key) {}
    nlohmann::json ToJson() const final;

    // const std::string &GetTableName() const;
    SegmentID GetNextSegmentID() const;
    SegmentID GetUnsealedSegmentID() const;
    size_t GetCurrentSegmentRowCount(Storage *storage_ptr) const;
    std::shared_ptr<TableCache> RestoreTableCache(Storage *storage_ptr) const;

    std::map<std::string, std::shared_ptr<MetaKey>> column_map_;
    std::map<SegmentID, std::shared_ptr<MetaObject>> segment_map_;
    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
    std::map<std::string, std::shared_ptr<MetaObject>> index_map_;
};

export struct MetaSegmentObject final : public MetaObject {
    MetaSegmentObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kSegment, meta_key) {}
    nlohmann::json ToJson() const final;

    BlockID GetCurrentBlockID() const;

    std::map<BlockID, std::shared_ptr<MetaObject>> block_map_;
    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
};

export struct MetaBlockObject final : public MetaObject {
    MetaBlockObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kBlock, meta_key) {}
    nlohmann::json ToJson() const final;

    std::map<ColumnID, std::shared_ptr<MetaKey>> column_map_;
    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
};

export struct MetaBlockColumnObject final : public MetaObject {
    MetaBlockColumnObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kBlockColumn, meta_key) {}
    nlohmann::json ToJson() const final;

    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
};

export struct MetaTableIndexObject final : public MetaObject {
    MetaTableIndexObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kTableIndex, meta_key) {}
    nlohmann::json ToJson() const final;
    std::shared_ptr<TableIndexCache> RestoreTableIndexCache(Storage *storage_ptr) const;

    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_;
    std::map<SegmentID, std::shared_ptr<MetaObject>> segment_map_;
};

export struct MetaSegmentIndexObject final : public MetaObject {
    MetaSegmentIndexObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kSegmentIndex, meta_key) {}
    nlohmann::json ToJson() const final;

    std::map<ChunkID, std::shared_ptr<MetaKey>> chunk_map_{};
};

export struct MetaChunkIndexObject final : public MetaObject {
    MetaChunkIndexObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kChunkIndex, meta_key) {}
    nlohmann::json ToJson() const final;

    std::map<std::string, std::shared_ptr<MetaKey>> tag_map_{};
};

export struct MetaPmObject final : public MetaObject {
    MetaPmObject(const std::shared_ptr<MetaKey> &meta_key) : MetaObject(MetaType::kPmStat, meta_key) {}
    nlohmann::json ToJson() const final;

    std::shared_ptr<MetaKey> object_{};
    std::map<std::string, std::shared_ptr<MetaKey>> path_map_{};
};

export struct MetaTree {
    static std::shared_ptr<MetaTree> MakeMetaTree(const std::vector<std::shared_ptr<MetaKey>> &meta_keys);

    struct MyHash {
        template <typename T, typename... Ts>
        static size_t GetHash(T &&t, Ts &&...args) {
            return std::hash<std::decay_t<T>>{}(t) ^ (... ^ (std::hash<std::decay_t<Ts>>{}(args) << 1));
        }

        template <typename T> // is_tuple_like
        size_t operator()(const T &tuple) const noexcept {
            return std::apply([](auto &&...elems) { return MyHash::GetHash(std::forward<decltype(elems)>(elems)...); }, tuple);
        }
    };

    bool SegmentExistInMetas(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id) const;

    bool BlockExistInMetas(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id, BlockID block_id) const;

    bool ColumnExistInMetas(const std::string &db_id_str, const std::string &table_id_str, ColumnID column_id) const;
    bool IndexExistInMetas(const std::string &db_id_str, const std::string &table_id_str, const std::string &index_id_str) const;

    bool SegmentIndexExistInMetas(const std::string &db_id_str,
                                  const std::string &table_id_str,
                                  const std::string &index_id_str,
                                  SegmentID segment_id) const;

    bool ChunkIndexExistInMetas(const std::string &db_id_str,
                                const std::string &table_id_str,
                                const std::string &index_id_str,
                                SegmentID segment_id,
                                ChunkID chunk_id) const;

public:
    void Prepare();
    static bool PathFilter(std::string_view path, CheckStmtType tag, std::optional<std::string> db_table_str);
    bool CheckData(const std::string &path);
    static std::unordered_set<std::string> GetDataVfsPathSet();
    static std::unordered_set<std::string> GetDataVfsOffPathSet();
    std::vector<std::string> CheckMetaDataMapping(CheckStmtType tag, std::optional<std::string> db_table_str);

    // std::vector<MetaTableObject *> ListTables() const;
    std::unique_ptr<SystemCache> RestoreSystemCache(Storage *storage_ptr) const;

    nlohmann::json ToJson() const;

    std::map<std::string, std::string> system_tag_map_{};
    std::map<std::string, std::shared_ptr<MetaDBObject>> db_map_{};
    std::map<std::string, std::shared_ptr<MetaPmObject>> pm_object_map_{};
    std::vector<std::shared_ptr<MetaKey>> metas_;

    // std::unordered_map<MetaType, std::tuple<std::string, std::string, SegmentID>> table_dic_;
    std::unordered_set<std::tuple<std::string, std::string, SegmentID>, MyHash> segment_dic_;
    std::unordered_set<std::tuple<std::string, std::string, SegmentID, BlockID>, MyHash> block_dic_;
    std::unordered_set<std::tuple<std::string, std::string, ColumnID>, MyHash> column_dic_;
    std::unordered_set<std::tuple<std::string, std::string, std::string>, MyHash> index_dic_;
    std::unordered_set<std::tuple<std::string, std::string, std::string, SegmentID>, MyHash> segment_index_dic_;
    std::unordered_set<std::tuple<std::string, std::string, std::string, SegmentID, ChunkID>, MyHash> chunk_index_dic_;
};

} // namespace infinity