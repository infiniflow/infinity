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

#include <csignal>
#include <cstdio>
#include <sstream>
#include <memory>

module infinity_core:query_context.impl;

import :query_context;

import base_statement;
import parser_result;
import sql_parser;
import admin_statement;
import show_statement;
import parser_assert;
import global_resource_usage;
import :logical_planner;
import :optimizer;
import :physical_planner;
import :fragment_builder;
import :resource_manager;
import :infinity_context;
import :session_manager;
import :txn_state;
import :new_txn;
import :new_catalog;
import :bg_query_state;
import :new_txn_manager;
import :admin_executor;

namespace infinity {

QueryContext::QueryContext(BaseSession *session) : session_ptr_(session) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("QueryContext");
#endif
}

QueryContext::~QueryContext() {
    UnInit();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("QueryContext");
#endif
}

void QueryContext::Init(Config *global_config_ptr,
                        TaskScheduler *scheduler_ptr,
                        Storage *storage_ptr,
                        ResourceManager *resource_manager_ptr,
                        SessionManager *session_manager,
                        PersistenceManager *persistence_manager) {
    global_config_ = global_config_ptr;
    scheduler_ = scheduler_ptr;
    storage_ = storage_ptr;
    resource_manager_ = resource_manager_ptr;
    session_manager_ = session_manager;
    persistence_manager_ = persistence_manager;

    initialized_ = true;
    cpu_number_limit_ = resource_manager_ptr->GetCpuResource();
    memory_size_limit_ = resource_manager_ptr->GetMemoryResource();

    parser_ = MakeUnique<SQLParser>();
    logical_planner_ = MakeUnique<LogicalPlanner>(this);
    optimizer_ = MakeUnique<Optimizer>(this);
    physical_planner_ = MakeUnique<PhysicalPlanner>(this);
    fragment_builder_ = MakeUnique<FragmentBuilder>(this);
}

QueryResult QueryContext::Query(const String &query) {
    CreateQueryProfiler();

    StartProfile(QueryPhase::kParser);
    UniquePtr<ParserResult> parsed_result = MakeUnique<ParserResult>();
    parser_->Parse(query, parsed_result.get());

    if (parsed_result->IsError()) {
        StopProfile(QueryPhase::kParser);
        QueryResult query_result;
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::InvalidCommand(parsed_result->error_message_);
        return query_result;
    }

    if (parsed_result->statements_ptr_->size() != 1) {
        String error_message = "Only support single statement.";
        UnrecoverableError(error_message);
    }
    StopProfile(QueryPhase::kParser);

    BaseStatement *base_statement = parsed_result->statements_ptr_->at(0);

    QueryResult query_result = QueryStatement(base_statement);

    return query_result;
}

QueryResult QueryContext::QueryStatement(const BaseStatement *base_statement) {
    QueryResult query_result;
    do {
        query_result = QueryStatementInternal(base_statement);
    } while (!query_result.status_.ok() && query_result.status_.code_ == ErrorCode::kTxnConflict);

    return query_result;
}

