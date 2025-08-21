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

export module infinity_core:column_expression;

import :column_binding;
import :base_expression;
import :special_function;

import internal_types;
import data_type;

namespace infinity {

export class ColumnExpression final : public BaseExpression {
public:
    static inline std::shared_ptr<ColumnExpression> Make(DataType data_type,
                                                         std::string table_name,
                                                         u64 table_index,
                                                         std::string column_name,
                                                         i64 column_index,
                                                         i64 depth,
                                                         std::optional<SpecialType> special = std::nullopt) {
        return std::make_shared<ColumnExpression>(std::move(data_type),
                                                  std::move(table_name),
                                                  table_index,
                                                  std::move(column_name),
                                                  column_index,
                                                  depth,
                                                  special);
    }

public:
    ColumnExpression(DataType data_type,
                     std::string table_name,
                     u64 table_index,
                     std::string column_name,
                     i64 column_index,
                     i64 depth,
                     std::optional<SpecialType> special = std::nullopt);

    inline DataType Type() const override { return data_type_; };

    const std::string &table_name() const { return table_name_; }

    //    inline u64
    //    table_index() const {
    //        return binding_.table_idx;
    //    }

    const std::string &column_name() const { return column_name_; }

    inline void SetBinding(size_t table_idx, size_t column_idx) {
        binding_.table_idx = table_idx;
        binding_.column_idx = column_idx;
    }

    inline ColumnBinding binding() const { return binding_; }

    inline bool IsCorrelated() const { return depth_ > 0; }

    inline void SetDepth(i64 depth) { depth_ = depth; }

    inline i64 depth() const { return depth_; }

    inline std::optional<SpecialType> special() const { return special_; }

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

    std::string ToString() const final;

private:
    DataType data_type_;

    ColumnBinding binding_{};

    std::string table_name_{};

    std::string column_name_{};

    i64 depth_{};

    std::optional<SpecialType> special_{std::nullopt};
};

} // namespace infinity