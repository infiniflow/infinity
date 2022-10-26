//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalExport : public PhysicalOperator {
public:
    explicit PhysicalExport(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kExport, nullptr, nullptr, id) {}
    ~PhysicalExport() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}
