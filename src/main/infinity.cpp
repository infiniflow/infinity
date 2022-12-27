//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "logger.h"

namespace infinity {

Infinity::Infinity()
    : scheduler_(MakeUnique<NaiveScheduler>()),
    config_(MakeUnique<Config>()),
    storage_(MakeUnique<Storage>(String())){
}

void
Infinity::Init() {
    if(initialized_) {
        return ;
    } else {
        storage_->Init();
        Logger::Initialize();
        initialized_ = true;
    }
}

void
Infinity::UnInit() {
    if(!initialized_) return;
    initialized_ = false;
    storage_->Uninit();
    Logger::Shutdown();
}

}
