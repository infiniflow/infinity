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

export module fusion_expression;

import stl;
import base_expression;
import match_tensor_expression;
import data_type;
import logical_type;
import internal_types;
import search_options;

namespace infinity {

export class FusionExpression : public BaseExpression {
public:
    FusionExpression(const String &method, SharedPtr<SearchOptions> options);

    [[nodiscard]] inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

public:
    String method_{};
    SharedPtr<SearchOptions> options_{};
    SharedPtr<MatchTensorExpression> match_tensor_expr_;
};

} // namespace infinity