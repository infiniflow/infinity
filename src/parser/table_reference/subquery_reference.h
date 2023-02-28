//
// Created by jinhai on 23-2-28.
//

#include "base_table_reference.h"
#include "parser/statement/select_statement.h"

namespace infinity {

class SubqueryReference: public BaseTableReference {

public:

    SubqueryReference() : BaseTableReference(TableRefType::kSubquery) {}
    ~SubqueryReference() override;

    String
    ToString() override;

    UniquePtr<SelectStatement> select_statement_{nullptr};
    Vector<char*>* column_alias_array_{nullptr};
};

}

