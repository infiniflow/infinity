//
// Created by JinHai on 2022/7/23.
//

#include "logical_create_table.h"

#include <sstream>

namespace infinity {

std::string LogicalCreateTable::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "create table: " << *schema_name_ << "." << table_definitions_->name() << std::endl;
    return ss.str();
}

}


