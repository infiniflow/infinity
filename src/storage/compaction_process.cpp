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

void CompactionProcessor::Submit(SharedPtr<BGTask> bg_task) {
    task_queue_.Enqueue(std::move(bg_task));
    ++task_count_;
}

void CompactionProcessor::DoCompact() {
    Txn *scan_txn = txn_mgr_->BeginTxn(MakeUnique<String>("ScanForCompact"));
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
        bool res = wrapper.query_context_->ExecuteBGStatement(statement.get(), state);
        if (res) {
            wrappers.emplace_back(std::move(wrapper), std::move(state));
        }
    }
    for (auto &[wrapper, query_state] : wrappers) {
        TxnTimeStamp commit_ts_out = 0;
        wrapper.query_context_->JoinBGStatement(query_state, commit_ts_out);
    }
    txn_mgr_->CommitTxn(scan_txn);
    success = true;
}

TxnTimeStamp
CompactionProcessor::ManualDoCompact(const String &schema_name, const String &table_name, bool rollback, Optional<std::function<void()>> mid_func) {
    auto statement = MakeUnique<ManualCompactStatement>(schema_name, table_name);
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("ManualCompact"));
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
                Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Compact"));
                TransactionID txn_id = txn->TxnID();
                auto compact_segments = table_entry->CheckCompaction(txn_id);
                if (compact_segments.empty()) {
                    txn_mgr_->RollBackTxn(txn);
                    break;
                }

                compaction_tasks.emplace_back(MakeUnique<AutoCompactStatement>(table_entry, std::move(compact_segments)), txn);
            }
        }
    }

    return compaction_tasks;
}

void CompactionProcessor::ScanAndOptimize() {
    Txn *opt_txn = txn_mgr_->BeginTxn(MakeUnique<String>("ScanAndOptimize"));
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

void CompactionProcessor::DoDump(DumpIndexTask *dump_task) {
    Txn *dump_txn = txn_mgr_->BeginTxn(MakeUnique<String>(dump_task->ToString()));
    TransactionID txn_id = dump_txn->TxnID();
    TxnTimeStamp begin_ts = dump_txn->BeginTS();

    auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
    try {
        auto [table_entry, status1] = catalog_->GetTableByName(*dump_task->db_name_, *dump_task->table_name_, txn_id, begin_ts);
        if (!status1.ok()) {
            RecoverableError(status1);
        }
        auto [table_index_entry, status2] = table_entry->GetIndex(*dump_task->index_name_, txn_id, begin_ts);
        if (!status2.ok()) {
            RecoverableError(status2);
        }

        TxnTableStore *txn_table_store = dump_txn->GetTxnTableStore(table_entry);
        SizeT dump_size = 0;
        table_index_entry->MemIndexDump(dump_txn, txn_table_store, false /*spill*/, &dump_size);

        txn_mgr_->CommitTxn(dump_txn);
        memindex_tracer->DumpDone(dump_size, dump_task->task_id_);
    } catch (const RecoverableException &e) {
        txn_mgr_->RollBackTxn(dump_txn);
        memindex_tracer->DumpFail(dump_task->task_id_);
        LOG_WARN(fmt::format("Dump index task failed: {}, task: {}", e.what(), dump_task->ToString()));
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
                    LOG_DEBUG("Do compact start.");
                    DoCompact();
                    LOG_DEBUG("Do compact end.");
                    break;
                }
                case BGTaskType::kNotifyOptimize: {
                    LOG_DEBUG("Optimize start.");
                    ScanAndOptimize();
                    LOG_DEBUG("Optimize done.");
                    break;
                }
                case BGTaskType::kDumpIndex: {
                    auto dump_task = static_cast<DumpIndexTask *>(bg_task.get());
                    LOG_DEBUG(dump_task->ToString());
                    DoDump(dump_task);
                    LOG_DEBUG("Dump index done.");
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