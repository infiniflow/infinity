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

#include <tuple>

export module infinity_core:meta_tree;

import :stl;
import :meta_type;
import :meta_key;
import :third_party;
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
    MetaObject(MetaType type, const SharedPtr<MetaKey> &meta_key) : type_(type), meta_key_(meta_key) {}
    virtual ~MetaObject() = default;
    virtual nlohmann::json ToJson() const = 0;

    MetaType type_{MetaType::kInvalid};
    SharedPtr<MetaKey> meta_key_{nullptr};
};

export struct MetaDBObject final : public MetaObject {
    MetaDBObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kDB, meta_key) {}
    nlohmann::json ToJson() const final;

    SharedPtr<DbCache> RestoreDbCache(Storage *storage_ptr) const;

    Map<String, SharedPtr<MetaObject>> table_map_;
    Map<String, SharedPtr<MetaKey>> tag_map_;
};

export struct MetaTableObject final : public MetaObject {
    MetaTableObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kTable, meta_key) {}
    nlohmann::json ToJson() const final;

    const String &GetTableName() const;
    SegmentID GetNextSegmentID() const;
    SegmentID GetUnsealedSegmentID() const;
    SizeT GetCurrentSegmentRowCount(Storage *storage_ptr) const;
    SharedPtr<TableCache> RestoreTableCache(Storage *storage_ptr) const;

    Map<String, SharedPtr<MetaKey>> column_map_;
    Map<SegmentID, SharedPtr<MetaObject>> segment_map_;
    Map<String, SharedPtr<MetaKey>> tag_map_;
    Map<String, SharedPtr<MetaObject>> index_map_;
};

export struct MetaSegmentObject final : public MetaObject {
    MetaSegmentObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kSegment, meta_key) {}
    nlohmann::json ToJson() const final;

    BlockID GetCurrentBlockID() const;

    Map<BlockID, SharedPtr<MetaObject>> block_map_;
    Map<String, SharedPtr<MetaKey>> tag_map_;
};

export struct MetaBlockObject final : public MetaObject {
    MetaBlockObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kBlock, meta_key) {}
    nlohmann::json ToJson() const final;

    Map<ColumnID, SharedPtr<MetaKey>> column_map_;
    Map<String, SharedPtr<MetaKey>> tag_map_;
};

export struct MetaBlockColumnObject final : public MetaObject {
    MetaBlockColumnObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kBlockColumn, meta_key) {}
    nlohmann::json ToJson() const final;

    Map<String, SharedPtr<MetaKey>> tag_map_;
};

export struct MetaTableIndexObject final : public MetaObject {
    MetaTableIndexObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kTableIndex, meta_key) {}
    nlohmann::json ToJson() const final;
    SharedPtr<TableIndexCache> RestoreTableIndexCache(Storage *storage_ptr) const;

    Map<String, SharedPtr<MetaKey>> tag_map_;
    Map<SegmentID, SharedPtr<MetaObject>> segment_map_;
};

export struct MetaSegmentIndexObject final : public MetaObject {
    MetaSegmentIndexObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kSegmentIndex, meta_key) {}
    nlohmann::json ToJson() const final;

    Map<ChunkID, SharedPtr<MetaKey>> chunk_map_{};
};

export struct MetaChunkIndexObject final : public MetaObject {
    MetaChunkIndexObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kChunkIndex, meta_key) {}
    nlohmann::json ToJson() const final;

    Map<String, SharedPtr<MetaKey>> tag_map_{};
};

export struct MetaPmObject final : public MetaObject {
    MetaPmObject(const SharedPtr<MetaKey> &meta_key) : MetaObject(MetaType::kPmStat, meta_key) {}
    nlohmann::json ToJson() const final;

    SharedPtr<MetaKey> object_{};
    Map<String, SharedPtr<MetaKey>> path_map_{};
};

export struct MetaTree {
    static SharedPtr<MetaTree> MakeMetaTree(const Vector<SharedPtr<MetaKey>> &meta_keys);

    struct MyHash {
        template <typename T, typename... Ts>
        static SizeT GetHash(T &&t, Ts &&...args) {
            return std::hash<std::decay_t<T>>{}(t) ^ (... ^ (std::hash<std::decay_t<Ts>>{}(args) << 1));
        }

        template <typename T> // is_tuple_like
        SizeT operator()(const T &tuple) const noexcept {
            return std::apply([](auto &&...elems) { return MyHash::GetHash(std::forward<decltype(elems)>(elems)...); }, tuple);
        }
    };

    bool SegmentExistInMetas(const String &db_id_str, const String &table_id_str, SegmentID segment_id) const;

    bool BlockExistInMetas(const String &db_id_str, const String &table_id_str, SegmentID segment_id, BlockID block_id) const;

    bool ColumnExistInMetas(const String &db_id_str, const String &table_id_str, ColumnID column_id) const;
    bool IndexExistInMetas(const String &db_id_str, const String &table_id_str, const String &index_id_str) const;

    bool SegmentIndexExistInMetas(const String &db_id_str, const String &table_id_str, const String &index_id_str, SegmentID segment_id) const;

    bool ChunkIndexExistInMetas(const String &db_id_str,
                                const String &table_id_str,
                                const String &index_id_str,
                                SegmentID segment_id,
                                ChunkID chunk_id) const;

public:
    void Prepare();
    static bool PathFilter(std::string_view path, CheckStmtType tag, Optional<String> db_table_str);
    bool CheckData(const String &path);
    static HashSet<String> GetDataVfsPathSet();
    static HashSet<String> GetDataVfsOffPathSet();
    Vector<String> CheckMetaDataMapping(CheckStmtType tag, Optional<String> db_table_str);

    Vector<MetaTableObject *> ListTables() const;
    UniquePtr<SystemCache> RestoreSystemCache(Storage *storage_ptr) const;

    nlohmann::json ToJson() const;

    Map<String, String> system_tag_map_{};
    Map<String, SharedPtr<MetaDBObject>> db_map_{};
    Map<String, SharedPtr<MetaPmObject>> pm_object_map_{};
    Vector<SharedPtr<MetaKey>> metas_;

    // HashMap<MetaType, Tuple<String, String, SegmentID>> table_dic_;
    HashSet<Tuple<String, String, SegmentID>, MyHash> segment_dic_;
    HashSet<Tuple<String, String, SegmentID, BlockID>, MyHash> block_dic_;
    HashSet<Tuple<String, String, ColumnID>, MyHash> column_dic_;
    HashSet<Tuple<String, String, String>, MyHash> index_dic_;
    HashSet<Tuple<String, String, String, SegmentID>, MyHash> segment_index_dic_;
    HashSet<Tuple<String, String, String, SegmentID, ChunkID>, MyHash> chunk_index_dic_;
};

} // namespace infinity