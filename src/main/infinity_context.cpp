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

namespace infinity {

String ToString(InfinityRole role) {
    switch (role) {
        case InfinityRole::kUnInitialized:
            return "uninitialized";
        case InfinityRole::kAdmin:
            return "admin";
        case InfinityRole::kStandalone:
            return "standalone";
        case InfinityRole::kLeader:
            return "leader";
        case InfinityRole::kFollower:
            return "follower";
        case InfinityRole::kLearner:
            return "learner";
    }
}

InfinityRole InfinityContext::GetServerRole() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return current_server_role_;
}

void InfinityContext::SetServerRole(InfinityRole server_role) {
    std::unique_lock<std::mutex> lock(mutex_);
    current_server_role_ = server_role;
}

void InfinityContext::Init(const SharedPtr<String> &config_path, bool admin_flag, DefaultConfig *default_config) {
    //    PythonInstance::Init();
    if (GetServerRole() != InfinityRole::kUnInitialized) {
        LOG_ERROR("Infinity is already initialized.");
        return;
    }

    // Variables
    VarUtil::InitVariablesMap();

    // Config
    config_ = MakeUnique<Config>();
    auto status = config_->Init(config_path, default_config);
    if (!status.ok()) {
        fmt::print("Error: {}", *status.msg_);
        std::exit(static_cast<int>(status.code()));
    }

    Logger::Initialize(config_.get());

    resource_manager_ = MakeUnique<ResourceManager>(config_->CPULimit(), 0);

    session_mgr_ = MakeUnique<SessionManager>();

    String persistence_dir = config_->PersistenceDir();
    if (!persistence_dir.empty()) {
        i64 persistence_object_size_limit = config_->PersistenceObjectSizeLimit();
        persistence_manager_ = MakeUnique<PersistenceManager>(persistence_dir, config_->DataDir(), (SizeT)persistence_object_size_limit);
    }

    Status change_result = ChangeRole(InfinityRole::kAdmin);
    if (!status.ok()) {
        UnrecoverableError(status.message());
        return;
    }

    if (admin_flag or config_->ServerMode() == "cluster") {
        // Admin mode or cluster start phase
        return;
    }

    if (config_->ServerMode() == "standalone") {
        Status change_to_standalone = ChangeRole(InfinityRole::kStandalone);
        if (!change_to_standalone.ok()) {
            UnrecoverableError(change_to_standalone.message());
            return;
        }
    } else {
        UnrecoverableError(fmt::format("Unexpected server mode: {}", config_->ServerMode()));
        return;
    }
}

