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
import session_manager;
import persistence_manager;
import profiler;
import storage;
import txn;
import data_table;
import sql_parser;
import optimizer;
import status;
import query_result;
import base_statement;
import admin_statement;

export module query_context;

namespace infinity {

class LogicalPlanner;
class PhysicalPlanner;
class FragmentBuilder;
class TaskScheduler;
struct BGQueryState;

export class QueryContext {

public:
    explicit QueryContext(BaseSession* session);

    ~QueryContext();

    void Init(Config *global_config_ptr,
              TaskScheduler *scheduler_ptr,
              Storage *storage_ptr,
              ResourceManager *resource_manager_ptr,
              SessionManager* session_manager,
              PersistenceManager* persistence_manager);

    inline void UnInit() {
        initialized_ = false;
        session_ptr_ = nullptr;
        global_config_ = nullptr;
        scheduler_ = nullptr;
        storage_ = nullptr;
        resource_manager_ = nullptr;
        persistence_manager_ = nullptr;
    }

    QueryResult Query(const String &query);

    QueryResult QueryStatement(const BaseStatement *statement);

    bool ExecuteBGStatement(BaseStatement *statement, BGQueryState &state);

    bool JoinBGStatement(BGQueryState &state, TxnTimeStamp &commit_ts, bool rollback = false);

    inline void set_current_schema(const String &current_schema) { session_ptr_->set_current_schema(current_schema); }

    [[nodiscard]] inline const String &schema_name() const { return session_ptr_->current_database(); }

    [[nodiscard]] inline u64 cpu_number_limit() const { return cpu_number_limit_; }

    [[nodiscard]] inline bool is_enable_profiling() const { return session_ptr_->GetProfile(); }

    [[nodiscard]] inline u64 memory_size_limit() const { return memory_size_limit_; }

    [[nodiscard]] inline u64 query_id() const { return query_id_; }

    [[nodiscard]] inline u64 max_node_id() const { return current_max_node_id_; }

    inline void set_max_node_id(u64 node_id) { current_max_node_id_ = node_id; }

    inline u64 GetNextNodeID() { return ++current_max_node_id_; }

    void BeginTxn(const BaseStatement *statement = nullptr);

    TxnTimeStamp CommitTxn();

    void RollbackTxn();


    [[nodiscard]] Txn *GetTxn() const { return session_ptr_->GetTxn(); }

    bool SetTxn(Txn *txn) const {
        if (session_ptr_->GetTxn() == nullptr) {
            session_ptr_->SetTxn(txn);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline Storage *storage() const { return storage_; }

    [[nodiscard]] inline TaskScheduler *scheduler() const { return scheduler_; }

    [[nodiscard]] inline Config *global_config() const { return global_config_; }

    [[nodiscard]] inline ResourceManager *resource_manager() { return resource_manager_; }

    [[nodiscard]] inline SessionManager *session_manager() { return session_manager_; }

    [[nodiscard]] inline PersistenceManager *persistence_manager() { return persistence_manager_; }

    [[nodiscard]] inline SQLParser *parser() const { return parser_.get(); }
    [[nodiscard]] inline LogicalPlanner *logical_planner() const { return logical_planner_.get(); }
    [[nodiscard]] inline Optimizer *optimizer() const { return optimizer_.get(); }
    [[nodiscard]] inline PhysicalPlanner *physical_planner() const { return physical_planner_.get(); }
    [[nodiscard]] inline FragmentBuilder *fragment_builder() const { return fragment_builder_.get(); }

    [[nodiscard]] BaseSession* current_session() const { return session_ptr_; }

    void FlushProfiler(TaskProfiler &&profiler) {
        if(query_profiler_) {
            query_profiler_->Flush(std::move(profiler));
        }
    }

private:
    QueryResult HandleAdminStatement(const AdminStatement* admin_statement);

private:
    inline void CreateQueryProfiler() {
        if (is_enable_profiling()) {
            query_profiler_ = MakeShared<QueryProfiler>(true);
        }
    }

    inline void RecordQueryProfiler(const StatementType &type) {
        if (type != StatementType::kCommand && type != StatementType::kExplain && type != StatementType::kShow) {
            GetTxn()->GetCatalog()->AppendProfileRecord(query_profiler_);
        }
    }

    inline void StartProfile(QueryPhase phase) {
        if(query_profiler_) {
            query_profiler_->StartPhase(phase);
        }
    }
    inline void StopProfile(QueryPhase phase) {
        if(query_profiler_) {
            query_profiler_->StopPhase(phase);
        }
    }

    inline void StopProfile() {
        if(query_profiler_) {
            query_profiler_->Stop();
        }
    }

private:
    // Parser
    UniquePtr<SQLParser> parser_{};
    UniquePtr<LogicalPlanner> logical_planner_{};
    UniquePtr<Optimizer> optimizer_{};
    UniquePtr<PhysicalPlanner> physical_planner_{};
    UniquePtr<FragmentBuilder> fragment_builder_{};

    SharedPtr<QueryProfiler> query_profiler_{};

    Config *global_config_{};
    TaskScheduler *scheduler_{};
    Storage *storage_{};
    BaseSession *session_ptr_{};
    ResourceManager *resource_manager_{};
    SessionManager *session_manager_{};
    PersistenceManager* persistence_manager_{};

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