QueryResult QueryContext::QueryStatementInternal(const BaseStatement *base_statement) {
    QueryResult query_result;

    if (base_statement->Type() == StatementType::kAdmin) {
        if (InfinityContext::instance().IsAdminRole()) {
            const AdminStatement *admin_statement = static_cast<const AdminStatement *>(base_statement);
            return HandleAdminStatement(admin_statement);
        } else {
            const AdminStatement *admin_statement = static_cast<const AdminStatement *>(base_statement);
            if (admin_statement->admin_type_ == AdminStmtType::kShowNode) {
                return HandleAdminStatement(admin_statement);
            }

            if (!InfinityContext::instance().InfinityContextStarted()) {
                query_result.result_table_ = nullptr;
                query_result.status_ = Status::InfinityIsStarting();
                return query_result;
            }

            switch (admin_statement->admin_type_) {
                case AdminStmtType::kShowVariable: {
                    String var_name = admin_statement->variable_name_.value();
                    ToLower(var_name);
                    if (var_name == "server_role") {
                        return HandleAdminStatement(admin_statement);
                    }
                    break;
                }
                case AdminStmtType::kShowNode:
                case AdminStmtType::kShowCurrentNode:
                case AdminStmtType::kListNodes:
                case AdminStmtType::kRemoveNode:
                case AdminStmtType::kSetRole: {
                    return HandleAdminStatement(admin_statement);
                }
                default: {
                    break;
                }
            }

            query_result.result_table_ = nullptr;
            query_result.status_ = Status::AdminOnlySupportInMaintenanceMode();
            return query_result;
        }
    } else {
        if (!InfinityContext::instance().InfinityContextStarted()) {
            query_result.result_table_ = nullptr;
            query_result.status_ = Status::InfinityIsStarting();
            return query_result;
        }
    }

    Vector<SharedPtr<LogicalNode>> logical_plans{};
    Vector<UniquePtr<PhysicalOperator>> physical_plans{};
    SharedPtr<PlanFragment> plan_fragment{};
    UniquePtr<Notifier> notifier{};

    query_id_ = session_ptr_->query_count();
    //    ProfilerStart("Query");
    //    BaseProfiler profiler;
    //    profiler.Begin();
    try {

        if (global_config_->RecordRunningQuery()) {
            bool add_record_flag = false;
            if (base_statement->type_ == StatementType::kShow) {
                const ShowStatement *show_statement = static_cast<const ShowStatement *>(base_statement);
                ShowStmtType show_type = show_statement->show_type_;
                if (show_type != ShowStmtType::kQueries and show_type != ShowStmtType::kQuery) {
                    add_record_flag = true;
                }
            } else {
                add_record_flag = true;
            }

            if (add_record_flag) {
                LOG_DEBUG(fmt::format("Record running query: {}", base_statement->ToString()));
                session_manager_->AddQueryRecord(session_ptr_->session_id(),
                                                 query_id_,
                                                 StatementType2Str(base_statement->type_),
                                                 base_statement->ToString());
            }
        }

        this->BeginTxn(base_statement);
        //        LOG_INFO(fmt::format("created transaction, txn_id: {}, begin_ts: {}, base_statement: {}",
        //                        session_ptr_->GetTxn()->TxnID(),
        //                        session_ptr_->GetTxn()->BeginTS(),
        //                        base_statement->ToString()));
        RecordQueryProfiler(base_statement->type_);

        // Build unoptimized logical plan for each SQL base_statement.
        StartProfile(QueryPhase::kLogicalPlan);
        SharedPtr<BindContext> bind_context;
        auto status = logical_planner_->Build(base_statement, bind_context);
        // FIXME
        if (!status.ok()) {
            RecoverableError(status);
        }

        current_max_node_id_ = bind_context->GetNewLogicalNodeId();
        logical_plans = logical_planner_->LogicalPlans();
        StopProfile(QueryPhase::kLogicalPlan);
        //        LOG_WARN(fmt::format("Before optimizer cost: {}", profiler.ElapsedToString()));
        // Apply optimized rule to the logical plan
        StartProfile(QueryPhase::kOptimizer);
        for (auto &logical_plan : logical_plans) {
            optimizer_->optimize(logical_plan, base_statement->type_);
        }
        StopProfile(QueryPhase::kOptimizer);

        // Build physical plan
        StartProfile(QueryPhase::kPhysicalPlan);
        for (auto &logical_plan : logical_plans) {
            auto physical_plan = physical_planner_->BuildPhysicalOperator(logical_plan);
            physical_plans.push_back(std::move(physical_plan));
        }
        StopProfile(QueryPhase::kPhysicalPlan);
        //        LOG_WARN(fmt::format("Before pipeline cost: {}", profiler.ElapsedToString()));
        StartProfile(QueryPhase::kPipelineBuild);
        // Fragment Builder, only for test now.
        {
            Vector<PhysicalOperator *> physical_plan_ptrs;
            for (auto &physical_plan : physical_plans) {
                physical_plan_ptrs.push_back(physical_plan.get());
            }
            plan_fragment = fragment_builder_->BuildFragment(physical_plan_ptrs);
        }
        StopProfile(QueryPhase::kPipelineBuild);

        StartProfile(QueryPhase::kTaskBuild);
        notifier = MakeUnique<Notifier>();
        FragmentContext::BuildTask(this, nullptr, plan_fragment.get(), notifier.get());
        StopProfile(QueryPhase::kTaskBuild);
        //        LOG_WARN(fmt::format("Before execution cost: {}", profiler.ElapsedToString()));
        StartProfile(QueryPhase::kExecution);
        scheduler_->Schedule(plan_fragment.get(), base_statement);
        query_result.result_table_ = plan_fragment->GetResult();
        query_result.root_operator_type_ = logical_plans.back()->operator_type();
        StopProfile(QueryPhase::kExecution);
        //        LOG_WARN(fmt::format("Before commit cost: {}", profiler.ElapsedToString()));
        StartProfile(QueryPhase::kCommit);
        this->CommitTxn();
        StopProfile(QueryPhase::kCommit);

    } catch (RecoverableException &e) {

        // If txn has been rollbacked, do not rollback again here.
        NewTxn *new_txn = this->GetNewTxn();
        if (new_txn != nullptr) {
            StopProfile();
            StartProfile(QueryPhase::kRollback);
            TxnState txn_state = new_txn->GetTxnState();
            if (txn_state == TxnState::kRollbacking or txn_state == TxnState::kStarted) {
                this->RollbackTxn();
            }
            StopProfile(QueryPhase::kRollback);
        }
        query_result.result_table_ = nullptr;
        query_result.status_.Init(e.ErrorCode(), e.what());

    } catch (ParserException &e) {

        query_result.result_table_ = nullptr;
        query_result.status_.Init(ErrorCode::kParserError, e.what());

    } catch (UnrecoverableException &e) {
        printf("UnrecoverableException %s\n", e.what());
        LOG_CRITICAL(e.what());
        raise(SIGUSR1);
        //        throw e;
    }

    //    ProfilerStop();
    session_ptr_->IncreaseQueryCount();
    session_manager_->IncreaseQueryCount();

    if (global_config_->RecordRunningQuery()) {
        bool remove_record_flag = false;
        if (base_statement->type_ == StatementType::kShow) {
            const ShowStatement *show_statement = static_cast<const ShowStatement *>(base_statement);
            ShowStmtType show_type = show_statement->show_type_;
            if (show_type != ShowStmtType::kQueries and show_type != ShowStmtType::kQuery) {
                remove_record_flag = true;
            }
        } else {
            remove_record_flag = true;
        }

        if (remove_record_flag) {
            LOG_DEBUG(fmt::format("Remove the query string from running query container: {}", base_statement->ToString()));
            session_manager_->RemoveQueryRecord(session_ptr_->session_id());
        }
    }
    //    profiler.End();
    //    LOG_WARN(fmt::format("Query cost: {}", profiler.ElapsedToString()));
    return query_result;
}

