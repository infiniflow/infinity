//
// Created by jinhai on 23-3-16.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

enum class BaseTableType {
    kInvalid,
    kTable,
    kCollection,
};

inline String
ToString(BaseTableType type) {
    switch (type) {
        case BaseTableType::kTable: {
            return "Table";
        }
        case BaseTableType::kCollection: {
            return "Collection";
        }
        default: {
            ExecutorError("Invalid base table type")
        }
    }
}

class BaseTable {
public:
    explicit
    BaseTable(BaseTableType kind) : kind_(kind) {}

    inline BaseTableType
    kind() const {
        return kind_;
    }

    BaseTableType kind_{BaseTableType::kInvalid};
};

}
