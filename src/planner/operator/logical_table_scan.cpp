//
// Created by JinHai on 2022/8/10.
//

#include "logical_table_scan.h"

#include <sstream>

namespace infinity {

std::string
LogicalTableScan::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "TableScan: " << table_ptr_->table_def()->name() << std::endl;
    return ss.str();
}

}