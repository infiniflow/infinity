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

#include <string>

namespace infinity::client {

enum class ExtraColumnType {
    kEmbedding,
};

class ExtraTypeInfo {
public:
    explicit ExtraTypeInfo(ExtraColumnType type) : type_(ExtraColumnType::kEmbedding) {}

private:
    ExtraColumnType type_;
};

enum class EmbeddingElemType { kBit, kInt8, kInt16, kInt32, kInt64, kFloat32, kFloat64 };

class EmbeddingTypeInfo : public ExtraTypeInfo {
public:
    explicit EmbeddingTypeInfo(int64_t dimension, EmbeddingElemType elem_type);

private:
    EmbeddingElemType elem_type_;
    int64_t dimension_;
};

enum class ColumnType : int8_t {
    kBoolean = 0,
    kTinyInt = 1,
    kSmallInt = 2,
    kInteger = 3,
    kBigInt = 4,
    kHugeInt = 5,
    kFloat = 6,
    kDouble = 7,
    kVarchar = 8,
    kEmbedding = 9,
    kRowID = 10,
    kInvalid,
};

enum class Constraint {
    kPrimaryKey,
    kNotNull,
    kNull,
    kUnique,
};

class ColumnDefinition {
public:
    ColumnDefinition(const std::string& column_name, ColumnType column_type, std::unique_ptr<ExtraTypeInfo> extra_type_info, std::set<Constraint> constraints);

private:
    std::string column_name_;
    ColumnType column_type_;
    std::unique_ptr<ExtraTypeInfo> extra_type_info_;
    std::set<Constraint> constraints_;
};

class TableProperties {
public:
    TableProperties() = default;


private:
    std::map<std::string, std::string> properties_;
};

} // namespace infinity::client