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

namespace infinity {

void InfinityContext::Init(const SharedPtr<String> &config_path, DefaultConfig *default_config) {
    if (initialized_) {
        return;
    } else {
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

        task_scheduler_ = MakeUnique<TaskScheduler>(config_.get());

        session_mgr_ = MakeUnique<SessionManager>();

        String persistence_dir = config_->PersistenceDir();
        if (!persistence_dir.empty()) {
            i64 persistence_object_size_limit = config_->PersistenceObjectSizeLimit();
            persistence_manager_ = MakeUnique<PersistenceManager>(persistence_dir, config_->DataDir(), (SizeT)persistence_object_size_limit);
        }

        storage_ = MakeUnique<Storage>(config_.get());
        storage_->Init();

        inverting_thread_pool_.resize(config_->CPULimit());
        commiting_thread_pool_.resize(config_->CPULimit());
        hnsw_build_thread_pool_.resize(config_->CPULimit());
        initialized_ = true;
    }
}

void InfinityContext::UnInit() {
    if (!initialized_) {
        return;
    }
    initialized_ = false;

    storage_->UnInit();
    storage_.reset();

    session_mgr_.reset();

    task_scheduler_->UnInit();
    task_scheduler_.reset();

    resource_manager_.reset();
    persistence_manager_.reset();

    Logger::Shutdown();
    persistence_manager_.reset();

    config_.reset();
}

} // namespace infinity
