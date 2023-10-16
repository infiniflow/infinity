//
// Created by JinHai on 2022/7/22.
//

module;

import logical_node_type;
import stl;
import config;
import session;
import resource_manager;
import fragment_scheduler;
import profiler;
import storage;
import txn;

export module query_context;

namespace infinity {

//class Session;
//class Storage;
//class ResourceManager;
//class FragmentScheduler;
//class QueryProfiler;
//class Table;
//class Txn;
//class Config;

struct QueryResult {
//    SharedPtr<Table> result_;
    LogicalNodeType root_operator_type_;

    [[nodiscard]] String ToString() const;
};

export class QueryContext {

public:
    explicit QueryContext() = default;

    inline ~QueryContext() { UnInit(); }

    void Init(Session *session_ptr,
              const Config *global_config_ptr,
              FragmentScheduler *scheduler_ptr,
              Storage *storage_ptr,
              ResourceManager *resource_manager_ptr);

    inline void UnInit() {
        initialized_ = false;
        session_ptr_ = nullptr;
        global_config_ = nullptr;
        scheduler_ = nullptr;
        storage_ = nullptr;
        resource_manager_ = nullptr;
    }

    QueryResult Query(const String &query);

    inline void set_current_schema(const String &current_schema) { current_schema_ = current_schema; }

    [[nodiscard]] inline const String &schema_name() const { return current_schema_; }

    [[nodiscard]] inline u64 cpu_number_limit() const { return cpu_number_limit_; }

    [[nodiscard]] inline u64 memory_size_limit() const { return memory_size_limit_; }

    [[nodiscard]] inline u64 query_id() const { return query_id_; }

    [[nodiscard]] inline u64 max_node_id() const { return current_max_node_id_; }

    inline void set_max_node_id(u64 node_id) { current_max_node_id_ = node_id; }

    inline u64 GetNextNodeID() { return ++current_max_node_id_; }

    void CreateTxn();

    void BeginTxn();

    void CommitTxn();

    void RollbackTxn();

    [[nodiscard]] Txn *GetTxn() const;

    [[nodiscard]] inline Storage *storage() const { return storage_; }

    [[nodiscard]] inline FragmentScheduler *scheduler() const { return scheduler_; }

    [[nodiscard]] inline const Config *global_config() const { return global_config_; }

    [[nodiscard]] inline ResourceManager *resource_manager() { return resource_manager_; }

private:

    SharedPtr<QueryProfiler> query_metrics_;

    const Config *global_config_{};
    FragmentScheduler *scheduler_{};
    Storage *storage_{};
    Session *session_ptr_{};
    ResourceManager *resource_manager_{};

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

    u64 cpu_number_limit_{};
    u64 memory_size_limit_{};

    bool initialized_{false};

};

} // namespace infinity
