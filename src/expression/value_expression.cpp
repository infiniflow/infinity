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
import expression_type;

module value_expression;

namespace infinity {

String ValueExpression::ToString() const { return value_.ToString(); }

u64 ValueExpression::Hash() const { return 0; }

bool ValueExpression::Eq(const BaseExpression &other_base) const { 
    if (other_base.type() != ExpressionType::kValue) {
        return false;
    }
    const auto &other = static_cast<const ValueExpression &>(other_base);
    return value_ == other.value_; }

} // namespace infinity
