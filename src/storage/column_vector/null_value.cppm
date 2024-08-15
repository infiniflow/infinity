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

#include <string>

export module null_value;


import stl;
import infinity_exception;
import internal_types;
import data_type;

export namespace infinity {

template <typename ValueType>
inline ValueType NullValue() {
    UnrecoverableError(DataType::TypeToString<ValueType>() + " doesn't have null value.");
    return ValueType();
}

template <>
inline BooleanT NullValue() {
    return false;
}

template <>
inline u8 NullValue() {
    return std::numeric_limits<u8>::infinity();
}

template <>
inline TinyIntT NullValue() {
    return std::numeric_limits<i8>::infinity();
}

template <>
inline SmallIntT NullValue() {
    return std::numeric_limits<i16>::infinity();
}

template <>
inline IntegerT NullValue() {
    return std::numeric_limits<i32>::infinity();
}

template <>
inline BigIntT NullValue() {
    return std::numeric_limits<i64>::infinity();
}

template <>
inline HugeIntT NullValue() {
    HugeIntT value;
    value.lower = std::numeric_limits<i64>::infinity();
    value.upper = std::numeric_limits<i64>::infinity();
    return value;
}

template <>
inline Float16T NullValue() {
    return std::numeric_limits<Float16T>::infinity();
}

template <>
inline BFloat16T NullValue() {
    return std::numeric_limits<BFloat16T>::infinity();
}

template <>
inline FloatT NullValue() {
    return std::numeric_limits<f32>::infinity();
}

template <>
inline DoubleT NullValue() {
    return std::numeric_limits<f64>::infinity();
}

template <>
inline DecimalT NullValue() {
    return DecimalT(std::numeric_limits<i64>::infinity(), std::numeric_limits<i64>::infinity());
}

template <>
inline VarcharT NullValue() {
    return VarcharT();
}

template <>
inline DateT NullValue() {
    return DateT(std::numeric_limits<i32>::infinity());
}

template <>
inline TimeT NullValue() {
    return TimeT(std::numeric_limits<i32>::infinity());
}

template <>
inline DateTimeT NullValue() {
    return DateTimeT(std::numeric_limits<i32>::infinity(), std::numeric_limits<i32>::infinity());
}

template <>
inline TimestampT NullValue() {
    return TimestampT(std::numeric_limits<i32>::infinity(), std::numeric_limits<i32>::infinity());
}

template <>
inline IntervalT NullValue() {
    IntervalT interval;
    interval.value = std::numeric_limits<i32>::infinity();
    interval.unit = TimeUnit::kInvalidUnit;
    return interval;
}

template <>
inline PointT NullValue() {
    return PointT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
}

template <>
inline LineT NullValue() {
    return LineT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
}

template <>
inline LineSegT NullValue() {
    PointT p1(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    PointT p2(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    return LineSegT(p1, p2);
}

template <>
inline BoxT NullValue() {
    PointT p1(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    PointT p2(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    return BoxT(p1, p2);
}

//template <>
//inline PathT NullValue() {
//    PathT path(std::numeric_limits<u32>::infinity(), std::numeric_limits<i32>::infinity());
//    path.ptr = std::numeric_limits<ptr_t>::infinity();
//    return path;
//}
//
//template <>
//inline PolygonT NullValue() {
//    PolygonT polygon;
//    polygon.ptr = std::numeric_limits<ptr_t>::infinity();
//    polygon.point_count = u64_inf;
//    PointT null_point = PointT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
//    polygon.bounding_box.upper_left.x = std::numeric_limits<f64>::infinity();
//    polygon.bounding_box.upper_left.y = std::numeric_limits<f64>::infinity();
//    polygon.bounding_box.lower_right.x = std::numeric_limits<f64>::infinity();
//    polygon.bounding_box.lower_right.y = std::numeric_limits<f64>::infinity();
//    return polygon;
//}

template <>
inline CircleT NullValue() {
    return CircleT(PointT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity()), std::numeric_limits<f64>::infinity());
}

//template <>
//inline BitmapT NullValue() {
//    BitmapT bitmap;
//    bitmap.ptr = u64_ptr_inf;
//    bitmap.count = u64_inf;
//    return bitmap;
//}

template <>
inline UuidT NullValue() {
    UuidT uuid;
    return uuid;
}

//template <>
//inline BlobT NullValue() {
//    BlobT blob;
//    blob.size = u64_inf;
//    blob.ptr = std::numeric_limits<ptr_t>::infinity();
//    return blob;
//}

template <>
inline EmbeddingT NullValue() {
    ptr_t ptr = std::numeric_limits<ptr_t>::infinity();
    EmbeddingT embedding(std::move(ptr), false);
    return embedding;
}

template <>
inline RowID NullValue() {
    RowID row_id;
    row_id.segment_id_ = std::numeric_limits<u32>::infinity();
    row_id.segment_offset_ = std::numeric_limits<u32>::infinity();
    return row_id;
}

template <>
inline MixedT NullValue() {
    auto null_value = MixedT::MakeNull();
    return null_value;
}

template <typename ElemType>
inline void SetEmbeddingNullValue(ElemType *ptr, SizeT dim) {
    for (SizeT i = 0; i < dim; ++i) {
        ptr[i] = NullValue<ElemType>();
    }
}

} // namespace infinity