//
// Created by jinhai on 23-3-8.
//

module;

import stl;
import data_table;
import logger;
import session;
import query_context;
import infinity_context;
import third_party;
import parser;
import logical_planner;
import optimizer;
import physical_planner;
import fragment_builder;
import bind_context;
import logical_node;
import physical_operator;
import fragment_scheduler;
import infinity_assert;
import infinity_exception;
import singleton;
import resource_manager;
import storage;

module sql_runner;

namespace infinity {

/**
 * @brief For testing the new push based execution engine
 * @param sql_text
 * @param print
 * @return Table
 */
SharedPtr<DataTable> SQLRunner::Run(const String &sql_text, bool print) {
//    if (print) {
//        LOG_TRACE(Format("{}", sql_text));
//    }

    SharedPtr<RemoteSession> session_ptr = MakeShared<RemoteSession>();
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_ptr.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> parsed_result = MakeShared<ParserResult>();
    parser->Parse(sql_text, parsed_result);

    if (parsed_result->IsError()) {
        Error<PlannerException>(parsed_result->error_message_, __FILE_NAME__, __LINE__);
    }

    query_context_ptr->CreateTxn();
    query_context_ptr->BeginTxn();

//    LogicalPlanner logical_planner(query_context_ptr.get());
//    Optimizer optimizer(query_context_ptr.get());
//    PhysicalPlanner physical_planner(query_context_ptr.get());
//    FragmentBuilder fragment_builder(query_context_ptr.get());
    BaseStatement *statement = (*parsed_result->statements_ptr_)[0];

    SharedPtr<BindContext> bind_context;
    query_context_ptr->logical_planner()->Build(statement, bind_context);
    query_context_ptr->set_max_node_id(bind_context->GetNewLogicalNodeId());

    SharedPtr<LogicalNode> unoptimized_plan = query_context_ptr->logical_planner()->LogicalPlan();

    // Apply optimized rule to the logical plan
    SharedPtr<LogicalNode> optimized_plan = query_context_ptr->optimizer()->optimize(unoptimized_plan);

    // Build physical plan
    SharedPtr<PhysicalOperator> physical_plan = query_context_ptr->physical_planner()->BuildPhysicalOperator(optimized_plan);

    // Create execution pipeline
    // Fragment Builder, only for test now. plan fragment is same as pipeline.
    auto plan_fragment = query_context_ptr->fragment_builder()->BuildFragment(physical_plan.get());

    // Schedule the query pipeline
    query_context_ptr->scheduler()->Schedule(query_context_ptr.get(), plan_fragment.get());

    // Initialize query result
    QueryResponse query_response;
    query_response.result_ = plan_fragment->GetResult();
    query_response.root_operator_type_ = unoptimized_plan->operator_type();

    parsed_result->Reset();
    query_context_ptr->CommitTxn();
    return query_response.result_;
}
} // namespace infinity