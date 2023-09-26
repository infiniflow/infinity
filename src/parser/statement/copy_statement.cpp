//
// Created by jinhai on 23-2-25.
//

#include "copy_statement.h"

namespace infinity {

String
CopyStatement::ToString() const {
    std::stringstream ss;
    String copy_direction;
    if (copy_from_) {
        copy_direction = " FROM ";
    } else {
        copy_direction = " TO ";
    }

    String file_format;
    switch (copy_file_type_) {
        case CopyFileType::kCSV: {
            file_format = "CSV";
            break;
        }
        case CopyFileType::kJSON: {
            file_format = "JSON";
            break;
        }
        case CopyFileType::kFVECS: {
            file_format = "FVECS";
            break;
        }
    }

    ss << "COPY: " << schema_name_
       << "." << table_name_
       << copy_direction << file_path_
       << " WITH " << file_format
       << " delimiter: " << delimiter_;
    return ss.str();
}

}
