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

#include <ranges>
#include <thread>

module infinity_core;

import :stl;
import :bg_task;
import :logger;
import :infinity_exception;
import :third_party;
import :blocking_queue;
import :infinity_context;
import compilation_config;
import :defer_op;
import :bg_query_state;
import :base_memindex;
import :status;
import :default_values;
import :wal_manager;
import global_resource_usage;
import :txn_state;

import :new_txn_manager;
import :new_txn;
import :new_compaction_alg;
import :table_meeta;
import :db_meeta;
import :segment_meta;
import :bg_task;
import :base_txn_store;

namespace infinity {

CompactionProcessor::CompactionProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("CompactionProcessor");
#endif
}

CompactionProcessor::~CompactionProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("CompactionProcessor");
#endif
}

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

void CompactionProcessor::Submit(SharedPtr<BGTask> bg_task) {
    task_queue_.Enqueue(std::move(bg_task));
    ++task_count_;
}

void CompactionProcessor::NewDoCompact() {
    LOG_TRACE("Background task triggered compaction");
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    Vector<Pair<String, String>> db_table_names;
    {
        auto *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>("list table for compaction"), TransactionType::kNormal);

        Vector<String> db_names;
        Status status = new_txn->ListDatabase(db_names);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        for (const auto &db_name : db_names) {
            Vector<String> table_names;
            status = new_txn->ListTable(db_name, table_names);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            for (const auto &table_name : table_names) {
                db_table_names.emplace_back(db_name, table_name);
            }
        }
        status = new_txn_mgr->CommitTxn(new_txn);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
    }

    auto compact_table = [&](const String &db_name, const String &table_name, SharedPtr<BGTaskInfo> &bg_task_info) {
        auto new_txn_shared =
            new_txn_mgr->BeginTxnShared(MakeUnique<String>(fmt::format("compact table {}.{}", db_name, table_name)), TransactionType::kNormal);
        LOG_INFO(fmt::format("Compact begin ts: {}", new_txn_shared->BeginTS()));
        Status status = Status::OK();
        DeferFn defer_fn([&] {
            if (status.ok()) {
                Status commit_status = new_txn_mgr->CommitTxn(new_txn_shared.get());
                if (!commit_status.ok()) {
                    LOG_ERROR(fmt::format("Commit compact table {}.{} failed: {}", db_name, table_name, commit_status.message()));
                }

                CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(new_txn_shared->GetTxnStore());
                if (compact_txn_store != nullptr) {
                    // Record compact info
                    String task_text = fmt::format("Txn: {}, commit: {}, compact table: {}.{} with segments: {} into {}",
                                                   new_txn_shared->TxnID(),
                                                   new_txn_shared->CommitTS(),
                                                   db_name,
                                                   table_name,
                                                   fmt::join(compact_txn_store->deprecated_segment_ids_, ","),
                                                   compact_txn_store->new_segment_id_);

                    bg_task_info->task_info_list_.emplace_back(task_text);
                    if (commit_status.ok()) {
                        bg_task_info->status_list_.emplace_back("OK");
                    } else {
                        bg_task_info->status_list_.emplace_back(commit_status.message());
                    }
                }
            } else {
                LOG_ERROR(fmt::format("Compact table {}.{} failed: {}", db_name, table_name, status.message()));
                Status rollback_status = new_txn_mgr->RollBackTxn(new_txn_shared.get());
                if (!rollback_status.ok()) {
                    UnrecoverableError(rollback_status.message());
                }
                String task_text = fmt::format("Compact table: {}.{}", db_name, table_name);
                bg_task_info->task_info_list_.emplace_back(task_text);
                bg_task_info->status_list_.emplace_back(status.message());
            }
        });

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = new_txn_shared->GetTableMeta(db_name, table_name, db_meta, table_meta);
        if (!status.ok()) {
            return;
        }
        Vector<SegmentID> segment_ids;
        {
            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            segment_ids = *segment_ids_ptr;
            if (segment_ids.empty()) {
                LOG_TRACE(fmt::format("No segment to compact for table: {}.{}", db_name, table_name));
                return;
            }
            SegmentID unsealed_id = 0;
            status = table_meta->GetUnsealedSegmentID(unsealed_id);
            if (!status.ok()) {
                if (status.code() == ErrorCode::kNotFound) {
                    status = Status::OK(); // Ignore the error.
                } else {
                    UnrecoverableError(status.message());
                }
            } else {
                segment_ids.erase(std::remove(segment_ids.begin(), segment_ids.end(), unsealed_id), segment_ids.end());
            }
        }

        if (segment_ids.empty()) {
            LOG_TRACE(fmt::format("No segment to compact for table: {}.{}", db_name, table_name));
            return;
        }

        auto compaction_alg = NewCompactionAlg::GetInstance();

        for (SegmentID segment_id : segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            auto [segment_row_cnt, segment_status] = segment_meta.GetRowCnt1();
            if (!segment_status.ok()) {
                UnrecoverableError(segment_status.message());
            }

            compaction_alg->AddSegment(segment_id, segment_row_cnt);
        }
        Vector<SegmentID> compactible_segment_ids = compaction_alg->GetCompactiableSegments();

        if (!compactible_segment_ids.empty()) {
            status = new_txn_shared->Compact(db_name, table_name, compactible_segment_ids);
        }
    };

    if (db_table_names.empty()) {
        LOG_TRACE("No table to compact.");
    } else {
        SharedPtr<BGTaskInfo> bg_task_info = MakeShared<BGTaskInfo>(BGTaskType::kNotifyCompact);
        for (const auto &[db_name, table_name] : db_table_names) {
            compact_table(db_name, table_name, bg_task_info);
        }
        if (!bg_task_info->task_info_list_.empty()) {
            new_txn_mgr->AddTaskInfo(bg_task_info);
        }
    }
}

