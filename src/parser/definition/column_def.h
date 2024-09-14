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

#pragma once

// #include "type/data_type.h"
#include "expr/constant_expr.h"
#include "expr/parsed_expr.h"
#include "type/logical_type.h"
#include "type/type_info.h"

#include <memory>
#include <string>
#include <set>
#include <vector>

namespace infinity {

class DataType;

enum class TableElementType {
    kConstraint,
    kColumn,
};

enum class ConstraintType : char {
    kNull,
    kNotNull,
    kPrimaryKey,
    kUnique,
    kInvalid,
};

std::string ConstrainTypeToString(ConstraintType type);
ConstraintType StringToConstraintType(const std::string &type);

class TableElement {
public:
    explicit TableElement(TableElementType type) : type_(type) {}
    virtual ~TableElement() = default;
    TableElementType type_{TableElementType::kColumn};
};

struct ColumnType {
    LogicalType logical_type_;
    int64_t width;
    int64_t precision;
    int64_t scale;
    EmbeddingDataType embedding_type_;
};

class TableConstraint : public TableElement {
public:
    TableConstraint() : TableElement(TableElementType::kConstraint) {}

    inline ~TableConstraint() override {
        delete names_ptr_;
        names_ptr_ = nullptr;
    }

    [[nodiscard]] std::string ToString() const;

    std::vector<std::string> *names_ptr_{nullptr};
    ConstraintType constraint_{ConstraintType::kNotNull};
};

class ColumnDef : public TableElement {
public:
    ColumnDef(int64_t id,
              std::shared_ptr<DataType> column_type,
              std::string column_name,
              std::set<ConstraintType> constraints,
              std::shared_ptr<ParsedExpr> default_expr = nullptr);

    ColumnDef(LogicalType logical_type, const std::shared_ptr<TypeInfo> &type_info_ptr, std::shared_ptr<ParsedExpr> default_expr = nullptr);

    inline ~ColumnDef() override = default;

    bool operator==(const ColumnDef &other) const;

    int32_t GetSizeInBytes() const;

    void WriteAdv(char *&ptr) const;

    static std::shared_ptr<ColumnDef> ReadAdv(const char *&ptr, int32_t maxbytes);

    std::string ToString() const;

    inline const std::string &name() const { return name_; }

    [[nodiscard]] inline int64_t id() const { return id_; }

    inline const std::shared_ptr<DataType> &type() const { return column_type_; }

    inline bool has_default_value() const {
        auto const_expr = std::dynamic_pointer_cast<ConstantExpr>(default_expr_);
        return const_expr != nullptr && const_expr->literal_type_ != LiteralType::kNull;
    }

    const std::shared_ptr<ConstantExpr> default_value() const {
        return std::dynamic_pointer_cast<ConstantExpr>(default_expr_);
    }

public:
    int64_t id_{-1};
    const std::shared_ptr<DataType> column_type_{};
    std::string name_{};
    std::set<ConstraintType> constraints_{};
    std::shared_ptr<ParsedExpr> default_expr_{nullptr};
    bool build_bloom_filter_{};
};
} // namespace infinity
