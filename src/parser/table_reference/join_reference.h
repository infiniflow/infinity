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

#include "base_table_reference.h"

namespace infinity {

enum class JoinType {
    kInner,
    kFull,
    kLeft,
    kRight,
    kCross,
    kNatural,
    kSemi,
    kMark,
    kAnti,
};

std::string ToString(JoinType type);

class JoinReference : public BaseTableReference {

public:
    explicit JoinReference() : BaseTableReference(TableRefType::kJoin) {}

    ~JoinReference() override;

    std::string ToString() override;

    BaseTableReference *left_{nullptr};
    BaseTableReference *right_{nullptr};

    ParsedExpr *condition_{nullptr};
    JoinType join_type_{JoinType::kInner};

    std::vector<char *> *using_columns_{nullptr};
};

} // namespace infinity
