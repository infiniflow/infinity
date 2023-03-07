//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "base_table_reference.h"

namespace infinity {

class CrossProductReference: public BaseTableReference {

public:

    explicit
    CrossProductReference() : BaseTableReference(TableRefType::kCrossProduct) {}
    ~CrossProductReference() override;

    String
    ToString() override;

    Vector<BaseTableReference*> tables_;

};

}

