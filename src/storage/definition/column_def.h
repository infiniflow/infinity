#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/strings.h"
#include "common/types/data_type.h"
#include "common/types/logical_type.h"

namespace infinity {

enum class TableElementType {
    kConstraint,
    kColumn,
};

enum class ConstraintType : i8 {
    kNull,
    kNotNull,
    kPrimaryKey,
    kUnique,
};

String ConstrainTypeToString(ConstraintType type);

class TableElement {
public:
    explicit TableElement(TableElementType type) : type_(type) {}

    virtual ~TableElement() = default;

    TableElementType type_;
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

    inline ~TableConstraint() override {
        delete names_ptr_;
        names_ptr_ = nullptr;
    }

    [[nodiscard]] String ToString() const;

    Vector<String> *names_ptr_{nullptr};
    ConstraintType constraint_{ConstraintType::kNotNull};
};

class ColumnDef : public TableElement {
public:
    ColumnDef(i64 id, SharedPtr<DataType> column_type, String column_name, HashSet<ConstraintType> constraints)
        : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
          constraints_(std::move(constraints)) {}

    ColumnDef(LogicalType logical_type, const SharedPtr<TypeInfo> &type_info_ptr)
        : TableElement(TableElementType::kColumn), column_type_(MakeShared<DataType>(logical_type, std::move(type_info_ptr))) {}

    inline ~ColumnDef() override = default;

    String ToString() const;

    inline String &name() { return name_; }

    [[nodiscard]] inline i64 id() const { return id_; }

    inline SharedPtr<DataType> &type() { return column_type_; }

public:
    i64 id_{-1};
    SharedPtr<DataType> column_type_;
    String name_{};
    HashSet<ConstraintType> constraints_{};
};
} // namespace infinity
