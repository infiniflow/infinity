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

#include <cstdlib>
#include <string>

module infinity_context;

import stl;
import third_party;
import logger;
import config;
import resource_manager;
import task_scheduler;
import storage;
import session_manager;
import variables;
// import python_instance;
import status;
import infinity_exception;
import wal_manager;
import global_resource_usage;
import infinity_thrift_service;

namespace infinity {

InfinityContext::InfinityContext() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("InfinityContext");
#endif
}

InfinityContext::~InfinityContext() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("InfinityContext");
#endif
}

NodeRole InfinityContext::GetServerRole() const {
    if (cluster_manager_ == nullptr) {
        return NodeRole::kUnInitialized;
    }
    return cluster_manager_->GetNodeRole();
}

void InfinityContext::Init(const SharedPtr<String> &config_path, bool admin_flag, DefaultConfig *default_config) {

    if (GetServerRole() != NodeRole::kUnInitialized) {
        LOG_ERROR("Infinity is already initialized.");
        return;
    }

    // Variables
    VarUtil::InitVariablesMap();

    // Config
    config_ = MakeUnique<Config>();
    auto status = config_->Init(config_path, default_config);
    if (!status.ok()) {
        std::exit(static_cast<int>(status.code()));
    }
    Logger::Initialize(config_.get());

    resource_manager_ = MakeUnique<ResourceManager>(config_->CPULimit(), 0);

    session_mgr_ = MakeUnique<SessionManager>();

    Status change_result = ChangeRole(NodeRole::kAdmin);
    if (!status.ok()) {
        UnrecoverableError(status.message());
        return;
    }
    if (admin_flag or config_->ServerMode() == "cluster") {
        // Admin mode or cluster start phase
        return;
    }

    if (config_->ServerMode() == "standalone") {
        Status change_to_standalone = ChangeRole(NodeRole::kStandalone);
        if (!change_to_standalone.ok()) {
            UnrecoverableError(change_to_standalone.message());
            return;
        }
    } else {
        UnrecoverableError(fmt::format("Unexpected server mode: {}", config_->ServerMode()));
        return;
    }
}

