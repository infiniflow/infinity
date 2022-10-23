//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id) {}
    ~PhysicalImport() = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};


}