//
// Created by JinHai on 2022/7/28.
//

#pragma once

//#include <utility>

#include "expression/base_expression.h"
#include "executor/physical_operator.h"
#include "executor/expression/expression_executor.h"

namespace infinity {

class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(uint64_t id, SharedPtr<PhysicalOperator> left, Vector<SharedPtr<BaseExpression>> expressions)
        : PhysicalOperator(PhysicalOperatorType::kProjection, std::move(left), nullptr, id),
          expressions_(std::move(expressions))
          {}

    ~PhysicalProject() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    Vector<SharedPtr<BaseExpression>> expressions_;

    ExpressionExecutor executor;
};

}


