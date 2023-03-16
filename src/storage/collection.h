//
// Created by jinhai on 23-3-16.
//

#pragma once

#include "base_table.h"
#include "common/types/internal_types.h"

namespace infinity {

class Collection : public BaseTable {
public:
    explicit
    Collection(String collection_name) : BaseTable(BaseTableType::kCollection),
                                         collection_name_(std::move(collection_name))
                                         {}

    [[nodiscard]] inline String
    collection_name() const {
        return collection_name_;
    }

    [[nodiscard]] inline SizeT
    row_count() const {
        return row_count_;
    }

private:
    String collection_name_{};
    SizeT row_count_{0};

};

}
