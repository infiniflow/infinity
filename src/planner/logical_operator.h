//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "logical_operator_type.h"
#include "expression/base_expression.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace infinity {

class LogicalOperator {
public:
    explicit LogicalOperator(LogicalOperatorType node_type, uint64_t node_id) ;
    virtual ~LogicalOperator() = default;

    [[nodiscard]] std::shared_ptr<LogicalOperator> left_node() const { return left_node_; }
    [[nodiscard]] std::shared_ptr<LogicalOperator> right_node() const { return right_node_; }
    void set_left_node(const std::shared_ptr<LogicalOperator>& left) { left_node_ = left; }
    void set_right_node(const std::shared_ptr<LogicalOperator>& right) { right_node_ = right; };

    void AddOutputNode(const std::shared_ptr<LogicalOperator>& output);
    void RemoveOutputNode(const std::shared_ptr<LogicalOperator>& output);
    void ClearOutputs();
    [[nodiscard]] std::vector<std::shared_ptr<LogicalOperator>> outputs() const { return outputs_; }

    virtual std::string ToString(uint64_t space) = 0;
    static uint64_t get_new_id() { return ++ node_id_count_; }
    [[nodiscard]] uint64_t node_id() const { return node_id_; }

    [[nodiscard]] LogicalOperatorType operator_type() const {return operator_type_; }

protected:
    LogicalOperatorType operator_type_ = LogicalOperatorType::kInvalid;

    std::shared_ptr<LogicalOperator> left_node_;
    std::shared_ptr<LogicalOperator> right_node_;
    std::vector<std::shared_ptr<LogicalOperator>> outputs_;

    // Each node has an id which is unique in this plan tree.
    uint64_t node_id_{0};
    static uint64_t node_id_count_;

};

struct ColumnId {
public:
    [[nodiscard]] std::string ToString() const {
        if(table_name_) return *table_name_ + "." + column_name_;
        else return column_name_;
    }

    [[nodiscard]] bool operator==(const ColumnId& other) const {
        return column_name_ == other.column_name_ && table_name_ == other.table_name_;
    }

    std::string column_name_;
    std::optional<std::string> table_name_;
};

struct SelectListElement {
    explicit SelectListElement(std::shared_ptr<BaseExpression> expr) : expression_(std::move(expr)) {};
    void AddColumnId(const ColumnId& column_id) { identifiers_.push_back(column_id); }

    std::shared_ptr<BaseExpression> expression_;
    std::vector<ColumnId> identifiers_;
};



}

