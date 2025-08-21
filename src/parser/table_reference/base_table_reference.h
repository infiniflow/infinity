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

#include "expr/parsed_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

class TableAlias {
public:
    ~TableAlias();

    char *alias_{};
    std::vector<std::string> *column_alias_array_{nullptr};
};

enum class TableRefType {
    kCrossProduct,
    kJoin,
    kTable,
    kSubquery,
    kDummy,
};

class BaseTableReference {
public:
    explicit BaseTableReference(TableRefType type) : type_(type) {}

    virtual ~BaseTableReference();

    virtual std::string ToString() = 0;

    TableRefType type_{TableRefType::kTable};
    TableAlias *alias_{nullptr};
};

} // namespace infinity
