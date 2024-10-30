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

#include <vector>

module cleanup_scanner;

import stl;
import catalog;
import base_entry;
import infinity_exception;
import status;
import logger;
import third_party;
import infinity_context;
import table_entry;
import table_index_entry;
import virtual_store;
import segment_index_entry;
import chunk_index_entry;

namespace infinity {

String CleanupInfoTracer::GetCleanupInfo() const {
    String info;
    info += fmt::format("Cleanup ts: {}\n", cleanup_ts_);
    for (const auto &path : cleanup_info_) {
        info += fmt::format("Cleanup: {}\n", path);
    }
    return info;
}

CleanupScanner::CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr)
    : catalog_(catalog), visible_ts_(visible_ts), buffer_mgr_(buffer_mgr) {}

// dropped is true denotes the data file can be cleaned up, or only metadata can be cleaned up
void CleanupScanner::AddEntry(SharedPtr<BaseEntry> entry, bool dropped) {
    switch (entry->entry_type_) {
        case EntryType::kTableIndex: {
            auto *table_index_entry = static_cast<TableIndexEntry *>(entry.get());
            TableEntry *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
            table_entry->InvalidateFullTextIndexCache(table_index_entry);
            break;
        }
        case EntryType::kSegmentIndex: {
            auto *segment_index_entry = static_cast<SegmentIndexEntry *>(entry.get());
            TableEntry *table_entry = segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry();
            table_entry->InvalidateFullTextSegmentIndexCache(segment_index_entry);
            break;
        }
        case EntryType::kChunkIndex: {
            auto *chunk_index_entry = static_cast<ChunkIndexEntry *>(entry.get());
            SegmentIndexEntry *segment_index_entry = chunk_index_entry->segment_index_entry_;
            TableEntry *table_entry = segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry();
            table_entry->InvalidateFullTextChunkIndexCache(chunk_index_entry);
            break;
        }
        default: {
        }
    }
    entries_.emplace_back(std::move(entry), dropped);
}

void CleanupScanner::Scan() {
    LOG_DEBUG(fmt::format("CleanupScanner: Start scanning, ts: {}", visible_ts_));
    catalog_->PickCleanup(this);
}

void CleanupScanner::Cleanup(CleanupInfoTracer *info_tracer) && {
    LOG_DEBUG(fmt::format("CleanupScanner cleanup {} entries", entries_.size()));
    for (auto &[entry, dropped] : entries_) {
        LOG_DEBUG(fmt::format("CleanupScanner cleanup entry: {}", entry->encode()));
        entry->Cleanup(info_tracer, dropped);
        entry.reset();
    }
    buffer_mgr_->RemoveClean();
}

void CleanupScanner::CleanupDir(const String &abs_dir) {
    try {
        VirtualStore::RemoveDirectory(abs_dir);
    } catch (const RecoverableException &e) {
        if (e.ErrorCode() == ErrorCode::kDirNotFound) {
            // this happens when delta checkpoint records "drop table/db/...", and cleanup is called.
            // then restart the system, table entry will be created but no directory will be found
            LOG_WARN(fmt::format("Cleanup: Dir {} not found. Skip", abs_dir));
        } else {
            String error_message = fmt::format("Cleanup {} encounter unexpected error: {}", abs_dir, e.what());
            UnrecoverableError(error_message);
        }
    }
}

} // namespace infinity
