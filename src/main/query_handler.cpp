//
// Created by JinHai on 2022/7/22.
//

#include "query_handler.h"
#include "planner/planner.h"

#include "SQLParser.h"
#include "SQLParserResult.h"

namespace infinity {

void
infinity::QueryHandler::execute_query(const std::string &query) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);

    // Build unoptimized logical plan for each SQL statement.
    Planner planner;
    planner.create_logical_plan(parse_result);
}

}
