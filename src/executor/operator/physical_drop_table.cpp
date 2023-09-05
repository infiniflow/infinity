//
// Created by JinHai on 2022/7/28.
//

#include "physical_drop_table.h"

namespace infinity {

void
PhysicalDropTable::Init() {

}

void
PhysicalDropTable::Execute(QueryContext* query_context) {

    Txn* txn = query_context->GetTxn();
    EntryResult res = txn->DropTableCollectionByName(*schema_name_, *table_name_, conflict_type_);
    if(res.err_ != nullptr) {
        ExecutorError(*res.err_);
    }

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}
