//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "logical_node_type.h"
#include "expression/base_expression.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace infinity {

class LogicalNode {
public:
    explicit LogicalNode(LogicalNodeType node_type) ;
    virtual ~LogicalNode() = default;

    [[nodiscard]] std::shared_ptr<LogicalNode> left_node() const { return left_node_; }
    [[nodiscard]] std::shared_ptr<LogicalNode> right_node() const { return right_node_; }
    void set_left_node(const std::shared_ptr<LogicalNode>& left) { left_node_ = left; }
    void set_right_node(const std::shared_ptr<LogicalNode>& right) { right_node_ = right; };

    void AddOutputNode(const std::shared_ptr<LogicalNode>& output);
    void RemoveOutputNode(const std::shared_ptr<LogicalNode>& output);
    void ClearOutputs();
    [[nodiscard]] std::vector<std::shared_ptr<LogicalNode>> outputs() const { return outputs_; }

    virtual std::string ToString(uint64_t space) = 0;
    [[nodiscard]] uint64_t node_id() const { return node_id_; }
    void set_node_id(uint64_t node_id) { node_id_ = node_id; }

    [[nodiscard]] LogicalNodeType operator_type() const {return operator_type_; }

protected:
    LogicalNodeType operator_type_ = LogicalNodeType::kInvalid;

    std::shared_ptr<LogicalNode> left_node_;
    std::shared_ptr<LogicalNode> right_node_;
    std::vector<std::shared_ptr<LogicalNode>> outputs_;

    // Each node has an id which is unique in this plan tree.
    uint64_t node_id_{0};

};

struct ColumnIdentifier {
public:
    explicit ColumnIdentifier(std::shared_ptr<std::string> table_name, std::shared_ptr<std::string> column_name,
                              std::shared_ptr<std::string> alias_name)
        : column_name_ptr_(std::move(column_name)),
        table_name_ptr_(std::move(table_name)),
        alias_name_ptr_(std::move(alias_name))
        {}

    [[nodiscard]] std::string ToString() const {
        if(table_name_ptr_ != nullptr) return *table_name_ptr_ + "." + *column_name_ptr_;
        else return *column_name_ptr_;
    }

    [[nodiscard]] bool operator==(const ColumnIdentifier& other) const {
        if(*column_name_ptr_ != *other.column_name_ptr_) {
            return false;
        }
        if(table_name_ptr_ != nullptr && other.table_name_ptr_ != nullptr) {
            return *table_name_ptr_ == *other.table_name_ptr_;
        }

        if(table_name_ptr_ == nullptr && other.table_name_ptr_ == nullptr) {
            return true;
        }

        return false;
    }

    std::shared_ptr<std::string> column_name_ptr_;
    std::shared_ptr<std::string> table_name_ptr_;
    std::shared_ptr<std::string> alias_name_ptr_;
};

struct SelectListElement {
    explicit SelectListElement(std::shared_ptr<BaseExpression> expr) : expression_(std::move(expr)) {};
    void AddColumnIdentifier(const ColumnIdentifier& column_identifier) { identifiers_.emplace_back(column_identifier); }

    std::shared_ptr<BaseExpression> expression_;
    std::vector<ColumnIdentifier> identifiers_{};
};



}

