//
// Created by JinHai on 2022/7/22.
//

#pragma once

#include "storage/table.h"
#include "planner/logical_node_type.h"
#include "transaction_context.h"
#include "main/profiler/query_profiler.h"
#include "scheduler/scheduler.h"
#include "storage/storage.h"

#include <string>

namespace infinity {

class Session;

struct QueryResult {
    SharedPtr<Table> result_;
    LogicalNodeType root_operator_type_;

    [[nodiscard]] String
    ToString() const;
};

class QueryContext {
public:
    explicit
    QueryContext(Session* session_ptr,
                 const Config* global_config,
                 Scheduler* scheduler,
                 Storage* storage)
                 : session_ptr_(session_ptr),
                 global_config_(global_config),
                 scheduler_(scheduler),
                 storage_(storage)
    {}

    QueryResult
    Query(const String& query);
//    static QueryResult Execute(const String& query);

    inline void
    set_current_schema(const String& current_schema) {
        current_schema_ = current_schema;
    }

    [[nodiscard]] inline const String&
    schema_name() const { 
        return current_schema_; 
    }

    [[nodiscard]] inline u64
    query_id() const {
        return query_id_;
    }

    [[nodiscard]] inline u64
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

    void
    CreateTxn();

    void
    BeginTxn();

    void
    CommitTxn();

    Txn*
    GetTxn() const;

    inline Storage*
    storage() const {
        return storage_;
    }

    inline Scheduler*
    scheduler() const {
        return scheduler_;
    }

    inline const Config*
    global_config() const {
        return global_config_;
    }

private:
    UniquePtr<QueryProfiler> query_metrics_;

    const Config* global_config_{};
    Scheduler* scheduler_{};
    Storage* storage_{};

    Session* session_ptr_{};

    // Get following information from session.
    // Current schema
    String current_schema_;

    u64 catalog_version_{};

    // User / Tenant information
    String tenant_name_;
    String user_name_;

    u64 query_id_{0};
    u64 tenant_id_{0};
    u64 user_id_{0};
    u64 current_max_node_id_{0};
};

}




