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

module infinity_core:kv_utility.impl;

import :kv_utility;
import :kv_code;
import :kv_store;
import :index_base;
import :utility;
import :logger;
import :new_catalog;
import :infinity_context;
import :buffer_handle;
import :buffer_obj;
import :block_version;
import :buffer_manager;
import :infinity_exception;
import :config;

import std.compat;
import third_party;

namespace infinity {

std::shared_ptr<std::vector<SegmentID>> GetTableSegments(KVInstance *kv_instance,
                                                         const std::string &db_id_str,
                                                         const std::string &table_id_str,
                                                         TxnTimeStamp begin_ts,
                                                         TxnTimeStamp commit_ts) {
    std::shared_ptr<std::vector<SegmentID>> segment_ids = std::make_shared<std::vector<SegmentID>>();

    std::string segment_id_prefix = KeyEncode::CatalogTableSegmentKeyPrefix(db_id_str, table_id_str);
    auto iter = kv_instance->GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        TxnTimeStamp segment_commit_ts = std::stoull(iter->Value().ToString());
        if (segment_commit_ts > begin_ts and segment_commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
            LOG_DEBUG(fmt::format("SKIP SEGMENT: {} {} {}", iter->Key().ToString(), segment_commit_ts, begin_ts));
            iter->Next();
            continue;
        }
        // the key is committed before the txn or the key isn't committed
        SegmentID segment_id = std::stoull(iter->Key().ToString().substr(segment_id_prefix.size()));
        std::string drop_segment_ts{};
        kv_instance->Get(KeyEncode::DropSegmentKey(db_id_str, table_id_str, segment_id), drop_segment_ts);

        if (drop_segment_ts.empty() || (std::stoull(drop_segment_ts) > begin_ts && std::stoull(drop_segment_ts) != commit_ts)) {
            segment_ids->push_back(segment_id);
        }
        iter->Next();
    }
    std::sort(segment_ids->begin(), segment_ids->end());
    return segment_ids;
}

std::vector<SegmentID> GetTableIndexSegments(KVInstance *kv_instance,
                                             const std::string &db_id_str,
                                             const std::string &table_id_str,
                                             const std::string &index_id_str,
                                             TxnTimeStamp begin_ts,
                                             TxnTimeStamp commit_ts) {
    std::vector<SegmentID> segment_ids;

    const std::string segment_id_prefix = KeyEncode::CatalogIdxSegmentKeyPrefix(db_id_str, table_id_str, index_id_str);
    auto iter = kv_instance->GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        std::string key = iter->Key().ToString();
        auto [segment_id, is_segment_id] = ExtractU64FromStringSuffix(key, segment_id_prefix.size());
        if (is_segment_id) {
            TxnTimeStamp segment_commit_ts = std::stoull(iter->Value().ToString());
            if (segment_commit_ts > begin_ts && segment_commit_ts != commit_ts && segment_commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                LOG_DEBUG(fmt::format("SKIP SEGMENT INDEX: {} {} {}", iter->Key().ToString(), commit_ts, begin_ts));
                iter->Next();
                continue;
            }
            // the key is committed before the txn or the key isn't committed
            segment_ids.push_back(segment_id);
        } else {
            LOG_DEBUG(fmt::format("Key: {} isn't segment id", iter->Key().ToString()));
        }
        iter->Next();
    }
    std::sort(segment_ids.begin(), segment_ids.end());
    return segment_ids;
}

