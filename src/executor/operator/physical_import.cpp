//
// Created by JinHai on 2022/7/28.
//

#include "physical_import.h"

namespace infinity {

void
PhysicalImport::Init() {

}

void
PhysicalImport::Execute(SharedPtr<QueryContext>& query_context) {
    String file_type;
    switch(import_file_type_) {
        case CopyFileType::kCSV: {
            file_type = "CSV";
            break;
        }
        case CopyFileType::kJSON: {
            file_type = "JSON";
            break;
        }
    }

    String err = fmt::format("IMPORT Table: {}.{} FROM file: {} WITH format: {}",
                             schema_name_,
                             table_name_,
                             file_path_,
                             file_type);

    NotImplementError("PhysicalImport");
}

}
