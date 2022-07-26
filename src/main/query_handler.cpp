//
// Created by JinHai on 2022/7/22.
//

#include "query_handler.h"
#include "planner/planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "executor/physical_operator.h"
#include "common/utility/asserter.h"

#include "SQLParser.h"
#include "SQLParserResult.h"

namespace infinity {

void
infinity::QueryHandler::ExecuteQuery(const std::string &query) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
    if(!parse_result.isValid()) {
        ResponseError(parse_result.errorMsg())
    }

    Planner logical_planner;
    Optimizer optimizer;
    PhysicalPlanner physical_planner;

    for (hsql::SQLStatement *statement : parse_result.getStatements()) {
        // Build unoptimized logical plan for each SQL statement.
        std::shared_ptr<LogicalOperator> unoptimized_plan = logical_planner.CreateLogicalOperator(*statement);

        // Apply optimized rule to the logical plan
        std::shared_ptr<LogicalOperator> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        std::shared_ptr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        ResponseError(optimized_plan->ToString(0));
    }
}

}
