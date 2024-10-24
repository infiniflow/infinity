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

export module reference_expression;

import stl;
import expression_type;
import base_expression;
import data_type;
import internal_types;

namespace infinity {

export class ReferenceExpression : public BaseExpression {
public:
    static inline SharedPtr<ReferenceExpression> Make(DataType data_type, String table_name, String column_name, String alias, SizeT column_idx) {
        return MakeShared<ReferenceExpression>(std::move(data_type), std::move(table_name), std::move(column_name), std::move(alias), column_idx);
    }

public:
    explicit ReferenceExpression(DataType data_type, String table_name, String column_name, String alias, SizeT column_idx)
        : BaseExpression(ExpressionType::kReference, {}, std::move(alias)), data_type_(std::move(data_type)), table_name_(std::move(table_name)),
          column_name_(std::move(column_name)), column_index_(column_idx) {}

    inline SizeT column_index() const { return column_index_; }

    inline DataType Type() const override { return data_type_; };

    String ToString() const override;

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

private:
    DataType data_type_;

    String table_name_{};

    String column_name_{};

    SizeT column_index_{};
};

} // namespace infinity
