//
// Created by JinHai on 2022/8/10.
//

#include "view.h"

infinity::View::View(std::string view_name, std::string sql_text, std::shared_ptr<LogicalNode> logical_plan,
         std::vector<std::string> column_names)
         : name_(std::move(view_name)),
         sql_text_(std::move(sql_text)),
         logical_plan_(std::move(logical_plan)),
         column_names_(std::move(column_names))
         {}