void QueryContext::CreateQueryProfiler() {
    bool query_profiler_flag = false;
    NewCatalog *catalog = InfinityContext::instance().storage()->new_catalog();
    if (catalog == nullptr) {
        return;
    }
    query_profiler_flag = catalog->GetProfile();

    if (query_profiler_flag or explain_analyze_) {
        if (query_profiler_ == nullptr) {
            query_profiler_ = MakeShared<QueryProfiler>(query_profiler_flag);
        }
    }
}

void QueryContext::RecordQueryProfiler(const StatementType &type) {
    if (type != StatementType::kCommand && type != StatementType::kExplain && type != StatementType::kShow) {
        NewCatalog *catalog = InfinityContext::instance().storage()->new_catalog();
        catalog->AppendProfileRecord(query_profiler_);
    }
}

void QueryContext::StartProfile(QueryPhase phase) {
    if (query_profiler_) {
        query_profiler_->StartPhase(phase);
    }
}

void QueryContext::StopProfile(QueryPhase phase) {
    if (query_profiler_) {
        query_profiler_->StopPhase(phase);
    }
}

void QueryContext::StopProfile() {
    if (query_profiler_) {
        query_profiler_->Stop();
    }
}

bool QueryContext::ExecuteBGStatement(BaseStatement *base_statement, BGQueryState &state) {
    QueryResult query_result;
    try {
        SharedPtr<BindContext> bind_context;
        auto status = logical_planner_->Build(base_statement, bind_context);
        if (!status.ok()) {
            RecoverableError(status);
        }
        current_max_node_id_ = bind_context->GetNewLogicalNodeId();
        state.logical_plans = logical_planner_->LogicalPlans();

        for (auto &logical_plan : state.logical_plans) {
            auto physical_plan = physical_planner_->BuildPhysicalOperator(logical_plan);
            state.physical_plans.push_back(std::move(physical_plan));
        }

        {
            Vector<PhysicalOperator *> physical_plan_ptrs;
            for (auto &physical_plan : state.physical_plans) {
                physical_plan_ptrs.push_back(physical_plan.get());
            }
            state.plan_fragment = fragment_builder_->BuildFragment(physical_plan_ptrs);
        }

        state.notifier = MakeUnique<Notifier>();
        FragmentContext::BuildTask(this, nullptr, state.plan_fragment.get(), state.notifier.get());

        scheduler_->Schedule(state.plan_fragment.get(), base_statement);
    } catch (RecoverableException &e) {
        this->RollbackTxn();
        query_result.result_table_ = nullptr;
        query_result.status_.Init(e.ErrorCode(), e.what());
        return false;

    } catch (UnrecoverableException &e) {
        LOG_CRITICAL(e.what());
        raise(SIGUSR1);
    }
    return true;
}

