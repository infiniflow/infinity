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
        : PhysicalOperator(PhysicalOperatorType::kProduct,
                           std::move(left),
                           std::move(right),
                           id)
        {}

    ~PhysicalCrossProduct() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

private:
    SharedPtr<Table> left_table_{};
    SharedPtr<Table> right_table_{};
};

}
