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

    ChangeRole(InfinityRole::kAdmin);
    if (admin_flag or config_->ServerMode() == "cluster") {
        // Admin mode or cluster start phase
        return;
    }

    if (config_->ServerMode() == "standalone") {
        ChangeRole(InfinityRole::kStandalone);
    } else {
        UnrecoverableError(fmt::format("Unexpected server mode: {}", config_->ServerMode()));
        return;
    }
}

void InfinityContext::ChangeRole(InfinityRole target_role) {
    InfinityRole current_role = GetServerRole();
    if (current_role == target_role) {
        Status status = Status::InvalidServerRole("Infinity switch to same role");
        RecoverableError(status);
    }

    switch (current_role) {
        case InfinityRole::kUnInitialized: {
            if (target_role != InfinityRole::kAdmin) {
                Status status = Status::InvalidServerRole("Un-init role can only be switch to Admin role");
                RecoverableError(status);
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
                    return;
                }
                case InfinityRole::kAdmin: {
                    Status status = Status::InvalidServerRole("Can't switch admin role to admin");
                    RecoverableError(status);
                }
            }
            task_scheduler_ = MakeUnique<TaskScheduler>(config_.get());

            i64 cpu_limit = config_->CPULimit();
            inverting_thread_pool_.resize(cpu_limit);
            commiting_thread_pool_.resize(cpu_limit);
            hnsw_build_thread_pool_.resize(cpu_limit);
            break;
        }
        case InfinityRole::kStandalone: {
            switch (target_role) {
                case InfinityRole::kAdmin: {
                    storage_->SetStorageMode(StorageMode::kAdmin);
                    break;
                }
                case InfinityRole::kUnInitialized: {
                    Status status = Status::InvalidServerRole("Can't switch standalone role to un-init");
                    RecoverableError(status);
                }
                default: {
                    Status status = Status::InvalidServerRole("Can't switch Infinity to cluster mode");
                    RecoverableError(status);
                }
            }
            break;
        }
        case InfinityRole::kLeader: {
            switch (target_role) {
                case InfinityRole::kAdmin: {
                    storage_->SetStorageMode(StorageMode::kAdmin);
                    break;
                }
                case InfinityRole::kStandalone: {
                    Status status = Status::InvalidServerRole("Can't switch leader role to standalone");
                    RecoverableError(status);
                    break;
                }
                case InfinityRole::kUnInitialized: {
                    Status status = Status::InvalidServerRole("Can't switch leader role to un-init");
                    RecoverableError(status);
                    break;
                }
                case InfinityRole::kFollower: {
                    case InfinityRole::kLeader: {
                        storage_->SetStorageMode(StorageMode::kReadable);
                        break;
                    }
                    default: {
                        Status status = Status::InvalidServerRole("Error to switch infinity role");
                        RecoverableError(status);
                    }
                } break;
            }
            case InfinityRole::kLearner:
            case InfinityRole::kFollower: {
                switch (target_role) {
                    case InfinityRole::kAdmin: {
                        storage_->SetStorageMode(StorageMode::kAdmin);
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
            }
        }
    }
    SetServerRole(target_role);
}

void InfinityContext::UnInit() {
    if (GetServerRole() == InfinityRole::kUnInitialized) {
        LOG_INFO("Infinity is already un-initialized.");
        return;
    }

    inverting_thread_pool_.resize(0);
    commiting_thread_pool_.resize(0);
    hnsw_build_thread_pool_.resize(0);

    task_scheduler_->UnInit();
    task_scheduler_.reset();

    storage_->SetStorageMode(StorageMode::kAdmin);
    storage_->SetStorageMode(StorageMode::kUnInitialized);

    storage_.reset();
    persistence_manager_.reset();
    session_mgr_.reset();
    resource_manager_.reset();
    Logger::Shutdown();
    config_.reset();

    //    PythonInstance::UnInit();
}

} // namespace infinity