bool QueryContext::JoinBGStatement(BGQueryState &state, TxnTimeStamp &commit_ts, bool rollback) {
    QueryResult query_result;
    if (rollback) {
        query_result.result_table_ = state.plan_fragment->GetResult();
        this->RollbackTxn();
        return false;
    }
    try {
        query_result.result_table_ = state.plan_fragment->GetResult();
        query_result.root_operator_type_ = state.logical_plans.back()->operator_type();
        commit_ts = this->CommitTxn();
    } catch (RecoverableException &e) {
        query_result.result_table_ = nullptr;
        query_result.status_.Init(e.ErrorCode(), e.what());
        this->RollbackTxn();
        return false;
    } catch (UnrecoverableException &e) {
        LOG_CRITICAL(e.what());
        raise(SIGUSR1);
    }
    return true;
}

QueryResult QueryContext::HandleAdminStatement(const AdminStatement *admin_statement) { return AdminExecutor::Execute(this, admin_statement); }

void QueryContext::BeginTxn(const BaseStatement *base_statement) {
    NewTxnManager *txn_manager = storage_->new_txn_manager();
    UniquePtr<String> txn_text = MakeUnique<String>(base_statement ? base_statement->ToString() : "");

    SharedPtr<NewTxn> new_txn{};
    if (base_statement->type_ == StatementType::kFlush) {
        new_txn = txn_manager->BeginTxnShared(MakeUnique<String>(base_statement->ToString()), TransactionType::kNewCheckpoint);
        if (new_txn == nullptr) {
            RecoverableError(Status::FailToStartTxn("System is checkpointing"));
        }
    } else {
        new_txn = txn_manager->BeginTxnShared(MakeUnique<String>(base_statement->ToString()), TransactionType::kNormal);
    }

    if (new_txn == nullptr) {
        UnrecoverableError("Cannot get new transaction.");
    }

    session_ptr_->SetNewTxn(new_txn);
}

TxnTimeStamp QueryContext::CommitTxn() {
    TxnTimeStamp commit_ts = 0;
    NewTxn *new_txn = session_ptr_->GetNewTxn();
    Status status = storage_->new_txn_manager()->CommitTxn(new_txn, &commit_ts);
    if (!status.ok()) {
        session_ptr_->ResetNewTxn();
        RecoverableError(status);
    }
    session_ptr_->IncreaseCommittedTxnCount();
    session_ptr_->ResetNewTxn();
    return commit_ts;
}

void QueryContext::RollbackTxn() {
    NewTxn *new_txn = session_ptr_->GetNewTxn();
    Status status = storage_->new_txn_manager()->RollBackTxn(new_txn);
    if (!status.ok()) {
        session_ptr_->ResetNewTxn();
        RecoverableError(status);
    }
    session_ptr_->IncreaseRollbackedTxnCount();
    session_ptr_->ResetNewTxn();
    return;
}

NewTxn *QueryContext::GetNewTxn() const { return session_ptr_->GetNewTxn(); }

} // namespace infinity
