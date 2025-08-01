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

#include <memory>

#ifdef CI
module sql_runner;

import infinity_core;
#else
module infinity_core:ut.sql_runner.impl;

import :ut.sql_runner;
import :stl;
import :fragment_builder;
import :data_table;
import :infinity_context;
import :logical_node;
import :bind_context;
import :session_manager;
import :query_result;
import :fragment_context;
import :logical_planner;
import :query_context;
import :physical_planner;
#endif

import sql_parser;
import base_statement;
import parser_result;

namespace infinity {

/**
 * @brief For testing the new push based execution engine
 * @param sql_text
 * @param print
 * @return Table
 */
SharedPtr<DataTable> SQLRunner::Run(const String &sql_text, bool print) {
    //    if (print) {
    //        LOG_TRACE(fmt::format("{}", sql_text));
    //    }

    //    UniquePtr<SessionManager> session_manager = MakeUnique<SessionManager>();
    SharedPtr<RemoteSession> session_ptr = InfinityContext::instance().session_manager()->CreateRemoteSession();

    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_ptr.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager(),
                            InfinityContext::instance().persistence_manager());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> parsed_result = MakeShared<ParserResult>();
    parser->Parse(sql_text, parsed_result.get());

    if (parsed_result->IsError()) {
        UnrecoverableError(parsed_result->error_message_);
    }

    //    LogicalPlanner logical_planner(query_context_ptr.get());
    //    Optimizer optimizer(query_context_ptr.get());
    //    PhysicalPlanner physical_planner(query_context_ptr.get());
    //    FragmentBuilder fragment_builder(query_context_ptr.get());
    BaseStatement *statement = (*parsed_result->statements_ptr_)[0];

    query_context_ptr->BeginTxn(statement);

    SharedPtr<BindContext> bind_context;
    query_context_ptr->logical_planner()->Build(statement, bind_context);
    query_context_ptr->set_max_node_id(bind_context->GetNewLogicalNodeId());

    SharedPtr<LogicalNode> logical_plan = query_context_ptr->logical_planner()->LogicalPlans()[0];

    // Apply optimized rule to the logical plan
    query_context_ptr->optimizer()->optimize(logical_plan, statement->Type());

    // Build physical plan
    SharedPtr<PhysicalOperator> physical_plan = query_context_ptr->physical_planner()->BuildPhysicalOperator(logical_plan);

    // Create execution pipeline
    // Fragment Builder, only for test now. plan fragment is same as pipeline.
    auto plan_fragment = query_context_ptr->fragment_builder()->BuildFragment({physical_plan.get()});

    auto notifier = MakeUnique<Notifier>();

    FragmentContext::BuildTask(query_context_ptr.get(), nullptr, plan_fragment.get(), notifier.get());

    // Schedule the query tasks
    query_context_ptr->scheduler()->Schedule(plan_fragment.get(), statement);

    // Initialize query result
    QueryResult query_result;
    query_result.result_table_ = plan_fragment->GetResult();
    query_result.root_operator_type_ = logical_plan->operator_type();

    parsed_result->Reset();
    query_context_ptr->CommitTxn();
    return query_result.result_table_;
}

} // namespace infinity
