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
import multi_vector_cast;
import empty_array_cast;
import logger;
import stl;
import sparse_cast;
import third_party;
import logical_type;

module cast_function;

namespace infinity {

BoundCastFunc CastFunction::GetBoundFunc(const DataType &source, const DataType &target) {
    if (source == target) {
        UnrecoverableError(fmt::format("Attempt to cast from {} to {}", source.ToString(), target.ToString()));
    }

    switch (source.type()) {
        case LogicalType::kBoolean: {
            return BindBoolCast(source, target);
        }
        case LogicalType::kTinyInt: {
            return BindIntegerCast<TinyIntT>(source, target);
        }
        case LogicalType::kSmallInt: {
            return BindIntegerCast<SmallIntT>(source, target);
        }
        case LogicalType::kInteger: {
            return BindIntegerCast<IntegerT>(source, target);
        }
        case LogicalType::kBigInt: {
            return BindIntegerCast<BigIntT>(source, target);
        }
        case LogicalType::kHugeInt: {
            return BindIntegerCast<HugeIntT>(source, target);
        }
        case LogicalType::kFloat16: {
            return BindFloatCast<Float16T>(source, target);
        }
        case LogicalType::kBFloat16: {
            return BindFloatCast<BFloat16T>(source, target);
        }
        case LogicalType::kFloat: {
            return BindFloatCast<FloatT>(source, target);
        }
        case LogicalType::kDouble: {
            return BindFloatCast<DoubleT>(source, target);
        }
        case LogicalType::kVarchar: {
            return BindVarcharCast(source, target);
        }
        case LogicalType::kEmbedding: {
            return BindEmbeddingCast(source, target);
        }
        case LogicalType::kMultiVector: {
            return BindMultiVectorCast(source, target);
        }
        case LogicalType::kTensor: {
            return BindTensorCast(source, target);
        }
        case LogicalType::kTensorArray: {
            return BindTensorArrayCast(source, target);
        }
        case LogicalType::kSparse: {
            return BindSparseCast(source, target);
        }
        case LogicalType::kEmptyArray: {
            return BindEmptyArrayCast(source, target);
        }
        case LogicalType::kDecimal:
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kDateTime:
        case LogicalType::kTimestamp:
        case LogicalType::kInterval:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kRowID:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            UnrecoverableError(fmt::format("Can't cast from {} to {}", source.ToString(), target.ToString()));
            break;
        }
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
