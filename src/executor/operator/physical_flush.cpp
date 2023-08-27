//
// Created by jinhai on 23-8-26.
//

#include "physical_flush.h"

namespace infinity {

void
PhysicalFlush::Init() {

}

void
PhysicalFlush::Execute(SharedPtr<QueryContext>& query_context) {
    switch(flush_type_) {
        case FlushType::kData: {
            FlushData(query_context);
            break;
        }
        case FlushType::kLog: {
            FlushLog(query_context);
            break;
        }
        case FlushType::kBuffer: {
            FlushBuffer(query_context);
            break;
        }
    }
}

void
PhysicalFlush::FlushData(SharedPtr<QueryContext>& query_context) {
    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

void
PhysicalFlush::FlushLog(SharedPtr<QueryContext>& query_context) {
    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

void
PhysicalFlush::FlushBuffer(SharedPtr<QueryContext>& query_context) {
    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}
