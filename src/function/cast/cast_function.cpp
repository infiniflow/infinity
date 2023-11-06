// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

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
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kSmallInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kInteger: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kBigInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kHugeInt: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kFloat: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        case kDouble: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
        default: {
            Error<NotImplementException>(Format("Can't cast to {}", target.ToString()));
        }
    }
}

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    Assert<TypeException>(source != target, Format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));

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
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kVarchar: {
            return BindVarcharCast(source, target);
        }
        case kDate: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTime: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kDateTime: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTimestamp: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kInterval: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kArray: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTuple: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kPoint: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLine: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLineSeg: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kBox: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kPath: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kPolygon: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kCircle: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kBitmap: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kUuid: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kBlob: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kEmbedding: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kRowID: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kMixed: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kNull: {
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        default:
            Error<NotImplementException>(Format("Can't cast from {} to {}", source.ToString(), target.ToString()));
    }
}

} // namespace infinity
