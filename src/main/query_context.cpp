//
// Created by JinHai on 2022/7/22.
//

#include "query_context.h"
#include "infinity.h"
#include "planner/logical_planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "executor/physical_operator.h"
#include "common/utility/infinity_assert.h"
#include "scheduler/operator_pipeline.h"

#include "parser/parser_result.h"
#include "parser/sql_parser.h"

#include <sstream>
#include <utility>

namespace infinity {

class Pipeline;

String
QueryResult::ToString() const {
    std::stringstream ss;

    switch (root_operator_type_) {
        case LogicalNodeType::kInsert: {
            return "INSERT 0 1";
        }
        case LogicalNodeType::kUpdate: {
            return "UPDATE 0 1";
        }
        case LogicalNodeType::kDelete: {
            return "DELETE 0 1";
        }
        default: {
            ss << std::endl;
        }
    }

    SizeT column_count = result_->ColumnCount();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        String end;
        if(idx != column_count - 1) {
            end = " ";
        }
        ss << result_->GetColumnNameById(idx) << end;
    }
    ss << std::endl;

    // Get Block count
    SizeT block_count = result_->DataBlockCount();

    // Iterate all blocks
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        // Get current block
        SharedPtr<DataBlock> current_block = result_->GetDataBlockById(idx);

        ss << current_block->ToString();
    }

    return ss.str();
}

QueryContext::QueryContext(Session* session_ptr)
    : session_ptr_(session_ptr) {
    transaction_ = MakeUnique<TransactionContext>();
}

QueryResult
QueryContext::Query(const String &query) {
    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> parsed_result = MakeShared<ParserResult>();

    parser->Parse(query, parsed_result);

    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    SharedPtr<QueryContext> query_context = shared_from_this();

    LogicalPlanner logical_planner(query_context);
    Optimizer optimizer(query_context);
    PhysicalPlanner physical_planner(query_context);

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Only support single statement.");
    for (BaseStatement* statement : *parsed_result->statements_ptr_) {
        // Build unoptimized logical plan for each SQL statement.
        logical_planner.Build(statement);
        parsed_result->Reset();

        SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

        // Apply optimized rule to the logical plan
        SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        // Create execution pipeline
        SharedPtr<Pipeline> pipeline = OperatorPipeline::Create(physical_plan);

        // Schedule the query pipeline
        Infinity::instance().scheduler()->Schedule(query_context, pipeline);

        QueryResult query_result;
        query_result.result_ = pipeline->GetResult();
        query_result.root_operator_type_ = unoptimized_plan->operator_type();

        return query_result;
    }

    NetworkError("Not reachable");
}

}
