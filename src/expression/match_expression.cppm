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

export module infinity_core:match_expression;

import :base_expression;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

export class MatchExpression : public BaseExpression {
public:
    MatchExpression(const std::string &fields, const std::string &matching_text, const std::string &options_text);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    std::string ToString() const override;

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

public:
    std::string fields_;
    std::string matching_text_;
    std::string options_text_;
    std::shared_ptr<BaseExpression> optional_filter_;
};

} // namespace infinity
