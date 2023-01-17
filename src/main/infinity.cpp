//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "logger.h"

namespace infinity {

void
Infinity::Init() {
    if(initialized_) {
        return ;
    } else {
        scheduler_ = MakeUnique<NaiveScheduler>();
        config_ = MakeUnique<Config>();
        storage_ = MakeUnique<Storage>(String());

        storage_->Init();
        Logger::Initialize();
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
    Logger::Shutdown();

    storage_.reset();
    config_.reset();
    scheduler_.reset();
}

}
