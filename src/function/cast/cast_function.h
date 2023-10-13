//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function/cast/bound_cast_func.h"
//#include "function/cast/column_vector_cast.h"
//#include "common/column_vector/column_vector.h"
//#include "common/types/data_type.h"

namespace infinity {

class DataType;

class CastFunction {
public:
    static BoundCastFunc GetBoundFunc(const DataType &source, const DataType &target);

private:
    static BoundCastFunc NumericCast(const DataType &source, const DataType &target);

    static BoundCastFunc DecimalCast(const DataType &source, const DataType &target);

    static BoundCastFunc StringCast(const DataType &source, const DataType &target);
};

} // namespace infinity
