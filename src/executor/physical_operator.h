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
    node_id() const {
        return operator_id_;
    }

    virtual void
    Execute(QueryContext* query_context) = 0;

    const SharedPtr<Table>&
    output() const {
        return output_;
    }

    inline PhysicalOperatorType
    operator_type() const {
        return operator_type_;
    }

    virtual SharedPtr<Vector<String>>
    GetOutputNames() const = 0;

    virtual SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const = 0;

    virtual String
    GetName () const ;

protected:
    u64 operator_id_;
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<PhysicalOperator> left_{nullptr};
    SharedPtr<PhysicalOperator> right_{nullptr};

    std::weak_ptr<OperatorPipeline> operator_pipeline_{};

    SharedPtr<Table> output_{};


public:
    // Operator

    virtual bool ParallelOperator() const {
        return false;
    }



public:
    // Exchange
    virtual bool IsExchange() const {
        return false;
    }

    virtual bool ParallelExchange() const {
        return false;
    }

public:
    // Sink
    virtual bool IsSink() const {
        return false;
    }

    virtual bool ParallelSink() const {
        return false;
    }

    virtual bool SinkOrderMatters() const {
        return false;
    }
};
} // namespace infinity

