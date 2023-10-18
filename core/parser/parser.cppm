//
// Created by jinhai on 23-10-17.
//
module;

#include "statement/extra/create_table_info.h"
#include "type/logical_type.h"
#include "type/number/float16.h"

export module parser;

namespace infinity {

export using LogicalType = LogicalType;
export using ColumnDef = ColumnDef;
export using DataType = DataType;
export using RowID = RowID;

// Bool
export using BooleanT = BooleanT;

// Numeric
export using TinyIntT = TinyIntT;
export using SmallIntT = SmallIntT;
export using IntegerT = IntegerT;
export using BigIntT = BigIntT;
export using HugeIntT = HugeIntT;

export using FloatT = FloatT;
export using DoubleT = DoubleT;

export using DecimalT = DecimalT;

// std::string
export using VarcharT = VarcharType;

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
export using PathT = PathType;
export using PolygonT = PolygonType;
export using CircleT = CircleType;

// Other
export using BitmapT = BitmapType;
export using UuidT = UuidType;
export using BlobT = BlobType;
export using EmbeddingT = EmbeddingType;
export using RowT = RowID;

// Heterogeneous
export using MixedT = MixedType;

// TimeUnit
export using TimeUnit = TimeUnit;

export using float16_t = float16_t;

export using IntegerMixedType = IntegerMixedType;
export using FloatMixedType = FloatMixedType;
export using ArrayMixedType = ArrayMixedType;
export using BaseMixedType = BaseMixedType;
export using LongStrMixedType = LongStrMixedType;
export using MissingMixedType = MissingMixedType;
export using ShortStrMixedType = ShortStrMixedType;
export using MixedType = MixedType;
export using MixedValueType = MixedValueType;
export using TupleMixedType = TupleMixedType;

export using TypeInfo = TypeInfo;
export using EmbeddingDataType = EmbeddingDataType;
export using EmbeddingInfo = EmbeddingInfo;
export using DecimalInfo = DecimalInfo;
export using BitmapInfo = BitmapInfo;
export using VarcharInfo = VarcharInfo;

export using TypeInfoType = TypeInfoType;

export template <typename T>
int32_t GetSizeInBytes(const T &value);

export template <>
int32_t GetSizeInBytes(const std::string &value);

export constexpr int64_t MAX_VARCHAR_SIZE = MAX_VARCHAR_SIZE_INTERNAL;
export constexpr int64_t EMBEDDING_LIMIT = EMBEDDING_LIMIT_INTERNAL;
export constexpr int64_t MAX_BITMAP_SIZE = MAX_BITMAP_SIZE_INTERNAL;

// Parser Exception
export using ParserException = ParserException;

//export template <typename T>
//T ReadBuf(char *const buf);
//
//export template <typename T>
//T ReadBufAdv(char *&buf);
//
//export template <>
//std::string ReadBuf<std::string>(char *const buf);
//
//export template <>
//std::string ReadBufAdv<std::string>(char *&buf);
//
//export template <typename T>
//void WriteBuf(char *const buf, const T &value);
//
//export template <typename T>
//void WriteBufAdv(char *&buf, const T &value);
//
//export template <>
//void WriteBuf<std::string>(char *const buf, const std::string &value);
//
//template <>
//void WriteBufAdv<std::string>(char *&buf, const std::string &value);

}


