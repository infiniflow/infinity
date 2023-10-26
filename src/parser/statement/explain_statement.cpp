//
// Created by jinhai on 23-2-22.
//

#include "explain_statement.h"

namespace infinity {

ExplainStatement::~ExplainStatement() {
    if (statement_ != nullptr) {
        delete statement_;
        statement_ = nullptr;
    }
}

std::string ExplainStatement::ToString() const { return "explain statement"; }

} // namespace infinity