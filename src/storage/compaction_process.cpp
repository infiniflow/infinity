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
import catalog;
import txn_manager;
import db_entry;
import table_entry;
import logger;
import infinity_exception;
import third_party;
import blocking_queue;
import bg_query_state;
import query_context;
import infinity_context;
import compact_statement;
import compilation_config;
import defer_op;
import bg_query_state;
import txn_store;
import memindex_tracer;
import segment_entry;
import table_index_entry;
import base_memindex;
import segment_index_entry;
import status;
import default_values;
import wal_manager;
import global_resource_usage;
import txn_state;

import new_txn_manager;
import new_txn;
import new_compaction_alg;
import table_meeta;
import db_meeta;
import segment_meta;

namespace infinity {

CompactionProcessor::CompactionProcessor(Catalog *catalog, TxnManager *txn_mgr) : catalog_(catalog), txn_mgr_(txn_mgr) {
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

void CompactionProcessor::DoCompact() {
    Txn *scan_txn = txn_mgr_->BeginTxn(MakeUnique<String>("ScanForCompact"), TransactionType::kNormal);
    bool success = false;
    DeferFn defer_fn([&] {
        if (!success) {
            txn_mgr_->RollBackTxn(scan_txn);
        }
    });

    Vector<Pair<UniquePtr<BaseStatement>, Txn *>> statements = this->ScanForCompact(scan_txn);
    Vector<Pair<BGQueryContextWrapper, BGQueryState>> wrappers;
    for (const auto &[statement, txn] : statements) {
        BGQueryContextWrapper wrapper(txn);
        BGQueryState state;
        try {
            bool res = wrapper.query_context_->ExecuteBGStatement(statement.get(), state);
            if (res) {
                wrappers.emplace_back(std::move(wrapper), std::move(state));
            }
        } catch (const std::exception &e) {
            LOG_CRITICAL(fmt::format("DoCompact failed: {}", e.what()));
            throw;
        }
    }
    for (auto &[wrapper, query_state] : wrappers) {
        TxnTimeStamp commit_ts_out = 0;
        wrapper.query_context_->JoinBGStatement(query_state, commit_ts_out);
    }
    txn_mgr_->CommitTxn(scan_txn);
    success = true;
}

void CompactionProcessor::NewDoCompact() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    Vector<Pair<String, String>> db_table_names;
    {
        auto *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>("scan table"), TransactionType::kNormal);

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

    auto compact_table = [&](const String &db_name, const String &table_name) {
        auto *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>(fmt::format("compact table {}.{}", db_name, table_name)), TransactionType::kNormal);
        Status status = Status::OK();
        DeferFn defer_fn([&] {
            if (status.ok()) {
                status = new_txn_mgr->CommitTxn(new_txn);
            }
            if (!status.ok()) {
                status = new_txn_mgr->RollBackTxn(new_txn);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
            }
        });

        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = new_txn->GetTableMeta(db_name, table_name, db_meta, table_meta);
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
            SegmentID unsealed_id = 0;
            status = table_meta->GetUnsealedSegmentID(unsealed_id);
            if (!status.ok()) {
                if (status.code() != ErrorCode::kNotFound) {
                    UnrecoverableError(status.message());
                }
            } else {
                segment_ids.erase(std::remove(segment_ids.begin(), segment_ids.end(), unsealed_id), segment_ids.end());
            }
        }

        auto compaction_alg = NewCompactionAlg::GetInstance();

        for (SegmentID segment_id : segment_ids) {
            SegmentMeta segment_meta(segment_id, *table_meta);
            auto [segment_row_cnt, status] = segment_meta.GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            compaction_alg->AddSegment(segment_id, segment_row_cnt);
        }
        Vector<SegmentID> compactible_segment_ids = compaction_alg->GetCompactiableSegments();

        status = new_txn->Compact(db_name, table_name, compactible_segment_ids);
    };
    for (const auto &[db_name, table_name] : db_table_names) {
        compact_table(db_name, table_name);
    }
}

