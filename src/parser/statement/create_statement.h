//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/extra/create_schema_info.h"
#include "parser/statement/extra/create_collection_info.h"
#include "common/types/data_type.h"

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
    ~TableElement() = 0;

    TableElementType type_;
};

class ColumnDef : public TableElement {
public:
    ColumnDef(LogicalType logical_type, SharedPtr<TypeInfo> type_info_ptr)
        : TableElement(TableElementType::kColumn), column_type_(logical_type, type_info_ptr) {}

    DataType column_type_;
    String name_{};
    HashSet<ConstraintType> constraints_{};
};

class TableConstraint : public TableElement {
public:
    TableConstraint() : TableElement(TableElementType::kConstraint) {}
    Vector<String> names_{};
    ConstraintType constraint_{ConstraintType::kNotNull};
};

class CreateStatement : public BaseStatement {
public:
    CreateStatement() : BaseStatement(StatementType::kCreate) {}

    [[nodiscard]] String
    ToString() const final;

    UniquePtr<ExtraDDLInfo> create_info_{};

};

}
