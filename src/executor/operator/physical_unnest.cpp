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

#include <vector>

module physical_unnest;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import expression_state;
import expression_selector;
import data_block;
import logger;
import third_party;

import infinity_exception;
import array_info;
import column_vector;
import logical_type;
import status;

import unnest_expression;
import reference_expression;
import expression_type;

namespace infinity {

void PhysicalUnnest::Init() {}

void AppendArrayToColumnVector(const char *array_data, SizeT size, SizeT offset, ColumnVector *column_vector, const DataType &data_type) {
    switch (data_type.type()) {
        case LogicalType::kInteger: {
            auto *typed_data = reinterpret_cast<const i32 *>(array_data);
            for (SizeT i = offset; i < size; ++i) {
                column_vector->AppendByPtr(reinterpret_cast<const_ptr_t>(&typed_data[i]));
            }
            break;
        }
        default: {
            UnrecoverableError("Not implemented.");
        }
    }
}

void AppendScaleToColumnVector(const ColumnVector &input_col, SizeT write_size, SizeT written, ColumnVector &cur_col) {
    for (SizeT i = 0; i < write_size; ++i) {
        cur_col.AppendWith(input_col, written, 1);
    }
}

Vector<UniquePtr<DataBlock>> MakeDataBlocks(const Vector<Vector<SharedPtr<ColumnVector>>> &output_datas) {
    int block_count = -1;
    for (SizeT col_idx = 0; col_idx < output_datas.size(); ++col_idx) {
        const auto &output_column_vector = output_datas[col_idx];
        if (block_count == -1) {
            block_count = output_column_vector.size();
        } else if (block_count != int(output_column_vector.size())) {
            UnrecoverableError("Column count mismatch.");
        }
    }
    Vector<UniquePtr<DataBlock>> data_blocks;
    data_blocks.reserve(block_count);
    for (SizeT block_i = 0; block_i < SizeT(block_count); ++block_i) {
        data_blocks.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_ptr = data_blocks.back().get();
        for (SizeT col_idx = 0; col_idx < output_datas.size(); ++col_idx) {
            SharedPtr<ColumnVector> vec = output_datas[col_idx][block_i];
            output_ptr->InsertVector(vec, col_idx);
        }
        output_ptr->Finalize();
    }
    return data_blocks;
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

    Vector<SharedPtr<DataType>> output_types = *this->GetOutputTypes();
    Vector<SharedPtr<DataType>> input_types = input_data_blocks[0]->types();

    SizeT unnest_idx = GetUnnestIdx();

    Vector<Vector<SharedPtr<ColumnVector>>> output_datas(output_types.size());
    for (const auto &input_data_block : input_data_blocks) {
        u16 row_count = input_data_block->row_count();
        Vector<SizeT> array_lengths;
        {
            const auto &unnest_col = input_data_block->column_vectors[unnest_idx];
            auto *array_info = static_cast<ArrayInfo *>(unnest_col->data_type()->type_info().get());

            auto &output_cols = output_datas[unnest_idx];
            if (output_cols.empty()) {
                auto col = ColumnVector::Make(output_types[unnest_idx]);
                col->Initialize();
                output_cols.push_back(std::move(col));
            }
            ColumnVector *cur_col = output_cols.back().get();

            auto *data = reinterpret_cast<ArrayT *>(unnest_col->data());
            for (u16 row_idx = 0; row_idx < row_count; ++row_idx) {
                Pair<Span<const char>, SizeT> array_raw = unnest_col->GetArray(data[row_idx], unnest_col->buffer_.get(), array_info);
                const char *array_data = array_raw.first.data();
                SizeT array_size = array_raw.second;
                array_lengths.push_back(array_size);

                SizeT written = 0;
                while (written < array_size) {
                    SizeT write_size = std::min(array_size - written, cur_col->capacity() - cur_col->Size());
                    AppendArrayToColumnVector(array_data, write_size, written, cur_col, *output_types[unnest_idx]);
                    written += write_size;

                    if (cur_col->capacity() == cur_col->Size()) {
                        auto new_col = ColumnVector::Make(output_types[unnest_idx]);
                        new_col->Initialize();
                        output_cols.push_back(std::move(new_col));
                        cur_col = output_cols.back().get();
                    }
                }
            }
        }
        for (SizeT col_idx = 0; col_idx < output_types.size(); ++col_idx) {
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
            SizeT input_offset = 0;
            for (SizeT array_length : array_lengths) {
                SizeT written = 0;
                while (written < array_length) {
                    SizeT write_size = std::min(array_length - written, cur_col->capacity() - cur_col->Size());
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

SharedPtr<Vector<String>> PhysicalUnnest::GetOutputNames() const {
    auto ret = PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this);
    return ret;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalUnnest::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
    SizeT unnest_idx = GetUnnestIdx();
    (*result)[unnest_idx] = MakeShared<DataType>(static_cast<ArrayInfo *>((*result)[unnest_idx]->type_info().get())->ElemType());
    return result;
}

SizeT PhysicalUnnest::GetUnnestIdx() const {
    if (expression_list_.size() != 1) {
        RecoverableError(Status::SyntaxError(fmt::format("UNNEST only supports one expression, but got {}", expression_list_.size())));
    }
    auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[0].get());
    if (unnest_expr->arguments().size() != 1 || unnest_expr->arguments()[0]->type() != ExpressionType::kReference) {
        UnrecoverableError("UNNEST only supports one column reference.");
    }
    auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
    SizeT unnest_idx = unnest_ref_expr->column_index();
    return unnest_idx;
}

} // namespace infinity
