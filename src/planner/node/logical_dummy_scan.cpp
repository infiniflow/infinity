//
// Created by JinHai on 2022/10/25.
//

#include "logical_dummy_scan.h"

#include <sstream>

namespace infinity {

String
LogicalDummyScan::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> " << "DummyScan: " << this->table_alias_ << "(" << this->table_index_ << ")";
    space += arrow_str.size();

    return ss.str();
}

}
