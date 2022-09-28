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

#include <sstream>
#include <utility>

namespace infinity {

class Pipeline;

std::string
QueryResult::ToString() const {
    std::stringstream ss;

    switch (root_operator_type_) {
        case LogicalNodeType::kInsert: {
            return "INSERT 0 1";
        }
        case LogicalNodeType::kUpdate: {
            return "UPDATE 0 1";
            break;
        }
        case LogicalNodeType::kDelete: {
            return "DELETE 0 1";
        }
        default: {
            ;
        }
    }


    size_t column_count = result_->table_def()->column_count();
    for(size_t column_id = 0; column_id < column_count; ++ column_id) {
        ss << result_->table_def()->columns()[column_id].name() << " ";
    }
    ss << std::endl;

    // Get Block count
    uint64_t block_count = result_->block_count();

    // Iterate all blocks
    for(uint64_t block_id = 0; block_id < block_count; ++ block_id) {
        // Get current block
        std::shared_ptr<Block>& current_block = result_->blocks()[block_id];

        ss << current_block->AsStringRow();
    }

    return ss.str();
}

// Get get
QueryContext::QueryContext(const std::shared_ptr<Session>& session_ptr, std::unique_ptr<TransactionContext>& transaction)
    : transaction_(transaction) {}

QueryResult
QueryContext::Query(const std::string &query) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
    if(!parse_result.isValid()) {
        ParserError(parse_result.errorMsg())
    }

    std::shared_ptr<QueryContext> query_context = shared_from_this();

    Planner logical_planner(query_context);
    Optimizer optimizer(query_context);
    PhysicalPlanner physical_planner(query_context);

    PlannerAssert(parse_result.getStatements().size() == 1, "Not support more statements");
    for (hsql::SQLStatement *statement : parse_result.getStatements()) {
        // Build unoptimized logical plan for each SQL statement.
        std::shared_ptr<LogicalNode> unoptimized_plan = logical_planner.BuildLogicalPlan(*statement);

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
