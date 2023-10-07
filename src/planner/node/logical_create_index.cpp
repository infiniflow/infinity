#include "logical_create_index.h"

namespace infinity {
String
LogicalCreateIndex::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create Table: " << *schema_name_ << "." << index_definition_->ToString();
    space += arrow_str.size();

    return ss.str();
}
}// namespace infinity