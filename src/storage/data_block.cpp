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

import stl;
import selection;

import infinity_exception;
import column_vector;
import parser;
import value;
import third_party;
import default_values;
import serialize;

module data_block;

namespace infinity {

void DataBlock::Init(const DataBlock *input, const SharedPtr<Selection> &input_select) {
    if (initialized) {
        Error<StorageException>("Data block was initialized before.");
    }
    if (input == nullptr || input_select.get() == nullptr) {
        Error<StorageException>("Invalid input data block or select");
    }
    column_count_ = input->column_count();
    if (column_count_ == 0) {
        Error<StorageException>("Empty column vectors.");
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
        Error<StorageException>("Data block was initialized before.");
    }
    if (input.get() == nullptr) {
        Error<StorageException>("Invalid input data block");
    }
    column_count_ = input->column_count();
    if (column_count_ == 0) {
        Error<StorageException>("Empty column vectors.");
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
        Error<StorageException>("Input data block is not finalized.");
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
        Error<StorageException>("Data block was initialized before.");
    }
    if (types.empty()) {
        Error<StorageException>("Empty data types collection.");
    }
    column_count_ = types.size();
    column_vectors.reserve(column_count_);
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(types[idx]));
        column_vectors[idx]->Initialize(ColumnVectorType::kFlat, capacity);
    }
    capacity_ = capacity;
    initialized = true;
}

void DataBlock::Init(const Vector<SharedPtr<ColumnVector>> &input_vectors) {
    if (input_vectors.empty()) {
        Error<StorageException>("Empty column vectors.");
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

    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.

    for (SizeT i = 0; i < column_count_; ++i) {
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize();
    }

    row_count_ = 0;
}

//TODO: May cause error when capacity is larger than the originally allocated size
//TODO: Initialize() parameter may not be ColumnVectorType::kFlat ?
void DataBlock::Reset(SizeT capacity) {
    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.
    for (SizeT i = 0; i < column_count_; ++i) {
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize(ColumnVectorType::kFlat, capacity);
    }
    row_count_ = 0;
    capacity_ = capacity;
}

Value DataBlock::GetValue(SizeT column_index, SizeT row_index) const { return column_vectors[column_index]->GetValue(row_index); }

void DataBlock::SetValue(SizeT column_index, SizeT row_index, const Value &val) {
    if (column_index >= column_count_) {
        Error<StorageException>(Format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->SetValue(row_index, val);
}

void DataBlock::AppendValue(SizeT column_index, const Value &value) {
    if (column_index >= column_count_) {
        Error<StorageException>(Format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->AppendValue(value);
    finalized = false;
}

void DataBlock::AppendValueByPtr(SizeT column_index, const_ptr_t value_ptr) {
    if (column_index >= column_count_) {
        Error<StorageException>(Format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->AppendByPtr(value_ptr);
    finalized = false;
}

void DataBlock::Finalize() {
    if(finalized) {
        return ;
    }
    bool first_flat_column_vector = false;
    SizeT row_count = 0;
    for (SizeT idx = 0; idx < column_count_; ++idx) {
        if (column_vectors[idx]->vector_type() == ColumnVectorType::kConstant) {
            continue;
        } else {
            if (first_flat_column_vector) {
                if (row_count != column_vectors[idx]->Size()) {
                    Error<StorageException>("Column vectors in same data block have different size.");
                }
            } else {
                first_flat_column_vector = true;
                row_count = column_vectors[idx]->Size();
            }
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

void DataBlock::FillRowIDVector(SharedPtr<Vector<RowID>> &row_ids, u32 block_id) const {
    if (!finalized) {
        Error<StorageException>("DataBlock isn't finalized.");
    }
    u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
    for (u32 offset = 0; offset < row_count_; ++offset) {
        row_ids->emplace_back(INVALID_SEGMENT_ID, segment_offset_start + offset);
    }
}

void DataBlock::UnionWith(const SharedPtr<DataBlock> &other) {
    if (this->row_count_ != other->row_count_) {
        Error<StorageException>("Attempt to union two block with different row count");
    }
    if (this->capacity_ != other->capacity_) {
        Error<StorageException>("Attempt to union two block with different row count");
    }
    if (!this->initialized || !other->initialized) {
        Error<StorageException>("Attempt to union two block with different row count");
    }
    if (this->finalized != other->finalized) {
        Error<StorageException>("Attempt to union two block with different row count");
    }
    column_count_ += other->column_count_;
    column_vectors.reserve(column_count_);
    column_vectors.insert(column_vectors.end(), other->column_vectors.begin(), other->column_vectors.end());
}

void DataBlock::AppendWith(const SharedPtr<DataBlock> &other) { AppendWith(other.get()); }

void DataBlock::AppendWith(const DataBlock *other) {
    if (other->column_count() != this->column_count()) {
        Error<StorageException>(
            Format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
        ;
    }
    if (this->row_count_ + other->row_count_ > this->capacity_) {
        Error<StorageException>(Format("Attempt append block with row count {} into block with row count {}, "
                                       "which exceeds the capacity {}",
                                       other->row_count(),
                                       this->row_count(),
                                       this->capacity()));
    }

    SizeT column_count = this->column_count();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx]);
    }
}

void DataBlock::AppendWith(const DataBlock *other, SizeT from, SizeT count) {
    if (other->column_count() != this->column_count()) {
        Error<StorageException>(
            Format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
        ;
    }
    if (this->row_count_ + count > this->capacity_) {
        Error<StorageException>(Format("Attempt append block with row count {} into block with row count{}, "
                                       "which exceeds the capacity {}",
                                       count,
                                       this->row_count(),
                                       this->capacity()));
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
        Error<StorageException>("Data block is not finalized.");
    }
    i32 size = sizeof(i32);
    for (SizeT i = 0; i < column_count_; i++) {
        size += this->column_vectors[i]->GetSizeInBytes();
    }
    return size;
}

void DataBlock::WriteAdv(char *&ptr) const {
    if (!finalized) {
        Error<StorageException>("Data block is not finalized.");
    }
    WriteBufAdv<i32>(ptr, column_count_);
    for (SizeT i = 0; i < column_count_; i++) {
        this->column_vectors[i]->WriteAdv(ptr);
    }
}

SharedPtr<DataBlock> DataBlock::ReadAdv(char *&ptr, i32 maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    i32 column_count = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnVector>> column_vectors;
    for (int i = 0; i < column_count; i++) {
        maxbytes = ptr_end - ptr;
        if (maxbytes <= 0) {
            Error<StorageException>("ptr goes out of range when reading DataBlock");
        }
        SharedPtr<ColumnVector> column_vector = ColumnVector::ReadAdv(ptr, maxbytes);
        column_vectors.push_back(column_vector);
    }
    SharedPtr<DataBlock> block = DataBlock::Make();
    block->Init(column_vectors);
    block->Finalize();
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        Error<StorageException>("ptr goes out of range when reading DataBlock");
    }
    return block;
}

} // namespace infinity
