//
// Created by jinhai on 23-3-6.
//

#include "logical_drop_collection.h"
#include <sstream>

namespace infinity {

String LogicalDropCollection::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Drop Collection: " << *schema_name_ << "." << *collection_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
