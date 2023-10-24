//
// Created by jinhai on 23-10-14.
//

module;

module infinity;

import stl;
import logger;
import config;
import resource_manager;
import fragment_scheduler;
import storage;

namespace infinity {

void
Infinity::Init(const SharedPtr <String>& config_path) {
    if(initialized_) {
        return;
    } else {
        // Config
        config_ = MakeUnique<Config>();
        config_->Init(config_path);

        Logger::Initialize(config_.get());

        resource_manager_ = MakeUnique<ResourceManager>(config_->total_cpu_number(), config_->total_memory_size());

        fragment_scheduler_ = MakeUnique<FragmentScheduler>(config_.get());

        storage_ = MakeUnique<Storage>(config_.get());
        storage_->Init();

        initialized_ = true;
    }
}

void Infinity::UnInit() {
    if (!initialized_) {
        return;
    }
    initialized_ = false;

    storage_->UnInit();
    storage_.reset();

    fragment_scheduler_->UnInit();

    resource_manager_.reset();

    Logger::Shutdown();

    config_.reset();
}

}