//
// Created by JinHai on 2022/8/10.
//

#include "view.h"


namespace infinity {
View::View(std::string view_name, std::string sql_text, hsql::SQLParserResult sql_parser_result,
         std::vector<std::string> column_names)
         : name_(std::move(view_name)),
         sql_text_(std::move(sql_text)),
         sql_parser_result_(std::move(sql_parser_result)),
         column_names_(std::move(column_names))
         {}
}
