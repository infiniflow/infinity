//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "expression/base_expression.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalInsert : public PhysicalOperator {
public:
    explicit PhysicalInsert(u64 id,
                            SharedPtr<Table> table_ptr,
                            u64 table_index,
                            Vector<SharedPtr<BaseExpression>> value_list)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id),
          table_ptr_(std::move(table_ptr)),
          table_index_(table_index),
          value_list_(std::move(value_list)) {
    }
    ~PhysicalInsert() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) override;

    inline const SharedPtr<Table>&
    table() const {
        return table_ptr_;
    }

    inline const Vector<SharedPtr<BaseExpression>>&
    value_list() const {
        return value_list_;
    }

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }
private:
    SharedPtr<Table> table_ptr_{};
    Vector<SharedPtr<BaseExpression>> value_list_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

}
