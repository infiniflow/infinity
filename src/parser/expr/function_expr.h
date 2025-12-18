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

#include "parsed_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

enum class ExtraInfoType { kInvalid, kJson };
class BaseExtraInfo;
class JsonExtraInfo;

class FunctionExpr : public ParsedExpr {
public:
    explicit FunctionExpr() : ParsedExpr(ParsedExprType::kFunction) {}

    ~FunctionExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    std::string func_name_{};
    std::vector<ParsedExpr *> *arguments_{nullptr};
    bool distinct_{false};

    std::shared_ptr<BaseExtraInfo> extra_info_{nullptr};
};

class BaseExtraInfo {
public:
    BaseExtraInfo() = default;
    explicit BaseExtraInfo(ExtraInfoType type) : type_(type) {}
    virtual ~BaseExtraInfo() = default;

    ExtraInfoType type_{ExtraInfoType::kInvalid};
};

class JsonExtraInfo : public BaseExtraInfo {
public:
    explicit JsonExtraInfo(char *json_extra_info) : BaseExtraInfo(ExtraInfoType::kJson), json_extra_info_(json_extra_info) {}
    ~JsonExtraInfo() override {
        if (json_extra_info_ != nullptr) {
            free(json_extra_info_);
            json_extra_info_ = nullptr;
        }
        json_tokens_.clear();
    }

    bool Init();

    char *json_extra_info_{nullptr};
    std::vector<std::string> json_tokens_{};
};

} // namespace infinity
