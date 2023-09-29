//
// Created by JinHai on 2022/7/23.
//

#include "logical_drop_table.h"
#include <sstream>

namespace infinity {

String
LogicalDropTable::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Drop Table: " << *schema_name_ << "." << *table_name_;
    space += arrow_str.size();

    return ss.str();
}

}
