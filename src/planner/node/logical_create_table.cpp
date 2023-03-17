//
// Created by JinHai on 2022/7/23.
//

#include "logical_create_table.h"

#include <sstream>

namespace infinity {

String LogicalCreateTable::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create Table: " << *schema_name_ << "." << table_definition_->ToString();
    space += arrow_str.size();

    return ss.str();
}

}


