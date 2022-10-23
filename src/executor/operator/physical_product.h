//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalProduct : public PhysicalOperator {
public:
    explicit PhysicalProduct(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kProduct, nullptr, nullptr, id) {}
    ~PhysicalProduct() = default;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}
