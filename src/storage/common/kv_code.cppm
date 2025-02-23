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

export module kv_code;

import stl;

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
    static UniquePtr<KeyBase> FromString(const String &key_str);
};

export struct KeyDb : public KeyBase {
    KeyDb() : KeyBase(KeyType::kDb) {}
    String db_name_{};
    TxnTimeStamp ts_{};

    String ToString() const;
};

export struct KeyDbTag : public KeyBase {
    KeyDbTag() : KeyBase(KeyType::kDbTag) {}
    String db_id_{};
    String tag_name_{};

    String ToString() const;
};

export struct KeyTable : public KeyBase {
    KeyTable() : KeyBase(KeyType::kTable) {}
    String db_id_{};
    String table_name_{};
    TxnTimeStamp ts_{};

    String ToString() const;
};

export struct KeyTableTag : public KeyBase {
    KeyTableTag() : KeyBase(KeyType::kTableTag) {}
    String db_id_{};
    String table_id_{};
    String tag_name_{};

    String ToString() const;
};

export struct KeyIndex : public KeyBase {
    KeyIndex() : KeyBase(KeyType::kIndex) {}
    String db_id_{};
    String table_id_{};
    String index_name_{};
    TxnTimeStamp ts_{};

    String ToString() const;
};

export struct KeyIndexTag : public KeyBase {
    KeyIndexTag() : KeyBase(KeyType::kIndexTag) {}
    String db_id_{};
    String table_id_{};
    String index_id_{};
    String tag_name_{};

    String ToString() const;
};

export class KeyEncode {
public:
    static String CatalogDbKey(const String &db_name, TxnTimeStamp ts);
    static String CatalogDbPrefix(const String &db_name);
    static constexpr String kCatalogDbHeader = "catalog|db|";

    static String CatalogDbTagKey(const String &db_id, const String &tag_name);
    static String CatalogDbTagPrefix(const String &db_id, const String &tag_name);

    static String CatalogTableKey(const String &db_id, const String &table_name, TxnTimeStamp ts);
    static String CatalogTablePrefix(const String &db_id, const String &table_name);
    static String CatalogDbTablePrefix(const String &db_id);

    static String CatalogTableTagKey(const String &db_id, const String &table_id, const String &tag_name);
    static String CatalogTableTagPrefix(const String &db_id, const String &table_id, const String &tag_name);

    static String CatalogIndexKey(const String &db_id, const String &table_id, const String &index_name, TxnTimeStamp ts);
    static String CatalogIndexPrefix(const String &db_id, const String &table_id, const String &index_name);
    static String CatalogTableIndexPrefix(const String &db_id, const String &table_id);

    static String CatalogIndexTagKey(const String &db_id, const String &table_id, const String &index_name, const String &tag_name);
    static String CatalogIndexTagKeyPrefix(const String &db_id, const String &table_id, const String &index_name, const String &tag_name);

    static String CatalogIdxSegmentKey(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id);
    static String
    CatalogIdxSegmentTagKey(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id, const String &tag_name);
    static String CatalogIdxChunkPrefix(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id);
    static String CatalogIdxChunkTagKey(const String &db_id,
                                        const String &table_id,
                                        const String &index_id,
                                        SegmentID segment_id,
                                        ChunkID chunk_id,
                                        const String &tag_name);

    static String TableColumnKey(const String &db_id, const String &table_id, const String &column_name);
    static String TableColumnPrefix(const String &db_id, const String &table_id);

    static String TableColumnTagKey(const String &db_id, const String &table_id, const String &column_id, const String &tag_name);

    static String CatalogTableSegmentKey(const String &db_id, const String &table_id, SegmentID segment_id);
    static String CatalogTableSegmentTagKey(const String &db_id, const String &table_id, SegmentID segment_id, const String &tag_name);
    static String CatalogTableSegmentBlockKey(const String &db_id, const String &table_id, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts);
    static String
    CatalogTableSegmentBlockTagKey(const String &db_id, const String &table_id, SegmentID segment_id, BlockID block_id, const String &tag_name);
    static String CatalogTableSegmentBlockColumnKey(const String &db_id,
                                                    const String &table_id,
                                                    SegmentID segment_id,
                                                    BlockID block_id,
                                                    ColumnID column_id,
                                                    TxnTimeStamp ts);
    static String CatalogTableSegmentBlockColumnTagKey(const String &db_id,
                                                       const String &table_id,
                                                       SegmentID segment_id,
                                                       BlockID block_id,
                                                       ColumnID column_id,
                                                       const String &tag_name);
    //    static String MetaTableColumnKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaSegmentKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaSegmentBlockKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaSegmentBlockColumnKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaIndexKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaIndexSegmentKey(const String &key, const String& db_name, TxnTimeStamp ts);
    //    static String MetaIndexChunkKey(const String &key, const String& db_name, TxnTimeStamp ts);

    static String DatabaseKey(const String &db_name, TxnTimeStamp ts);
    static String DatabasePrefix(const String &db_name);

    static String TableKey(const String &db_name, const String &tbl_name, TxnTimeStamp ts);
    static String TableKeyPrefix(const String &db_name, const String &tbl_name);

    static String TableSegmentKey(const String &db_name, const String &tbl_name, SegmentID segment_id, TxnTimeStamp ts);
    static String TableSegmentKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id);

    static String TableSegmentBlockKey(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts);
    static String TableSegmentBlockKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id);

    static String TableSegmentBlockColumnKey(const String &db_name,
                                             const String &tbl_name,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             ColumnID column_id,
                                             TxnTimeStamp ts);
    static String
    TableSegmentBlockColumnKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id, ColumnID column_id);

    static String TableIndexKey(const String &db_name, const String &tbl_name, const String &index_name, TxnTimeStamp ts);
    static String TableIndexKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);

    static String
    TableIndexSegmentKey(const String &db_name, const String &tbl_name, const String &index_name, SegmentID segment_id, TxnTimeStamp ts);
    static String TableIndexSegmentKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);

    static String TableIndexChunkKey(const String &db_name,
                                     const String &tbl_name,
                                     const String &index_name,
                                     SegmentID segment_id,
                                     ChunkID chunk_id,
                                     TxnTimeStamp ts);
    static String TableIndexChunkKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);
};

} // namespace infinity