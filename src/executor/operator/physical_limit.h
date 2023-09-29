//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "expression/base_expression.h"

namespace infinity {

class PhysicalLimit : public PhysicalOperator {
public:
    explicit
    PhysicalLimit(u64 id,
                  SharedPtr<PhysicalOperator> left,
                  SharedPtr<BaseExpression> limit_expr,
                  SharedPtr<BaseExpression> offset_expr)
            : PhysicalOperator(PhysicalOperatorType::kLimit, std::move(left), nullptr, id),
              limit_expr_(std::move(limit_expr)),
              offset_expr_(std::move(offset_expr)) {}

    ~PhysicalLimit() override = default;

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
    limit_expr() const {
        return limit_expr_;
    }

    inline const SharedPtr<BaseExpression>&
    offset_expr() const {
        return offset_expr_;
    }

    static SharedPtr<Table>
    GetLimitOutput(const SharedPtr<Table>& input_table, i64 limit, i64 offset);

private:
    SharedPtr<BaseExpression> limit_expr_{};
    SharedPtr<BaseExpression> offset_expr_{};

    SharedPtr<Table> input_table_{};
    u64 input_table_index_{};
};

}
