//
// Created by jinhai on 23-3-6.
//

#include "logical_create_collection.h"

#include <sstream>

namespace infinity {

String
LogicalCreateCollection::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create Collection: " << *schema_name_ << "." << *collection_name_;
    space += arrow_str.size();

    return ss.str();
}

}
