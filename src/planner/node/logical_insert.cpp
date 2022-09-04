//
// Created by JinHai on 2022/7/23.
//

#include "logical_insert.h"
#include <sstream>

namespace infinity {

std::string LogicalInsert::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "insert table: " << table_ptr_->table_def()->name() << std::endl;
    return ss.str();
}

}
