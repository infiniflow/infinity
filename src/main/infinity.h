//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "network/connection.h"
#include <memory>
#include "scheduler/naive_scheduler.h"
#include "storage/catalog.h"
#include "storage/storage.h"
#include "config.h"

namespace infinity {

class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] std::unique_ptr<Scheduler>& scheduler() { return scheduler_; }
    [[nodiscard]] std::unique_ptr<Catalog>& catalog() { return storage_->catalog(); }
    [[nodiscard]] std::unique_ptr<Config>& config() { return config_; }
    [[nodiscard]] std::unique_ptr<Storage>& storage() { return storage_; }

    void Init();

private:
    friend class Singleton;
    Infinity();

    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<Config> config_;
    std::unique_ptr<Storage> storage_;

    bool initialized_false_{false};
};

}

