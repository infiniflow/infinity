//
// Created by JinHai on 2022/7/28.
//

#pragma

#include "scheduler.h"

namespace infinity {

class NaiveScheduler : public Scheduler {
public:
    ~NaiveScheduler() override = default;

    void
    Schedule(std::shared_ptr<QueryContext>& query_context,
             const std::shared_ptr<Pipeline>& pipeline) override;
};

}

