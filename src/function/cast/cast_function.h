//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "common/column_vector/column_vector.h"
#include "common/types/data_type.h"
#include "bound_cast_func.h"

namespace infinity {

class CastFunction {
public:
    static BoundCastFunc
    GetBoundFunc(const DataType &source, const DataType &target);
private:
    static BoundCastFunc
    NumericCast(const DataType &source, const DataType &target);

    static BoundCastFunc
    DecimalCast(const DataType &source, const DataType &target);

    static BoundCastFunc
    StringCast(const DataType &source, const DataType &target);

};

}




