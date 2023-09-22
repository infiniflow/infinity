//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "legacy_sched/naive_scheduler.h"

namespace infinity {

void
Infinity::Init(const SharedPtr<String>& config_path) {
    if(initialized_) {
        return ;
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

void
Infinity::UnInit() {
    if(!initialized_) {
        return;
    }
    initialized_ = false;

    storage_->Uninit();
    storage_.reset();

    resource_manager_.reset();

    Logger::Shutdown();

    config_.reset();
}

}
