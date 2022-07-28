//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "network/session.h"
#include "scheduler/naive_scheduler.h"

namespace infinity {

class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] const std::shared_ptr<Scheduler>& scheduler() const { return scheduler_; }

private:
    friend class Singleton;
    Infinity();

    std::shared_ptr<Scheduler> scheduler_;
};

}

