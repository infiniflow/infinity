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

#include "sql_parser.h"
#include "type/info/array_info.h"
#include "type/number/float16.h"

export module parser;

namespace infinity {

export using IrsFilter = irs::filter;

export using infinity::SQLParser;
export using infinity::ParserResult;

export using infinity::ColumnDef;
export using infinity::RowID;

// TimeUnit
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

export using infinity::EmbeddingDataType;

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
export using infinity::ParserException;


export using infinity::DDLType;
export using infinity::ConflictType;
export using infinity::ConstraintType;
export using infinity::KnnDistanceType;
export using infinity::TableRefType;
export using infinity::ExplainType;
export using infinity::FlushType;
export using infinity::OptimizeType;
export using infinity::IndexType;

export using infinity::ExtraDDLInfo;
export using infinity::CreateTableInfo;
export using infinity::CreateIndexInfo;
export using infinity::CreateViewInfo;
export using infinity::CreateCollectionInfo;
export using infinity::CreateSchemaInfo;

export using infinity::DropIndexInfo;
export using infinity::DropTableInfo;
export using infinity::DropCollectionInfo;
export using infinity::DropSchemaInfo;
export using infinity::DropViewInfo;
export using infinity::CommandInfo;
export using infinity::IndexInfo;

export using infinity::UseCmd;
export using infinity::CheckTable;
export using infinity::SetCmd;
export using infinity::ExportCmd;
export using infinity::CompactTable;

export using infinity::InitParameter;

export using infinity::BaseTableReference;
export using infinity::TableReference;
export using infinity::JoinReference;
export using infinity::CrossProductReference;
export using infinity::SubqueryReference;

export using infinity::ShowStmtType;
export using infinity::CopyFileType;
export using infinity::SetOperatorType;

//export using infinity::BaseStatement;
//export using infinity::CreateStatement;
//export using infinity::SelectStatement;
//export using infinity::UpdateStatement;
//export using infinity::DeleteStatement;
//export using infinity::InsertStatement;
//export using infinity::DropStatement;
//export using infinity::ShowStatement;
//export using infinity::CopyStatement;
//export using infinity::PrepareStatement;
//export using infinity::ExecuteStatement;
//export using infinity::FlushStatement;
//export using infinity::OptimizeStatement;
//export using infinity::AlterStatement;
//export using infinity::ExplainStatement;
//export using infinity::CommandStatement;

export using infinity::ParsedExprType;
export using infinity::OrderType;
export using infinity::LiteralType;
export using infinity::SubqueryType;
export using infinity::JoinType;
export using infinity::KnnDistanceType;
export using infinity::CommandType;
export using infinity::SetVarType;
export using infinity::ExportType;
export using infinity::SetScope;

export using infinity::ParsedExpr;
export using infinity::ColumnExpr;
export using infinity::ConstantExpr;
export using infinity::FunctionExpr;
export using infinity::KnnExpr;
export using infinity::MatchExpr;
export using infinity::QueryExpr;
export using infinity::FusionExpr;
export using infinity::SearchExpr;
export using infinity::BetweenExpr;
export using infinity::SubqueryExpr;
export using infinity::CaseExpr;
export using infinity::WhenThen;
export using infinity::CastExpr;
export using infinity::WithExpr;
export using infinity::UpdateExpr;
export using infinity::InExpr;
export using infinity::OrderByExpr;

export using infinity::ColumnDef;
export using infinity::TableConstraint;

export using infinity::ConflictTypeToStr;
export using infinity::EmbeddingType;
export using infinity::ConstrainTypeToString;
export using infinity::ToEmbeddingDataType;

}


