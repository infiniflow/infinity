//
// Created by jinhai on 23-2-28.
//

#include "cross_product_reference.h"

namespace infinity {

CrossProductReference::~CrossProductReference() {
    for (auto *table_ptr : tables_) {
        delete (table_ptr);
    }
}

std::string CrossProductReference::ToString() { return "CrossProductReference"; }

}; // namespace infinity