std::vector<BlockID> GetTableSegmentBlocks(KVInstance *kv_instance,
                                           const std::string &db_id_str,
                                           const std::string &table_id_str,
                                           SegmentID segment_id,
                                           TxnTimeStamp begin_ts,
                                           TxnTimeStamp commit_ts) {
    std::vector<BlockID> block_ids;

    std::string block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(db_id_str, table_id_str, segment_id);
    auto iter = kv_instance->GetIterator();
    iter->Seek(block_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
        TxnTimeStamp block_commit_ts = std::stoull(iter->Value().ToString());
        if (block_commit_ts > begin_ts and block_commit_ts != commit_ts and block_commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
            LOG_DEBUG(fmt::format("SKIP BLOCK: {} {} {}", iter->Key().ToString(), block_commit_ts, begin_ts));
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

std::vector<ColumnID> GetTableSegmentBlockColumns(KVInstance *kv_instance,
                                                  const std::string &db_id_str,
                                                  const std::string &table_id_str,
                                                  SegmentID segment_id,
                                                  BlockID block_id,
                                                  TxnTimeStamp begin_ts) {
    std::vector<ColumnID> column_ids;
    std::string block_column_id_prefix = KeyEncode::CatalogTableSegmentBlockColumnKeyPrefix(db_id_str, table_id_str, segment_id, block_id);
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

std::shared_ptr<IndexBase> GetTableIndexDef(KVInstance *kv_instance,
                                            const std::string &db_id_str,
                                            const std::string &table_id_str,
                                            const std::string &index_id_str,
                                            TxnTimeStamp begin_ts) {
    std::string index_def_key = KeyEncode::CatalogIndexTagKey(db_id_str, table_id_str, index_id_str, "index_base");
    std::string index_def_str;
    Status status = kv_instance->Get(index_def_key, index_def_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get index definition from kv store, key: {}, cause: {}", index_def_key, status.message()));
        return nullptr;
    }
    std::shared_ptr<IndexBase> index_base = IndexBase::Deserialize(index_def_str);
    return index_base;
}

size_t GetBlockRowCount(KVInstance *kv_instance,
                        const std::string &db_id_str,
                        const std::string &table_id_str,
                        SegmentID segment_id,
                        BlockID block_id,
                        TxnTimeStamp begin_ts,
                        TxnTimeStamp commit_ts) {

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    std::string block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");

    std::shared_ptr<BlockLock> block_lock;
    Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
    if (!status.ok()) {
        UnrecoverableError("Failed to get block lock");
    }

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    std::string version_filepath = fmt::format("{}/db_{}/tbl_{}/seg_{}/blk_{}/{}",
                                               InfinityContext::instance().config()->DataDir(),
                                               db_id_str,
                                               table_id_str,
                                               segment_id,
                                               block_id,
                                               BlockVersion::PATH);
    BufferObj *version_buffer = buffer_mgr->GetBufferObject(version_filepath);
    if (version_buffer == nullptr) {
        UnrecoverableError(fmt::format("Get version buffer failed: {}", version_filepath));
    }

    BufferHandle buffer_handle = version_buffer->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(buffer_handle.GetData());

    size_t row_cnt = 0;
    {
        std::shared_lock lock(block_lock->mtx_);
        row_cnt = block_version->GetRowCount(begin_ts);
        auto [offset, commit_cnt] = block_version->GetCommitRowCount(commit_ts);
        row_cnt += commit_cnt;
    }

    return row_cnt;
}

size_t GetSegmentRowCount(KVInstance *kv_instance,
                          const std::string &db_id_str,
                          const std::string &table_id_str,
                          SegmentID segment_id,
                          TxnTimeStamp begin_ts,
                          TxnTimeStamp commit_ts) {
    std::vector<BlockID> blocks = GetTableSegmentBlocks(kv_instance, db_id_str, table_id_str, segment_id, begin_ts, commit_ts);

    size_t segment_row_count = 0;
    for (BlockID block_id : blocks) {
        size_t block_row_cnt = GetBlockRowCount(kv_instance, db_id_str, table_id_str, segment_id, block_id, begin_ts, commit_ts);
        segment_row_count += block_row_cnt;
    }

    return segment_row_count;
}

std::string GetLastPartOfKey(const std::string &key, char delimiter) {
    size_t last_pos = key.rfind(delimiter);

    if (last_pos != std::string::npos) {
        return key.substr(last_pos + 1);
    } else {
        UnrecoverableError(fmt::format("No '{}' found", delimiter));
    }
    return key;
}

u64 GetTimestampFromKey(const std::string &key) {
    std::string ts_str = GetLastPartOfKey(key, '|');
    return std::stoull(ts_str);
}

} // namespace infinity