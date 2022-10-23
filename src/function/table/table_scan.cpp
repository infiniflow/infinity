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
    std::shared_ptr<TableFunctionSet> function_set_ptr = std::make_shared<TableFunctionSet>("seq_scan");

    TableScanFunction seq_scan("seq_scan", TableScanFunc);
    function_set_ptr->AddFunction(seq_scan);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
