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

#include "query_driver.h"
#include "search_options.h"
#include "sql_parser.h"
#include "type/info/array_info.h"
#include "type/number/float16.h"

export module parser;

namespace infinity {

export using IrsFilter = irs::filter;
export using SearchOptions = SearchOptions;
export using QueryDriver = QueryDriver;

export using SQLParser = SQLParser;
export using ParserResult = ParserResult;

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
export using ArrayInfo = ArrayInfo;

export using TypeInfoType = TypeInfoType;

export template <typename T>
int32_t GetSizeInBytes(const T &value);

export template <>
int32_t GetSizeInBytes(const std::string &value);

export constexpr int64_t EMBEDDING_LIMIT = EMBEDDING_LIMIT_INTERNAL;
export constexpr int64_t MAX_BITMAP_SIZE = MAX_BITMAP_SIZE_INTERNAL;
export constexpr uint64_t VARCHAR_PREFIX_LEN = VARCHAR_PREFIX_LENGTH;
export constexpr uint64_t VARCHAR_INLINE_LEN = VARCHAR_INLINE_LENGTH;
export constexpr uint64_t VARCHAR_LEN_LIMIT = VARCHAR_LENGTH_LIMIT;

// Parser Exception
export using ParserException = ParserException;

export using StatementType = StatementType;
export using DDLType = DDLType;
export using ConflictType = ConflictType;
export using ConstraintType = ConstraintType;
export using KnnDistanceType = KnnDistanceType;
export using TableRefType = TableRefType;
export using ExplainType = ExplainType;
export using FlushType = FlushType;
export using OptimizeType = OptimizeType;
export using EmbeddingDataType = EmbeddingDataType;
export using IndexType = IndexType;

export using ExtraDDLInfo = ExtraDDLInfo;
export using CreateTableInfo = CreateTableInfo;
export using CreateIndexInfo = CreateIndexInfo;
export using CreateViewInfo = CreateViewInfo;
export using CreateCollectionInfo = CreateCollectionInfo;
export using CreateSchemaInfo = CreateSchemaInfo;

export using DropIndexInfo = DropIndexInfo;
export using DropTableInfo = DropTableInfo;
export using DropCollectionInfo = DropCollectionInfo;
export using DropSchemaInfo = DropSchemaInfo;
export using DropViewInfo = DropViewInfo;
export using CommandInfo = CommandInfo;
export using IndexInfo = IndexInfo;

export using UseCmd = UseCmd;
export using CheckTable = CheckTable;
export using SetCmd = SetCmd;
export using ExportCmd = ExportCmd;

export using InitParameter = InitParameter;

export using BaseTableReference = BaseTableReference;
export using TableReference = TableReference;
export using JoinReference = JoinReference;
export using CrossProductReference = CrossProductReference;
export using SubqueryReference = SubqueryReference;

export using ShowStmtType = ShowStmtType;
export using CopyFileType = CopyFileType;
export using SetOperatorType = SetOperatorType;

export using BaseStatement = BaseStatement;
export using CreateStatement = CreateStatement;
export using SelectStatement = SelectStatement;
export using UpdateStatement = UpdateStatement;
export using DeleteStatement = DeleteStatement;
export using InsertStatement = InsertStatement;
export using DropStatement = DropStatement;
export using ShowStatement = ShowStatement;
export using CopyStatement = CopyStatement;
export using PrepareStatement = PrepareStatement;
export using ExecuteStatement = ExecuteStatement;
export using FlushStatement = FlushStatement;
export using OptimizeStatement = OptimizeStatement;
export using AlterStatement = AlterStatement;
export using ExplainStatement = ExplainStatement;
export using CommandStatement = CommandStatement;

export using ParsedExprType = ParsedExprType;
export using OrderType = OrderType;
export using LiteralType = LiteralType;
export using SubqueryType = SubqueryType;
export using JoinType = JoinType;
export using KnnDistanceType = KnnDistanceType;
export using CommandType = CommandType;
export using SetVarType = SetVarType;
export using ExportType = ExportType;
export using SetScope = SetScope;

export using ParsedExpr = ParsedExpr;
export using ColumnExpr = ColumnExpr;
export using ConstantExpr = ConstantExpr;
export using FunctionExpr = FunctionExpr;
export using KnnExpr = KnnExpr;
export using MatchExpr = MatchExpr;
export using QueryExpr = QueryExpr;
export using FusionExpr = FusionExpr;
export using SearchExpr = SearchExpr;
export using BetweenExpr = BetweenExpr;
export using SubqueryExpr = SubqueryExpr;
export using CaseExpr = CaseExpr;
export using WhenThen = WhenThen;
export using CastExpr = CastExpr;
export using WithExpr = WithExpr;
export using UpdateExpr = UpdateExpr;
export using InExpr = InExpr;
export using OrderByExpr = OrderByExpr;

export using ColumnDef = ColumnDef;
export using TableConstraint = TableConstraint;

export inline std::string ConflictType2Str(ConflictType type) {
    return ConflictTypeToStr(type);
}

export inline std::string OrderBy2Str(OrderType type) {
    return ToString(type);
}

export inline std::string JoinType2Str(JoinType type) {
    return ToString(type);
}

export inline std::string ConstrainType2String(ConstraintType type) {
    return ConstrainTypeToString(type);
}

export inline std::string EmbeddingDataType2String(EmbeddingDataType type) {
    return EmbeddingType::EmbeddingDataType2String(type);
}

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


