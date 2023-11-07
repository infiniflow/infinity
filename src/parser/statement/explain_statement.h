// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>

#include "base_statement.h"

namespace infinity {

enum class ExplainType { kAnalyze, kAst, kUnOpt, kOpt, kPhysical, kPipeline, kFragment };

class ExplainStatement final : public BaseStatement {
public:
    ExplainStatement() : BaseStatement(StatementType::kExplain) {}

    ~ExplainStatement() final;

    [[nodiscard]] std::string ToString() const final;

    ExplainType type_{ExplainType::kOpt};

    BaseStatement *statement_{nullptr};
};

} // namespace infinity
