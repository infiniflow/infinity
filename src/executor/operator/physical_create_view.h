//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalCreateView : public PhysicalOperator {
public:
    explicit PhysicalCreateView(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kCreateView, nullptr, nullptr, id) {}
    ~PhysicalCreateView() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}

