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

#include "base_statement.h"
#include "statement/extra/create_collection_info.h"
#include "statement/extra/create_index_info.h"
#include "statement/extra/create_schema_info.h"
#include "statement/extra/create_table_info.h"
#include "statement/extra/create_view_info.h"

// #include <memory>
// #include <string>

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

class CreateStatement : public BaseStatement {
public:
    CreateStatement() : BaseStatement(StatementType::kCreate) {}

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] inline DDLType ddl_type() const { return create_info_->type_; }

    std::shared_ptr<ExtraDDLInfo> create_info_{};
};

} // namespace infinity
