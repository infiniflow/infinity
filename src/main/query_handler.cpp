//
// Created by JinHai on 2022/7/22.
//

#include "query_handler.h"
#include "planner/planner.h"
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

    // Build unoptimized logical plan for each SQL statement.
    Planner planner;

    for (hsql::SQLStatement *statement : parse_result.getStatements()) {
        planner.CreateLogicalPlan(statement);
        ResponseError(planner.root_operator()->ToString(0));
    }
}

}
