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

#include <string>

module meta_key;

import column_def;
import utility;
import third_party;
import kv_code;
import infinity_exception;
import meta_type;
import default_values;

namespace infinity {

ColumnMetaKey::ColumnMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, SharedPtr<ColumnDef> column_def)
    : MetaKey(MetaType::kBlockColumn), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id),
      block_id_(block_id), column_def_(std::move(column_def)) {}

ColumnMetaKey::~ColumnMetaKey() = default;

String DBMetaKey::ToString() const { return fmt::format("db: {}:{}", KeyEncode::CatalogDbKey(db_name_, commit_ts_), db_id_str_); }

String DBTagMetaKey::ToString() const { return fmt::format("db_tag: {}:{}", KeyEncode::CatalogDbTagKey(db_id_str_, tag_name_), value_); }

String TableMetaKey::ToString() const {
    return fmt::format("table: {}:{}", KeyEncode::CatalogTableKey(db_id_str_, table_name_, commit_ts_), table_id_str_);
}
String TableColumnMetaKey::ToString() const {
    return fmt::format("table_column: {}:{}", KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name_), value_);
}
String TableTagMetaKey::ToString() const {
    return fmt::format("table_tag: {}:{}", KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag_name_), value_);
}

String SegmentMetaKey::ToString() const {
    return fmt::format("segment: {}:{}", KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id_), commit_ts_);
}

String SegmentTagMetaKey::ToString() const {
    if (tag_name_ == "fast_rough_filter") {
        return fmt::format("segment_tag: {}", KeyEncode::CatalogTableSegmentTagKey(db_id_str_, table_id_str_, segment_id_, tag_name_));
    }
    return fmt::format("segment_tag: {}:{}", KeyEncode::CatalogTableSegmentTagKey(db_id_str_, table_id_str_, segment_id_, tag_name_), value_);
}

String BlockMetaKey::ToString() const {
    return fmt::format("block: {}:{}", KeyEncode::CatalogTableSegmentBlockKey(db_id_str_, table_id_str_, segment_id_, block_id_), commit_ts_);
}

String BlockTagMetaKey::ToString() const {
    return fmt::format("block_tag: {}:{}",
                       KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str_, table_id_str_, segment_id_, block_id_, tag_name_),
                       value_);
}

String ColumnMetaKey::ToString() const { return fmt::format("block column: not implemented"); }

String TableIndexMetaKey::ToString() const {
    return fmt::format("table_index: {}:{}", KeyEncode::CatalogIndexKey(db_id_str_, table_id_str_, index_name_, commit_ts_), index_id_str_);
}

String TableIndexTagMetaKey::ToString() const {
    return fmt::format("table_index_tag: {}:{}", KeyEncode::CatalogIndexTagKey(db_id_str_, table_id_str_, index_id_str_, tag_name_), value_);
}

String SegmentIndexMetaKey::ToString() const {
    return fmt::format("table_index: {}:{}", KeyEncode::CatalogIdxSegmentKey(db_id_str_, table_id_str_, index_id_str_, segment_id_), commit_ts_);
}

String SegmentIndexTagMetaKey::ToString() const {
    return fmt::format("table_index_tag: {}:{}",
                       KeyEncode::CatalogIdxSegmentTagKey(db_id_str_, table_id_str_, index_id_str_, segment_id_, tag_name_),
                       value_);
}

String ChunkIndexMetaKey::ToString() const {
    return fmt::format("table_index: {}:{}",
                       KeyEncode::CatalogIdxChunkKey(db_id_str_, table_id_str_, index_id_str_, segment_id_, chunk_id_),
                       commit_ts_);
}

String ChunkIndexTagMetaKey::ToString() const {
    return fmt::format("table_index: {}:{}",
                       KeyEncode::CatalogIdxChunkTagKey(db_id_str_, table_id_str_, index_id_str_, segment_id_, chunk_id_, tag_name_),
                       value_);
}

String SystemTagMetaKey::ToString() const { return fmt::format("system_tag: {}:{}", tag_name_, value_); }

String PmPathMetaKey::ToString() const { return fmt::format("pm_path: {}:{}", KeyEncode::PMObjectKey(path_key_), value_); }

String PmObjectMetaKey::ToString() const { return fmt::format("pm_object: {}:{}", KeyEncode::PMObjectStatKey(object_key_), value_); }

String DropMetaKey::ToString() const { return fmt::format("drop_key: drop|{}:{}", object_key_, value_); }

nlohmann::json DBMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["db_id"] = std::stoull(db_id_str_);
    json_res["db_name"] = db_name_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }

    return json_res;
}

nlohmann::json DBTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = value_;
    return json_res;
}

