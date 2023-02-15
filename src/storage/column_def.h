//
// Created by jinhai on 22-12-25.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "common/types/data_type.h"

namespace infinity {

enum class ConstrainType {
    kInvalid,
    kPrimaryKey,
    kUnique,
    kNull,
    kNotNull,
};

class ColumnDef {
public:
    static SharedPtr<ColumnDef>
    Make(String name, i64 column_id, DataType data_type, Set<ConstrainType> constrains) {
        return MakeShared<ColumnDef>(std::move(name), column_id, std::move(data_type), std::move(constrains));
    }

public:
    explicit
    ColumnDef(String name, i64 column_id, DataType data_type, Set<ConstrainType> constrains)
        : name_(std::move(name)),
        id_(column_id),
        type_(std::move(data_type)),
        constrains_(std::move(constrains)) {}

    [[nodiscard]] String
    ToString() const;

    [[nodiscard]] String
    name() const {
        return name_;
    }

    [[nodiscard]] i64
    id() const {
        return id_;
    }

    [[nodiscard]] DataType
    type() const {
        return type_;
    }

    [[maybe_unused]] Set<ConstrainType>
    constrains() const {
        return constrains_;
    }

private:
    String name_;
    i64 id_{-1};
    DataType type_{LogicalType::kInvalid};
    Set<ConstrainType> constrains_;
};

}
