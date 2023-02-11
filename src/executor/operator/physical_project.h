//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "expression/base_expression.h"
#include "executor/physical_operator.h"
#include "executor/expression/expression_executor.h"

namespace infinity {

class PhysicalProject : public PhysicalOperator {
public:
    explicit
    PhysicalProject(u64 id,
                    u64 table_index,
                    SharedPtr<PhysicalOperator> left,
                    Vector<SharedPtr<BaseExpression>> expressions)
                    : PhysicalOperator(PhysicalOperatorType::kProjection, std::move(left), nullptr, id),
                    output_table_index_(table_index),
                    expressions_(std::move(expressions))
                    {}

    ~PhysicalProject() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    Vector<SharedPtr<BaseExpression>> expressions_{};

private:
    ExpressionExecutor executor;
    u64 output_table_index_{};

    SharedPtr<Table> input_table_{};
    u64 input_table_index_{};

};

}


