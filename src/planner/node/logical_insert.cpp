//
// Created by JinHai on 2022/7/23.
//

#include "logical_insert.h"
#include <sstream>

namespace infinity {

std::string LogicalInsert::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Insert Table: " << table_ptr_->table_def()->name();
    space += arrow_str.size();
    return ss.str();
}

}
