//
// Created by jinhai on 23-2-22.
//

#pragma once

#include <string>

#include "base_statement.h"

namespace infinity {

enum class ExplainType { kAnalyze, kAst, kUnOpt, kOpt, kPhysical, kPipeline };

class ExplainStatement final : public BaseStatement {
public:
    ExplainStatement() : BaseStatement(StatementType::kExplain) {}

    ~ExplainStatement() final;

    [[nodiscard]] std::string ToString() const final;

    ExplainType type_{ExplainType::kOpt};

    BaseStatement *statement_{nullptr};
};

} // namespace infinity
