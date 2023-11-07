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

import column_binding;
import base_expression;
import parser;
import stl;

export module between_expression;

namespace infinity {

export class BetweenExpression : public BaseExpression {
public:
    BetweenExpression(const SharedPtr<BaseExpression> &value,
                      const SharedPtr<BaseExpression> &upper_bound,
                      const SharedPtr<BaseExpression> &lower_bound,
                      bool upper_inclusive,
                      bool lower_inclusive);

    String ToString() const override;

    DataType Type() const override;

private:
    bool lower_inclusive_{false};
    bool upper_inclusive_{false};
};

} // namespace infinity
