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