//
// Created by jinhai on 23-9-3.
//

module;

import stl;
import infinity_assert;
import infinity_exception;

export module table_collection_type;

namespace infinity {

export enum TableCollectionType : i8 {
    kTableEntry,
    kCollectionEntry,
};

export inline String ToString(TableCollectionType type) {
    switch (type) {
        case TableCollectionType::kTableEntry: {
            return "Table";
        }
        case TableCollectionType::kCollectionEntry: {
            return "Collection";
        }
        default: {
            Error<ExecutorException>("Invalid base table type", __FILE_NAME__, __LINE__);
        }
    }
}

} // namespace infinity
