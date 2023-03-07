//
// Created by JinHai on 2022/8/10.
//

#include "view.h"


namespace infinity {
View::View(String view_name, String sql_text, SelectStatement* select_statement,
         Vector<String> column_names)
         : name_(std::move(view_name)),
         sql_text_(std::move(sql_text)),
         select_statement_(select_statement),
         column_names_(std::move(column_names))
         {}
}