void CompactionProcessor::NewManualCompact(NewTxn *new_txn, const String &db_name, const String &table_name) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = new_txn->GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    Vector<SegmentID> segment_ids;
    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        segment_ids = *segment_ids_ptr;
        SegmentID unsealed_id = 0;
        status = table_meta->GetUnsealedSegmentID(unsealed_id);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                UnrecoverableError(status.message());
            }
        } else {
            segment_ids.erase(std::remove(segment_ids.begin(), segment_ids.end(), unsealed_id), segment_ids.end());
        }
    }
    status = new_txn->Compact(db_name, table_name, segment_ids);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

TxnTimeStamp
CompactionProcessor::ManualDoCompact(const String &schema_name, const String &table_name, bool rollback, Optional<std::function<void()>> mid_func) {
    auto statement = MakeUnique<ManualCompactStatement>(schema_name, table_name);
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("ManualCompact"), TransactionType::kNormal);
    LOG_INFO(fmt::format("Compact txn id {}.", txn->TxnID()));
    BGQueryContextWrapper wrapper(txn);
    BGQueryState state;
    bool res = wrapper.query_context_->ExecuteBGStatement(statement.get(), state);
    if (mid_func) {
        mid_func.value()();
    }
    TxnTimeStamp out_commit_ts = 0;
    if (res) {
        wrapper.query_context_->JoinBGStatement(state, out_commit_ts, rollback);
    }
    return out_commit_ts;
}

Vector<Pair<UniquePtr<BaseStatement>, Txn *>> CompactionProcessor::ScanForCompact(Txn *scan_txn) {

    Vector<Pair<UniquePtr<BaseStatement>, Txn *>> compaction_tasks;
    TransactionID txn_id = scan_txn->TxnID();
    TxnTimeStamp begin_ts = scan_txn->BeginTS();
    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            while (true) {
                Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Compact"), TransactionType::kNormal);
                TransactionID txn_id = txn->TxnID();
                auto compact_segments = table_entry->CheckCompaction(txn_id);
                if (compact_segments.empty()) {
                    txn_mgr_->RollBackTxn(txn);
                    break;
                }

                LOG_TRACE("Construct compact task: ");
                for (SegmentEntry *segment_ptr : compact_segments) {
                    LOG_TRACE(fmt::format("To compact segment: {}", segment_ptr->ToString()));
                }

                compaction_tasks.emplace_back(
                    MakeUnique<AutoCompactStatement>(*table_entry->GetDBName(), *table_entry->GetTableName(), std::move(compact_segments)),
                    txn);
            }
        }
    }

    return compaction_tasks;
}

void CompactionProcessor::ScanAndOptimize() {
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("ScanAndOptimize"), TransactionType::kNormal);
    LOG_INFO(fmt::format("ScanAndOptimize opt begin ts: {}", txn->BeginTS()));
    txn->OptimizeIndexes();
    try {
        txn_mgr_->CommitTxn(txn);
    } catch (const RecoverableException &e) {
        txn_mgr_->RollBackTxn(txn);
    }
}

void CompactionProcessor::NewScanAndOptimize() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>("optimize index"), TransactionType::kNormal);
    LOG_INFO(fmt::format("ScanAndOptimize opt begin ts: {}", new_txn->BeginTS()));

    Status status = new_txn->OptimizeAllIndexes();
    if (status.ok()) {
        status = new_txn_mgr->CommitTxn(new_txn);
    }
    if (!status.ok()) {
        status = new_txn_mgr->RollBackTxn(new_txn);
        if (!status.ok()) {
            RecoverableError(status);
        }
    }
}

void CompactionProcessor::DoDump(DumpIndexTask *dump_task) {
    auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();

    Txn *dump_txn = dump_task->txn_;
    if (!dump_txn) {
        auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

        NewTxn *new_txn = dump_task->new_txn_;
        const String &db_name = dump_task->mem_index_->db_name_;
        const String &table_name = dump_task->mem_index_->table_name_;
        const String &index_name = dump_task->mem_index_->index_name_;
        SegmentID segment_id = dump_task->mem_index_->segment_id_;
        
        Status status = new_txn->DumpMemIndex(db_name, table_name, index_name, segment_id);
        if (status.ok()) {
            status = new_txn_mgr->CommitTxn(new_txn);
        }
        if (!status.ok()) {
            Status rollback_status = new_txn_mgr->RollBackTxn(new_txn);
            if (!rollback_status.ok()) {
                UnrecoverableError(rollback_status.message());
            }
        }

        return;
    }

    BaseMemIndex *mem_index = dump_task->mem_index_;
    try {
        TableIndexEntry *table_index_entry = mem_index->table_index_entry();
        auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
        TxnTableStore *txn_table_store = dump_txn->GetTxnTableStore(table_entry);
        SizeT dump_size = 0;
        table_index_entry->MemIndexDump(dump_txn, txn_table_store, false /*spill*/, &dump_size);
        LOG_TRACE(fmt::format("Dump size = {}", dump_size));

        txn_mgr_->CommitTxn(dump_txn);

        memindex_tracer->DumpDone(dump_size, mem_index);
    } catch (const RecoverableException &e) {
        txn_mgr_->RollBackTxn(dump_txn);
        memindex_tracer->DumpFail(mem_index);
        LOG_WARN(fmt::format("Dump index task failed: {}, task: {}", e.what(), dump_task->ToString()));
    }
}

