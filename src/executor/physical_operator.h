//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "physical_operator_type.h"

#include <memory>
#include <utility>

namespace infinity {


class PhysicalOperator {

public:
    explicit PhysicalOperator(
            PhysicalOperatorType type,
            std::shared_ptr<PhysicalOperator> left,
            std::shared_ptr<PhysicalOperator> right)
            : operator_type_(type),
            left_(std::move(left)),
            right_(std::move(right)) {}
    virtual ~PhysicalOperator() = 0;

protected:
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    std::shared_ptr<PhysicalOperator> left_{nullptr};
    std::shared_ptr<PhysicalOperator> right_{nullptr};

};


}

