//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "network/session.h"
#include "scheduler/naive_scheduler.h"
#include "storage/catalog.h"

namespace infinity {

class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] std::shared_ptr<Scheduler> scheduler() const { return scheduler_; }
    [[nodiscard]] std::shared_ptr<Catalog> catalog() const { return catalog_; }

private:
    friend class Singleton;
    Infinity();

    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<Catalog> catalog_;
};

}

