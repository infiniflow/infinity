//
// Created by jinhai on 23-2-28.
//

#include "subquery_reference.h"

namespace infinity {

SubqueryReference::~SubqueryReference() {
    if (select_statement_ != nullptr) {
        delete select_statement_;
        select_statement_ = nullptr;
    }
}

String SubqueryReference::ToString() { return "SubqueryReference"; }

}; // namespace infinity
