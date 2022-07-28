//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "pipeline.h"

namespace infinity {

class Scheduler {
public:
    Scheduler() = default;
    virtual ~Scheduler() = 0;

    virtual void Schedule(const std::shared_ptr<Pipeline>& pipeline) = 0;
};

}
