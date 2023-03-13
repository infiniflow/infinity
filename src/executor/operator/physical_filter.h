//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"
#include "expression/base_expression.h"
#include "executor/expression/expression_executor.h"
#include "executor/expression/expression_selector.h"

namespace infinity {

class PhysicalFilter : public PhysicalOperator {
public:
    explicit
    PhysicalFilter(u64 id,
                   SharedPtr<PhysicalOperator> left,
                   const SharedPtr<BaseExpression>& condition)
                   : PhysicalOperator(PhysicalOperatorType::kFilter, left, nullptr, id),
                     condition_(condition)
                     {}

    ~PhysicalFilter() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    const SharedPtr<BaseExpression>& condition_;

    ExpressionExecutor executor;
    ExpressionSelector selector_;

    SharedPtr<Table> input_table_{};
};


}
