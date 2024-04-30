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
import bg_task;
import compact_segments_task;
import catalog;
import txn_manager;
import db_entry;
import table_entry;
import logger;
import infinity_exception;
import third_party;
import blocking_queue;

namespace infinity {

CompactionProcessor::CompactionProcessor(Catalog *catalog, TxnManager *txn_mgr) : catalog_(catalog), txn_mgr_(txn_mgr) {}

void CompactionProcessor::Start() {
    LOG_INFO("Compaction processor is started.");
    processor_thread_ = Thread([this] { Process(); });
}

void CompactionProcessor::Stop() {
    LOG_INFO("Compaction processor is stopping.");
    SharedPtr<StopProcessorTask> stop_task = MakeShared<StopProcessorTask>();
    this->Submit(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Compaction processor is stopped.");
}

void CompactionProcessor::Submit(SharedPtr<BGTask> bg_task) { task_queue_.Enqueue(std::move(bg_task)); }

Vector<UniquePtr<CompactSegmentsTask>> CompactionProcessor::ScanForCompact() {
    auto generate_txn = [this]() { return txn_mgr_->BeginTxn(); };

    Txn *scan_txn = txn_mgr_->BeginTxn();

    Vector<UniquePtr<CompactSegmentsTask>> compaction_tasks;
    TransactionID txn_id = scan_txn->TxnID();
    TxnTimeStamp begin_ts = scan_txn->BeginTS();
    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            while (true) {
                auto compaction_info = table_entry->CheckCompaction(generate_txn);
                if (!compaction_info) {
                    break;
                }

                UniquePtr<CompactSegmentsTask> compact_task =
                    CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry, std::move(compaction_info->segments_), compaction_info->txn_);
                compaction_tasks.emplace_back(std::move(compact_task));
            }
        }
    }

    txn_mgr_->CommitTxn(scan_txn);
    return compaction_tasks;
}

void CompactionProcessor::ScanAndOptimize() {
    Txn *opt_txn = txn_mgr_->BeginTxn();
    TransactionID txn_id = opt_txn->TxnID();
    TxnTimeStamp begin_ts = opt_txn->BeginTS();

    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            table_entry->OptimizeIndex(opt_txn);
        }
    }
    try {
        txn_mgr_->CommitTxn(opt_txn);
    } catch (const RecoverableException &e) {
        txn_mgr_->RollBackTxn(opt_txn);
    }
}

void CompactionProcessor::Process() {
    bool running = true;
    while (running) {
        Deque<SharedPtr<BGTask>> tasks;
        task_queue_.DequeueBulk(tasks);
        for (const auto &bg_task : tasks) {
            switch (bg_task->type_) {
                case BGTaskType::kStopProcessor: {
                    running = false;
                    break;
                }
                case BGTaskType::kNotifyCompact: {
                    Vector<UniquePtr<CompactSegmentsTask>> compact_tasks = this->ScanForCompact();
                    for (auto &compact_task : compact_tasks) {
                        LOG_TRACE(fmt::format("Compact {} start.", compact_task->table_name()));
                        compact_task->Execute();
                        if (compact_task->TryCommitTxn()) {
                            LOG_TRACE(fmt::format("Compact {} done.", compact_task->table_name()));
                        } else {
                            LOG_TRACE(fmt::format("Compact {} rollback.", compact_task->table_name()));
                        }
                    }
                    break;
                }
                case BGTaskType::kNotifyOptimize: {
                    LOG_TRACE("Optimize start.");
                    ScanAndOptimize();
                    LOG_TRACE("Optimize done.");
                    break;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid background task: {}", (u8)bg_task->type_));
                    break;
                }
            }
            bg_task->Complete();
        }
    }
}

} // namespace infinity