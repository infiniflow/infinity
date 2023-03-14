//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "common/types/internal_types.h"
#include <string>

namespace infinity {

class SchemaDefinition {
public:
    SchemaDefinition(SharedPtr<String> schema_name, ConflictType conflict_type)
            : name_(std::move(schema_name)), conflict_type_(conflict_type)
            {}

    [[nodiscard]] const String&
    name() const {
        return *name_;
    }

    [[nodiscard]] ConflictType
    conflict_type() const {
        return conflict_type_;
    }

private:
    SharedPtr<String> name_;
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}
