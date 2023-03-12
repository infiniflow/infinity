//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "expression/base_expression.h"
#include "storage/table.h"
#include "planner/logical_node.h"

namespace infinity {

class LogicalInsert : public LogicalNode {
public:
    explicit LogicalInsert(u64 node_id,
                           SharedPtr<Table> table_ptr,
                           u64 table_index,
                           Vector<SharedPtr<BaseExpression>> value_list)
            : LogicalNode(node_id, LogicalNodeType::kInsert),
              table_ptr_(std::move(table_ptr)),
              table_index_(table_index),
              value_list_(std::move(value_list)) {};

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String 
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalInsert";
    }

    void 
    set_value_list(const Vector<SharedPtr<BaseExpression>>& value_list) {
        value_list_ = value_list;
    }

    SharedPtr<Table>& table_ptr() {
        return table_ptr_;
    }

    Vector<SharedPtr<BaseExpression>>&
    value_list() {
        return value_list_;
    }

    [[nodiscard]] inline u64
    table_index() const {
        return table_index_;
    }

public:
    static bool
    NeedCastInInsert(const DataType& from, const DataType& to) {
        if(from.type() == to.type()) {
            switch (from.type()) {
                case kVarchar: {
                    auto* from_type_info = (VarcharInfo*)from.type_info().get();
                    auto* to_type_info = (VarcharInfo*)to.type_info().get();
                    if(to_type_info->length_limit() >= from_type_info->length_limit()) {
                        // Unify the length limit of insert varchar
                        from_type_info->UpdateLengthLimit(to_type_info->length_limit());
                        return false;
                    }
                    break;
                }
                case kChar: {
                    auto* from_type_info = (CharInfo*)from.type_info().get();
                    auto* to_type_info = (CharInfo*)to.type_info().get();
                    if(to_type_info->length_limit() >= from_type_info->length_limit()) {
                        // Unify the length limit of insert char
                        from_type_info->UpdateLengthLimit(to_type_info->length_limit());
                        return false;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return true;
    }

private:
    SharedPtr<Table> table_ptr_{};
    Vector<SharedPtr<BaseExpression>> value_list_{};
    u64 table_index_{};
};

}
