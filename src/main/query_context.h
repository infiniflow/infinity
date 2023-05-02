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
    SharedPtr<Table> result_;
    LogicalNodeType root_operator_type_;

    [[nodiscard]] String
    ToString() const;
};

class QueryContext : public std::enable_shared_from_this<QueryContext> {
public:
    explicit
    QueryContext(SharedPtr<Session> session_ptr, UniquePtr<TransactionContext>& transaction);

    QueryResult
    Query(const String& query);
//    static QueryResult Execute(const String& query);

    inline void
    set_current_schema(const String& current_schema) {
        current_schema_ = current_schema;
    }

    inline UniquePtr<TransactionContext>& 
    transaction() { 
        return transaction_; 
    }

    inline const String&
    schema_name() const { 
        return current_schema_; 
    }

    inline u64
    query_id() const {
        return query_id_;
    }

    inline u64
    max_node_id() const {
        return current_max_node_id_;
    }

    inline void
    set_max_node_id(u64 node_id) {
        current_max_node_id_ = node_id;
    }

    inline u64
    GetNextNodeID() {
        return ++ current_max_node_id_;
    }

private:
    UniquePtr<TransactionContext>& transaction_;
    UniquePtr<QueryProfiler> query_metrics_;

    SharedPtr<Session> session_ptr_;

    // Get following information from session.
    // Current schema
    String current_schema_;

    // User / Tenant information
    String tenant_name_;
    String user_name_;

    u64 query_id_{0};
    u64 tenant_id_{0};
    u64 user_id_{0};
    u64 current_max_node_id_{0};
};

}