Status CompactionProcessor::NewManualCompact(const String &db_name, const String &table_name) {
    //    LOG_TRACE(fmt::format("Compact command triggered compaction: {}.{}", db_name, table_name));
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("compact table {}.{}", db_name, table_name)), TransactionType::kNormal);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = new_txn->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    Vector<SegmentID> segment_ids;
    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        segment_ids = *segment_ids_ptr;
        SegmentID unsealed_id = 0;
        status = table_meta->GetUnsealedSegmentID(unsealed_id);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
        } else {
            segment_ids.erase(std::remove(segment_ids.begin(), segment_ids.end(), unsealed_id), segment_ids.end());
        }
    }
    status = new_txn->Compact(db_name, table_name, segment_ids);
    if (!status.ok()) {
        return status;
    }
    status = new_txn_mgr->CommitTxn(new_txn);
    return status;
}

void CompactionProcessor::NewScanAndOptimize() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("optimize index"), TransactionType::kNormal);
    LOG_INFO(fmt::format("Optimize all indexes begin ts: {}", new_txn_shared->BeginTS()));

    SharedPtr<BGTaskInfo> bg_task_info = MakeShared<BGTaskInfo>(BGTaskType::kNotifyOptimize);
    Status status = new_txn_shared->OptimizeAllIndexes();
    if (status.ok()) {
        Status commit_status = new_txn_mgr->CommitTxn(new_txn_shared.get());
        if (!commit_status.ok()) {
            LOG_ERROR(fmt::format("Commit optimize all indexes failed: {}", commit_status.message()));
        }

        OptimizeIndexTxnStore *optimize_idx_store = static_cast<OptimizeIndexTxnStore *>(new_txn_shared->GetTxnStore());
        if (optimize_idx_store != nullptr) {
            for (const OptimizeIndexStoreEntry &store_entry : optimize_idx_store->entries_) {
                String task_text = fmt::format(
                    "Txn: {}, commit: {}, optimize index: {}.{}.{} segment: {} with chunks: {} into {}",
                    new_txn_shared->TxnID(),
                    new_txn_shared->CommitTS(),
                    store_entry.db_name_,
                    store_entry.table_name_,
                    store_entry.index_name_,
                    store_entry.segment_id_,
                    fmt::join(store_entry.deprecate_chunks_, ","),
                    fmt::join(store_entry.new_chunk_infos_ | std::views::transform([](const auto &info) { return info.chunk_id_; }), ","));

                bg_task_info->task_info_list_.emplace_back(task_text);
                if (commit_status.ok()) {
                    bg_task_info->status_list_.emplace_back("OK");
                } else {
                    bg_task_info->status_list_.emplace_back(commit_status.message());
                }
            }
        }
    } else {
        LOG_ERROR(fmt::format("optimize all indexes failed: {}", status.message()));
        Status rollback_status = new_txn_mgr->RollBackTxn(new_txn_shared.get());
        if (!rollback_status.ok()) {
            UnrecoverableError(rollback_status.message());
        }
        String task_text = "Optimize all indexes";
        bg_task_info->task_info_list_.emplace_back(task_text);
        bg_task_info->status_list_.emplace_back(status.message());
    }

    if (!bg_task_info->task_info_list_.empty()) {
        new_txn_mgr->AddTaskInfo(bg_task_info);
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
                case BGTaskType::kNewCompact: {
                    // Triggered by compact command
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Command compact start.");

                        auto *compact_task = static_cast<NewCompactTask *>(bg_task.get());

                        compact_task->result_status_ = NewManualCompact(compact_task->db_name_, compact_task->table_name_);

                        LOG_DEBUG("Command compact end.");
                    }
                    break;
                }
                case BGTaskType::kNotifyCompact: {
                    // Triggered by periodic thread
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Periodic compact start.");

                        //                        auto *compact_task = static_cast<NotifyCompactTask *>(bg_task.get());
                        NewDoCompact();

                        LOG_DEBUG("Periodic compact end.");
                    }
                    break;
                }
                case BGTaskType::kNotifyOptimize: {
                    //                    auto *optimize_task = static_cast<NotifyOptimizeTask *>(bg_task.get());

                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Optimize start.");

                        NewScanAndOptimize();
                        LOG_DEBUG("Optimize done.");
                    }
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid background task: {}", (u8)bg_task->type_);
                    UnrecoverableError(error_message);
                    break;
                }
            }
            bg_task->Complete();
        }
        task_count_ -= tasks.size();
        tasks.clear();
    }
}

} // namespace infinity
