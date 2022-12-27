//
// Created by JinHai on 2022/7/23.
//

#include "logical_insert.h"
#include <sstream>

namespace infinity {

String 
LogicalInsert::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Insert Table: " << table_ptr_->TableName();
    space += arrow_str.size();
    return ss.str();
}

}
