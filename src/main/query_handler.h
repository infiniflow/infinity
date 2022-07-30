//
// Created by JinHai on 2022/7/22.
//

#pragma once

#include "storage/table.h"
#include "planner/logical_operator_type.h"

#include <string>

namespace infinity {

struct QueryResult {
    std::shared_ptr<Table> result_;
    LogicalOperatorType root_operator_type_;
};

class QueryHandler {
public:
    static QueryResult ExecuteQuery(const std::string& query);
private:
};

}