nlohmann::json TableMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["table_id"] = std::stoull(table_id_str_);
    json_res["table_name"] = table_name_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json TableColumnMetaKey::ToJson() const {
    nlohmann::json json_res;
    // json_res["column_definition"] = nlohmann::json::parse(value_);
    json_res.push_back(nlohmann::json::parse(value_));
    return json_res;
}

nlohmann::json TableTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json SegmentMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["segment_id"] = segment_id_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json SegmentTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["tag_name"] = tag_name_;
    // TODO: fast rough filter isn't deserialized here.
    //    json_res["tag_value"] = value_;
    return json_res;
}

nlohmann::json BlockMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["block_id"] = block_id_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json BlockTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json ColumnMetaKey::ToJson() const {
    nlohmann::json json_res;
    return json_res;
}

nlohmann::json TableIndexMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["index_id"] = index_id_str_;
    json_res["index_name"] = index_name_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json TableIndexTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json SegmentIndexMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["segment_id"] = segment_id_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json SegmentIndexTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json ChunkIndexMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["chunk_id"] = chunk_id_;
    if (commit_ts_ != UNCOMMIT_TS) {
        json_res["commit_ts"] = commit_ts_;
    }
    return json_res;
}

nlohmann::json ChunkIndexTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json SystemTagMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res[tag_name_] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json PmPathMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["path"] = path_key_;
    json_res["description"] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json PmObjectMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["object"] = object_key_;
    json_res["stat"] = nlohmann::json::parse(value_);
    return json_res;
}

nlohmann::json DropMetaKey::ToJson() const {
    nlohmann::json json_res;
    json_res["drop_key"] = object_key_;
    json_res["value"] = nlohmann::json::parse(value_);
    return json_res;
}