Status InfinityContext::ChangeRole(NodeRole target_role, bool from_leader, const String &node_name, String node_ip, u16 node_port) {
    NodeRole current_role = GetServerRole();
    if (current_role == target_role) {
        return Status::OK();
    }
    LOG_INFO(fmt::format("ChangeRole from {} to {}", ToString(current_role), ToString(target_role)));

    switch (current_role) {
        case NodeRole::kUnInitialized: {
            if (target_role != NodeRole::kAdmin) {
                Status status = Status::InvalidNodeRole("Un-init role can only be switch to Admin role");
                return status;
            }
            storage_ = MakeUnique<Storage>(config_.get());
            if (cluster_manager_ != nullptr) {
                UnrecoverableError("Cluster manager was initialized before");
            }
            cluster_manager_ = MakeUnique<ClusterManager>();
            cluster_manager_->InitAsAdmin();
            storage_->SetStorageMode(StorageMode::kAdmin);
            StartThriftServers();
            break;
        }
        case NodeRole::kAdmin: {
            switch (target_role) {
                case NodeRole::kStandalone: {
                    cluster_manager_->InitAsStandalone();
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kWritable);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError(fmt::format("Failed to init storage to standalone, messgae: {}", set_storage_status.message()));
                    }
                    break;
                }
                case NodeRole::kLeader: {
                    // No need to un-init cluster manager, since current is admin
                    Status init_status = cluster_manager_->InitAsLeader(node_name);
                    if (!init_status.ok()) {
                        return init_status;
                    }
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kWritable);
                    if (!set_storage_status.ok()) {
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_->InitAsAdmin();
                        return set_storage_status;
                    }
                    break;
                }
                case NodeRole::kFollower: {
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kReadable);
                    if (!set_storage_status.ok()) {
                        return set_storage_status;
                    }

                    // No need to un-init cluster manager
                    Status init_status = cluster_manager_->InitAsFollower(node_name, node_ip, node_port);
                    if (!init_status.ok()) {
                        storage_->SetStorageMode(StorageMode::kAdmin);
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_.reset();
                        cluster_manager_ = MakeUnique<ClusterManager>();
                        cluster_manager_->InitAsAdmin();
                        return init_status;
                    }
                    break;
                }
                case NodeRole::kLearner: {
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kReadable);
                    if (!set_storage_status.ok()) {
                        return set_storage_status;
                    }

                    // No need to un-init cluster manager, since current is admin
                    Status init_status = cluster_manager_->InitAsLearner(node_name, node_ip, node_port);
                    if (!init_status.ok()) {
                        storage_->SetStorageMode(StorageMode::kAdmin);
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_.reset();
                        cluster_manager_ = MakeUnique<ClusterManager>();
                        cluster_manager_->InitAsAdmin();
                        return init_status;
                    }
                    break;
                }
                case NodeRole::kUnInitialized: {
                    StopThriftServers();
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kUnInitialized);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to un-init infinity context.");
                    }
                    storage_.reset();
                    cluster_manager_->UnInit(from_leader);
                    cluster_manager_.reset();
                    return Status::OK();
                }
                case NodeRole::kAdmin: {
                    Status status = Status::InvalidNodeRole("Can't switch admin role to admin");
                    return status;
                }
            }
            task_scheduler_ = MakeUnique<TaskScheduler>(config_.get());

            i64 cpu_limit = config_->CPULimit();
            SetIndexThreadPool(cpu_limit);
            break;
        }
        case NodeRole::kStandalone: {
            switch (target_role) {
                case NodeRole::kAdmin: {
                    cluster_manager_->UnInit(from_leader);
                    cluster_manager_->InitAsAdmin();
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to standalone.");
                    }

                    RestoreIndexThreadPoolToDefault();

                    if (task_scheduler_ != nullptr) {
                        task_scheduler_->UnInit();
                        task_scheduler_.reset();
                    }
                    break;
                }
                case NodeRole::kUnInitialized: {
                    Status status = Status::InvalidNodeRole("Can't switch standalone role to un-init");
                    return status;
                }
                default: {
                    Status status = Status::InvalidNodeRole("Can't switch Infinity from standalone to cluster mode");
                    return status;
                }
            }
            break;
        }
        case NodeRole::kLeader: {
            switch (target_role) {
                case NodeRole::kAdmin: {
                    if (cluster_manager_ == nullptr) {
                        UnrecoverableError("cluster manager wasn't valid.");
                    }
                    // TODO: disconnect with all follower/learner
                    cluster_manager_->UnInit(from_leader);
                    cluster_manager_->InitAsAdmin();

                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to admin.");
                    }
                    RestoreIndexThreadPoolToDefault();

                    if (task_scheduler_ != nullptr) {
                        task_scheduler_->UnInit();
                        task_scheduler_.reset();
                    }
                    break;
                }
                case NodeRole::kFollower: {
                    cluster_manager_->UnInit(from_leader);

                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to admin.");
                    }

                    set_storage_status = storage_->SetStorageMode(StorageMode::kReadable);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to follower.");
                    }

                    Status init_status = cluster_manager_->InitAsFollower(node_name, node_ip, node_port);
                    if (!init_status.ok()) {
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_.reset();
                        return init_status;
                    }
                    break;
                }
                case NodeRole::kLearner: {
                    cluster_manager_->UnInit(from_leader);

                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to admin.");
                    }

                    set_storage_status = storage_->SetStorageMode(StorageMode::kReadable);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to follower.");
                    }

                    Status init_status = cluster_manager_->InitAsLearner(node_name, node_ip, node_port);
                    if (!init_status.ok()) {
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_.reset();
                        return init_status;
                    }
                    break;
                }
                case NodeRole::kStandalone: {
                    return Status::InvalidNodeRole("Can't switch infinity role from leader to standalone");
                }
                default: {
                    Status status = Status::InvalidNodeRole(fmt::format("Can't switch infinity role from leader to {}", ToString(target_role)));
                    return status;
                }
            }
            break;
        }
        case NodeRole::kLearner:
        case NodeRole::kFollower: {
            switch (target_role) {
                case NodeRole::kAdmin: {
                    if (cluster_manager_ == nullptr) {
                        UnrecoverableError("cluster manager wasn't valid.");
                    }
                    // TODO: disconnect from leader;
                    cluster_manager_->UnInit(from_leader);
                    cluster_manager_->InitAsAdmin();

                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to admin.");
                    }

                    RestoreIndexThreadPoolToDefault();

                    if (task_scheduler_ != nullptr) {
                        task_scheduler_->UnInit();
                        task_scheduler_.reset();
                    }

                    break;
                }
                case NodeRole::kStandalone: {
                    Status status = Status::InvalidNodeRole("Can't switch follower role to standalone");
                    RecoverableError(status);
                    break;
                }
                case NodeRole::kUnInitialized: {
                    Status status = Status::InvalidNodeRole("Can't switch follower role to un-init");
                    RecoverableError(status);
                    break;
                }
                case NodeRole::kLeader: {
                    if (current_role == NodeRole::kLearner) {
                        Status status = Status::CantSwitchRole("Can't switch from learner to leader");
                        return status;
                    }
                    if (cluster_manager_ == nullptr) {
                        UnrecoverableError("cluster manager wasn't valid.");
                    }
                    // TODO: disconnect from leader;
                    cluster_manager_->UnInit(from_leader);
                    Status set_storage_status = storage_->SetStorageMode(StorageMode::kAdmin);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to admin.");
                    }

                    Status init_status = cluster_manager_->InitAsLeader(node_name);
                    if (!init_status.ok()) {
                        cluster_manager_->UnInit(from_leader);
                        cluster_manager_.reset();
                        return init_status;
                    }

                    set_storage_status = storage_->SetStorageMode(StorageMode::kWritable);
                    if (!set_storage_status.ok()) {
                        UnrecoverableError("Failed to set node storage to leader.");
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
    }

    Status status = Status::OK();
    if (target_role == NodeRole::kFollower or target_role == NodeRole::kLearner) {
        status = cluster_manager_->RegisterToLeader();
    } else if (target_role == NodeRole::kLeader) {
        cluster_manager_->CheckHeartBeat();
    }
    return status;
}

