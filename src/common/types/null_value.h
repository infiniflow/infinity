//
// Created by JinHai on 2022/12/16.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "common/types/data_type.h"

namespace infinity {

template <typename ValueType>
inline ValueType
NullValue() {
    TypeError(DataType::TypeToString<ValueType>() + " doesn't have null value.");
}

template <>
inline BooleanT
NullValue() {
    return false;
}

template <>
inline TinyIntT
NullValue() {
    return std::numeric_limits<TinyIntT>::infinity();
}

template <>
inline SmallIntT
NullValue() {
    return std::numeric_limits<SmallIntT>::infinity();
}

template <>
inline IntegerT
NullValue() {
    return std::numeric_limits<IntegerT>::infinity();
}

template <>
inline BigIntT
NullValue() {
    return std::numeric_limits<BigIntT>::infinity();
}

template <>
inline HugeIntT
NullValue() {
    HugeIntT value;
    value.lower = std::numeric_limits<i64>::infinity();
    value.upper = std::numeric_limits<i64>::infinity();
    return value;
}

template <>
inline FloatT
NullValue() {
    return std::numeric_limits<f32>::infinity();
}

template <>
inline DoubleT
NullValue() {
    return std::numeric_limits<f64>::infinity();
}

template <>
inline DecimalT
NullValue() {
    return DecimalT(std::numeric_limits<i64>::infinity(), std::numeric_limits<i64>::infinity());
}

template <>
inline VarcharT
NullValue() {
    return VarcharT(nullptr, 0);
}

template <>
inline DateT
NullValue() {
    return DateT(std::numeric_limits<i32>::infinity());
}

template <>
inline TimeT
NullValue() {
    return TimeT(std::numeric_limits<i32>::infinity());
}

template <>
inline DateTimeT
NullValue() {
    return DateTimeT(std::numeric_limits<i32>::infinity(), std::numeric_limits<i32>::infinity());
}

template <>
inline TimestampT
NullValue() {
    return TimestampT(std::numeric_limits<i32>::infinity(), std::numeric_limits<i32>::infinity());
}

template <>
inline IntervalT
NullValue() {
    IntervalT interval;
    interval.value = std::numeric_limits<i32>::infinity();
    interval.unit = TimeUnit::kInvalidUnit;
    return interval;
}

template <>
inline PointT
NullValue() {
    return PointT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
}

template <>
inline LineT
NullValue() {
    return LineT(std::numeric_limits<f64>::infinity(),
                 std::numeric_limits<f64>::infinity(),
                 std::numeric_limits<f64>::infinity());
}

template <>
inline LineSegT
NullValue() {
    PointT p1(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    PointT p2(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    return LineSegT(p1, p2);
}

template <>
inline BoxT
NullValue() {
    PointT p1(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    PointT p2(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity());
    return BoxT(p1, p2);
}

template <>
inline PathT
NullValue() {
    PathType path(std::numeric_limits<u32>::infinity(), std::numeric_limits<i32>::infinity());
    path.ptr = std::numeric_limits<ptr_t>::infinity();
    return path;
}

template <>
inline PolygonT
NullValue() {
    ptr_t ptr {};
    u64 point_count {0}; // 65535 point are the max point count
    BoxType bounding_box {};
    PolygonT polygon;
    polygon.ptr = std::numeric_limits<ptr_t>::infinity();
    polygon.point_count = std::numeric_limits<u64>::infinity();
    PointT null_point = PointT(std::numeric_limits<f64>::infinity(),
                               std::numeric_limits<f64>::infinity());
    polygon.bounding_box.upper_left.x = std::numeric_limits<f64>::infinity();
    polygon.bounding_box.upper_left.y = std::numeric_limits<f64>::infinity();
    polygon.bounding_box.lower_right.x = std::numeric_limits<f64>::infinity();
    polygon.bounding_box.lower_right.y = std::numeric_limits<f64>::infinity();
    return polygon;
}

template <>
inline CircleT
NullValue() {
    return CircleT(PointT(std::numeric_limits<f64>::infinity(), std::numeric_limits<f64>::infinity()),
                   std::numeric_limits<f64>::infinity());
}

template <>
inline BitmapT
NullValue() {
    BitmapT bitmap;
    bitmap.ptr = std::numeric_limits<u64*>::infinity();
    bitmap.count = std::numeric_limits<u64>::infinity();
    return bitmap;
}

template <>
inline UuidT
NullValue() {
    UuidT uuid;
    return uuid;
}

template <>
inline BlobT
NullValue() {
    BlobT blob;
    blob.size = std::numeric_limits<u64>::infinity();
    blob.ptr = std::numeric_limits<ptr_t>::infinity();
    return blob;
}

template <>
inline EmbeddingT
NullValue() {
    ptr_t ptr = std::numeric_limits<ptr_t>::infinity();
    EmbeddingT embedding(std::move(ptr));
    return embedding;
}

template <>
inline MixedT
NullValue() {
    auto null_value = MixedT::MakeNull();
    return null_value;
}

}