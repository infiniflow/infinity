//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "main/query_context.h"
#include "pipeline.h"

namespace infinity {

class Scheduler {
public:
    Scheduler() = default;
    virtual ~Scheduler() = 0;

    virtual void Schedule(std::shared_ptr<QueryContext>& query_context,
                          const std::shared_ptr<Pipeline>& pipeline) = 0;
};

}
