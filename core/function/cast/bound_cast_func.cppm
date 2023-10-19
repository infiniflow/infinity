//
// Created by JinHai on 2022/12/15.
//

module;

import stl;

export module bound_cast_func;

namespace infinity {

class ColumnVector;

struct CastParameters {
    bool strict{false};
};

using cast_function_t = bool (*)(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &result, SizeT count, CastParameters &parameters);

export struct BoundCastFunc {
    explicit BoundCastFunc(cast_function_t func) : function(func){};

    cast_function_t function = nullptr;
};

} // namespace infinity
