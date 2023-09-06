//
// Created by JinHai on 2022/9/14.
//

#include "table_scan.h"
#include "main/logger.h"

namespace infinity {

void
RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<TableScanFunction> seq_scan_ptr = MakeShared<TableScanFunction>("seq_scan", TableScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), seq_scan_ptr);

}

}
