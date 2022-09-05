//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "network/connection.h"
#include "scheduler/naive_scheduler.h"
#include "storage/catalog.h"
#include "config.h"

namespace infinity {

class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] std::unique_ptr<Scheduler>& scheduler() { return scheduler_; }
    [[nodiscard]] std::unique_ptr<Catalog>& catalog() { return catalog_; }
    [[nodiscard]] std::unique_ptr<Config>& config() { return config_; }

private:
    friend class Singleton;
    Infinity();

    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<Catalog> catalog_;
    std::unique_ptr<Config> config_;
};

}

