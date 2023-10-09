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

    virtual void Schedule(QueryContext *query_context, const SharedPtr<Pipeline> &pipeline) = 0;
};

} // namespace infinity
