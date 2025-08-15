// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:physical_unnest.impl;

import :physical_unnest;
import :stl;
import :query_context;
import :table_def;
import :data_table;
import :physical_operator_type;
import :operator_state;
import :expression_state;
import :expression_selector;
import :data_block;
import :logger;
import :infinity_exception;
import :column_vector;
import :status;
import :unnest_expression;
import :reference_expression;
import :expression_type;

import std;
import third_party;

import data_type;
import logical_type;
import array_info;

namespace infinity {

void PhysicalUnnest::Init(QueryContext* query_context) {}

namespace {

void AppendScaleToColumnVector(const ColumnVector &input_col, size_t write_size, size_t written, ColumnVector &cur_col) {
    for (size_t i = 0; i < write_size; ++i) {
        cur_col.AppendWith(input_col, written, 1);
    }
}

std::vector<std::unique_ptr<DataBlock>> MakeDataBlocks(const std::vector<std::vector<std::shared_ptr<ColumnVector>>> &output_datas) {
    int block_count = -1;
    for (size_t col_idx = 0; col_idx < output_datas.size(); ++col_idx) {
        const auto &output_column_vector = output_datas[col_idx];
        if (block_count == -1) {
            block_count = output_column_vector.size();
        } else if (block_count != int(output_column_vector.size())) {
            UnrecoverableError("Column count mismatch.");
        }
    }
    std::vector<std::unique_ptr<DataBlock>> data_blocks;
    data_blocks.reserve(block_count);
    for (size_t block_i = 0; block_i < size_t(block_count); ++block_i) {
        data_blocks.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_ptr = data_blocks.back().get();
        for (size_t col_idx = 0; col_idx < output_datas.size(); ++col_idx) {
            std::shared_ptr<ColumnVector> vec = output_datas[col_idx][block_i];
            output_ptr->InsertVector(vec, col_idx);
        }
        output_ptr->Finalize();
    }
    return data_blocks;
}
}

bool PhysicalUnnest::Execute(QueryContext *, OperatorState *operator_state) {
    OperatorState *prev_op_state = operator_state->prev_op_state_;
    auto *unnest_op_state = static_cast<UnnestOperatorState *>(operator_state);
    const auto &input_data_blocks = prev_op_state->data_block_array_;

    if (input_data_blocks.empty()) {
        unnest_op_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_ptr = unnest_op_state->data_block_array_.back().get();
        output_ptr->Init(*GetOutputTypes());

        if (prev_op_state->Complete()) {
            prev_op_state->data_block_array_.clear();
            operator_state->SetComplete();
        }
        return true;
    }

    std::vector<std::shared_ptr<DataType>> output_types = *this->GetOutputTypes();
    std::vector<std::shared_ptr<DataType>> input_types = input_data_blocks[0]->types();

    size_t unnest_idx = GetUnnestIdx();

    std::vector<std::vector<std::shared_ptr<ColumnVector>>> output_datas(output_types.size());
    for (const auto &input_data_block : input_data_blocks) {
        u16 row_count = input_data_block->row_count();
        std::vector<size_t> array_lengths;
        {
            const auto &unnest_col = input_data_block->column_vectors[unnest_idx];
            auto &output_cols = output_datas[unnest_idx];
            if (output_cols.empty()) {
                auto col = ColumnVector::Make(output_types[unnest_idx]);
                col->Initialize();
                output_cols.push_back(std::move(col));
            }
            ColumnVector *cur_col = output_cols.back().get();

            for (u16 row_idx = 0; row_idx < row_count; ++row_idx) {
                size_t written = 0;
                while (true) {
                    bool complete = cur_col->AppendUnnestArray(*unnest_col, row_idx, written);
                    if (complete) {
                        array_lengths.push_back(written);
                        break;
                    }

                    if (cur_col->capacity() == cur_col->Size()) {
                        auto new_col = ColumnVector::Make(output_types[unnest_idx]);
                        new_col->Initialize();
                        output_cols.push_back(std::move(new_col));
                        cur_col = output_cols.back().get();
                    }
                }
            }
        }
        for (size_t col_idx = 0; col_idx < output_types.size(); ++col_idx) {
            if (col_idx == unnest_idx) {
                continue;
            }
            const auto &input_col = input_data_block->column_vectors[col_idx];
            auto &output_cols = output_datas[col_idx];
            if (output_cols.empty()) {
                auto col = ColumnVector::Make(output_types[col_idx]);
                col->Initialize();
                output_cols.push_back(std::move(col));
            }
            ColumnVector *cur_col = output_cols.back().get();
            size_t input_offset = 0;
            for (size_t array_length : array_lengths) {
                size_t written = 0;
                while (written < array_length) {
                    size_t write_size = std::min(array_length - written, cur_col->capacity() - cur_col->Size());
                    AppendScaleToColumnVector(*input_col, write_size, input_offset, *cur_col);
                    written += write_size;

                    if (cur_col->capacity() == cur_col->Size()) {
                        auto new_col = ColumnVector::Make(output_types[col_idx]);
                        new_col->Initialize();
                        output_cols.push_back(std::move(new_col));
                        cur_col = output_cols.back().get();
                    }
                }
                ++input_offset;
            }
        }
    }

    for (auto &output_column_vector : output_datas) {
        if (auto &last_col = output_column_vector.back(); last_col->Size() == 0) {
            output_column_vector.pop_back();
        }
    }

    unnest_op_state->data_block_array_ = MakeDataBlocks(output_datas);

    if (prev_op_state->Complete()) {
        prev_op_state->data_block_array_.clear();
        operator_state->SetComplete();
    }
    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalUnnest::GetOutputNames() const {
    auto ret = PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this);
    return ret;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalUnnest::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
    size_t unnest_idx = GetUnnestIdx();
    (*result)[unnest_idx] = std::make_shared<DataType>(static_cast<ArrayInfo *>((*result)[unnest_idx]->type_info().get())->ElemType());
    return result;
}

size_t PhysicalUnnest::GetUnnestIdx() const {
    if (expression_list_.size() != 1) {
        RecoverableError(Status::SyntaxError(fmt::format("UNNEST only supports one expression, but got {}", expression_list_.size())));
    }
    auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[0].get());
    if (unnest_expr->arguments().size() != 1 || unnest_expr->arguments()[0]->type() != ExpressionType::kReference) {
        UnrecoverableError("UNNEST only supports one column reference.");
    }
    auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
    size_t unnest_idx = unnest_ref_expr->column_index();
    return unnest_idx;
}

} // namespace infinity
