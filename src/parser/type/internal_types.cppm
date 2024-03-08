//
// Created by jinhai on 24-2-4.
//

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
