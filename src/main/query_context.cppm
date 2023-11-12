// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import logical_node_type;
import stl;
import config;
import session;
import resource_manager;
//import fragment_scheduler;
import profiler;
import storage;
import txn;
import data_table;
import parser;
import optimizer;
import status;
import query_result;

export module query_context;

namespace infinity {

class LogicalPlanner;
class PhysicalPlanner;
class FragmentBuilder;
class FragmentScheduler;

export class QueryContext {

public:
    explicit QueryContext(SessionBase* session);

    ~QueryContext();

    void Init(const Config *global_config_ptr,
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

    QueryResult QueryStatement(const BaseStatement *statement);

    inline void set_current_schema(const String &current_schema) { session_ptr_->set_current_schema(current_schema); }

    [[nodiscard]] inline const String &schema_name() const { return session_ptr_->current_database(); }

    [[nodiscard]] inline u64 cpu_number_limit() const { return cpu_number_limit_; }

    [[nodiscard]] inline bool is_enable_profiler() const { return session_ptr_->options()->enable_profiling_; }

    [[nodiscard]] inline u64 memory_size_limit() const { return memory_size_limit_; }

    [[nodiscard]] inline u64 query_id() const { return query_id_; }

    [[nodiscard]] inline u64 max_node_id() const { return current_max_node_id_; }

    void FlushProfiler(TaskProfiler &&profiler) {
        query_metrics_->Flush(Move(profiler));
    }

    inline void set_max_node_id(u64 node_id) { current_max_node_id_ = node_id; }

    inline u64 GetNextNodeID() { return ++current_max_node_id_; }

    void CreateTxn();

    void BeginTxn();

    void CommitTxn();

    void RollbackTxn();

    void TryMarkProfiler(const StatementType &type) const {
        if (is_enable_profiler() && type != StatementType::kCommand && type != StatementType::kExplain && type != StatementType::kShow) {
            session_ptr_->AppendProfilerRecord(query_metrics_);
        }
    }

    [[nodiscard]] Txn *GetTxn() const { return session_ptr_->GetTxn(); }

    [[nodiscard]] inline Storage *storage() const { return storage_; }

    [[nodiscard]] inline FragmentScheduler *scheduler() const { return scheduler_; }

    [[nodiscard]] inline const Config *global_config() const { return global_config_; }

    [[nodiscard]] inline ResourceManager *resource_manager() { return resource_manager_; }

    [[nodiscard]] inline SQLParser *parser() const { return parser_.get(); }
    [[nodiscard]] inline LogicalPlanner *logical_planner() const { return logical_planner_.get(); }
    [[nodiscard]] inline Optimizer *optimizer() const { return optimizer_.get(); }
    [[nodiscard]] inline PhysicalPlanner *physical_planner() const { return physical_planner_.get(); }
    [[nodiscard]] inline FragmentBuilder *fragment_builder() const { return fragment_builder_.get(); }

    [[nodiscard]] SessionBase* current_session() const { return session_ptr_; }
private:
    // Parser
    UniquePtr<SQLParser> parser_{};
    UniquePtr<LogicalPlanner> logical_planner_{};
    UniquePtr<Optimizer> optimizer_{};
    UniquePtr<PhysicalPlanner> physical_planner_{};
    UniquePtr<FragmentBuilder> fragment_builder_{};

    SharedPtr<QueryProfiler> query_metrics_;

    const Config *global_config_{};
    FragmentScheduler *scheduler_{};
    Storage *storage_{};
    SessionBase *session_ptr_{};
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
