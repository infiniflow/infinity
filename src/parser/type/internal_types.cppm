// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include "internal_types.h"

export module internal_types;

namespace infinity {

export using infinity::BooleanT;

export using infinity::TinyIntT;
export using infinity::SmallIntT;
export using infinity::IntegerT;
export using infinity::BigIntT;
export using infinity::HugeIntT;

export using infinity::DoubleT;
export using infinity::FloatT;

export using infinity::DecimalT;

export using infinity::VarcharT;
export using infinity::DateT;

export using infinity::TimeT;
export using infinity::DateTimeT;
export using infinity::TimestampT;
export using infinity::IntervalT;

// Nest types
export using infinity::ArrayT;
export using infinity::TupleT;

// Geography
export using infinity::PointT;
export using infinity::LineT;
export using infinity::LineSegT;
export using infinity::BoxT;
export using infinity::CircleT;

// Other
export using infinity::UuidT;
export using infinity::EmbeddingT;

// Heterogeneous
export using infinity::MixedT;

// tensor
export using infinity::TensorT;
// tensor-array
export using infinity::TensorArrayT;

// sparse
export using infinity::SparseT;
export using infinity::SparseRefT;

export using infinity::EmptyArrayT;

export using infinity::RowID;
export using infinity::TimeUnit;

export using infinity::IntegerMixedType;
export using infinity::FloatMixedType;
export using infinity::ArrayMixedType;
export using infinity::BaseMixedType;
export using infinity::LongStrMixedType;
export using infinity::MissingMixedType;
export using infinity::ShortStrMixedType;
export using infinity::MixedType;
export using infinity::MixedValueType;
export using infinity::TupleMixedType;

export using infinity::EmbeddingType;
export using infinity::ToEmbeddingDataType;

export constexpr uint64_t VARCHAR_PREFIX_LEN = VARCHAR_PREFIX_LENGTH;
export constexpr uint64_t VARCHAR_INLINE_LEN = VARCHAR_INLINE_LENGTH;
export constexpr uint64_t VARCHAR_LEN_LIMIT = VARCHAR_LENGTH_LIMIT;

}
