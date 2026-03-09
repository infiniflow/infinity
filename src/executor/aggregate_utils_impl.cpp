// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:aggregate_utils.impl;

import :aggregate_utils;
import :column_vector;
import :aggregate_expression;
import :data_block;
import :infinity_exception;
import :status;

import std;
import third_party;
import internal_types;
import data_type;
import logical_type;

namespace infinity {

void BuildHashKey(const std::vector<std::shared_ptr<ColumnVector>> &columns,
                  size_t row_id,
                  const std::vector<std::shared_ptr<DataType>> &types,
                  std::string &hash_key) {
    size_t column_count = columns.size();
    hash_key.clear();
    bool has_null = false;
    for (size_t column_id = 0; column_id < column_count; ++column_id) {
        if (!columns[column_id]->nulls_ptr_->IsTrue(row_id)) {
            hash_key += "\0\0";
            has_null = true;
            continue;
        }

        const DataType &data_type = *types[column_id];

        if (data_type.type() == LogicalType::kVarchar) {
            std::span<const char> text = columns[column_id]->GetVarchar(row_id);
            hash_key.append(text.begin(), text.end());
        } else {
            size_t type_size = types[column_id]->Size();
            std::span<const char> binary(reinterpret_cast<const char *>(columns[column_id]->data() + type_size * row_id), type_size);
            hash_key.append(binary.begin(), binary.end());
        }
        hash_key += '\0';
    }
    if (!has_null) {
        size_t key_size = CalculateHashKeySize(types);
        if (key_size && hash_key.size() != key_size) {
            UnrecoverableError(fmt::format("Hash key size mismatch: {} vs {}", hash_key.size(), key_size));
        }
    }
}

size_t CalculateHashKeySize(const std::vector<std::shared_ptr<DataType>> &types) {
    size_t key_size = 0;
    size_t type_count = types.size();
    for (size_t idx = 0; idx < type_count; ++idx) {
        const DataType &data_type = *types[idx];
        switch (data_type.type()) {
            case LogicalType::kBoolean:
            case LogicalType::kTinyInt:
            case LogicalType::kSmallInt:
            case LogicalType::kInteger:
            case LogicalType::kBigInt:
            case LogicalType::kFloat:
            case LogicalType::kDouble:
            case LogicalType::kDate:
            case LogicalType::kTime:
            case LogicalType::kDateTime:
            case LogicalType::kTimestamp: {
                size_t type_size = data_type.Size();
                key_size += type_size;
                break;
            }
            case LogicalType::kVarchar: {
                // Varchar has variable length, cannot calculate the hash key size in advance
                key_size = 0;
                break;
            }
            default: {
                RecoverableError(Status::NotSupport(fmt::format("Attempt to construct hash key for type: {}", data_type.ToString())));
            }
        }
        if (key_size == 0) {
            break;
        }
    }

    if (key_size) {
        // Key layout: col1\0col2\0col3\0.
        key_size += type_count;
    }
    return key_size;
}

void GenerateDefaultAggregateOutput(const std::vector<std::shared_ptr<BaseExpression>> &aggregates, std::unique_ptr<DataBlock> &output_block) {
    size_t aggregates_count = aggregates.size();

    // Build output types
    std::vector<std::shared_ptr<DataType>> output_types;
    output_types.reserve(aggregates_count);
    for (const auto &expr : aggregates) {
        output_types.emplace_back(std::make_shared<DataType>(expr->Type()));
    }

    output_block = DataBlock::MakeUniquePtr();
    output_block->Init(output_types, 1);

    for (size_t idx = 0; idx < aggregates_count; ++idx) {
        auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates[idx]);
        const std::string &func_name = agg_expr->aggregate_function_.GetFuncName();

        // Initialize and finalize to get default value
        auto agg_state = agg_expr->aggregate_function_.InitState();
        agg_expr->aggregate_function_.init_func_(agg_state.get());
        const char *result_ptr = agg_expr->aggregate_function_.finalize_func_(agg_state.get());
        output_block->column_vectors_[idx]->AppendByPtr(result_ptr);

        // For non-COUNT aggregates, mark as NULL
        if (func_name != "COUNT") {
            output_block->column_vectors_[idx]->nulls_ptr_->SetFalse(0);
        }
    }

    output_block->Finalize();
}

} // namespace infinity
