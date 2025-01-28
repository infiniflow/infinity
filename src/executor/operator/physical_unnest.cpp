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

namespace infinity {

void PhysicalUnnest::Init(QueryContext* query_context) {}

namespace {

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

void AppendRowIDToColumnVector(RowID row_id, SizeT size, SizeT offset, ColumnVector *column_vector) {
    for (SizeT i = offset; i < size; ++i) {
        column_vector->AppendByPtr(reinterpret_cast<const_ptr_t>(&row_id));
    }
}

[[maybe_unused]] bool CheckRowIDInColumnVector(RowID row_id, SizeT size, SizeT offset, ColumnVector *column_vector) {
    for (SizeT i = offset; i < size; ++i) {
        RowID cur_row_id = column_vector->GetValue(i).value_.row;
        if (cur_row_id != row_id) {
            return false;
        }
    }
    return true;
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

} // namespace

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

    Vector<SharedPtr<DataType>> output_types;
    Vector<SharedPtr<DataType>> input_types = input_data_blocks[0]->types();
    output_types.reserve(input_types.size());
    Vector<ArrayInfo *> array_infos;

    Optional<SizeT> rowid_idx = None;
    for (SizeT col_idx = 0; const auto &col : input_data_blocks[0]->column_vectors) {
        if (col->data_type()->type() == LogicalType::kArray) {
            auto *array_info = static_cast<ArrayInfo *>(col->data_type()->type_info().get());
            array_infos.push_back(static_cast<ArrayInfo *>(array_info));
            output_types.push_back(MakeShared<DataType>(array_info->ElemType()));
        } else {
            if (col->data_type()->type() == LogicalType::kRowID) {
                rowid_idx = col_idx;
                // skip
            } else {
                // not implemented.
                RecoverableError(Status::InvalidDataType());
            }
            output_types.push_back(col->data_type());
        }
        ++col_idx;
    }

    Vector<Vector<SharedPtr<ColumnVector>>> output_datas(output_types.size());
    for (const auto &input_data_block : input_data_blocks) {
        bool set_row_id = false;
        u16 row_count = input_data_block->row_count();
        for (SizeT col_idx = 0; col_idx < output_types.size(); ++col_idx) {
            const auto &col = input_data_block->column_vectors[col_idx];
            if (rowid_idx && col_idx == *rowid_idx) {
                continue;
            }

            auto &output_column_vector = output_datas[col_idx];
            ColumnVector *cur_col = nullptr;
            ColumnVector *rowid_col = nullptr;
            if (output_column_vector.empty()) {
                auto col = ColumnVector::Make(output_types[col_idx]);
                col->Initialize();
                output_column_vector.push_back(std::move(col));
            }
            cur_col = output_column_vector.back().get();

            if (!set_row_id) {
                if (output_datas[*rowid_idx].empty()) {
                    auto col = ColumnVector::Make(MakeShared<DataType>(LogicalType::kRowID));
                    col->Initialize();
                    output_datas[*rowid_idx].push_back(std::move(col));
                }
                rowid_col = output_datas[*rowid_idx].back().get();
            }

            if (*col->data_type() != *input_types[col_idx]) {
                UnrecoverableError(fmt::format("Column type mismatch: {} vs {}", col->data_type()->ToString(), input_types[col_idx]->ToString()));
            }
            auto *data = reinterpret_cast<ArrayT *>(col->data());
            for (u16 row_idx = 0; row_idx < row_count; ++row_idx) {
                Pair<Span<const char>, SizeT> array_raw = col->GetArray(data[row_idx], col->buffer_.get(), array_infos[col_idx]);
                const char *array_data = array_raw.first.data();
                SizeT array_size = array_raw.second;

                SizeT written = 0;
                while (written < array_size) {
                    SizeT write_size = std::min(array_size - written, cur_col->capacity() - cur_col->Size());
                    AppendArrayToColumnVector(array_data, write_size, written, cur_col, *output_types[col_idx]);

                    if (rowid_col) {
                        RowID row_id = input_data_block->GetValue(*rowid_idx, row_idx).value_.row;
                        AppendRowIDToColumnVector(row_id, write_size, written, rowid_col);
                    }
                    written += write_size;

                    if (cur_col->capacity() == cur_col->Size()) {
                        auto new_col = ColumnVector::Make(output_types[col_idx]);
                        new_col->Initialize();
                        output_column_vector.push_back(std::move(new_col));
                        cur_col = output_column_vector.back().get();

                        if (set_row_id) {
                            auto new_col = ColumnVector::Make(MakeShared<DataType>(LogicalType::kRowID));
                            new_col->Initialize();
                            output_datas[*rowid_idx].push_back(std::move(new_col));
                            rowid_col = output_datas[*rowid_idx].back().get();
                        }
                    }
                }
            }
            if (!set_row_id) {
                set_row_id = true;
            }
        }
        if (!set_row_id) {
            UnrecoverableError("RowID not set.");
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

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalUnnest::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    for (auto &expr : expression_list_) {
        result->push_back(MakeShared<DataType>(expr->Type()));
    }
    return result;
}

} // namespace infinity
