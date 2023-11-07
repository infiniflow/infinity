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

import parser;
import stl;
import base_expression;

export module correlated_column_expression;

namespace infinity {

class CorrelatedColumnExpression : public BaseExpression {
public:
    CorrelatedColumnExpression(DataType data_type, String column_name);

    inline DataType Type() const override { return data_type_; }

    String ToString() const override;

private:
    DataType data_type_;
    String column_name_;
};

} // namespace infinity