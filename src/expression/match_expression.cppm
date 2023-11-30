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

module;

import stl;
import parser;
import base_expression;

export module match_expression;

namespace infinity {

export class MatchExpression : public BaseExpression {
public:
    MatchExpression(const String &fields, const String &matching_text, const String &options_text);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

public:
    String fields_;
    String matching_text_;
    String options_text_;
};

} // namespace infinity