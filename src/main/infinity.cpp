//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "logger.h"

namespace infinity {

Infinity::Infinity()
    : scheduler_(std::make_unique<NaiveScheduler>()),
    config_(std::make_unique<Config>()),
    storage_(std::make_unique<Storage>(std::string())){
}

void
Infinity::Init() {
    if(initialized_false_) {
        return ;
    } else {
        storage_->Init();
        Logger::Initialize();
    }
}

}
