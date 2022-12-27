//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {
class PhysicalDropView : public PhysicalOperator {
public:
    explicit PhysicalDropView(u64 id)
        : PhysicalOperator(PhysicalOperatorType::kDropView, nullptr, nullptr, id) {}
    ~PhysicalDropView() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;
};

}
