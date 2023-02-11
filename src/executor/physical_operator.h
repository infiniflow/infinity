//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "main/query_context.h"
#include "physical_operator_type.h"
#include "storage/table.h"

#include <memory>
#include <utility>

namespace infinity {

class OperatorPipeline;

class PhysicalOperator : public std::enable_shared_from_this<PhysicalOperator> {

public:
    explicit PhysicalOperator(
            PhysicalOperatorType type,
            SharedPtr<PhysicalOperator> left,
            SharedPtr<PhysicalOperator> right,
            u64 id)
            : operator_id_(id),
            operator_type_(type),
            left_(std::move(left)),
            right_(std::move(right)) {}

    virtual
    ~PhysicalOperator() = default;

    virtual void
    Init() = 0;

    SharedPtr<OperatorPipeline>
    GenerateOperatorPipeline();

    SharedPtr<PhysicalOperator>
    left() const {
        return left_;
    }

    SharedPtr<PhysicalOperator>
    right() const {
        return right_;
    }

    u64
    operator_id() const {
        return operator_id_;
    }

    virtual void
    Execute(SharedPtr<QueryContext>& query_context) = 0;

    const HashMap<u64, SharedPtr<Table>>&
    outputs() const {
        return outputs_;
    }

protected:
    u64 operator_id_;
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<PhysicalOperator> left_{nullptr};
    SharedPtr<PhysicalOperator> right_{nullptr};

    std::weak_ptr<OperatorPipeline> operator_pipeline_{};

    HashMap<u64, SharedPtr<Table>> outputs_{};
};


}

