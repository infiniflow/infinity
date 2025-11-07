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

module infinity_core:data_block.impl;

import :data_block;
import :selection;
import :status;
import :infinity_exception;
import :column_vector;
import :value;
import :default_values;

import std;
import third_party;

import data_type;
import row_id;
import serialize;
import logical_type;

namespace infinity {

bool DataBlock::AppendColumns(const DataBlock &other, const std::vector<size_t> &column_idxes) {
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
    for (size_t idx : column_idxes) {
        column_vectors.push_back(other.column_vectors[idx]);
    }
    return true;
}

std::unique_ptr<DataBlock> DataBlock::Clone() const {
    if (!finalized) {
        return nullptr;
    }
    auto data_block = std::make_unique<DataBlock>();
    data_block->Init(column_vectors);
    return data_block;
}

void DataBlock::Init(const DataBlock *input, const std::shared_ptr<Selection> &input_select) {
    if (initialized) {
        UnrecoverableError("Data block was initialized before.");
    }
    if (input == nullptr || input_select.get() == nullptr) {
        UnrecoverableError("Invalid input data block or select");
    }
    column_count_ = input->column_count();
    if (column_count_ == 0) {
        UnrecoverableError("Empty column vectors.");
    }
    column_vectors.reserve(column_count_);
    for (size_t idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(std::make_shared<ColumnVector>(input->column_vectors[idx]->data_type()));
        column_vectors.back()->Initialize(*(input->column_vectors[idx]), *input_select);
    }
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    this->Finalize();
}

void DataBlock::Init(const std::vector<std::shared_ptr<DataType>> &types, size_t capacity) {
    if (initialized) {
        UnrecoverableError("Data block was initialized before.");
    }
    if (types.empty()) {
        UnrecoverableError("Empty data types collection.");
    }
    column_count_ = types.size();
    column_vectors.reserve(column_count_);
    for (size_t idx = 0; idx < column_count_; ++idx) {
        column_vectors.emplace_back(std::make_shared<ColumnVector>(types[idx]));
        auto column_vector_type = (types[idx]->type() == LogicalType::kBoolean) ? ColumnVectorType::kCompactBit : ColumnVectorType::kFlat;
        column_vectors[idx]->Initialize(column_vector_type, capacity);
    }
    capacity_ = capacity;
    initialized = true;
}

void DataBlock::Init(const std::vector<std::shared_ptr<ColumnVector>> &input_vectors) {
    if (input_vectors.empty()) {
        UnrecoverableError("Empty column vectors.");
    }
    column_count_ = input_vectors.size();
    column_vectors = input_vectors;
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    Finalize();
}

// void DataBlock::UnInit() {
//     if (!initialized) {
//         // Already in un-initialized state
//         return;
//     }
//
//     column_vectors.clear();
//
//     row_count_ = 0;
//     initialized = false;
//     finalized = false;
// }

void DataBlock::Reset() {
    if (!initialized) {
        UnrecoverableError("Should not reset an uninitialized block.");
    }

    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.

    for (size_t i = 0; i < column_count_; ++i) {
        ColumnVectorType old_vector_type = column_vectors[i]->vector_type();
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize(old_vector_type);
    }

    row_count_ = 0;
    finalized = false;
}

// TODO: May cause error when capacity is larger than the originally allocated size
// TODO: Initialize() parameter may not be ColumnVectorType::kFlat ?
void DataBlock::Reset(size_t capacity) {
    if (!initialized) {
        UnrecoverableError("Should not reset an uninitialized block.");
    }
    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.
    for (size_t i = 0; i < column_count_; ++i) {
        ColumnVectorType old_vector_type = column_vectors[i]->vector_type();
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize(old_vector_type, capacity);
    }
    row_count_ = 0;
    capacity_ = capacity;
    finalized = false;
}

Value DataBlock::GetValue(size_t column_index, size_t row_index) const { return column_vectors[column_index]->GetValueByIndex(row_index); }

void DataBlock::SetValue(size_t column_index, size_t row_index, const Value &val) {
    if (column_index >= column_count_) {
        UnrecoverableError(fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->SetValueByIndex(row_index, val);
}

void DataBlock::AppendValue(size_t column_index, const Value &value) {
    if (column_index >= column_count_) {
        UnrecoverableError(fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->AppendValue(value);
    finalized = false;
}

void DataBlock::AppendValueByPtr(size_t column_index, const char *value_ptr) {
    if (column_index >= column_count_) {
        UnrecoverableError(fmt::format("Attempt to access invalid column index: {} in column count: {}", column_index, column_count_));
    }
    column_vectors[column_index]->AppendByPtr(value_ptr);
    finalized = false;
}

void DataBlock::Finalize() {
    if (finalized) {
        return;
    }
    bool have_flat_column_vector = false;
    size_t row_count = 0;
    for (size_t idx = 0; idx < column_count_; ++idx) {
        if (column_vectors[idx]->vector_type() != ColumnVectorType::kConstant) {
            const size_t current_row_count = column_vectors[idx]->Size();
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
    initialized = true;
    if (capacity_ == 0) {
        capacity_ = row_count;
    }
}

std::string DataBlock::ToString() const {
    std::stringstream ss;
    for (size_t idx = 0; idx < column_count_; ++idx) {
        ss << "column " << idx << std::endl;
        ss << column_vectors[idx]->ToString() << std::endl;
    }
    return ss.str();
}

std::string DataBlock::ToBriefString() const {
    std::stringstream ss;
    ss << "row count: " << row_count_ << std::endl;
    ss << "column: ";
    for (size_t idx = 0; idx < column_count_; ++idx) {
        ss << column_vectors[idx]->data_type()->ToString() << " ";
    }
    ss << std::endl;
    return ss.str();
}

void DataBlock::FillRowIDVector(std::shared_ptr<std::vector<RowID>> &row_ids, u32 block_id) const {
    if (!finalized) {
        UnrecoverableError("DataBlock isn't finalized.");
    }
    u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
    for (u32 offset = 0; offset < row_count_; ++offset) {
        row_ids->emplace_back(INVALID_SEGMENT_ID, segment_offset_start + offset);
    }
}

void DataBlock::UnionWith(const std::shared_ptr<DataBlock> &other) {
    if (this->row_count_ != other->row_count_) {
        UnrecoverableError("Attempt to union two block with different row count");
    }
    if (this->capacity_ != other->capacity_) {
        UnrecoverableError("Attempt to union two block with different row count");
    }
    if (!this->initialized || !other->initialized) {
        UnrecoverableError("Attempt to union two block with different row count");
    }
    if (this->finalized != other->finalized) {
        UnrecoverableError("Attempt to union two block with different row count");
    }
    column_count_ += other->column_count_;
    column_vectors.reserve(column_count_);
    column_vectors.insert(column_vectors.end(), other->column_vectors.begin(), other->column_vectors.end());
}

void DataBlock::AppendWith(const std::shared_ptr<DataBlock> &other) { AppendWith(other.get()); }

void DataBlock::AppendWith(const DataBlock *other) {
    if (other->column_count() != this->column_count()) {
        UnrecoverableError(
            fmt::format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
    }
    if (this->row_count_ + other->row_count_ > this->capacity_) {
        UnrecoverableError(fmt::format("Attempt append block with row count {} into block with row count {}, "
                                       "which exceeds the capacity {}",
                                       other->row_count(),
                                       this->row_count(),
                                       this->capacity()));
    }

    size_t column_count = this->column_count();
    for (size_t idx = 0; idx < column_count; ++idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx]);
    }
    row_count_ += other->row_count_;
}

void DataBlock::AppendWith(const DataBlock *other, size_t from, size_t count) {
    if (other->column_count() != this->column_count()) {
        UnrecoverableError(
            fmt::format("Attempt merge block with column count {} into block with column count {}", other->column_count(), this->column_count()));
    }
    if (this->row_count_ + count > this->capacity_) {
        UnrecoverableError(fmt::format("Attempt append block with row count {} into block with row count{}, "
                                       "which exceeds the capacity {}",
                                       count,
                                       this->row_count(),
                                       this->capacity()));
    }
    size_t column_count = this->column_count();
    for (size_t idx = 0; idx < column_count; ++idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx], from, count);
    }
    row_count_ += count;
}

void DataBlock::InsertVector(const std::shared_ptr<ColumnVector> &vector, size_t index) {
    column_vectors.insert(column_vectors.begin() + index, vector);
    column_count_++;
}

bool DataBlock::operator==(const DataBlock &other) const {
    if (!this->initialized && !other.initialized)
        return true;
    if (!this->initialized || !other.initialized || this->column_count_ != other.column_count_)
        return false;
    for (size_t i = 0; i < this->column_count_; i++) {
        const std::shared_ptr<ColumnVector> &column1 = this->column_vectors[i];
        const std::shared_ptr<ColumnVector> &column2 = other.column_vectors[i];
        if (column1.get() == nullptr || column2.get() == nullptr || *column1 != *column2)
            return false;
    }
    return true;
}

i32 DataBlock::GetSizeInBytes() const {
    if (!finalized) {
        UnrecoverableError("Data block is not finalized.");
    }
    i32 size = sizeof(i32);
    for (size_t i = 0; i < column_count_; i++) {
        size += this->column_vectors[i]->GetSizeInBytes();
    }
    return size;
}

void DataBlock::WriteAdv(char *&ptr) const {
    if (!finalized) {
        UnrecoverableError("Data block is not finalized.");
    }
    WriteBufAdv<i32>(ptr, column_count_);
    for (size_t i = 0; i < column_count_; i++) {
        this->column_vectors[i]->WriteAdv(ptr);
    }
}

std::shared_ptr<DataBlock> DataBlock::ReadAdv(const char *&ptr, i32 maxbytes) {
    const char *const ptr_end = ptr + maxbytes;
    i32 column_count = ReadBufAdv<i32>(ptr);
    std::vector<std::shared_ptr<ColumnVector>> column_vectors;
    for (int i = 0; i < column_count; i++) {
        maxbytes = ptr_end - ptr;
        if (maxbytes <= 0) {
            UnrecoverableError("ptr goes out of range when reading DataBlock");
        }
        std::shared_ptr<ColumnVector> column_vector = ColumnVector::ReadAdv(ptr, maxbytes);
        column_vectors.push_back(column_vector);
    }
    std::shared_ptr<DataBlock> block = DataBlock::Make();
    block->Init(column_vectors);
    block->Finalize();
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        UnrecoverableError("ptr goes out of range when reading DataBlock");
    }
    return block;
}

} // namespace infinity
