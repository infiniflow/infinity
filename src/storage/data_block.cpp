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

#include <sstream>

module data_block;

import stl;
import selection;
import status;
import infinity_exception;
import column_vector;

import value;
import third_party;
import default_values;
import serialize;
import logical_type;
import logger;

namespace infinity {

bool DataBlock::AppendColumns(const DataBlock &other, const Vector<SizeT> &column_idxes) {
    if (!initialized || !other.initialized) {
        return false;
    }
    if (row_count_ != other.row_count_) {
        return false;
    }
    if (capacity_ != other.capacity_) {
        return false;
    }
    if (!finalized || !other.finalized) {
        return false;
    }
    for (SizeT idx : column_idxes) {
        column_vectors.push_back(other.column_vectors[idx]);
    }
    return true;
}

UniquePtr<DataBlock> DataBlock::Clone() const {
    if (!finalized) {
        String error_message = "Not finalized data block";
        UnrecoverableError(error_message);
    }
    auto data_block = MakeUnique<DataBlock>();
    data_block->Init(column_vectors);
    return data_block;
}

void DataBlock::Init(const DataBlock *input, const SharedPtr<Selection> &input_select) {
    if (initialized) {
        String error_message = "Data block was initialized before.";
        UnrecoverableError(error_message);
    }
    if (input == nullptr || input_select.get() == nullptr) {
        String error_message = "Invalid input data block or select";
        UnrecoverableError(error_message);
    }
    column_count_ = input->column_count();
    if (column_count_ == 0) {
        String error_message = "Empty column vectors.";
        UnrecoverableError(error_message);
    }
    column_vectors.reserve(column_count_);
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(input->column_vectors[idx]->data_type()));
        column_vectors.back()->Initialize(*(input->column_vectors[idx]), *input_select);
    }
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    this->Finalize();
}

void DataBlock::Init(const SharedPtr<DataBlock> &input, const SharedPtr<Selection> &input_select) { Init(input.get(), input_select); }

void DataBlock::Init(const SharedPtr<DataBlock> &input, SizeT start_idx, SizeT end_idx) {
    if (initialized) {
        String error_message = "Data block was initialized before.";
        UnrecoverableError(error_message);
    }
    if (input.get() == nullptr) {
        String error_message = "Invalid input data block";
        UnrecoverableError(error_message);
    }
    column_count_ = input->column_count();
    if (column_count_ == 0) {
        String error_message = "Empty column vectors.";
        UnrecoverableError(error_message);
    }
    column_vectors.reserve(column_count_);
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(input->column_vectors[idx]->data_type()));
        column_vectors.back()->Initialize(*(input->column_vectors[idx]), start_idx, end_idx);
    }
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    this->Finalize();
}

SharedPtr<DataBlock> DataBlock::MoveFrom(SharedPtr<DataBlock> &input) {
    if (!input->Finalized()) {
        String error_message = "Input data block is not finalized.";
        UnrecoverableError(error_message);
    }
    auto data_block = DataBlock::Make();
    SizeT capacity = input->row_count();
    if (capacity) {
        // because size of bitmap in datablock need to be power of 2
        if (__builtin_popcount(capacity) > 1) {
            capacity = 1 << (sizeof(SizeT) * 8 - __builtin_clz(capacity));
        }
        data_block->Init(input, 0, capacity);
        data_block->row_count_ = input->row_count();
        data_block->finalized = true;
    }
    input->Reset();
    return data_block;
}

void DataBlock::Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity) {
    if (initialized) {
        String error_message = "Data block was initialized before.";
        UnrecoverableError(error_message);
    }
    if (types.empty()) {
        String error_message = "Empty data types collection.";
        UnrecoverableError(error_message);
    }
    column_count_ = types.size();
    column_vectors.reserve(column_count_);
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(types[idx]));
        auto column_vector_type = (types[idx]->type() == LogicalType::kBoolean) ? ColumnVectorType::kCompactBit : ColumnVectorType::kFlat;
        column_vectors[idx]->Initialize(column_vector_type, capacity);
    }
    capacity_ = capacity;
    initialized = true;
}

