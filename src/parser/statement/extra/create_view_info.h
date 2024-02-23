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

#include "extra_ddl_info.h"
#include "statement/select_statement.h"

namespace infinity {

class CreateViewInfo final : public SchemaDDLInfo {
public:
    explicit CreateViewInfo() : SchemaDDLInfo(DDLType::kView) {}

    ~CreateViewInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string view_name_{};

    std::vector<std::string> *view_columns_{};
    SelectStatement *select_{nullptr};
};

} // namespace infinity
