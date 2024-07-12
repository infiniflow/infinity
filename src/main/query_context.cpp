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
//#include "gperftools/profiler.h"

module query_context;


import stl;
import session;
import config;
import task_scheduler;
import storage;
import resource_manager;
import txn;
import sql_parser;
import profiler;
import infinity_exception;
import logical_planner;
import logical_node_type;
import data_block;
import optimizer;
import physical_planner;
import fragment_builder;
import fragment_task;
import fragment_context;
import bind_context;
import logical_node;
import physical_operator;
import third_party;
import logger;
import query_result;
import status;
import session_manager;
import base_statement;
import parser_result;
import parser_assert;
import plan_fragment;
import bg_query_state;
import show_statement;

namespace infinity {

QueryContext::QueryContext(BaseSession *session) : session_ptr_(session){};

QueryContext::~QueryContext() { UnInit(); }

void QueryContext::Init(Config *global_config_ptr,
                        TaskScheduler *scheduler_ptr,
                        Storage *storage_ptr,
                        ResourceManager *resource_manager_ptr,
                        SessionManager* session_manager) {
    global_config_ = global_config_ptr;
    scheduler_ = scheduler_ptr;
    storage_ = storage_ptr;
    resource_manager_ = resource_manager_ptr;
    session_manager_ = session_manager;

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
        Status status = Status::InvalidCommand(parsed_result->error_message_);
        RecoverableError(status);
    }

    if (parsed_result->statements_ptr_->size() != 1) {
        String error_message = "Only support single statement.";
        UnrecoverableError(error_message);
    }
    StopProfile(QueryPhase::kParser);
    for (BaseStatement *statement : *parsed_result->statements_ptr_) {
        QueryResult query_result = QueryStatement(statement);
        return query_result;
    }

    String error_message = "Not reachable";
    UnrecoverableError(error_message);
    return QueryResult::UnusedResult();
}

QueryResult QueryContext::QueryStatement(const BaseStatement *statement) {
    QueryResult query_result;
    Vector<SharedPtr<LogicalNode>> logical_plans{};
    Vector<UniquePtr<PhysicalOperator>> physical_plans{};
    SharedPtr<PlanFragment> plan_fragment{};
    UniquePtr<Notifier> notifier{};

    query_id_ = session_ptr_->query_count();
//    ProfilerStart("Query");
//    BaseProfiler profiler;
//    profiler.Begin();
    try {

        if(global_config_->RecordRunningQuery()) {
            bool add_record_flag = false;
            if(statement->type_ == StatementType::kShow) {
                const ShowStatement* show_statement = static_cast<const ShowStatement*>(statement);
                ShowStmtType show_type = show_statement->show_type_;
                if(show_type != ShowStmtType::kQueries and show_type != ShowStmtType::kQuery) {
                    add_record_flag = true;
                }
            } else {
                add_record_flag = true;
            }

            if(add_record_flag) {
                LOG_DEBUG(fmt::format("Record running query: {}", statement->ToString()));
                session_manager_->AddQueryRecord(session_ptr_->session_id(),
                                                 query_id_,
                                                 StatementType2Str(statement->type_),
                                                 statement->ToString());
            }
        }

        this->BeginTxn();
//        LOG_INFO(fmt::format("created transaction, txn_id: {}, begin_ts: {}, statement: {}",
//                        session_ptr_->GetTxn()->TxnID(),
//                        session_ptr_->GetTxn()->BeginTS(),
//                        statement->ToString()));
        RecordQueryProfiler(statement->type_);

        // Build unoptimized logical plan for each SQL statement.
        StartProfile(QueryPhase::kLogicalPlan);
        SharedPtr<BindContext> bind_context;
        auto status = logical_planner_->Build(statement, bind_context);
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
            optimizer_->optimize(logical_plan, statement->type_);
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
        scheduler_->Schedule(plan_fragment.get(), statement);
        query_result.result_table_ = plan_fragment->GetResult();
        query_result.root_operator_type_ = logical_plans.back()->operator_type();
        StopProfile(QueryPhase::kExecution);
//        LOG_WARN(fmt::format("Before commit cost: {}", profiler.ElapsedToString()));
        StartProfile(QueryPhase::kCommit);
        this->CommitTxn();
        StopProfile(QueryPhase::kCommit);

    } catch (RecoverableException &e) {

        StopProfile();
        StartProfile(QueryPhase::kRollback);
        this->RollbackTxn();
        StopProfile(QueryPhase::kRollback);
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

    if(global_config_->RecordRunningQuery()) {
        bool remove_record_flag = false;
        if(statement->type_ == StatementType::kShow) {
            const ShowStatement* show_statement = static_cast<const ShowStatement*>(statement);
            ShowStmtType show_type = show_statement->show_type_;
            if(show_type != ShowStmtType::kQueries and show_type != ShowStmtType::kQuery) {
                remove_record_flag = true;
            }
        } else {
            remove_record_flag = true;
        }

        if(remove_record_flag) {
            LOG_DEBUG(fmt::format("Remove the query string from running query container: {}", statement->ToString()));
            session_manager_->RemoveQueryRecord(session_ptr_->session_id());
        }
    }
//    profiler.End();
//    LOG_WARN(fmt::format("Query cost: {}", profiler.ElapsedToString()));
    return query_result;
}

bool QueryContext::ExecuteBGStatement(BaseStatement *statement, BGQueryState &state) {
    QueryResult query_result;
    try {
        SharedPtr<BindContext> bind_context;
        auto status = logical_planner_->Build(statement, bind_context);
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

        scheduler_->Schedule(state.plan_fragment.get(), statement);
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

void QueryContext::BeginTxn() {
    if (session_ptr_->GetTxn() == nullptr) {
        Txn* new_txn = storage_->txn_manager()->BeginTxn(nullptr);
        session_ptr_->SetTxn(new_txn);
    }
}

TxnTimeStamp QueryContext::CommitTxn() {
    Txn* txn = session_ptr_->GetTxn();
    TxnTimeStamp commit_ts = storage_->txn_manager()->CommitTxn(txn);
    session_ptr_->SetTxn(nullptr);
    session_ptr_->IncreaseCommittedTxnCount();
    storage_->txn_manager()->IncreaseCommittedTxnCount();
    return commit_ts;
}

void QueryContext::RollbackTxn() {
    Txn* txn = session_ptr_->GetTxn();
    storage_->txn_manager()->RollBackTxn(txn);
    session_ptr_->SetTxn(nullptr);
    session_ptr_->IncreaseRollbackedTxnCount();
    storage_->txn_manager()->IncreaseRollbackedTxnCount();
}

} // namespace infinity
