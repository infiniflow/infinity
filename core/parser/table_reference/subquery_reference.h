//
// Created by jinhai on 23-2-28.
//

#include "base_table_reference.h"
#include "statement/select_statement.h"

namespace infinity {

class SelectStatement;

class SubqueryReference : public BaseTableReference {

public:
    SubqueryReference() : BaseTableReference(TableRefType::kSubquery) {}

    ~SubqueryReference() override;

    std::string ToString() override;

    SelectStatement *select_statement_{nullptr};
};

} // namespace infinity