void DataBlock::Init(const Vector<SharedPtr<ColumnVector>> &input_vectors) {
    if (input_vectors.empty()) {
        String error_message = "Empty column vectors.";
        UnrecoverableError(error_message);
    }
    column_count_ = input_vectors.size();
    column_vectors = input_vectors;
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    Finalize();
}

void DataBlock::UnInit() {
    if (!initialized) {
        // Already in un-initialized state
        return;
    }

    column_vectors.clear();

    row_count_ = 0;
    initialized = false;
    finalized = false;
}

void DataBlock::Reset() {
    if (!initialized) {
        String error_message = "Should not reset an uninitialized block.";
        UnrecoverableError(error_message);
    }

    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.

    for (SizeT i = 0; i < column_count_; ++i) {
        ColumnVectorType old_vector_type = column_vectors[i]->vector_type();
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize(old_vector_type);
    }

    row_count_ = 0;
    finalized = false;
}

// TODO: May cause error when capacity is larger than the originally allocated size
// TODO: Initialize() parameter may not be ColumnVectorType::kFlat ?
void DataBlock::Reset(SizeT capacity) {
    if (!initialized) {
        String error_message = "Should not reset an uninitialized block.";
        UnrecoverableError(error_message);
    }
    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.
    for (SizeT i = 0; i < column_count_; ++i) {
        ColumnVectorType old_vector_type = column_vectors[i]->vector_type();
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize(old_vector_type, capacity);
    }
    row_count_ = 0;
    capacity_ = capacity;
    finalized = false;
}

Value DataBlock::GetValue(SizeT column_index, SizeT row_index) const { return column_vectors[column_index]->GetValue(row_index); }

void DataBlock::SetValue(SizeT column_index, SizeT row_index, const Value &val) {
    if (column_index >= column_count_) {
        String error_message = fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_);
        UnrecoverableError(error_message);
    }
    column_vectors[column_index]->SetValue(row_index, val);
}

void DataBlock::AppendValue(SizeT column_index, const Value &value) {
    if (column_index >= column_count_) {
        String error_message = fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_);
        UnrecoverableError(error_message);
    }
    column_vectors[column_index]->AppendValue(value);
    finalized = false;
}

void DataBlock::AppendValueByPtr(SizeT column_index, const_ptr_t value_ptr) {
    if (column_index >= column_count_) {
        String error_message = fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_);
        UnrecoverableError(error_message);
    }
    column_vectors[column_index]->AppendByPtr(value_ptr);
    finalized = false;
}

void DataBlock::Finalize() {
    if (finalized) {
        return;
    }
    bool have_flat_column_vector = false;
    SizeT row_count = 0;
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        if (column_vectors[idx]->vector_type() != ColumnVectorType::kConstant) {
            const SizeT current_row_count = column_vectors[idx]->Size();
            if (have_flat_column_vector && row_count != current_row_count) {
                UnrecoverableError("Column vectors in same data block have different size.");
            }
            have_flat_column_vector = true;
            row_count = current_row_count;
        } else if (!have_flat_column_vector) {
            row_count = 1;
        }
    }
    row_count_ = row_count;
    finalized = true;
}

String DataBlock::ToString() const {
    std::stringstream ss;
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        ss << "column " << idx << std::endl;
        ss << column_vectors[idx]->ToString() << std::endl;
    }
    return ss.str();
}

String DataBlock::ToBriefString() const {
    std::stringstream ss;
    ss << "row count: " << row_count_ << std::endl;
    ss << "column: ";
    for (SizeT idx = 0; idx < column_count_; ++ idx) {
        ss << column_vectors[idx]->data_type()->ToString() << " ";
    }
    ss << std::endl;
    return ss.str();
}

void DataBlock::FillRowIDVector(SharedPtr<Vector<RowID>> &row_ids, u32 block_id) const {
    if (!finalized) {
        String error_message = "DataBlock isn't finalized.";
        UnrecoverableError(error_message);
    }
    u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
    for (u32 offset = 0; offset < row_count_; ++offset) {
        row_ids->emplace_back(INVALID_SEGMENT_ID, segment_offset_start + offset);
    }
}

