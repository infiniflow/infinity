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
import tensor_cast;
import tensor_array_cast;
import sparse_cast;

import third_party;

module cast_function;

namespace infinity {

template <typename SourceType>
static BoundCastFunc NumericCast(const DataType &target) {
    switch (target.type()) {
        case kTinyInt: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kSmallInt: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kInteger: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kBigInt: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kHugeInt: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kFloat: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        case kDouble: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast to {}", target.ToString()));
        }
    }
}

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    if (source == target) {
        UnrecoverableError(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
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
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kVarchar: {
            return BindVarcharCast(source, target);
        }
        case kDate: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTime: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kDateTime: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTimestamp: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kInterval: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kArray: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kTuple: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kPoint: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLine: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kLineSeg: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kBox: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kPath: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
//        case kPolygon: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kCircle: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kBitmap: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kUuid: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
//        case kBlob: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kEmbedding: {
            return BindEmbeddingCast(source, target);
        }
        case kTensor: {
            return BindTensorCast(source, target);
        }
        case kTensorArray: {
            return BindTensorArrayCast(source, target);
        }
        case kSparse: {
            return BindSparseCast(source, target);
        }
        case kRowID: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kMixed: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        case kNull: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
        }
        default:
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
