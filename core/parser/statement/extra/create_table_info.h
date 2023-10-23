//
// Created by jinhai on 23-2-25.
//

#pragma once

#include <utility>

#include "type/data_type.h"
#include "type/info/bitmap_info.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include "type/info/varchar_info.h"
#include "extra_ddl_info.h"
#include "statement/select_statement.h"
#include "statement/statement_common.h"
#include <unordered_set>

namespace infinity {

enum class TableElementType {
    kConstraint,
    kColumn,
};

enum class ConstraintType : int8_t {
    kNull,
    kNotNull,
    kPrimaryKey,
    kUnique,
};

std::string ConstrainTypeToString(ConstraintType type);

class TableElement {
public:
    explicit TableElement(TableElementType type) : type_(type) {}

    virtual ~TableElement() = default;

    TableElementType type_;
};

class ColumnDef : public TableElement {
public:
    ColumnDef(int64_t id, std::shared_ptr<DataType> column_type, std::string column_name, std::unordered_set<ConstraintType> constraints)
        : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
          constraints_(std::move(constraints)) {}

    ColumnDef(LogicalType logical_type, const std::shared_ptr<TypeInfo> &type_info_ptr)
        : TableElement(TableElementType::kColumn), column_type_(std::make_shared<DataType>(logical_type, std::move(type_info_ptr))) {}

    inline ~ColumnDef() override = default;

    std::string ToString() const;

    inline std::string &name() { return name_; }

    [[nodiscard]] inline int64_t id() const { return id_; }

    inline std::shared_ptr<DataType> &type() { return column_type_; }

public:
    int64_t id_{-1};
    std::shared_ptr<DataType> column_type_;
    std::string name_{};
    std::unordered_set<ConstraintType> constraints_{};
};

struct ColumnType {
    //
    //    explicit
    //    ColumnType(LogicalType logical_type) : logical_type_(logical_type) {}

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

class CreateTableInfo final : public ExtraDDLInfo {
public:
    explicit CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    ~CreateTableInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string table_name_{};

    std::vector<ColumnDef *> column_defs_{};
    std::vector<TableConstraint *> constraints_{};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
