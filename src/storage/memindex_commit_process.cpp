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

#include <bits/chrono.h>
#include <vector>

module memindex_commit_process;

import stl;
import memory_indexer;
import default_values;
import catalog;
import db_meta;
import db_entry;
import table_entry;
import table_index_entry;
import index_base;
import create_index_info;
import bg_query_state;
import txn_manager;
import txn;
import physical_memindex_commit;
import data_type;
import logical_type;
import logger;
import third_party;

namespace infinity {

MemIndexCommitProcessor::MemIndexCommitProcessor(Catalog *catalog, TxnManager *txn_mgr) : running_(false), catalog_(catalog), txn_mgr_(txn_mgr) {}

void MemIndexCommitProcessor::Start() {
    running_.store(true);
    mem_index_commit_thread_ = Thread([this]() { MemIndexCommitLoop(); });
}

void MemIndexCommitProcessor::Stop() {
    LOG_INFO("MemIndexCommitProcessor is stopping...");
    running_.store(false);
    mem_index_commit_thread_.join();
    LOG_INFO("MemIndexCommitProcessor is stopped.");
}

MemIndexCommitProcessor::~MemIndexCommitProcessor() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("MemIndexCommitLoop was stopped...");
        return;
    }
    mem_index_commit_thread_.join();
}

void MemIndexCommitProcessor::MemIndexCommitLoop() {
    auto prev_time = std::chrono::system_clock::now();
    while (running_.load()) {
        auto current_time = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - prev_time).count() >= 1) {
            MemIndexCommit();
            prev_time = current_time;
        }
    }
}

Vector<MemoryIndexer *> MemIndexCommitProcessor::ScanForMemoryIndexers() {
    Vector<MemoryIndexer *> mem_indexers;
    TransactionID txn_id = 0UL;
    TxnTimeStamp begin_ts = MAX_TIMESTAMP;

    Vector<DBEntry *> db_entries;
    {
        auto db_meta_map_guard = catalog_->GetDBMetaMap();
        for (auto &[_, db_meta] : *db_meta_map_guard) {
            auto [db_entry, status] = db_meta->GetEntryNolock(txn_id, begin_ts);
            if (status.ok()) {
                db_entries.push_back(db_entry);
            }
        }
    }
    Vector<TableEntry *> table_entries;
    for (auto *db_entry : db_entries) {
        auto table_meta_map_guard = db_entry->GetTableMetaMap();
        for (auto &[_, table_meta] : *table_meta_map_guard) {
            auto [table_entry, status] = table_meta->GetEntryNolock(txn_id, begin_ts);
            if (status.ok()) {
                table_entries.push_back(table_entry);
            }
        }
    }
    for (auto *table_entry : table_entries) {
        auto table_index_meta_map_guard = table_entry->GetTableIndexMetaMap();
        for (auto &[_, table_index_meta] : *table_index_meta_map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                continue;
            }
            const IndexBase *index_base = table_index_entry->index_base();
            if (index_base->index_type_ != IndexType::kFullText) {
                continue;
            }
            auto last_segment = table_index_entry->last_segment();
            if (last_segment.get() == nullptr || last_segment->GetMemoryIndexer() == nullptr) {
                continue;
            }
            LOG_TRACE(fmt::format("Try commit memindex for table: {}, index: {}", *table_entry->GetTableName(), *table_index_entry->GetIndexName()));
            mem_indexers.push_back(last_segment->GetMemoryIndexer());
        }
    }
    return mem_indexers;
}

void MemIndexCommitProcessor::MemIndexCommit() {
    Vector<MemoryIndexer *> mem_indexers = ScanForMemoryIndexers();
    Vector<Pair<BGQueryContextWrapper, BGQueryState>> wrappers;

    for (auto *mem_indexer : mem_indexers) {
        BGQueryState bg_state;
        auto physical_memindex_commit = this->MakeCommitOperator(mem_indexer);
        Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("CommitMemIndex"));
        BGQueryContextWrapper wrapper(txn);
        bool res = wrapper.query_context_->ExecuteBGOperator(std::move(physical_memindex_commit), bg_state);
        if (res) {
            wrappers.emplace_back(std::move(wrapper), std::move(bg_state));
        }
    }
    for (auto &[wrapper, query_state] : wrappers) {
        TxnTimeStamp commit_ts_out = 0;
        wrapper.query_context_->JoinBGStatement(query_state, commit_ts_out);
    }
}

UniquePtr<PhysicalMemIndexCommit> MemIndexCommitProcessor::MakeCommitOperator(MemoryIndexer *memory_indexer) {
    u64 id = 0;
    auto output_names = MakeShared<Vector<String>>();
    output_names->push_back("OK");
    auto output_types = MakeShared<Vector<SharedPtr<DataType>>>();
    output_types->push_back(MakeShared<DataType>(LogicalType::kInteger));
    return MakeUnique<PhysicalMemIndexCommit>(id, memory_indexer, nullptr, output_names, output_types);
}

} // namespace infinity