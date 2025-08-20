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

export module infinity_core:kv_code;

import :infinity_type;

import std;

import column_def;

namespace infinity {

enum class KeyType {
    kDb,
    kDbTag,
    kTable,
    kTableTag,
    kIndex,
    kIndexTag,
    kInvalid,
};

export struct KeyBase {
    KeyBase(KeyType key_type) : key_type_(key_type) {}
    KeyType key_type_{KeyType::kInvalid};
    static std::unique_ptr<KeyBase> FromString(const std::string &key_str);
};

export struct KeyDb : public KeyBase {
    KeyDb() : KeyBase(KeyType::kDb) {}
    std::string db_name_{};
    TxnTimeStamp ts_{};

    std::string ToString() const;
};

export struct KeyDbTag : public KeyBase {
    KeyDbTag() : KeyBase(KeyType::kDbTag) {}
    std::string db_id_{};
    std::string tag_name_{};

    std::string ToString() const;
};

export struct KeyTable : public KeyBase {
    KeyTable() : KeyBase(KeyType::kTable) {}
    std::string db_id_{};
    std::string table_name_{};
    TxnTimeStamp ts_{};

    std::string ToString() const;
};

export struct KeyTableTag : public KeyBase {
    KeyTableTag() : KeyBase(KeyType::kTableTag) {}
    std::string db_id_{};
    std::string table_id_{};
    std::string tag_name_{};

    std::string ToString() const;
};

export struct KeyIndex : public KeyBase {
    KeyIndex() : KeyBase(KeyType::kIndex) {}
    std::string db_id_{};
    std::string table_id_{};
    std::string index_name_{};
    TxnTimeStamp ts_{};

    std::string ToString() const;
};

export struct KeyIndexTag : public KeyBase {
    KeyIndexTag() : KeyBase(KeyType::kIndexTag) {}
    std::string db_id_{};
    std::string table_id_{};
    std::string index_id_{};
    std::string tag_name_{};

    std::string ToString() const;
};

export class KeyEncode {
public:
    static std::string CatalogDbKey(const std::string &db_name, TxnTimeStamp ts);
    static std::string CatalogDbPrefix(const std::string &db_name);
    static constexpr std::string kCatalogDbHeader = "catalog|db|";

    static std::string CatalogDbTagKey(const std::string &db_id, const std::string &tag_name);
    static std::string CatalogDbTagPrefix(const std::string &db_id, const std::string &tag_name);

    static std::string CatalogTableKey(const std::string &db_id, const std::string &table_name, TxnTimeStamp ts);
    static std::string CatalogTablePrefix(const std::string &db_id, const std::string &table_name);
    static std::string CatalogDbTablePrefix(const std::string &db_id);

    static std::string CatalogTableTagKey(const std::string &db_id, const std::string &table_id, const std::string &tag_name);
    static std::string CatalogTableTagPrefix(const std::string &db_id, const std::string &table_id, const std::string &tag_name);

    static std::string CatalogIndexKey(const std::string &db_id, const std::string &table_id, const std::string &index_name, TxnTimeStamp ts);
    static std::string CatalogIndexPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_name);
    static std::string CatalogTableIndexPrefix(const std::string &db_id, const std::string &table_id);

