//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalCrossProduct : public PhysicalOperator {
public:
    explicit PhysicalCrossProduct(u64 id,
                                  u64 table_index,
                                  SharedPtr<PhysicalOperator> left,
                                  SharedPtr<PhysicalOperator> right)
        : PhysicalOperator(PhysicalOperatorType::kProduct,
                           std::move(left),
                           std::move(right),
                           id),
          output_table_index_(table_index)
        {}

    ~PhysicalCrossProduct() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    SharedPtr<Table> left_table_{};
    SharedPtr<Table> right_table_{};
    u64 left_table_index_{};
    u64 right_table_index_{};
    u64 output_table_index_{};
};

}