void DataBlock::UnionWith(const SharedPtr<DataBlock> &other) {
    if (this->row_count_ != other->row_count_) {
        String error_message = "Attempt to union two block with different row count";
        UnrecoverableError(error_message);
    }
    if (this->capacity_ != other->capacity_) {
        String error_message = "Attempt to union two block with different row count";
        UnrecoverableError(error_message);
    }
    if (!this->initialized || !other->initialized) {
        String error_message = "Attempt to union two block with different row count";
        UnrecoverableError(error_message);
    }
    if (this->finalized != other->finalized) {
        String error_message = "Attempt to union two block with different row count";
        UnrecoverableError(error_message);
    }
    column_count_ += other->column_count_;
    column_vectors.reserve(column_count_);
    column_vectors.insert(column_vectors.end(), other->column_vectors.begin(), other->column_vectors.end());
}

void DataBlock::AppendWith(const SharedPtr<DataBlock> &other) { AppendWith(other.get()); }

void DataBlock::AppendWith(const DataBlock *other) {
    if (other->column_count() != this->column_count()) {
        UnrecoverableError(
            fmt::format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
    }
    if (this->row_count_ + other->row_count_ > this->capacity_) {
        String error_message = fmt::format("Attempt append block with row count {} into block with row count {}, "
                                           "which exceeds the capacity {}",
                                           other->row_count(),
                                           this->row_count(),
                                           this->capacity());
        UnrecoverableError(error_message);
    }

    SizeT column_count = this->column_count();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx]);
    }
}

void DataBlock::AppendWith(const DataBlock *other, SizeT from, SizeT count) {
    if (other->column_count() != this->column_count()) {
        UnrecoverableError(
            fmt::format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
    }
    if (this->row_count_ + count > this->capacity_) {
        String error_message = fmt::format("Attempt append block with row count {} into block with row count{}, "
                                           "which exceeds the capacity {}",
                                           count,
                                           this->row_count(),
                                           this->capacity());
        UnrecoverableError(error_message);
    }
    SizeT column_count = this->column_count();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx], from, count);
    }
}

void DataBlock::InsertVector(const SharedPtr<ColumnVector> &vector, SizeT index) {
    column_vectors.insert(column_vectors.begin() + index, vector);
    column_count_++;
}

bool DataBlock::operator==(const DataBlock &other) const {
    if (!this->initialized && !other.initialized)
        return true;
    if (!this->initialized || !other.initialized || this->column_count_ != other.column_count_)
        return false;
    for (SizeT i = 0; i < this->column_count_; i++) {
        const SharedPtr<ColumnVector> &column1 = this->column_vectors[i];
        const SharedPtr<ColumnVector> &column2 = other.column_vectors[i];
        if (column1.get() == nullptr || column2.get() == nullptr || *column1 != *column2)
            return false;
    }
    return true;
}

i32 DataBlock::GetSizeInBytes() const {
    if (!finalized) {
        String error_message = "Data block is not finalized.";
        UnrecoverableError(error_message);
    }
    i32 size = sizeof(i32);
    for (SizeT i = 0; i < column_count_; i++) {
        size += this->column_vectors[i]->GetSizeInBytes();
    }
    return size;
}

void DataBlock::WriteAdv(char *&ptr) const {
    if (!finalized) {
        String error_message = "Data block is not finalized.";
        UnrecoverableError(error_message);
    }
    WriteBufAdv<i32>(ptr, column_count_);
    for (SizeT i = 0; i < column_count_; i++) {
        this->column_vectors[i]->WriteAdv(ptr);
    }
}

SharedPtr<DataBlock> DataBlock::ReadAdv(const char *&ptr, i32 maxbytes) {
    const char *const ptr_end = ptr + maxbytes;
    i32 column_count = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnVector>> column_vectors;
    for (int i = 0; i < column_count; i++) {
        maxbytes = ptr_end - ptr;
        if (maxbytes <= 0) {
            String error_message = "ptr goes out of range when reading DataBlock";
            UnrecoverableError(error_message);
        }
        SharedPtr<ColumnVector> column_vector = ColumnVector::ReadAdv(ptr, maxbytes);
        column_vectors.push_back(column_vector);
    }
    SharedPtr<DataBlock> block = DataBlock::Make();
    block->Init(column_vectors);
    block->Finalize();
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        String error_message = "ptr goes out of range when reading DataBlock";
        UnrecoverableError(error_message);
    }
    return block;
}

} // namespace infinity
