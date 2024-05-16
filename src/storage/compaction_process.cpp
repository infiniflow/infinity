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
import session;
import compilation_config;
import defer_op;

namespace infinity {

struct BGQueryContextWrapper {
    UniquePtr<QueryContext> query_context_;

    SessionManager *session_mgr_;
    SharedPtr<BaseSession> session_;

    BGQueryContextWrapper(BGQueryContextWrapper &&other)
        : query_context_(std::move(other.query_context_)), session_mgr_(other.session_mgr_), session_(std::move(other.session_)) {
        other.session_mgr_ = nullptr;
    }

    BGQueryContextWrapper(Txn *txn, SessionManager *session_mgr) : session_mgr_(session_mgr) {
        session_ = session_mgr_->CreateLocalSession();
        query_context_ = MakeUnique<QueryContext>(session_.get());
        query_context_->Init(InfinityContext::instance().config(),
                             InfinityContext::instance().task_scheduler(),
                             InfinityContext::instance().storage(),
                             InfinityContext::instance().resource_manager(),
                             InfinityContext::instance().session_manager());
        query_context_->SetTxn(txn);
    }

    ~BGQueryContextWrapper() {
        if (session_mgr_ != nullptr) {
            auto *session = query_context_->current_session();
            session_mgr_->RemoveSessionByID(session->session_id());
        }
    }
};

CompactionProcessor::CompactionProcessor(Catalog *catalog, TxnManager *txn_mgr)
    : catalog_(catalog), txn_mgr_(txn_mgr), session_mgr_(InfinityContext::instance().session_manager()) {}

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
        BGQueryContextWrapper wrapper(txn, session_mgr_);
        BGQueryState state;
        bool res = wrapper.query_context_->ExecuteBGStatement(statement.get(), state);
        if (res) {
            wrappers.emplace_back(std::move(wrapper), std::move(state));
        }
    }
    for (auto &[wrapper, query_state] : wrappers) {
        TxnTimeStamp commit_ts = 0;
        wrapper.query_context_->JoinBGStatement(query_state, commit_ts);
    }
    txn_mgr_->CommitTxn(scan_txn);
    success = true;
}

TxnTimeStamp
CompactionProcessor::ManualDoCompact(const String &schema_name, const String &table_name, bool rollback, Optional<std::function<void()>> mid_func) {
    TxnTimeStamp commit_ts = 0;

    auto statement = MakeUnique<ManualCompactStatement>(schema_name, table_name);
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("ManualCompact"));
    BGQueryContextWrapper wrapper(txn, session_mgr_);
    BGQueryState state;
    bool res = wrapper.query_context_->ExecuteBGStatement(statement.get(), state);
    if (mid_func) {
        mid_func.value()();
    }
    if (res) {
        wrapper.query_context_->JoinBGStatement(state, commit_ts, rollback);
    }
    return commit_ts;
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
                    DoCompact();
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