void CompactionProcessor::DoDumpByline(DumpIndexBylineTask *dump_task) {
    String msg = fmt::format("Dump index by line, table name: {}, index name: {}", *dump_task->table_name_, *dump_task->index_name_);
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>(msg), TransactionType::kNormal);
    try {
        auto [table_index_entry, status] = txn->GetIndexByName(*dump_task->db_name_, *dump_task->table_name_, *dump_task->index_name_);
        if (!status.ok()) {
            RecoverableError(status);
        }
        auto *dumped_chunk = dump_task->dumped_chunk_.get();
        if (dumped_chunk->deprecate_ts_ != UNCOMMIT_TS) {
            RecoverableError(Status::TxnRollback(txn->TxnID(), fmt::format("Dumped chunk {} is deleted.", dumped_chunk->encode())));
        }

        SharedPtr<SegmentIndexEntry> segment_index_entry = table_index_entry->GetSegment(dump_task->segment_id_, txn);
        if (!segment_index_entry) {
            RecoverableError(Status::TxnRollback(txn->TxnID(), fmt::format("Cannot find segment index entry with id: {}", dump_task->segment_id_)));
        }
        segment_index_entry->AddWalIndexDump(dumped_chunk, txn);

        txn_mgr_->CommitTxn(txn);
    } catch (const RecoverableException &e) {
        txn_mgr_->RollBackTxn(txn);
        LOG_WARN(fmt::format("Rollback {}", msg));
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
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Do compact start.");

                        auto *compact_task = static_cast<NewCompactTask *>(bg_task.get());

                        NewManualCompact(compact_task->new_txn_, compact_task->db_name_, compact_task->table_name_);

                        LOG_DEBUG("Do compact end.");
                    }
                    break;
                }
                case BGTaskType::kNotifyCompact: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Do compact start.");

                        auto *compact_task = static_cast<NotifyCompactTask *>(bg_task.get());
                        if (compact_task->new_compact_) {
                            NewDoCompact();
                        } else {
                            DoCompact();
                        }

                        LOG_DEBUG("Do compact end.");
                    }
                    break;
                }
                case BGTaskType::kNotifyOptimize: {
                    auto *optimize_task = static_cast<NotifyOptimizeTask *>(bg_task.get());

                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Optimize start.");

                        if (!optimize_task->new_optimize_) {
                            ScanAndOptimize();
                        } else {
                            NewScanAndOptimize();
                        }
                        LOG_DEBUG("Optimize done.");
                    }
                    break;
                }
                case BGTaskType::kDumpIndex: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        auto dump_task = static_cast<DumpIndexTask *>(bg_task.get());
                        LOG_DEBUG(dump_task->ToString());
                        // Trigger transaction to save the mem index
                        DoDump(dump_task);
                        LOG_DEBUG("Dump index done.");
                    }
                    break;
                }
                case BGTaskType::kDumpIndexByline: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        if (auto cmd = test_commander_.Check(BGTaskType::kDumpIndexByline)) {
                            if (cmd.value() == "stuck for 3 seconds") {
                                LOG_INFO("Compact process stuck for 3 seconds");
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                            }
                        }
                        auto dump_task = static_cast<DumpIndexBylineTask *>(bg_task.get());
                        LOG_DEBUG(dump_task->ToString());
                        // Trigger transaction to save the mem index
                        DoDumpByline(dump_task);
                        LOG_DEBUG("Dump index byline done.");
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
