//
// Created by JinHai on 2022/7/22.
//

#include "query_context.h"
#include "infinity.h"
#include "planner/planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "executor/physical_operator.h"
#include "common/utility/infinity_assert.h"
#include "scheduler/operator_pipeline.h"

#include "SQLParser.h"
#include "SQLParserResult.h"

namespace infinity {

class Pipeline;

QueryResult
QueryContext::Execute(const std::string &query) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
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

        return query_result;
//        ResponseError(optimized_plan->ToString(0));
    }

    NetworkError("Can't reach here.")
//    return QueryResult();
}

}
