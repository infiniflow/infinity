//
// Created by jinhai on 23-8-27.
//

#include "logical_import.h"

#include <sstream>

namespace infinity {

String
LogicalImport::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << "-> " << "Import from: " << file_path_;

    switch(file_type_) {
        case CopyFileType::kCSV: {
            ss << "(CSV) ";
            break;
        }
        case CopyFileType::kJSON: {
            ss << "(JSON) ";
            break;
        }
    }
    ss << "to " << *table_ptr_->schema_name() << '.' << *table_ptr_->table_name();

    space += arrow_str.size();

    return ss.str();
}

}
