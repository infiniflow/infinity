//
// Created by jinhai on 23-3-6.
//

#include "logical_create_schema.h"

#include <sstream>

namespace infinity {

String
LogicalCreateSchema::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create Schema: " << *schema_name_;
    space += arrow_str.size();

    return ss.str();
}

}
