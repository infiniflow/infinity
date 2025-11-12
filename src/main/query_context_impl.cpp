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

module infinity_core:query_context.impl;

import :query_context;
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
import :new_txn_manager;
import :admin_executor;
import :utility;

import std.compat;

import base_statement;
import parser_result;
import sql_parser;
import admin_statement;
import show_statement;
import parser_assert;
import global_resource_usage;

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

    parser_ = std::make_unique<SQLParser>();
    logical_planner_ = std::make_unique<LogicalPlanner>(this);
    optimizer_ = std::make_unique<Optimizer>(this);
    physical_planner_ = std::make_unique<PhysicalPlanner>(this);
    fragment_builder_ = std::make_unique<FragmentBuilder>(this);
}

QueryResult QueryContext::Query(const std::string &query) {
    CreateQueryProfiler();

    StartProfile(QueryPhase::kParser);
    std::unique_ptr<ParserResult> parsed_result = std::make_unique<ParserResult>();
    parser_->Parse(query, parsed_result.get());

    if (parsed_result->IsError()) {
        StopProfile(QueryPhase::kParser);
        QueryResult query_result;
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::InvalidCommand(parsed_result->error_message_);
        return query_result;
    }

    if (parsed_result->statements_ptr_->size() != 1) {
        UnrecoverableError("Only support single statement.");
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
                    std::string var_name = admin_statement->variable_name_.value();
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

    std::vector<std::shared_ptr<LogicalNode>> logical_plans{};
    std::vector<std::unique_ptr<PhysicalOperator>> physical_plans{};
    std::shared_ptr<PlanFragment> plan_fragment{};
    std::unique_ptr<Notifier> notifier{};

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
        std::shared_ptr<BindContext> bind_context;
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
            std::vector<PhysicalOperator *> physical_plan_ptrs;
            for (auto &physical_plan : physical_plans) {
                physical_plan_ptrs.push_back(physical_plan.get());
            }
            plan_fragment = fragment_builder_->BuildFragment(physical_plan_ptrs);
        }
        StopProfile(QueryPhase::kPipelineBuild);

        StartProfile(QueryPhase::kTaskBuild);
        notifier = std::make_unique<Notifier>();
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
        CommitTxn();
        StopProfile(QueryPhase::kCommit);

    } catch (RecoverableException &e) {

        // If txn has been rollbacked, do not rollback again here.
        NewTxn *new_txn = GetNewTxn();
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
            query_profiler_ = std::make_shared<QueryProfiler>(query_profiler_flag);
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

QueryResult QueryContext::HandleAdminStatement(const AdminStatement *admin_statement) { return AdminExecutor::Execute(this, admin_statement); }

void QueryContext::BeginTxn(const BaseStatement *base_statement) {
    auto *txn_manager = storage_->new_txn_manager();
    auto txn_text = std::make_unique<std::string>(base_statement ? base_statement->ToString() : "");

    std::shared_ptr<NewTxn> new_txn{};
    TransactionType transaction_type = TransactionType::kInvalid;
    switch (base_statement->type_) {
        case StatementType::kFlush: {
            transaction_type = TransactionType::kNewCheckpoint;
            break;
        }
        case StatementType::kCopy: {
            const CopyStatement *copy_statement = static_cast<const CopyStatement *>(base_statement);
            if (copy_statement->copy_from_) {
                // Import
                transaction_type = TransactionType::kImport;
            } else {
                // Export
                transaction_type = TransactionType::kRead;
            }
            break;
        }
        case StatementType::kInsert: {
            transaction_type = TransactionType::kAppend;
            break;
        }
        case StatementType::kUpdate: {
            transaction_type = TransactionType::kUpdate;
            break;
        }
        case StatementType::kDelete: {
            transaction_type = TransactionType::kDelete;
            break;
        }
        case StatementType::kCheck:
        case StatementType::kExplain:
        case StatementType::kShow:
        case StatementType::kSelect: {
            transaction_type = TransactionType::kRead;
            break;
        }
        case StatementType::kAlter: {
            const AlterStatement *alter_statement = static_cast<const AlterStatement *>(base_statement);
            switch (alter_statement->type_) {
                case AlterStatementType::kAddColumns: {
                    transaction_type = TransactionType::kAddColumn;
                    break;
                }
                case AlterStatementType::kDropColumns: {
                    transaction_type = TransactionType::kDropColumn;
                    break;
                }
                case AlterStatementType::kRenameTable: {
                    transaction_type = TransactionType::kRenameTable;
                    break;
                }
                case AlterStatementType::kAlterIndex: {
                    transaction_type = TransactionType::kAlterIndex;
                    break;
                }
                case AlterStatementType::kInvalid: {
                    std::string error_msg = "Invalid alter statement type";
                    LOG_CRITICAL(error_msg);
                    UnrecoverableError(error_msg);
                }
            }
            break;
        }
        case StatementType::kCreate: {
            const CreateStatement *create_statement = static_cast<const CreateStatement *>(base_statement);
            switch (create_statement->ddl_type()) {
                case DDLType::kCollection:
                case DDLType::kTable: {
                    transaction_type = TransactionType::kCreateTable;
                    break;
                }
                case DDLType::kDatabase: {
                    transaction_type = TransactionType::kCreateDB;
                    break;
                }
                case DDLType::kIndex: {
                    transaction_type = TransactionType::kCreateIndex;
                    break;
                }
                default: {
                    UnrecoverableError("Unknown DDL type");
                }
            }
            break;
        }
        case StatementType::kDrop: {
            const DropStatement *drop_statement = static_cast<const DropStatement *>(base_statement);
            switch (drop_statement->ddl_type()) {
                case DDLType::kCollection:
                case DDLType::kTable: {
                    transaction_type = TransactionType::kDropTable;
                    break;
                }
                case DDLType::kDatabase: {
                    transaction_type = TransactionType::kDropDB;
                    break;
                }
                case DDLType::kIndex: {
                    transaction_type = TransactionType::kDropIndex;
                    break;
                }
                default: {
                    UnrecoverableError("Unknown DDL type");
                }
            }
            break;
        }
        case StatementType::kCompact: {
            transaction_type = TransactionType::kCompact;
            break;
        }
        case StatementType::kCommand: {
            const CommandStatement *command_statement = static_cast<const CommandStatement *>(base_statement);
            switch (command_statement->command_info_->type()) {
                case CommandType::kCleanup: {
                    transaction_type = TransactionType::kCleanup;
                    break;
                }
                case CommandType::kExport:
                case CommandType::kCheckTable: {
                    transaction_type = TransactionType::kRead;
                    break;
                }
                case CommandType::kDumpIndex: {
                    transaction_type = TransactionType::kDumpMemIndex;
                    break;
                }
                case CommandType::kSet: {
                    transaction_type = TransactionType::kSetCommand;
                    break;
                }
                case CommandType::kSnapshot: {
                    SnapshotCmd *snapshot_cmd = static_cast<SnapshotCmd *>(command_statement->command_info_.get());
                    SnapshotScope snapshot_scope = snapshot_cmd->scope();
                    switch (snapshot_cmd->operation()) {
                        case SnapshotOp::kDrop:
                        case SnapshotOp::kCreate: {
                            transaction_type = TransactionType::kCreateTableSnapshot;
                            break;
                        }
                        case SnapshotOp::kRestore: {
                            switch (snapshot_scope) {
                                case SnapshotScope::kDatabase: {
                                    transaction_type = TransactionType::kRestoreDatabase;
                                    break;
                                }
                                case SnapshotScope::kTable: {
                                    transaction_type = TransactionType::kRestoreTable;
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Unknown snapshot scope");
                                }
                            }
                            break;
                        }
                        default: {
                            UnrecoverableError("Unknown snapshot operation");
                        }
                    }
                    break;
                }
                case CommandType::kUse: {
                    transaction_type = TransactionType::kSetCommand;
                    break;
                }
                case CommandType::kTestCommand: {
                    transaction_type = TransactionType::kRead;
                    break;
                }
                case CommandType::kInvalid: {
                    UnrecoverableError("Invalid command type");
                }
            }
            break;
        }
        case StatementType::kAdmin: {
            UnrecoverableError("Not support admin type");
            break;
        }
        case StatementType::kOptimize: {
            transaction_type = TransactionType::kOptimizeIndex;
            break;
        }
        case StatementType::kInvalidStmt:
        case StatementType::kPrepare:
        case StatementType::kExecute: {
            UnrecoverableError("Unsupported statement type");
        }
    }

    if (transaction_type == TransactionType::kNewCheckpoint) {
        new_txn = txn_manager->BeginTxnShared(std::make_unique<std::string>(base_statement->ToString()), transaction_type);
        if (new_txn == nullptr) {
            RecoverableError(Status::FailToStartTxn("System is checkpointing"));
        }
    } else {
        new_txn = txn_manager->BeginTxnShared(std::make_unique<std::string>(base_statement->ToString()), transaction_type);
    }

    if (new_txn == nullptr) {
        UnrecoverableError("Cannot get new transaction.");
    }

    session_ptr_->SetNewTxn(new_txn);
}

TxnTimeStamp QueryContext::CommitTxn() {
    TxnTimeStamp commit_ts = 0;
    auto *new_txn = session_ptr_->GetNewTxn();
    if (auto status = storage_->new_txn_manager()->CommitTxn(new_txn, &commit_ts); !status.ok()) {
        session_ptr_->ResetNewTxn();
        RecoverableError(status);
    }
    session_ptr_->IncreaseCommittedTxnCount();
    session_ptr_->ResetNewTxn();
    return commit_ts;
}

void QueryContext::RollbackTxn() {
    auto *new_txn = session_ptr_->GetNewTxn();
    if (auto status = storage_->new_txn_manager()->RollBackTxn(new_txn); !status.ok()) {
        session_ptr_->ResetNewTxn();
        RecoverableError(status);
    }
    session_ptr_->IncreaseRollbackedTxnCount();
    session_ptr_->ResetNewTxn();
}

NewTxn *QueryContext::GetNewTxn() const { return session_ptr_->GetNewTxn(); }

} // namespace infinity
