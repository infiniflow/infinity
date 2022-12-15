//
// Created by JinHai on 2022/12/15.
//

#pragma once

namespace infinity {

struct CastParameters {
};

using cast_function_t = bool(*)(const ColumnVector &source, ColumnVector &result, idx_t count, CastParameters& parameters);

struct BoundCastFunc {
    explicit
    BoundCastFunc(cast_function_t func) : function(func) {};

    cast_function_t function = nullptr;
};

}