SharedPtr<MetaKey> MetaParse(const String &key, const String &value) {
    Vector<String> fields = infinity::Partition(key, '|');

    auto fields_size = fields.size();

    if (fields[0] == "catalog" && fields[1] == "db") {
        const String &db_name_str = fields[2];
        const String &commit_ts_str = fields[3];
        const String &db_id_str = value;
        SharedPtr<DBMetaKey> db_meta_key = MakeShared<DBMetaKey>(db_id_str, db_name_str);
        db_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return db_meta_key;
    }

    if (fields[0] == "catalog" && fields[1] == "tbl") {
        const String &db_id_str = fields[2];
        const String &table_name_str = fields[3];
        const String &commit_ts_str = fields[4];
        const String &table_id_str = value;

        SharedPtr<TableMetaKey> table_meta_key = MakeShared<TableMetaKey>(db_id_str, table_id_str, table_name_str);
        table_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return table_meta_key;
    }

    if (fields[0] == "catalog" && fields[1] == "seg") {
        const String &db_id_str = fields[2];
        const String &table_id_str = fields[3];
        const String &segment_id_str = fields[4];
        const String &commit_ts_str = value;
        SegmentID segment_id = std::stoul(segment_id_str);
        auto segment_meta_key = MakeShared<SegmentMetaKey>(db_id_str, table_id_str, segment_id);
        segment_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return segment_meta_key;
    }

    // construct segment tag meta key
    if (fields[0] == "seg") {
        const String &db_id_str = fields[1];
        const String &table_id_str = fields[2];
        const String &segment_id_str = fields[3];
        const String &tag_name_str = fields[4];
        SegmentID segment_id = std::stoul(segment_id_str);
        auto segment_tag_meta_key = MakeShared<SegmentTagMetaKey>(db_id_str, table_id_str, segment_id, tag_name_str);
        segment_tag_meta_key->value_ = value;
        return segment_tag_meta_key;
    }

    if (fields[0] == "catalog" && fields[1] == "blk") {
        const String &db_id_str = fields[2];
        const String &table_id_str = fields[3];
        const String &segment_id_str = fields[4];
        const String &block_id_str = fields[5];
        const String &commit_ts_str = value;
        SegmentID segment_id = std::stoul(segment_id_str);
        BlockID block_id = std::stoul(block_id_str);
        auto block_meta_key = MakeShared<BlockMetaKey>(db_id_str, table_id_str, segment_id, block_id);
        block_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return block_meta_key;
    }

    if (fields[0] == "db") {
        const String &db_id_str = fields[1];
        const String &tag_name_str = fields[2];
        SharedPtr<DBTagMetaKey> db_tag_meta_key = MakeShared<DBTagMetaKey>(db_id_str, tag_name_str);
        db_tag_meta_key->value_ = value;
        return db_tag_meta_key;
    }

    if (fields[0] == "tbl") {
        if (fields[1] == "col") {
            const String &db_id_str = fields[2];
            const String &table_id_str = fields[3];
            const String &column_name_str = fields[4];
            SharedPtr<TableColumnMetaKey> table_column_meta_key = MakeShared<TableColumnMetaKey>(db_id_str, table_id_str, column_name_str);
            table_column_meta_key->value_ = value;
            return table_column_meta_key;
        }
        const String &db_id_str = fields[1];
        const String &table_id_str = fields[2];
        const String &tag_name_str = fields[3];
        SharedPtr<TableTagMetaKey> table_tag_meta_key = MakeShared<TableTagMetaKey>(db_id_str, table_id_str, tag_name_str);
        table_tag_meta_key->value_ = value;
        return table_tag_meta_key;
    }

    if (fields[0] == "catalog" && fields[1] == "idx") {
        const String &db_id_str = fields[2];
        const String &table_id_str = fields[3];
        const String &index_name_str = fields[4];
        const String &commit_ts_str = fields[5];
        const String &index_id_str = value;

        SharedPtr<TableIndexMetaKey> table_index_meta_key = MakeShared<TableIndexMetaKey>(db_id_str, table_id_str, index_id_str, index_name_str);
        table_index_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return table_index_meta_key;
    }

    if (fields[0] == "idx") {
        const String &db_id_str = fields[1];
        const String &table_id_str = fields[2];
        const String &index_id_str = fields[3];
        const String &tag_name_str = fields[4];
        SharedPtr<TableIndexTagMetaKey> table_index_tag_meta_key =
            MakeShared<TableIndexTagMetaKey>(db_id_str, table_id_str, index_id_str, tag_name_str);
        table_index_tag_meta_key->value_ = value;
        return table_index_tag_meta_key;
    }

    if (fields[0] == "idx_seg") {
        const String &db_id_str = fields[1];
        const String &table_id_str = fields[2];
        const String &index_id_str = fields[3];
        SegmentID segment_id = std::stoull(fields[4]);
        if (fields_size == 6) {
            const String &tag_name_str = fields[5];
            auto segment_index_tag_meta_key = MakeShared<SegmentIndexTagMetaKey>(db_id_str, table_id_str, index_id_str, segment_id, tag_name_str);
            segment_index_tag_meta_key->value_ = value;
            return segment_index_tag_meta_key;
        }
        auto segment_index_meta_key = MakeShared<SegmentIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id);
        segment_index_meta_key->commit_ts_ = std::stoull(value);
        return segment_index_meta_key;
    }

    if (fields[0] == "idx_chunk") {
        const String &db_id_str = fields[1];
        const String &table_id_str = fields[2];
        const String &index_id_str = fields[3];
        SegmentID segment_id = std::stoull(fields[4]);
        ChunkID chunk_id = std::stoull(fields[5]);
        if (fields_size == 7) {
            const String &tag_name_str = fields[6];
            auto chunk_index_tag_meta_key =
                MakeShared<ChunkIndexTagMetaKey>(db_id_str, table_id_str, index_id_str, segment_id, chunk_id, tag_name_str);
            chunk_index_tag_meta_key->value_ = value;
            return chunk_index_tag_meta_key;
        }
        auto chunk_index_meta_key = MakeShared<ChunkIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id, chunk_id);
        chunk_index_meta_key->commit_ts_ = std::stoull(value);
        return chunk_index_meta_key;
    }

    if (fields[0] == "pm") {
        if (fields[1] == "object") {
            const String &path_key = fields[2];
            SharedPtr<PmPathMetaKey> pm_path_meta_key = MakeShared<PmPathMetaKey>(path_key);
            pm_path_meta_key->value_ = value; //
            return pm_path_meta_key;
        }
        if (fields[1] == "object_stat") {
            const String &object_key = fields[2];
            SharedPtr<PmObjectMetaKey> pm_object_meta_key = MakeShared<PmObjectMetaKey>(object_key);
            pm_object_meta_key->value_ = value;
            return pm_object_meta_key;
        }
        UnrecoverableError(fmt::format("Unexpected key: {}:{}", key, value));
    }

    if (fields[0] == "drop") {
        const String &object_key = fields[1];
        SharedPtr<DropMetaKey> drop_meta_key = MakeShared<DropMetaKey>(object_key);
        drop_meta_key->value_ = value;
        return drop_meta_key;
    }

    const String &tag_name_str = fields[0];
    SharedPtr<SystemTagMetaKey> system_tag_meta_key = MakeShared<SystemTagMetaKey>(tag_name_str);
    system_tag_meta_key->value_ = value;
    return system_tag_meta_key;
}

} // namespace infinity