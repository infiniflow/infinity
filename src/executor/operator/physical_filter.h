//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include <utility>

#include "executor/physical_operator.h"
#include "expression/base_expression.h"
#include "executor/expression/expression_evaluator.h"
#include "src/executor/expression/expression_selector.h"

namespace infinity {

class PhysicalFilter : public PhysicalOperator {
public:
    explicit
    PhysicalFilter(u64 id,
                   SharedPtr<PhysicalOperator> left,
                   SharedPtr<BaseExpression> condition)
            : PhysicalOperator(PhysicalOperatorType::kFilter, std::move(left), nullptr, id),
              condition_(std::move(condition)) {}

    ~PhysicalFilter() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return left_->GetOutputNames();
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return left_->GetOutputTypes();
    }

    inline const SharedPtr<BaseExpression>&
    condition() const {
        return condition_;
    }

private:
    SharedPtr<BaseExpression> condition_;

    ExpressionEvaluator executor_;
    ExpressionSelector selector_;

    SharedPtr<Table> input_table_{};
};


}
