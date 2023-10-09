//
// Created by jinhai on 23-3-6.
//

#include "logical_drop_index.h"
#include <sstream>

namespace infinity {

String LogicalDropIndex::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Drop Index: " << *schema_name_ << "." << *index_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
