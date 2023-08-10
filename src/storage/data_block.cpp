//
// Created by JinHai on 2022/11/12.
//

#include "data_block.h"

namespace infinity {

void
DataBlock::Init(const SharedPtr<DataBlock>& input, const SharedPtr<Selection>& input_select) {
    StorageAssert(!initialized, "Data block was initialized before.");
    StorageAssert(input != nullptr && input_select != nullptr, "Invalid input data block or select")
    column_count_ = input->column_count();
    StorageAssert(column_count_ > 0, "Empty column vectors.")
    column_vectors.reserve(column_count_);
    for(SizeT idx = 0; idx < column_count_; ++ idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(input->column_vectors[idx]->data_type()));
        column_vectors.back()->Initialize(*(input->column_vectors[idx]), *input_select);
    }
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    this->Finalize();
}

void
DataBlock::Init(const SharedPtr<DataBlock>& input, SizeT start_idx, SizeT end_idx) {
    StorageAssert(!initialized, "Data block was initialized before.");
    StorageAssert(input != nullptr, "Invalid input data block");
    column_count_ = input->column_count();
    StorageAssert(column_count_ > 0, "Empty column vectors.")
    column_vectors.reserve(column_count_);
    for(SizeT idx = 0; idx < column_count_; ++ idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(input->column_vectors[idx]->data_type()));
        column_vectors.back()->Initialize(*(input->column_vectors[idx]), start_idx, end_idx);
    }
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    this->Finalize();
}

void
DataBlock::Init(const Vector<SharedPtr<DataType>>& types, SizeT capacity) {
    StorageAssert(!initialized, "Data block was initialized before.");
    StorageAssert(!types.empty(), "Empty data types collection.")
    column_count_ = types.size();
    column_vectors.reserve(column_count_);
    for(SizeT idx = 0; idx < column_count_; ++ idx) {
        column_vectors.emplace_back(MakeShared<ColumnVector>(types[idx]));
        column_vectors[idx]->Initialize(ColumnVectorType::kFlat, capacity);
    }
    capacity_ = capacity;
    initialized = true;
}

void
DataBlock::Init(const Vector<SharedPtr<ColumnVector>>& input_vectors) {
    StorageAssert(!input_vectors.empty(), "Empty column vectors.")
    column_count_ = input_vectors.size();
    column_vectors = input_vectors;
    capacity_ = column_vectors[0]->capacity();
    initialized = true;
    Finalize();
}

void
DataBlock::Reset() {

    // Reset behavior:
    // Reset each column into just initialized status.
    // No data is appended into any column.

    for(SizeT i = 0; i < column_count_; ++ i) {
        column_vectors[i]->Reset();
        column_vectors[i]->Initialize();
    }

    row_count_ = 0;
}

Value
DataBlock::GetValue(SizeT column_index, SizeT row_index) const {
    return column_vectors[column_index]->GetValue(row_index);
}

void
DataBlock::SetValue(SizeT column_index, SizeT row_index, const Value &val) {
    StorageAssert(column_index < column_count_,
                  "Attempt to access invalid column index: " + std::to_string(column_index));
    column_vectors[column_index]->SetValue(row_index, val);
}

void
DataBlock::AppendValue(SizeT column_index, const Value& value) {
    StorageAssert(column_index < column_count_,
                  "Attempt to access invalid column index: " + std::to_string(column_index) +
                  " in column count: " + std::to_string(column_count_));
    column_vectors[column_index]->AppendValue(value);
    finalized = false;
}

void
DataBlock::Finalize() {
    bool first_flat_column_vector = false;
    SizeT row_count = 0;
    for(SizeT idx = 0; idx < column_count_; ++ idx) {
        if(column_vectors[idx]->vector_type() == ColumnVectorType::kConstant) {
            continue;
        } else {
            if(first_flat_column_vector) {
                if(row_count != column_vectors[idx]->Size()) {
                    StorageError("Column vectors in same data block have different size.")
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

String
DataBlock::ToString() const {
    std::stringstream ss;
    for(SizeT idx = 0; idx < column_count_; ++ idx) {
        ss << "column " << idx << std::endl;
        ss << column_vectors[idx]->ToString() << std::endl;
    }
    return ss.str();
}

void
DataBlock::FillRowIDVector(SharedPtr<Vector<RowID>>& row_ids, u32 block_id) const {
    StorageAssert(finalized, "DataBlock isn't finalized.")
    for(u32 offset = 0; offset < row_count_; ++ offset) {
        row_ids->emplace_back(block_id, offset);
    }
}

void
DataBlock::UnionWith(const SharedPtr<DataBlock>& other) {
    StorageAssert(this->row_count_ == other->row_count_, "Attempt to union two block with different row count");
    StorageAssert(this->capacity_ == other->capacity_, "Attempt to union two block with different row count");
    StorageAssert(this->initialized && other->initialized, "Attempt to union two uninitialized blocks");
    StorageAssert(this->finalized == other->finalized, "Attempt to union two block with different finalized status");
    column_count_ += other->column_count_;
    column_vectors.reserve(column_count_);
    column_vectors.insert(column_vectors.end(), other->column_vectors.begin(), other->column_vectors.end());
}

void
DataBlock::AppendWith(const SharedPtr<DataBlock>& other) {
    if(other->column_count() != this->column_count()) {
        StorageError(fmt::format("Attempt merge block with column count {} into block with column count {}",
                                 other->column_count(),
                                 this->column_count()));
    }
    if(this->row_count_ + other->row_count_ > this->capacity_) {
        StorageError(fmt::format("Attempt append block with row count {} into block with row count {}, "
                                 "which exceeds the capacity {}",
                                 other->row_count(),
                                 this->row_count(),
                                 this->capacity()));
    }

    SizeT column_count = this->column_count();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx]);
    }
}

void
DataBlock::AppendWith(const SharedPtr<DataBlock>& other, SizeT from, SizeT count) {
    if(other->column_count() != this->column_count()) {
        StorageError(fmt::format("Attempt merge block with column count {} into block with column count {}",
                                 other->column_count(),
                                 this->column_count()));
    }
    if(this->row_count_ + count > this->capacity_) {
        StorageError(fmt::format("Attempt append block with row count {} into block with row count{}, "
                                 "which exceeds the capacity {}",
                                 count,
                                 this->row_count(),
                                 this->capacity()));
    }
    SizeT column_count = this->column_count();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        this->column_vectors[idx]->AppendWith(*other->column_vectors[idx], from, count);
    }
}

}


