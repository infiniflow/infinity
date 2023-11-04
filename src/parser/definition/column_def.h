#pragma once

#include "definition/column_def.h"
#include "type/data_type.h"
#include "type/logical_type.h"

#include <string>
#include <unordered_set>

namespace infinity {

enum class TableElementType {
    kConstraint,
    kColumn,
};

enum class ConstraintType : char {
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
} // namespace infinity
