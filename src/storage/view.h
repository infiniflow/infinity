//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

// A view means a logical plan
namespace infinity {

class LogicalNode;

class View {
public:
    View(std::string view_name,
         std::string sql_text,
         std::shared_ptr<LogicalNode> logical_plan,
         std::vector<std::string> column_names);

    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] const std::string& sql_text() const { return sql_text_; }
    [[nodiscard]] const std::shared_ptr<LogicalNode>& logical_plan() const { return logical_plan_; }
    [[nodiscard]] const std::vector<std::string>& column_names() const { return column_names_; }
    void set_view_id(uint64_t view_id) { view_id_ = view_id; }

private:
    std::string name_;
    std::string sql_text_;
    std::shared_ptr<LogicalNode> logical_plan_;
    std::vector<std::string> column_names_;
    uint64_t view_id_{0};
};
}

