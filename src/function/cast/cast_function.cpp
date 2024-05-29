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

import third_party;

import infinity_exception;
import bound_cast_func;
import bool_cast;
import integer_cast;
import float_cast;
import embedding_cast;
import varchar_cast;
import tensor_cast;
import tensor_array_cast;
import logger;
import stl;

module cast_function;

namespace infinity {

template <typename SourceType>
static BoundCastFunc NumericCast(const DataType &target) {
    switch (target.type()) {
        case kTinyInt: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kSmallInt: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kInteger: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kBigInt: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kHugeInt: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kFloat: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kDouble: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        default: {
            String error_message = fmt::format("Can't cast to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
    }
}

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    if (source == target) {
        String error_message = fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kVarchar: {
            return BindVarcharCast(source, target);
        }
        case kDate: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kTime: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kDateTime: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kTimestamp: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kInterval: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kArray: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kTuple: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kPoint: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kLine: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kLineSeg: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kBox: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
//        case kPath: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
//        case kPolygon: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kCircle: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
//        case kBitmap: {
//            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
//        }
        case kUuid: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
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
        case kRowID: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kMixed: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case kNull: {
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        default:
            String error_message = fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
