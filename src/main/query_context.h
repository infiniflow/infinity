//
// Created by JinHai on 2022/7/22.
//

#pragma once

#include "storage/table.h"
#include "planner/logical_node_type.h"
#include "transaction_context.h"
#include "query_metrics.h"

#include <string>

namespace infinity {

struct QueryResult {
    std::shared_ptr<Table> result_;
    LogicalNodeType root_operator_type_;
};

class QueryContext {
public:
    static QueryResult Execute(const std::string& query);
private:
    // transaction context
    std::unique_ptr<TransactionContext> transaction_;
    std::unique_ptr<QueryMetrics> query_metrics_;
};

}




