//
// Created by JinHai on 2022/12/15.
//

#pragma once

#include "bound_cast_func.h"
#include "common/types/data_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

inline static BoundCastFunc
BindBoolCast(const DataType& source, DataType& target) {
    TypeAssert(source.type() == LogicalType::kBoolean, "Expect boolean type, but it is " + source.ToString());
    switch(target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar8: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar16: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar32: {
            return BoundCastFunc(nullptr);
        }
        case LogicalType::kChar64: {
            return BoundCastFunc(nullptr);
        }
        default: {
            TypeError("Can't cast from Boolean to " + target.ToString());
        }
    }
}

}