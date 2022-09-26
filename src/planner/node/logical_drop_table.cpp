//
// Created by JinHai on 2022/7/23.
//

#include "logical_drop_table.h"
#include <sstream>

namespace infinity {

std::string
LogicalDropTable::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Drop Table: " << *schema_name_<< "." << *table_name_ << std::endl;
    space += arrow_str.size();
    return ss.str();
}

}
