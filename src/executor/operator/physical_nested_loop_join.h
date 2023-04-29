//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "parser/table_reference/join_reference.h"
#include "expression/base_expression.h"

namespace infinity {

class PhysicalNestedLoopJoin : public PhysicalOperator {
public:
    explicit PhysicalNestedLoopJoin(u64 id,
                                    JoinType join_type,
                                    Vector<SharedPtr<BaseExpression>> conditions,
                                    SharedPtr<PhysicalOperator> left,
                                    SharedPtr<PhysicalOperator> right)
        : PhysicalOperator(PhysicalOperatorType::kJoinNestedLoop,
                           std::move(left),
                           std::move(right),
                           id),
          join_type_(join_type),
          conditions_(std::move(conditions))
        {}

    ~PhysicalNestedLoopJoin() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    SharedPtr<Vector<DataType>>
    GetOutputTypes() const final;

    inline const Vector<SharedPtr<BaseExpression>>&
    conditions() const {
        return conditions_;
    }

private:
    SharedPtr<Table> left_table_{};
    SharedPtr<Table> right_table_{};
    JoinType join_type_{JoinType::kInner};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

}