//
// Created by JinHai on 2022/7/23.
//

#include "logical_drop_table.h"
#include <sstream>

namespace infinity {

std::string
LogicalDropTable::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "drop table: " << *schema_name_<< "." << *table_name_ << std::endl;
    return ss.str();
}

}
