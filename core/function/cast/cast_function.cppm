//
// Created by JinHai on 2022/12/14.
//

module;

import bound_cast_func;
import parser;

export module cast_function;

namespace infinity {

export class CastFunction {
public:
    static BoundCastFunc GetBoundFunc(const DataType &source, const DataType &target);

private:
    static BoundCastFunc NumericCast(const DataType &source, const DataType &target);

    static BoundCastFunc DecimalCast(const DataType &source, const DataType &target);

    static BoundCastFunc StringCast(const DataType &source, const DataType &target);
};

} // namespace infinity
