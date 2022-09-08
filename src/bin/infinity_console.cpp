//
// Created by JinHai on 2022/9/7.
//

#include "compilation_config.h"
#include "common/utility/str.h"
#include "common/utility/infinity_assert.h"
#include "linenoise.h"

#include "main/profiler/show_logical_plan.h"

// Parser header
#include "SQLParser.h"
#include "SQLParserResult.h"
#include "util/sqlhelper.h"

// SQL compile
#include "parser/statement.h"
#include "planner/planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "scheduler/operator_pipeline.h"
#include "main/infinity.h"
#include "main/query_context.h"

#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace infinity {

class Console {
public:
    explicit Console();

    void HandleCommand(const char* command);

    [[nodiscard]] const std::string& history_file() const { return history_file_; }

private:
    void Execute(const std::string& command);
    void Register(const std::string& name, const std::function<void(const std::string&)>& func);

    // Command functions
    void Exit(const std::string& arguments);
    void Explain(const std::string& arguments);
    void Visualize(const std::string& arguments);
    void VerifyScript(const std::string& arguments);
    void RunScript(const std::string& arguments);

    void ExecuteSQL(const std::string& sql_text);

private:
    std::string history_file_{"command_history.log"};

    std::unordered_map<std::string, std::function<void(const std::string&)>> commands_;
};

Console::Console() {
    Register("exit", [this](auto && placeholder) { Exit(std::forward<decltype(placeholder)>(placeholder)); });
    Register("explain", [this](auto && placeholder) { Explain(std::forward<decltype(placeholder)>(placeholder)); });
    Register("visualize", [this](auto && placeholder) { Visualize(std::forward<decltype(placeholder)>(placeholder)); });
    Register("verify", [this](auto && placeholder) { VerifyScript(std::forward<decltype(placeholder)>(placeholder)); });
    Register("run", [this](auto && placeholder) { RunScript(std::forward<decltype(placeholder)>(placeholder)); });
}

void
Console::Register(const std::string& name, const std::function<void(const std::string&)>& func) { commands_[name] = func; }

void
Console::HandleCommand(const char* command) {

    std::string input(command);
    trim(input);

    while(!input.empty() && input.back() == ';') {
        input = input.substr(0, input.size() - 1);
    }

    // Empty input string, return directly.
    if (input.empty()) return;

    // Add input command into history
    linenoiseHistoryAdd(input.c_str());

    // Flush the persist the history file.
    linenoiseHistorySave(history_file_.c_str());

    Execute(input);
}

void
Console::Execute(const std::string& command) {
    if (command.empty()) return;

    std::unordered_map<std::string, std::function<void(const std::string&)>>::iterator iter;
    if ((iter = commands_.find(command.substr(0, command.find_first_of(' ')))) != std::end(commands_)) {
        std::string args = command.substr(command.find_first_of(' ') + 1, command.size());
        // To upper case
        std::transform(args.begin(),args.end(), args.begin(), toupper);
//        std::cout << "Args: << " << args << std::endl;
        iter->second(args);
        return ;
    }

    GeneralError("Invalid syntax: " + command);
}

void
Console::Exit(const std::string& command) {
    if(command == "EXIT") {
        std::cout << "Bye!" << std::endl;
        std::exit(0);
    }
    GeneralError("Invalid syntax: " + command);
}

void
Console::Explain(const std::string& arguments) {
    hsql::SQLParserResult parse_result;
    Planner logical_planner;
    Optimizer optimizer;
    PhysicalPlanner physical_planner;

    size_t option_pos = arguments.find_first_of(' ');
    std::string option = arguments.substr(0, option_pos);
    std::string query = arguments.substr(option_pos + 1, arguments.size());

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
    if(!parse_result.isValid()) {
        ParserError(parse_result.errorMsg())
    }

    PlannerAssert(parse_result.getStatements().size() == 1, "Not support more statements");

    if(option == "AST") {
        std::cout << "Explain AST: " << query << std::endl;
        hsql::printStatementInfo(parse_result.getStatements()[0]);
        return ;
    }

    // Build unoptimized logical plan for each SQL statement.
    std::shared_ptr<LogicalNode> unoptimized_plan = logical_planner.CreateLogicalOperator(*parse_result.getStatements()[0]);

    if(option == "LOGICAL") {
        std::cout << "Explain LOGICAL: " << query << std::endl;

        ShowLogicalPlan show_logical_plan(unoptimized_plan);
        std::cout << show_logical_plan.ToString() << std::endl;
        return ;
    }

    // Apply optimized rule to the logical plan
    std::shared_ptr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

    if(option == "OPT") {
        std::cout << "Explain OPTIMIZED LOGICAL: " << query << std::endl;

        ShowLogicalPlan show_logical_plan(optimized_plan);
        std::cout << show_logical_plan.ToString() << std::endl;
        return ;
    }

    // Build physical plan
    std::shared_ptr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

    if(option == "PHYSICAL") {
        std::cout << "Explain PHYSICAL: " << query << std::endl;
        return ;
    }

    // Create execution pipeline
    std::shared_ptr<Pipeline> pipeline = physical_plan->GenerateOperatorPipeline();

    if(option == "PIPELINE") {
        std::cout << "Explain PIPELINE: " << query << std::endl;
        return ;
    }

    GeneralError("Invalid Explain syntax: " + arguments);
}

