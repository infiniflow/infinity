//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "logger.h"
#include "scheduler/naive_scheduler.h"
#include "storage/storage.h"
#include "config.h"

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

        scheduler_ = MakeUnique<NaiveScheduler>();

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

    scheduler_.reset();

    Logger::Shutdown();

    config_.reset();
}

}
