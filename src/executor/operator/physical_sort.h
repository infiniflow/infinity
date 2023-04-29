//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "expression/base_expression.h"
#include "executor/physical_operator.h"
#include "executor/expression/expression_executor.h"

namespace infinity {

class PhysicalSort : public PhysicalOperator {
public:
    explicit
    PhysicalSort(u64 id,
                 SharedPtr<PhysicalOperator> left,
                 Vector<SharedPtr<BaseExpression>> expressions,
                 Vector<OrderType> order_by_types)
                 : PhysicalOperator(PhysicalOperatorType::kSort, std::move(left), nullptr, id),
                   expressions_(std::move(expressions)),
                   order_by_types_(std::move(order_by_types))
                   {}
    ~PhysicalSort() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return left_->GetOutputNames();
    }

    inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return left_->GetOutputTypes();
    }

    void
    Sort(const SharedPtr<Table>& order_by_table,
         const Vector<OrderType>& order_by_types_);

    static SharedPtr<Table>
    GenerateOutput(const SharedPtr<Table>& input_table,
                   const SharedPtr<Vector<RowID>>& rowid_vector);

    Vector<SharedPtr<BaseExpression>> expressions_;
    Vector<OrderType> order_by_types_{};
private:

    SharedPtr<Table>
    GetOrderTable() const;

private:
    ExpressionExecutor executor_;
    SharedPtr<Table> input_table_{};
    u64 input_table_index_{};
};

}

