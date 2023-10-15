//
// Created by jinhai on 23-10-14.
//

module;

module infinity;

import stl;
import logger;

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
//
//        resource_manager_ = MakeUnique<ResourceManager>(config_->total_cpu_number(), config_->total_memory_size());
//
//        fragment_scheduler_ = MakeUnique<FragmentScheduler>(config_.get());
//
//        storage_ = MakeUnique<Storage>(config_.get());
//        storage_->Init();

        initialized_ = true;
    }
}

}