Status InfinityContext::ChangeRole(InfinityRole target_role) {
    InfinityRole current_role = GetServerRole();
    if (current_role == target_role) {
        return Status::InvalidServerRole(fmt::format("Infinity is already the role of {}", ToString(current_role)));
    }

    switch (current_role) {
        case InfinityRole::kUnInitialized: {
            if (target_role != InfinityRole::kAdmin) {
                Status status = Status::InvalidServerRole("Un-init role can only be switch to Admin role");
                return status;
            }
            storage_ = MakeUnique<Storage>(config_.get());
            storage_->SetStorageMode(StorageMode::kAdmin);
            break;
        }
        case InfinityRole::kAdmin: {
            switch (target_role) {
                case InfinityRole::kStandalone:
                case InfinityRole::kLeader: {
                    storage_->SetStorageMode(StorageMode::kWritable);
                    break;
                }
                case InfinityRole::kFollower:
                case InfinityRole::kLearner: {
                    storage_->SetStorageMode(StorageMode::kReadable);
                    break;
                }
                case InfinityRole::kUnInitialized: {
                    storage_->SetStorageMode(StorageMode::kUnInitialized);
                    storage_.reset();
                    SetServerRole(InfinityRole::kUnInitialized);
                    return Status::OK();
                }
                case InfinityRole::kAdmin: {
                    Status status = Status::InvalidServerRole("Can't switch admin role to admin");
                    return status;
                }
            }
            task_scheduler_ = MakeUnique<TaskScheduler>(config_.get());

            i64 cpu_limit = config_->CPULimit();
            SetIndexThreadPool(cpu_limit);
            break;
        }
        case InfinityRole::kStandalone: {
            switch (target_role) {
                case InfinityRole::kAdmin: {
                    storage_->SetStorageMode(StorageMode::kAdmin);
                    RestoreIndexThreadPoolToDefault();

                    task_scheduler_->UnInit();
                    task_scheduler_.reset();
                    break;
                }
                case InfinityRole::kUnInitialized: {
                    Status status = Status::InvalidServerRole("Can't switch standalone role to un-init");
                    return status;
                }
                default: {
                    Status status = Status::InvalidServerRole("Can't switch Infinity to cluster mode");
                    return status;
                }
            }
            break;
        }
        case InfinityRole::kLeader: {
            switch (target_role) {
                case InfinityRole::kAdmin: {
                    storage_->SetStorageMode(StorageMode::kAdmin);
                    RestoreIndexThreadPoolToDefault();

                    task_scheduler_->UnInit();
                    task_scheduler_.reset();
                    break;
                }
                case InfinityRole::kStandalone: {
                    Status status = Status::InvalidServerRole("Can't switch leader role to standalone");
                    return status;
                }
                case InfinityRole::kUnInitialized: {
                    Status status = Status::InvalidServerRole("Can't switch leader role to un-init");
                    return status;
                }
                case InfinityRole::kFollower:
                case InfinityRole::kLeader: {
                    storage_->SetStorageMode(StorageMode::kReadable);
                    break;
                }
                default: {
                    Status status = Status::InvalidServerRole("Error to switch infinity role");
                    return status;
                }
            }
            break;
        }
        case InfinityRole::kLearner:
        case InfinityRole::kFollower: {
            switch (target_role) {
                case InfinityRole::kAdmin: {
                    storage_->SetStorageMode(StorageMode::kAdmin);
                    RestoreIndexThreadPoolToDefault();

                    task_scheduler_->UnInit();
                    task_scheduler_.reset();
                    break;
                }
                case InfinityRole::kStandalone: {
                    Status status = Status::InvalidServerRole("Can't switch follower role to standalone");
                    RecoverableError(status);
                    break;
                }
                case InfinityRole::kUnInitialized: {
                    Status status = Status::InvalidServerRole("Can't switch follower role to un-init");
                    RecoverableError(status);
                    break;
                }
                case InfinityRole::kLeader: {
                    storage_->SetStorageMode(StorageMode::kWritable);
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
    }
    SetServerRole(target_role);
    return Status::OK();
}

void InfinityContext::UnInit() {
    InfinityRole current_role = GetServerRole();

    switch (current_role) {
        case InfinityRole::kUnInitialized: {
            LOG_INFO("Infinity is already un-initialized.");
            return;
        }
        case InfinityRole::kAdmin: {
            Status status = ChangeRole(InfinityRole::kUnInitialized);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
            break;
        }
        default: {
            Status status = ChangeRole(InfinityRole::kAdmin);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
            status = ChangeRole(InfinityRole::kUnInitialized);
            if (!status.ok()) {
                UnrecoverableError(status.message());
                return;
            }
        }
    }

    storage_.reset();
    persistence_manager_.reset();
    session_mgr_.reset();
    resource_manager_.reset();
    Logger::Shutdown();
    config_.reset();

    //    PythonInstance::UnInit();
}

void InfinityContext::SetIndexThreadPool(SizeT thread_num) {
    inverting_thread_pool_.resize(thread_num);
    commiting_thread_pool_.resize(thread_num);
    hnsw_build_thread_pool_.resize(thread_num);
}

void InfinityContext::RestoreIndexThreadPoolToDefault() {
    inverting_thread_pool_.resize(4);
    commiting_thread_pool_.resize(2);
    hnsw_build_thread_pool_.resize(4);
}

} // namespace infinity
