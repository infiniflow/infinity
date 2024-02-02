//
// Created by jinhai on 24-2-1.
//

module;

#include "data_type.h"

export module data_type;

namespace infinity {

// Bool
export using infinity::BooleanT;

// Numeric
export using infinity::TinyIntT;
export using infinity::SmallIntT;
export using infinity::IntegerT;
export using infinity::BigIntT;
export using infinity::HugeIntT;

export using infinity::FloatT;
export using infinity::DoubleT;

export using infinity::DecimalT;

// std::string
export using VarcharT = Varchar;

// Date and Time
export using DateT = DateType;
export using TimeT = TimeType;
export using DateTimeT = DateTimeType;
export using TimestampT = TimestampType;
export using IntervalT = IntervalType;

// Nest types
export using ArrayT = std::vector<Value>;
export using TupleT = std::vector<Value>;
// using ArrayT = ArrayType;
// using TupleT = TupleType;

// Geography
export using PointT = PointType;
export using LineT = LineType;
export using LineSegT = LineSegmentType;
export using BoxT = BoxType;
//export using PathT = PathType;
//export using PolygonT = PolygonType;
export using CircleT = CircleType;

// Other
//export using BitmapT = BitmapType;
export using UuidT = UuidType;
//export using BlobT = BlobType;
export using EmbeddingT = EmbeddingType;
export using RowID = RowID;

// Heterogeneous
export using MixedT = MixedType;

export using infinity::DataType;

}