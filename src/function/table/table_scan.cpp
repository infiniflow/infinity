//
// Created by JinHai on 2022/9/14.
//

#include "table_scan.h"

namespace infinity {

static void
TableScanFunc(std::shared_ptr<QueryContext>& query_context, TableFunctionData &data, TransBlock &output) {
    FunctionError("Table scan function isn't implemented.");
}

void
RegisterTableScanFunction(const std::unique_ptr<Catalog> &catalog_ptr) {

    std::shared_ptr<TableScanFunction> seq_scan_ptr = std::make_shared<TableScanFunction>("seq_scan", TableScanFunc);

    catalog_ptr->AddTableFunction(seq_scan_ptr);

}

}
