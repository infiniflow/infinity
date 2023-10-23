//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "base_table_reference.h"

namespace infinity {

class CrossProductReference : public BaseTableReference {

public:
    explicit CrossProductReference() : BaseTableReference(TableRefType::kCrossProduct) {}

    ~CrossProductReference() override;

    std::string ToString() override;

    std::vector<BaseTableReference *> tables_;
};

} // namespace infinity
