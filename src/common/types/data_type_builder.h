//
// Created by jinhai on 23-2-26.
//

#pragma once

#include "data_type.h"
#include "sql/ColumnType.h"

namespace infinity {

class DataTypeBuilder {
public:
    static DataType
    Make(hsql::ColumnType type);

    static DataType
    ConvertType(hsql::ColumnType type);
};

}
