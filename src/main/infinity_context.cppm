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

export module infinity_context;

import stl;
import config;
import resource_manager;
import task_scheduler;
import storage;
import singleton;
import session_manager;
import persistence_manager;
import third_party;
import global_resource_usage;
import status;
import cluster_manager;
import peer_task;

namespace infinity {

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
    [[nodiscard]] inline ThreadPool &GetHnswBuildThreadPool() { return hnsw_build_thread_pool_; }

    NodeRole GetServerRole() const;
    void SetServerRole(NodeRole server_role);

    void Init(const SharedPtr<String> &config_path, bool admin_flag = false, DefaultConfig *default_config = nullptr);
//    void InitAdminMode(const SharedPtr<String> &config_path, bool m_flag = false, DefaultConfig *default_config = nullptr);
    Status ChangeRole(NodeRole target_role, const String& node_name = {}, String leader_ip = {}, i16 leader_port = {});
    bool IsAdminRole() const { return GetServerRole() == NodeRole::kAdmin; }
    bool IsClusterRole() const;

    void UnInit();

    void SetIndexThreadPool(SizeT thread_num);
    void RestoreIndexThreadPoolToDefault();

    void AddThriftServerFn(std::function<void()> start_func, std::function<void()> stop_func);
    void StartThriftServers();
    void StopThriftServers();
private:
    friend class Singleton;

    InfinityContext() = default;

    UniquePtr<Config> config_{};
    UniquePtr<ResourceManager> resource_manager_{};
    UniquePtr<TaskScheduler> task_scheduler_{};
    UniquePtr<Storage> storage_{};
    UniquePtr<SessionManager> session_mgr_{};
    UniquePtr<ClusterManager> cluster_manager_{};

    // For fulltext index
    ThreadPool inverting_thread_pool_{4};
    ThreadPool commiting_thread_pool_{2};

    // For hnsw index
    ThreadPool hnsw_build_thread_pool_{4};

    mutable std::mutex mutex_;
    NodeRole current_server_role_{NodeRole::kUnInitialized};

    std::function<void()> start_servers_func_{};
    std::function<void()> stop_servers_func_{};
};

} // namespace infinity
