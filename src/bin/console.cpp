//
// Created by JinHai on 2022/10/1.
//

#include "parser/sql_parser.h"
#include "console.h"
#include "common/utility/str.h"
#include "common/utility/infinity_assert.h"

#include "main/profiler/show_logical_plan.h"

#include "main/session.h"
#include "main/stats/global_resource_usage.h"

// SQL compile
#include "planner/logical_planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "legacy_sched/operator_pipeline.h"
#include "main/infinity.h"
#include "main/query_context.h"

#include <iostream>
#include <algorithm>
#include <unordered_map>


namespace infinity {

Console::Console() {
    Register("QUIT", [this](auto && placeholder) { Exit(std::forward<decltype(placeholder)>(placeholder)); });
    Register("EXIT", [this](auto && placeholder) { Exit(std::forward<decltype(placeholder)>(placeholder)); });
    Register("EXPLAIN", [this](auto && placeholder) { Explain(std::forward<decltype(placeholder)>(placeholder)); });
    Register("VISUALIZE", [this](auto && placeholder) { Visualize(std::forward<decltype(placeholder)>(placeholder)); });
    Register("VERFIY", [this](auto && placeholder) { VerifyScript(std::forward<decltype(placeholder)>(placeholder)); });
    Register("RUN", [this](auto && placeholder) { RunScript(std::forward<decltype(placeholder)>(placeholder)); });
}

void
Console::Init() {
    GlobalResourceUsage::Init();
    SharedPtr<String> config_path{};

    Infinity::instance().Init(nullptr);
}

void
Console::UnInit() {
//    infinity::Infinity::instance().UnInit();
    GlobalResourceUsage::UnInit();
}

void
Console::Register(const String& name, const std::function<void(const String&)>& func) { commands_[name] = func; }

String
Console::HandleCommand(const char* command) {

    String input(command);
    trim(input);

    while(!input.empty() && input.back() == ';') {
        input = input.substr(0, input.size() - 1);
    }

    // Not handle empty input string
    if (!input.empty()) {
        Execute(input);
    }

    return input;
}

void
Console::Execute(const String& statement) {
    if (statement.empty()) return;

    HashMap<String, std::function<void(const String&)>>::iterator iter;

    String command = statement.substr(0, statement.find_first_of(' '));

    // Transfer command to upper case
    std::transform(command.begin(),command.end(), command.begin(), toupper);
    if ((iter = commands_.find(command)) != std::end(commands_)) {
        String args = statement.substr(statement.find_first_of(' ') + 1, statement.size());
        iter->second(args);
        return ;
    }

    ExecuteSQL(statement);
}

void
Console::Exit(const String& command) {
    if(command == "EXIT" || command == "QUIT") {
        std::cout << "Bye!" << std::endl;
        std::exit(0);
    }
    GeneralError("Invalid syntax: " + command);
}

void
Console::Explain(const String& arguments) {
    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();

    UniquePtr<Session> session_ptr = MakeUnique<Session>();
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>();
    query_context_ptr->Init(session_ptr.get(),
                            Infinity::instance().config(),
                            Infinity::instance().fragment_scheduler(),
                            Infinity::instance().storage(),
                            Infinity::instance().resource_manager());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    LogicalPlanner logical_planner(query_context_ptr.get());
    Optimizer optimizer(query_context_ptr.get());
    PhysicalPlanner physical_planner(query_context_ptr.get());

    size_t parameter_pos = arguments.find_first_of(' ');
    String parameter = arguments.substr(0, parameter_pos);
    // Transfer second parameter to upper case
    std::transform(parameter.begin(),parameter.end(), parameter.begin(), toupper);

    String query = arguments.substr(parameter_pos + 1, arguments.size());

    // Parse sql
    parser->Parse(query, parsed_result);
    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Not support more statements");

    BaseStatement* statement = (*parsed_result->statements_ptr_)[0];

    if(parameter == "AST") {
        std::cout << "Explain AST: " << query << std::endl;
        NotImplementError("Not implement explain AST")
        return ;
    }

    // Build unoptimized logical plan for each SQL statement.
    logical_planner.Build(statement);
    SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

    if(parameter == "LOGICAL") {
        std::cout << "Explain LOGICAL: " << query << std::endl;

        ShowLogicalPlan show_logical_plan(unoptimized_plan);
        std::cout << show_logical_plan.ToString();
        return ;
    }

    // Apply optimized rule to the logical plan
    SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

    if(parameter == "OPT") {
        std::cout << "Explain OPTIMIZED LOGICAL: " << query << std::endl;

        ShowLogicalPlan show_logical_plan(optimized_plan);
        std::cout << show_logical_plan.ToString();
        return ;
    }

    // Build physical plan
    SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

    if(parameter == "PHYSICAL") {
        std::cout << "Explain PHYSICAL: " << query << std::endl;
        return ;
    }

    // Create execution pipeline
    SharedPtr<Pipeline> pipeline = physical_plan->GenerateOperatorPipeline();

    if(parameter == "PIPELINE") {
        std::cout << "Explain PIPELINE: " << query << std::endl;
        return ;
    }

    GeneralError("Invalid Explain syntax: " + arguments);
}

void
Console::Visualize(const String& arguments) {
    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();

    UniquePtr<Session> session_ptr = MakeUnique<Session>();
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>();
    query_context_ptr->Init(session_ptr.get(),
                            Infinity::instance().config(),
                            Infinity::instance().fragment_scheduler(),
                            Infinity::instance().storage(),
                            Infinity::instance().resource_manager());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    LogicalPlanner logical_planner(query_context_ptr.get());

    Optimizer optimizer(query_context_ptr.get());
    PhysicalPlanner physical_planner(query_context_ptr.get());

    size_t option_pos = arguments.find_first_of(' ');
    String option = arguments.substr(0, option_pos);
    String query = arguments.substr(option_pos + 1, arguments.size());

    // Parse sql
    parser->Parse(query, parsed_result);
    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Not support more statements");

    if(option == "AST") {
        std::cout << "Visualize AST: " << query << std::endl;
        return ;
    }

    // Build unoptimized logical plan for each SQL statement.
    logical_planner.Build((*parsed_result->statements_ptr_)[0]);
    SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

    if(option == "LOGICAL") {
        std::cout << "Visualize LOGICAL: " << query << std::endl;
        return ;
    }

    // Apply optimized rule to the logical plan
    SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

    if(option == "OPT") {
        std::cout << "Visualize OPTIMIZED LOGICAL: " << query << std::endl;
        return ;
    }

    // Build physical plan
    SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

    if(option == "PHYSICAL") {
        std::cout << "Visualize PHYSICAL: " << query << std::endl;
        return ;
    }

    // Create execution pipeline
    SharedPtr<Pipeline> pipeline = physical_plan->GenerateOperatorPipeline();

    if(option == "PIPELINE") {
        std::cout << "Visualize PIPELINE: " << query << std::endl;
        return ;
    }

    GeneralError("Invalid VISUALIZE syntax: " + arguments);
}

void
Console::VerifyScript(const String& arguments) {
    GeneralError("Verify script not supported now.");
}

void
Console::RunScript(const String& arguments) {
    GeneralError("Run script not supported now.");
}

void
Console::ExecuteSQL(const String& sql_text) {
    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();

    // Parse sql
    parser->Parse(sql_text, parsed_result);
    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Not support more statements");

    UniquePtr<Session> session_ptr = MakeUnique<Session>();
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>();
    query_context_ptr->Init(session_ptr.get(),
                            Infinity::instance().config(),
                            Infinity::instance().fragment_scheduler(),
                            Infinity::instance().storage(),
                            Infinity::instance().resource_manager());
    query_context_ptr->set_current_schema(session_ptr->current_database());

    LogicalPlanner logical_planner(query_context_ptr.get());

    Optimizer optimizer(query_context_ptr.get());
    PhysicalPlanner physical_planner(query_context_ptr.get());

    for (const BaseStatement *statement : *parsed_result->statements_ptr_) {
        // Build unoptimized logical plan for each SQL statement.
        logical_planner.Build(statement);
        SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

        // Apply optimized rule to the logical plan
        SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        // Create execution pipeline
        SharedPtr<Pipeline> pipeline = OperatorPipeline::Create(physical_plan);

        // Schedule the query pipeline
//        query_context_ptr->scheduler()->Schedule(query_context_ptr.get(), pipeline);

        // Initialize query result
        QueryResult query_result;
        query_result.result_ = pipeline->GetResult();
        query_result.root_operator_type_ = unoptimized_plan->operator_type();

        if(query_result.result_ == nullptr) {
            std::cout << "No result." << std::endl;
        } else{
            std::cout << query_result.ToString() << std::endl;
        }
    }
}

}