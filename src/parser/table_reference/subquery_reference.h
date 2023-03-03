//
// Created by jinhai on 23-2-28.
//

#include "base_table_reference.h"
#include "parser/statement/select_statement.h"

namespace infinity {

class SelectStatement;

class SubqueryReference: public BaseTableReference {

public:

    SubqueryReference() : BaseTableReference(TableRefType::kSubquery) {}
    ~SubqueryReference() override;

    String
    ToString() override;

    SelectStatement* select_statement_{nullptr};
};

}