bool InfinityContext::IsClusterRole() const {
    NodeRole node_role = GetServerRole();
    if (node_role == NodeRole::kLeader or node_role == NodeRole::kFollower or node_role == NodeRole::kLearner) {
        return true;
    } else {
        return false;
    }
}

void InfinityContext::UnInit() {
    StopThriftServers();

    NodeRole current_role = GetServerRole();

    switch (current_role) {
        case NodeRole::kUnInitialized: {
            LOG_INFO("Infinity is already un-initialized.");
            return;
        }
        case NodeRole::kAdmin: {
            Status status = ChangeRole(NodeRole::kUnInitialized);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
            break;
        }
        default: {
            Status status = ChangeRole(NodeRole::kAdmin);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
            status = ChangeRole(NodeRole::kUnInitialized);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
        }
    }

    storage_.reset();

    session_mgr_.reset();
    resource_manager_.reset();
    Logger::Shutdown();
    config_.reset();
}

void InfinityContext::SetIndexThreadPool(SizeT thread_num) {
    thread_num = thread_num / 2;
    if (thread_num < 2)
        thread_num = 2;
    LOG_TRACE(fmt::format("Set index thread pool size to {}", thread_num));
    inverting_thread_pool_.resize(thread_num);
    commiting_thread_pool_.resize(thread_num);
    hnsw_build_thread_pool_.resize(thread_num);
}

void InfinityContext::RestoreIndexThreadPoolToDefault() {
    LOG_TRACE("Restore index thread pool size to default");
    inverting_thread_pool_.resize(4);
    commiting_thread_pool_.resize(2);
    hnsw_build_thread_pool_.resize(4);
}

void InfinityContext::AddThriftServerFn(std::function<void()> start_func, std::function<void()> stop_func) {
    start_servers_func_ = start_func;
    stop_servers_func_ = stop_func;
}

void InfinityContext::StartThriftServers() {
    if (GetServerRole() == NodeRole::kUnInitialized) {
        UnrecoverableError("Invalid node role");
    }

    if (start_servers_func_ and !start_server_) {
        start_servers_func_();
        start_server_ = true;
    }
}

void InfinityContext::StopThriftServers() {
    if (stop_servers_func_ && start_server_) {
        stop_servers_func_();
        start_server_ = false;
        // Not set to nullptr, to enable restart the server.
        // start_servers_func_ = nullptr;
        // stop_servers_func_ = nullptr;
    }
    // close all thrift sessions
    const auto removed_session_count = InfinityThriftService::ClearSessionMap();
    LOG_INFO(fmt::format("Removed {} thrift sessions", removed_session_count));
}

} // namespace infinity
