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

export module column_expression;

import column_binding;
import base_expression;
import data_type;
import special_function;
import stl;
import internal_types;

namespace infinity {

export class ColumnExpression final : public BaseExpression {
public:
    static inline SharedPtr<ColumnExpression> Make(DataType data_type,
                                                   String table_name,
                                                   u64 table_index,
                                                   String column_name,
                                                   i64 column_index,
                                                   i64 depth,
                                                   Optional<SpecialType> special = None) {
        return MakeShared<ColumnExpression>(std::move(data_type), std::move(table_name), table_index, std::move(column_name), column_index, depth, special);
    }

public:
    ColumnExpression(DataType data_type,
                     String table_name,
                     u64 table_index,
                     String column_name,
                     i64 column_index,
                     i64 depth,
                     Optional<SpecialType> special = None);

    inline DataType Type() const override { return data_type_; };

    const String &table_name() const { return table_name_; }

    //    inline u64
    //    table_index() const {
    //        return binding_.table_idx;
    //    }

    const String &column_name() const { return column_name_; }

    inline void SetBinding(SizeT table_idx, SizeT column_idx) {
        binding_.table_idx = table_idx;
        binding_.column_idx = column_idx;
    }

    inline ColumnBinding binding() const { return binding_; }

    inline bool IsCorrelated() const { return depth_ > 0; }

    inline void SetDepth(i64 depth) { depth_ = depth; }

    inline i64 depth() const { return depth_; }

    inline Optional<SpecialType> special() const { return special_; }

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

private:
    String ToString() const final;

private:
    DataType data_type_;

    ColumnBinding binding_{};

    String table_name_{};

    String column_name_{};

    i64 depth_{};

    Optional<SpecialType> special_{None};
};

} // namespace infinity