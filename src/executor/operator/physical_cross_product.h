//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalCrossProduct final : public PhysicalOperator {
public:
    explicit PhysicalCrossProduct(u64 id,
                                  SharedPtr<PhysicalOperator> left,
                                  SharedPtr<PhysicalOperator> right)
            : PhysicalOperator(PhysicalOperatorType::kCrossProduct,
                               std::move(left),
                               std::move(right),
                               id) {}

    ~PhysicalCrossProduct() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final;

private:
    SharedPtr<Table> left_table_{};
    SharedPtr<Table> right_table_{};
};

}
