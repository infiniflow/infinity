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

module kv_utility;

import stl;
import kv_code;
import kv_store;
// import status;
import index_base;
import third_party;
import utility;
// import rocksdb_merge_operator;
import logger;

namespace infinity {

Vector<SegmentID> GetTableSegments(KVInstance *kv_instance, const String &db_id_str, const String &table_id_str, TxnTimeStamp begin_ts) {
    Vector<SegmentID> segment_ids;

    String segment_id_prefix = KeyEncode::CatalogTableSegmentKeyPrefix(db_id_str, table_id_str);
    auto iter = kv_instance->GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
            LOG_DEBUG(fmt::format("SKIP SEGMENT: {} {} {}", iter->Key().ToString(), commit_ts, begin_ts));
            iter->Next();
            continue;
        }
        // the key is committed before the txn or the key isn't committed
        SegmentID segment_id = std::stoull(iter->Key().ToString().substr(segment_id_prefix.size()));
        segment_ids.push_back(segment_id);
        iter->Next();
    }
    std::sort(segment_ids.begin(), segment_ids.end());
    return segment_ids;
}

Vector<SegmentID> GetTableIndexSegments(KVInstance *kv_instance,
                                        const String &db_id_str,
                                        const String &table_id_str,
                                        const String &index_id_str,
                                        TxnTimeStamp begin_ts) {
    Vector<SegmentID> segment_ids;

    String segment_id_prefix = KeyEncode::CatalogIdxSegmentKeyPrefix(db_id_str, table_id_str, index_id_str);
    auto iter = kv_instance->GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        String key = iter->Key().ToString();
        auto [segment_id, is_segment_id] = ExtractU64FromStringSuffix(key, segment_id_prefix.size());
        if (is_segment_id) {
            TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
            if (commit_ts > begin_ts and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                LOG_DEBUG(fmt::format("SKIP SEGMENT INDEX: {} {} {}", iter->Key().ToString(), commit_ts, begin_ts));
                iter->Next();
                continue;
            }
            // the key is committed before the txn or the key isn't committed
            segment_ids.push_back(segment_id);
        } else {
            LOG_DEBUG(fmt::format("Invalid segment id key: {}", iter->Key().ToString()));
        }
        iter->Next();
    }
    std::sort(segment_ids.begin(), segment_ids.end());
    return segment_ids;
}

Vector<BlockID>
GetTableSegmentBlocks(KVInstance *kv_instance, const String &db_id_str, const String &table_id_str, SegmentID segment_id, TxnTimeStamp begin_ts) {
    Vector<BlockID> block_ids;

    String block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(db_id_str, table_id_str, segment_id);
    auto iter = kv_instance->GetIterator();
    iter->Seek(block_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
            LOG_DEBUG(fmt::format("SKIP BLOCK: {} {} {}", iter->Key().ToString(), commit_ts, begin_ts));
            iter->Next();
            continue;
        }
        // the key is committed before the txn or the key isn't committed
        BlockID block_id = std::stoull(iter->Key().ToString().substr(block_id_prefix.size()));
        block_ids.push_back(block_id);
        iter->Next();
    }

    std::sort(block_ids.begin(), block_ids.end());
    return block_ids;
}

Vector<ColumnID> GetTableSegmentBlockColumns(KVInstance *kv_instance,
                                             const String &db_id_str,
                                             const String &table_id_str,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             TxnTimeStamp begin_ts) {
    Vector<ColumnID> column_ids;
    String block_column_id_prefix = KeyEncode::CatalogTableSegmentBlockColumnKeyPrefix(db_id_str, table_id_str, segment_id, block_id);
    auto iter = kv_instance->GetIterator();
    iter->Seek(block_column_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(block_column_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts) {
            LOG_DEBUG(fmt::format("SKIP BLOCK COLUMN: {} {} {}", iter->Key().ToString(), commit_ts, begin_ts));
            iter->Next();
            continue;
        }
        ColumnID column_id = std::stoull(iter->Key().ToString().substr(block_column_id_prefix.size()));
        column_ids.push_back(column_id);
        iter->Next();
    }
    std::sort(column_ids.begin(), column_ids.end());
    return column_ids;
}

SharedPtr<IndexBase>
GetTableIndexDef(KVInstance *kv_instance, const String &db_id_str, const String &table_id_str, const String &index_id_str, TxnTimeStamp begin_ts) {
    String index_def_key = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "index_base");
    String index_def_str;
    Status status = kv_instance->Get(index_def_key, index_def_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get index definition from kv store, key: {}, cause: {}", index_def_key, status.message()));
        return nullptr;
    }
    nlohmann::json index_def_json = nlohmann::json::parse(index_def_str);
    SharedPtr<IndexBase> index_base = IndexBase::Deserialize(index_def_json);
    return index_base;
}

} // namespace infinity