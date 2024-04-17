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

#include <thread>

module compaction_process;

import stl;
import compact_segments_task;
import catalog;
import txn_manager;
import db_entry;
import table_entry;
import logger;
import infinity_exception;
import third_party;

namespace infinity {

CompactionProcessor::CompactionProcessor(Catalog *catalog, TxnManager *txn_mgr, const std::chrono::seconds &interval)
    : catalog_(catalog), txn_mgr_(txn_mgr), interval_(interval) {}

void CompactionProcessor::Start() {
    stop_.store(false);
    processor_thread_ = Thread([this] { Process(); });
}

void CompactionProcessor::Stop() {
    stop_.store(true);
    processor_thread_.join();
}

Vector<UniquePtr<CompactSegmentsTask>> CompactionProcessor::Scan() {
    auto generate_txn = [this]() { return txn_mgr_->BeginTxn(); };

    Txn *scan_txn = txn_mgr_->BeginTxn();

    Vector<UniquePtr<CompactSegmentsTask>> compaction_tasks;
    TransactionID txn_id = scan_txn->TxnID();
    TxnTimeStamp begin_ts = scan_txn->BeginTS();
    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            auto compaction_info = table_entry->CheckCompaction(generate_txn);
            if (compaction_info) {
                UniquePtr<CompactSegmentsTask> compact_task =
                    CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry, std::move(compaction_info->segments_), compaction_info->txn_);
                compaction_tasks.emplace_back(std::move(compact_task));
            }
        }
    }

    txn_mgr_->CommitTxn(scan_txn);
    return compaction_tasks;
}

void CompactionProcessor::Process() {
    Vector<UniquePtr<CompactSegmentsTask>> compact_tasks;
    while (!stop_.load()) {
        std::this_thread::sleep_for(interval_);

        compact_tasks = this->Scan();

        for (auto &compact_task : compact_tasks) {
            LOG_INFO(fmt::format("Compact {} start.", compact_task->table_name()));
            compact_task->Execute();
            if (compact_task->TryCommitTxn()) {
                LOG_INFO(fmt::format("Compact {} done.", compact_task->table_name()));
            } else {
                LOG_INFO(fmt::format("Compact {} rollback.", compact_task->table_name()));
            }
            break;
        }
        compact_tasks.clear();
    }
}

} // namespace infinity