    static std::string
    CatalogIndexTagKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, const std::string &tag_name);
    static std::string
    CatalogIndexTagKeyPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id, const std::string &tag_name);

    static std::string CatalogIdxSegmentKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id);
    static std::string CatalogIdxSegmentKeyPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id);
    static std::string CatalogIdxSegmentTagKey(const std::string &db_id,
                                               const std::string &table_id,
                                               const std::string &index_id,
                                               SegmentID segment_id,
                                               const std::string &tag_name);

    static std::string
    CatalogIdxChunkKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id, ChunkID chunk_id);
    static std::string
    CatalogIdxChunkPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id);
    static std::string CatalogIdxChunkTagKey(const std::string &db_id,
                                             const std::string &table_id,
                                             const std::string &index_id,
                                             SegmentID segment_id,
                                             ChunkID chunk_id,
                                             const std::string &tag_name);

    static std::string TableColumnKey(const std::string &db_id, const std::string &table_id, const std::string &column_name, TxnTimeStamp ts);
    static std::string TableColumnPrefix(const std::string &db_id, const std::string &table_id);
    static std::string TableColumnPrefix(const std::string &db_id, const std::string &table_id, const std::string &column_name);

    static std::string
    TableColumnTagKey(const std::string &db_id, const std::string &table_id, const std::string &column_id, const std::string &tag_name);

    static std::string CatalogTableSegmentKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id);
    static std::string CatalogTableSegmentKeyPrefix(const std::string &db_id, const std::string &table_id);
    static std::string
    CatalogTableSegmentTagKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id, const std::string &tag_name);
    static std::string CatalogTableSegmentBlockKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id, BlockID block_id);
    static std::string CatalogTableSegmentBlockKeyPrefix(const std::string &db_id, const std::string &table_id, SegmentID segment_id);
    static std::string CatalogTableSegmentBlockTagKey(const std::string &db_id,
                                                      const std::string &table_id,
                                                      SegmentID segment_id,
                                                      BlockID block_id,
                                                      const std::string &tag_name);
    static std::string CatalogTableSegmentBlockColumnKey(const std::string &db_id,
                                                         const std::string &table_id,
                                                         SegmentID segment_id,
                                                         BlockID block_id,
                                                         ColumnID column_id,
                                                         TxnTimeStamp ts);
    static std::string CatalogTableSegmentBlockColumnKeyPrefix(const std::string &db_id,
                                                               const std::string &table_id,
                                                               SegmentID segment_id,
                                                               BlockID block_id /*, ColumnID column_id*/);
    static std::string CatalogTableSegmentBlockColumnTagKey(const std::string &db_id,
                                                            const std::string &table_id,
                                                            SegmentID segment_id,
                                                            BlockID block_id,
                                                            ColumnID column_id,
                                                            const std::string &tag_name);
    //    static std::string MetaTableColumnKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaSegmentKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaSegmentBlockKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaSegmentBlockColumnKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaIndexKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaIndexSegmentKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);
    //    static std::string MetaIndexChunkKey(const std::string &key, const std::string& db_name, TxnTimeStamp ts);

    static std::string DatabaseKey(const std::string &db_name, TxnTimeStamp ts);
    static std::string DatabasePrefix(const std::string &db_name);

    static std::string TableKey(const std::string &db_name, const std::string &tbl_name, TxnTimeStamp ts);
    static std::string TableKeyPrefix(const std::string &db_name, const std::string &tbl_name);

    static std::string TableSegmentKey(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, TxnTimeStamp ts);
    static std::string TableSegmentKeyPrefix(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id);

    static std::string
    TableSegmentBlockKey(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts);
    static std::string TableSegmentBlockKeyPrefix(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, BlockID block_id);

    static std::string TableSegmentBlockColumnKey(const std::string &db_name,
                                                  const std::string &tbl_name,
                                                  SegmentID segment_id,
                                                  BlockID block_id,
                                                  ColumnID column_id,
                                                  TxnTimeStamp ts);
    static std::string TableSegmentBlockColumnKeyPrefix(const std::string &db_name,
                                                        const std::string &tbl_name,
                                                        SegmentID segment_id,
                                                        BlockID block_id,
                                                        ColumnID column_id);

    static std::string TableIndexKey(const std::string &db_name, const std::string &tbl_name, const std::string &index_name, TxnTimeStamp ts);
    static std::string TableIndexKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name);

    static std::string TableIndexSegmentKey(const std::string &db_name,
                                            const std::string &tbl_name,
                                            const std::string &index_name,
                                            SegmentID segment_id,
                                            TxnTimeStamp ts);
    static std::string TableIndexSegmentKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name);

    static std::string TableIndexChunkKey(const std::string &db_name,
                                          const std::string &tbl_name,
                                          const std::string &index_name,
                                          SegmentID segment_id,
                                          ChunkID chunk_id,
                                          TxnTimeStamp ts);
    static std::string TableIndexChunkKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name);

    static std::string PMObjectPrefix();

    static std::string PMObjectStatPrefix();

    static std::string PMObjectKey(const std::string &key);

    static std::string PMObjectStatKey(const std::string &key);

    static std::string DropDBKey(const std::string &db_name, const TxnTimeStamp &commit_ts, const std::string &db_id_str);

    static std::string
    DropTableKey(const std::string &db_id_str, const std::string &table_name, const std::string &table_id_str, TxnTimeStamp create_ts);

    static std::string
    RenameTableKey(const std::string &db_id_str, const std::string &table_name, const std::string &table_id_str, TxnTimeStamp create_ts);

    static std::string DropTableKeyPrefix(const std::string &db_id_str, const std::string &table_name);

    static std::string DropSegmentKey(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id);

    static std::string DropBlockKey(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id, BlockID block_id);

    static std::string
    DropTableColumnKey(const std::string &db_id_str, const std::string &table_id_str, const std::string &column_name, TxnTimeStamp create_ts);

    static std::string DropBlockColumnKey(const std::string &db_id_str,
                                          const std::string &table_id_str,
                                          SegmentID segment_id,
                                          BlockID block_id,
                                          const std::shared_ptr<ColumnDef> &column_def);

    static std::string DropTableIndexKey(const std::string &db_id_str,
                                         const std::string &table_id_str,
                                         const std::string &index_name,
                                         const TxnTimeStamp &commit_ts,
                                         const std::string &index_id_str);

    static std::string
    DropSegmentIndexKey(const std::string &db_id_str, const std::string &table_id_str, const std::string &index_id_str, SegmentID segment_id);

    static std::string DropChunkIndexKey(const std::string &db_id_str,
                                         const std::string &table_id_str,
                                         const std::string &index_id_str,
                                         SegmentID segment_id,
                                         ChunkID chunk_id);
};

} // namespace infinity