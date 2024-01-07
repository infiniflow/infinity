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
import embedding_cast;
import varchar_cast;
import parser;
import third_party;

module cast_function;

namespace infinity {

template <typename SourceType>
static BoundCastFunc NumericCast(const DataType &target) {
    switch (target.type()) {
        case kTinyInt: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kSmallInt: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kInteger: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kBigInt: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kHugeInt: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kFloat: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kDouble: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
        default: {
            Error<NotImplementException>(fmt::format("Can't cast to {}", target.ToString()));
        }
    }
}

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    if (source == target) {
        Error<TypeException>(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
    }

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
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kVarchar: {
            return BindVarcharCast(source, target);
        }
        case kDate: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTime: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kDateTime: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTimestamp: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kInterval: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kArray: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTuple: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kPoint: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLine: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLineSeg: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kBox: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kPath: {
//            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
//        case kPolygon: {
//            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kCircle: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kBitmap: {
//            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kUuid: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kBlob: {
//            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kEmbedding: {
            return BindEmbeddingCast(source, target);
        }
        case kRowID: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kMixed: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kNull: {
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        default:
            Error<NotImplementException>(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
