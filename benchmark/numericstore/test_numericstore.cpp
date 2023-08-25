#include "storage/indexstore/numeric/numeric_db.h"

#include <iostream>
#include <string>

using namespace infinity;

int main() {
    std::string data_path = "./numeric";
    NumericDB db(data_path, 1);
    SharedPtr<TableDef> table_def;
    Vector<SharedPtr<ColumnDef>> columns;
    columns.push_back(MakeShared<ColumnDef>(0,
                                            MakeShared<DataType>(DataType(LogicalType::kInteger)),
                                            "c1",
                                            HashSet<ConstraintType>())
                      );

    table_def = MakeShared<TableDef>("test",columns);
    db.Create(table_def);


    return 0;
}