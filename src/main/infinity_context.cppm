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

export module infinity_core:infinity_context;

import :stl;
import :status;
import :singleton;
import admin_statement;
import :storage;
import :task_scheduler;
import :cluster_manager;

namespace infinity {
struct DefaultConfig;
class SessionManager;
class ResourceManager;
class PersistenceManager;
struct Config;

export class InfinityContext : public Singleton<InfinityContext> {
public:
    [[nodiscard]] inline TaskScheduler *task_scheduler() noexcept { return task_scheduler_.get(); }

    [[nodiscard]] inline Config *config() noexcept { return config_.get(); }

    [[nodiscard]] inline Storage *storage() noexcept { return storage_.get(); }

    [[nodiscard]] inline PersistenceManager *persistence_manager() noexcept { return storage_->persistence_manager(); }

    [[nodiscard]] inline ResourceManager *resource_manager() noexcept { return resource_manager_.get(); }

    [[nodiscard]] inline SessionManager *session_manager() noexcept { return session_mgr_.get(); }

    [[nodiscard]] inline ClusterManager *cluster_manager() noexcept { return cluster_manager_.get(); }

    [[nodiscard]] inline ThreadPool &GetFulltextInvertingThreadPool() { return inverting_thread_pool_; }
    [[nodiscard]] inline ThreadPool &GetFulltextCommitingThreadPool() { return commiting_thread_pool_; }
    [[nodiscard]] inline ThreadPool &GetLsgBuildThreadPool() { return lsg_build_thread_pool_; }
    [[nodiscard]] inline ThreadPool &GetHnswBuildThreadPool() { return hnsw_build_thread_pool_; }

    NodeRole GetServerRole() const;

    void InitPhase1(const SharedPtr<String> &config_path, DefaultConfig *default_config = nullptr);
    void InitPhase2(bool admin_flag = false);
    //    void InitAdminMode(const SharedPtr<String> &config_path, bool m_flag = false, DefaultConfig *default_config = nullptr);
    Status
    ChangeServerRole(NodeRole target_role, bool from_leader = false, const String &node_name = {}, String leader_ip = {}, u16 leader_port = {});
    bool IsAdminRole() const { return GetServerRole() == NodeRole::kAdmin; }
    bool IsClusterRole() const;

    void UnInit();

    void SetIndexThreadPool();
    void RestoreIndexThreadPoolToDefault();

    void AddThriftServerFn(std::function<void()> start_func, std::function<void()> stop_func);
    void StartThriftServers();
    void StopThriftServers();

    bool InfinityContextStarted() const { return infinity_context_started_; }
    bool InfinityContextInited() const { return infinity_context_inited_; }

    // Only used by UT
    void SetConfig(UniquePtr<Config> &&config);

private:
    friend class Singleton;

    InfinityContext();
    virtual ~InfinityContext();

    UniquePtr<Config> config_{};
    UniquePtr<ResourceManager> resource_manager_{};
    UniquePtr<TaskScheduler> task_scheduler_{};
    UniquePtr<Storage> storage_{};
    UniquePtr<SessionManager> session_mgr_{};
    UniquePtr<ClusterManager> cluster_manager_{};
    atomic_bool infinity_context_started_{false};
    atomic_bool infinity_context_inited_{false};

    // For fulltext index
    ThreadPool inverting_thread_pool_{2};
    ThreadPool commiting_thread_pool_{2};

    // For hnsw index
    ThreadPool lsg_build_thread_pool_{8};
    ThreadPool hnsw_build_thread_pool_{2};

    std::function<void()> start_servers_func_{};
    std::function<void()> stop_servers_func_{};
    atomic_bool start_server_{false};
};

} // namespace infinity
