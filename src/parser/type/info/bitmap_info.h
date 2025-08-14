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

#include "type/type_info.h"
// #include <memory>'

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

class BitmapInfo : public TypeInfo {
public:
    static std::shared_ptr<BitmapInfo> Make(int64_t limit);

    static std::shared_ptr<BitmapInfo> Make();

    explicit BitmapInfo(size_t limit) : TypeInfo(TypeInfoType::kBitmap), length_limit_(limit) {}

    ~BitmapInfo() override = default;

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] size_t Size() const override { return length_limit_; }

    [[nodiscard]] size_t length_limit() const { return length_limit_; }

    [[nodiscard]] nlohmann::json Serialize() const override;

    [[nodiscard]] inline std::string ToString() const override { return "bitmap(" + std::to_string(length_limit_) + ")"; }

private:
    size_t length_limit_{0};
};

} // namespace infinity
