//
// Created by jinhai on 23-9-3.
//
#pragma once

#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

enum TableCollectionType : i8 {
    kTableEntry,
    kCollectionEntry,
};

inline String
ToString(TableCollectionType type) {
    switch(type) {
        case TableCollectionType::kTableEntry: {
            return "Table";
        }
        case TableCollectionType::kCollectionEntry: {
            return "Collection";
        }
        default: {
            ExecutorError("Invalid base table type")
        }
    }
}

}