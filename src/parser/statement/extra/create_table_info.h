//
// Created by jinhai on 23-2-25.
//

#pragma once

#include <utility>

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"
#include "parser/statement/select_statement.h"
#include "common/types/data_type.h"
#include "common/types/info/char_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/decimal64_info.h"
#include "common/types/info/blob_info.h"
#include "common/types/info/bitmap_info.h"
#include "common/types/info/embedding_info.h"

namespace infinity {

enum class TableElementType {
    kConstraint,
    kColumn,
};

enum class ConstraintType {
    kNull,
    kNotNull,
    kPrimaryKey,
    kUnique,
};

class TableElement {
public:
    explicit
    TableElement(TableElementType type): type_(type) {}

    virtual
    ~TableElement() = default;

    TableElementType type_;
};

class ColumnDef : public TableElement {
public:
    ColumnDef(i64 id,
              DataType column_type,
              String column_name,
              HashSet<ConstraintType> constraints)
              : TableElement(TableElementType::kColumn),
              id_(id),
              column_type_(std::move(column_type)),
              name_(std::move(column_name)),
              constraints_(std::move(constraints))
    {}

    ColumnDef(LogicalType logical_type, SharedPtr<TypeInfo> type_info_ptr)
            : TableElement(TableElementType::kColumn), column_type_(logical_type, std::move(type_info_ptr)) {}

    inline
    ~ColumnDef() override = default;

    String
    ToString() const;

    inline String&
    name() { return name_; }

    [[nodiscard]] inline i64
    id() const {
        return id_;
    }

    inline DataType&
    type() {
        return column_type_;
    }

public:
    i64 id_{-1};
    DataType column_type_;
    String name_{};
    HashSet<ConstraintType> constraints_{};
};

struct ColumnType {
//
//    explicit
//    ColumnType(LogicalType logical_type) : logical_type_(logical_type) {}

    LogicalType logical_type_;
    i64 width;
    i64 precision;
    i64 scale;
    EmbeddingDataType embedding_type_;
};

class TableConstraint : public TableElement {
public:
    TableConstraint() : TableElement(TableElementType::kConstraint) {}

    inline
    ~TableConstraint() override {
        delete names_ptr_;
        names_ptr_ = nullptr;
    }

    Vector<String>* names_ptr_{nullptr};
    ConstraintType constraint_{ConstraintType::kNotNull};
};

class CreateTableInfo final : public ExtraDDLInfo {
public:
    explicit
    CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    ~CreateTableInfo() final;

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{"Default"};
    String table_name_{};

    Vector<ColumnDef *> column_defs_{};
    Vector<TableConstraint *> constraints_{};

    SelectStatement* select_{nullptr};
};

}
