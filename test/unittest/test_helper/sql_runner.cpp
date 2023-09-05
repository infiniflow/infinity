//
// Created by jinhai on 23-3-8.
//

#include "sql_runner.h"

namespace infinity {

String
SQLRunner::Run(const String& sql_text, bool print) {
    if(print) {
        LOG_TRACE("{}", sql_text);
    }

    SharedPtr<Session> session_ptr = MakeShared<Session>();
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_ptr.get(),
                                                                         Infinity::instance().config(),
                                                                         Infinity::instance().scheduler(),
                                                                         Infinity::instance().storage());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();
    parser->Parse(sql_text, parsed_result);

    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    query_context_ptr->CreateTxn();
    query_context_ptr->BeginTxn();

    SizeT statement_count = parsed_result->statements_ptr_->size();
    std::cout << "Statement count: " << parsed_result->statements_ptr_->size() << std::endl;

    for(SizeT idx = 0; idx < statement_count; ++ idx) {
        LogicalPlanner logical_planner(query_context_ptr.get());
        Optimizer optimizer(query_context_ptr.get());
        PhysicalPlanner physical_planner(query_context_ptr.get());

        BaseStatement* statement = (*parsed_result->statements_ptr_)[idx];

        logical_planner.Build(statement);

        SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

        if(print) {
            // Print unoptimized plan
            ShowLogicalPlan show_logical_plan(unoptimized_plan);
            LOG_TRACE("unoptimized plan: \n{}", show_logical_plan.ToString());
        }

        // Apply optimized rule to the logical plan
        SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        // Create execution pipeline
        SharedPtr<Pipeline> pipeline = OperatorPipeline::Create(physical_plan);

        // Schedule the query pipeline
        Infinity::instance().scheduler()->Schedule(query_context_ptr.get(), pipeline);

        // Initialize query result
        QueryResult query_result;
        query_result.result_ = pipeline->GetResult();
        query_result.root_operator_type_ = unoptimized_plan->operator_type();

//        if(query_result.result_ == nullptr) {
//            LOG_TRACE("No result!");
//        } else{
//            LOG_TRACE("{}", query_result.ToString());
//        }
    }
    LOG_TRACE("{} statements executed.", statement_count);
    parsed_result->Reset();

    query_context_ptr->CommitTxn();
    return String();
}

}