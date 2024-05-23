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

#pragma once


#include "type/number/huge_int.h"
#include "type/number/decimal_type.h"
#include "type/datetime/date_type.h"
#include "type/datetime/time_type.h"
#include "type/datetime/datetime_type.h"
#include "type/datetime/interval_type.h"
#include "type/geo/point_type.h"
#include "type/geo/line_type.h"
#include "type/geo/line_segment_type.h"
#include "type/geo/box_type.h"
#include "type/geo/circle_type.h"
#include "type/complex/embedding_type.h"
#include "type/complex/tensor_type.h"
#include "type/complex/tensor_array_type.h"
#include "type/complex/row_id.h"
#include "type/complex/varchar.h"
#include "type/complex/uuid_type.h"

#include <vector>

namespace infinity {

struct Value;

// Bool
using BooleanT = bool;

// Numeric
using TinyIntT = int8_t;
using SmallIntT = int16_t;
using IntegerT = int32_t;
using BigIntT = int64_t;
using HugeIntT = HugeInt;

using FloatT = float;
using DoubleT = double;

using DecimalT = DecimalType;

// std::string
//using VarcharT = VarcharType;
using VarcharT = Varchar;

// Date and Time
using DateT = DateType;
using TimeT = TimeType;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
using IntervalT = IntervalType;

// Nest types
using ArrayT = std::vector<Value>;
using TupleT = std::vector<Value>;
// using ArrayT = ArrayType;
// using TupleT = TupleType;

// Geography
using PointT = PointType;
using LineT = LineType;
using LineSegT = LineSegmentType;
using BoxT = BoxType;
//using PathT = PathType;
//using PolygonT = PolygonType;
using CircleT = CircleType;

// Other
//using BitmapT = BitmapType;
using UuidT = UuidType;
//using BlobT = BlobType;
using EmbeddingT = EmbeddingType;

// Heterogeneous
using MixedT = MixedType;

// tensor
using TensorT = TensorType;

// tensor-array
using TensorArrayT = TensorArrayType;

}
