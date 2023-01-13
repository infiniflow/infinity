//
// Created by JinHai on 2022/12/14.
//

#include "cast_function.h"
#include "bool_cast.h"
#include "integer_cast.h"

namespace infinity {

template <typename SourceType>
static BoundCastFunc
NumericCast(const infinity::DataType &target) {
    switch (target.type()) {
        case kTinyInt: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kSmallInt: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kInteger: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kBigInt: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kHugeInt: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kFloat: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        case kDouble: {
            NotImplementError("Can't cast to " + target.ToString());
        }
        default: {
            NotImplementError("Can't cast to " + target.ToString());
        }
    }
}


BoundCastFunc
CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    TypeAssert(source != target, "Attempt to cast from " + source.ToString() + " to " + target.ToString());

    switch(source.type()) {
        case kBoolean: {
            return BindBoolCast(source, target);
        }
        case kTinyInt: {
            return BindIntegerCast<TinyIntT>(source, target);
        }
        case kSmallInt: {
            return BindIntegerCast<SmallIntT>(source, target);
        }
        case kInteger: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kBigInt: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kHugeInt: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kFloat: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDouble: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDecimal16: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDecimal32: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDecimal64: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDecimal128: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kVarchar: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDate: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kTime: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kDateTime: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kTimestamp: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kTimestampTZ: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kInterval: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kArray: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kTuple: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kPoint: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kLine: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kLineSeg: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kBox: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kPath: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kPolygon: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kCircle: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kBitmap: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kUuid: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kBlob: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kEmbedding: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kMixed: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        case kNull: {
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
        }
        default:
            NotImplementError("Can't cast from " + source.ToString() + " to " + target.ToString());
    }
}


}