void
Console::Visualize(const std::string& arguments) {
    hsql::SQLParserResult parse_result;
    Planner logical_planner;
    Optimizer optimizer;
    PhysicalPlanner physical_planner;

    size_t option_pos = arguments.find_first_of(' ');
    std::string option = arguments.substr(0, option_pos);
    std::string query = arguments.substr(option_pos + 1, arguments.size());

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
    if(!parse_result.isValid()) {
        ParserError(parse_result.errorMsg())
    }

    PlannerAssert(parse_result.getStatements().size() == 1, "Not support more statements");

    if(option == "AST") {
        std::cout << "Visualize AST: " << query << std::endl;
        return ;
    }

    // Build unoptimized logical plan for each SQL statement.
    std::shared_ptr<LogicalNode> unoptimized_plan = logical_planner.CreateLogicalOperator(*parse_result.getStatements()[0]);

    if(option == "LOGICAL") {
        std::cout << "Visualize LOGICAL: " << query << std::endl;
        return ;
    }

    // Apply optimized rule to the logical plan
    std::shared_ptr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

    if(option == "OPT") {
        std::cout << "Visualize OPTIMIZED LOGICAL: " << query << std::endl;
        return ;
    }

    // Build physical plan
    std::shared_ptr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

    if(option == "PHYSICAL") {
        std::cout << "Visualize PHYSICAL: " << query << std::endl;
        return ;
    }

    // Create execution pipeline
    std::shared_ptr<Pipeline> pipeline = physical_plan->GenerateOperatorPipeline();

    if(option == "PIPELINE") {
        std::cout << "Visualize PIPELINE: " << query << std::endl;
        return ;
    }

    GeneralError("Invalid VISUALIZE syntax: " + arguments);
}

void
Console::VerifyScript(const std::string& arguments) {
    GeneralError("Verify script not supported now.");
}

void
Console::RunScript(const std::string& arguments) {
    GeneralError("Run script not supported now.");
}

void
Console::ExecuteSQL(const std::string& sql_text) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(sql_text, &parse_result);
    if(!parse_result.isValid()) {
        ParserError(parse_result.errorMsg())
    }

    Planner logical_planner;
    Optimizer optimizer;
    PhysicalPlanner physical_planner;

    PlannerAssert(parse_result.getStatements().size() == 1, "Not support more statements");
    for (hsql::SQLStatement *statement : parse_result.getStatements()) {
        // Build unoptimized logical plan for each SQL statement.
        std::shared_ptr<LogicalNode> unoptimized_plan = logical_planner.CreateLogicalOperator(*statement);

        // Apply optimized rule to the logical plan
        std::shared_ptr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        std::shared_ptr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        // Create execution pipeline
        std::shared_ptr<Pipeline> pipeline = physical_plan->GenerateOperatorPipeline();

        // Schedule the query pipeline
        Infinity::instance().scheduler()->Schedule(pipeline);

        QueryResult query_result;
        query_result.result_ = pipeline->GetResult();
        query_result.root_operator_type_ = unoptimized_plan->operator_type();

        std::cout << query_result.ToString() << std::endl;
    }
}

}

int main(int argc, char** argv) {
    infinity::Console console;

    std::cout << "Startup Infinity database console, version: "
              << VERSION_MAJOR << "."
              << VERSION_MINOR << "."
              << VERSION_PATCH << " built on "
              << BUILD_TIME << " from branch: "
              << GIT_BRANCH_NAME << std::endl;

    // Use multiple line mode
    linenoiseSetMultiLine(1);

    // Store the command history into the file
    linenoiseHistoryLoad("command_history.log");

    char *line = nullptr;
    while((line = linenoise("infinity> ")) != nullptr) {
//        std::cout << line << std::endl;
        try {
            console.HandleCommand(line);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        free(line);
    }
}