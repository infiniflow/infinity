//
// Created by JinHai on 2022/7/22.
//

#pragma once

#include "storage/table.h"
#include "planner/logical_node_type.h"
#include "transaction_context.h"
#include "main/profiler/query_profiler.h"

#include <string>

namespace infinity {

class Session;

struct QueryResult {
    std::shared_ptr<Table> result_;
    LogicalNodeType root_operator_type_;

    [[nodiscard]] std::string ToString() const;
};

class QueryContext : public std::enable_shared_from_this<QueryContext> {
public:
    explicit QueryContext(const std::shared_ptr<Session>& session_ptr, std::unique_ptr<TransactionContext>& transaction);
    QueryResult Query(const std::string& query);
//    static QueryResult Execute(const std::string& query);

    inline void set_current_schema(const std::string& current_schema) { current_schema_ = current_schema; }
    inline std::unique_ptr<TransactionContext>& transaction() { return transaction_; }

    const std::string& schema_name() const { return current_schema_; }
private:
    std::unique_ptr<TransactionContext>& transaction_;
    std::unique_ptr<QueryProfiler> query_metrics_;

//    std::weak_ptr<Session> session_ptr_;

    // Get following information from session.
    // Current schema
    std::string current_schema_;

    // User / Tenant information
    std::string tenant_name_;
    std::string user_name_;

    uint64_t tenant_id_{0};
    uint64_t user_id_{0};
};

}




