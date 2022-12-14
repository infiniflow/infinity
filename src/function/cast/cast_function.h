//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "common/column_vector/column_vector.h"
#include "common/types/data_type.h"

namespace infinity {

struct CastParameters {
};

using cast_function_t = bool(*)(const ColumnVector &source, ColumnVector &result, idx_t count, CastParameters& parameters);

struct BoundCastFunc {
    explicit
    BoundCastFunc(cast_function_t func) : function(func) {};

    cast_function_t function = nullptr;
};

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




