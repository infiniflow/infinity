//
// Created by JinHai on 2022/12/14.
//


module;

import infinity_assert;
import infinity_exception;
import bound_cast_func;
import bool_cast;
import integer_cast;
import float_cast;
import varchar_cast;
import parser;
import third_party;

module cast_function;

namespace infinity {

template <typename SourceType>
static BoundCastFunc NumericCast(const DataType &target) {
    switch (target.type()) {
        case kTinyInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kSmallInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kInteger: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kBigInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kHugeInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kFloat: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kDouble: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
        default: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    Assert<TypeException>(source != target, Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);

    switch (source.type()) {
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
            return BindIntegerCast<IntegerT>(source, target);
        }
        case kBigInt: {
            return BindIntegerCast<BigIntT>(source, target);
        }
        case kHugeInt: {
            return BindIntegerCast<HugeIntT>(source, target);
        }
        case kFloat: {
            return BindFloatCast<FloatT>(source, target);
        }
        case kDouble: {
            return BindFloatCast<DoubleT>(source, target);
        }
        case kDecimal: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kVarchar: {
            return BindVarcharCast(source, target);
        }
        case kDate: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kTime: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kDateTime: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kTimestamp: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kInterval: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kArray: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kTuple: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kPoint: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kLine: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kLineSeg: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kBox: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kPath: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kPolygon: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kCircle: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kBitmap: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kUuid: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kBlob: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kEmbedding: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kRowID: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kMixed: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        case kNull: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
        }
        default:
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()), __FILE_NAME__, __LINE__);
    }
}

} // namespace infinity
