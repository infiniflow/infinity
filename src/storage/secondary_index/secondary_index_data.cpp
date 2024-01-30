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

#include <concepts>
#include <vector>

module secondary_index_data;

import stl;
import parser;
import default_values;
import index_base;
import file_system;
import file_system_type;
import infinity_exception;
import column_vector;
import third_party;
import catalog;
import secondary_index_pgm;
import logger;

namespace infinity {

template <typename RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    static_assert(false, "type not supported");
}

template <KeepOrderedSelf RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value;
}

// DateT, TimeT
template <ConvertToOrderedI32 RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value.GetValue();
}

// DateTimeT, TimestampT
template <ConvertToOrderedI64 RawValueType>
ConvertToOrderedType<RawValueType> ConvertToOrderedKeyValue(RawValueType value) {
    return value.GetEpochTime();
}

// merge sort
template <typename RawValueType>
void SecondaryIndexDataBuilder<RawValueType>::AppendColumnVector(const void *append_ptr, u32 append_num) {
    if (append_num > input_block_max_row_) {
        UnrecoverableError("AppendColumnVector(): append_num > input_block_max_row_, need to separate input into several parts.");
    }
    if (size_ + append_num > data_num_) {
        UnrecoverableError("AppendColumnVector(): error: size_ + append_num > data_num_");
    }
    auto append_value_ptr = static_cast<const RawValueType *>(append_ptr);
    for (u32 i = 0; i < append_num; ++i) {
        // will convert RawValueType to KeyType
        append_key_offset_pair_[i] = {ConvertToOrderedKeyValue<RawValueType>(append_value_ptr[i]), size_ + i};
    }
    // 1. sort append_key_offset_pair_
    std::sort(append_key_offset_pair_.get(), append_key_offset_pair_.get() + append_num);
    // 2. merge sorted_key_offset_pair_ and append_key_offset_pair_
    for (u32 sorted_tail = size_, append_tail = append_num, result_tail = size_ + append_num; result_tail > 0;) {
        // when KeyType values are equal, we should put append_key_offset_pair_ in the tail
        if (append_tail == 0) {
            // sorted data already in sorted_key_offset_pair_
            break;
        } else if (sorted_tail == 0 or sorted_key_offset_pair_[sorted_tail - 1].first <= append_key_offset_pair_[append_tail - 1].first) {
            sorted_key_offset_pair_[--result_tail] = append_key_offset_pair_[--append_tail];
        } else {
            sorted_key_offset_pair_[--result_tail] = sorted_key_offset_pair_[--sorted_tail];
        }
    }
    // update size_
    size_ += append_num;
}

template <typename ValueT>
void SecondaryIndexDataBuilder<ValueT>::StartOutput() {
    if (size_ != data_num_) {
        UnrecoverableError("StartOutput(): error: size_ != data_num_, input data is not complete.");
    }
    output_row_progress_ = 0;
    output_part_progress_ = 0;
    // destroy append_key_offset_pair_
    append_key_offset_pair_.reset();
    // initialize sorted_keys_
    sorted_keys_ = MakeUniqueForOverwrite<KeyType[]>(data_num_);
    LOG_TRACE(fmt::format("StartOutput(), output_row_progress_: {}, data_num_: {}.", output_row_progress_, data_num_));
}

template <typename ValueT>
void SecondaryIndexDataBuilder<ValueT>::EndOutput() {
    if (output_row_progress_ != data_num_) {
        UnrecoverableError("EndOutput(): output is not complete: output_row_progress_ != data_num_.");
    }
    if (output_part_progress_ != output_part_num_ + 1) {
        UnrecoverableError("EndOutput(): output is not complete: output_part_progress_ != output_part_num_ + 1.");
    }
    // destroy sorted_keys_
    sorted_keys_.reset();
    LOG_TRACE(fmt::format("EndOutput(), output_row_progress_: {}, data_num_: {}.", output_row_progress_, data_num_));
}

// 1. output to part
template <typename ValueT>
void SecondaryIndexDataBuilder<ValueT>::OutputToPart(SecondaryIndexDataPart *index_part) {
    if (output_part_progress_ + 1 != index_part->part_id_) {
        UnrecoverableError("OutputToPart(): error: unexpected index_part->part_id_ value");
    }
    if (output_row_progress_ != output_part_progress_ * output_part_capacity_) {
        UnrecoverableError("OutputToPart(): error: output_row_progress_ != output_part_progress_ * output_part_capacity_");
    }
    if (output_part_progress_ >= output_part_num_) {
        UnrecoverableError("OutputToPart(): error: output_part_progress_ >= output_part_num_");
    }
    if (index_part->part_size_ != std::min(output_part_capacity_, data_num_ - output_row_progress_)) {
        UnrecoverableError("OutputToPart(): error: index_part->part_size_");
    }
    // 1. metadata
    {
        index_part->data_type_key_ = index_key_type_;
        index_part->data_type_offset_ = output_offset_type_;
    }
    // 2. raw data
    {
        auto data_type_key = MakeShared<DataType>(index_part->data_type_key_);
        index_part->column_key_ = MakeUnique<ColumnVector>(std::move(data_type_key));
        index_part->column_key_->Initialize();
        auto data_type_offset = MakeShared<DataType>(index_part->data_type_offset_);
        index_part->column_offset_ = MakeUnique<ColumnVector>(data_type_offset);
        index_part->column_offset_->Initialize();
        auto key_ptr = reinterpret_cast<KeyType *>(index_part->column_key_->data());
        auto offset_ptr = reinterpret_cast<OffsetType *>(index_part->column_offset_->data());
        static_assert(sizeof(IntegerT) == sizeof(OffsetType), "sizeof(IntegerT) != sizeof(u32), cannot use IntegerT as offset type");
        for (u32 i = 0; i < index_part->part_size_; ++i) {
            auto [key, offset] = sorted_key_offset_pair_[output_row_progress_ + i];
            key_ptr[i] = key;
            offset_ptr[i] = offset; // store u32 into IntegerT ColumnVector
        }
        // copy to sorted_keys_
        // used in OutputToHeader()
        std::copy(key_ptr, key_ptr + index_part->part_size_, sorted_keys_.get() + output_row_progress_);
        // finalize
        index_part->column_key_->Finalize(index_part->part_size_);
        index_part->column_offset_->Finalize(index_part->part_size_);
    }
    // 3. finish
    index_part->loaded_ = true;
    output_row_progress_ += index_part->part_size_;
    ++output_part_progress_;
    LOG_TRACE(fmt::format("OutputToPart(), output_row_progress_: {}, data_num_: {}.", output_row_progress_, data_num_));
}

// 2. output to header
template <typename ValueT>
void SecondaryIndexDataBuilder<ValueT>::OutputToHeader(SecondaryIndexDataHead *index_head) {
    if (output_part_progress_ != output_part_num_) {
        UnrecoverableError("OutputToHeader(): error: output_part_progress_ != output_part_num_, need to call OutputToHeader() after OutputToPart().");
    }
    if (output_row_progress_ != data_num_) {
        UnrecoverableError("OutputToHeader(): error: output_row_progress_ != data_num_, need to call OutputToHeader() after OutputToPart().");
    }
    // 1. metadata
    {
        if (index_head->data_num_ != data_num_) {
            UnrecoverableError("OutputToHeader(): error: index_head->data_num_ != data_num_");
        }
        if (index_head->part_capacity_ != output_part_capacity_) {
            UnrecoverableError("OutputToHeader(): error: index_head->part_capacity_ != output_part_capacity_");
        }
        if (index_head->part_num_ != output_part_num_) {
            UnrecoverableError("OutputToHeader(): error: index_head->part_num_ != output_part_num_");
        }
        // type of key and offset
        index_head->data_type_key_ = index_key_type_;
        index_head->data_type_offset_ = output_offset_type_;
    }
    // 2. pgm
    sorted_key_offset_pair_.reset(); // release some memory
    {
        index_head->pgm_index_ = GenerateSecondaryPGMIndex<KeyType>();
        index_head->pgm_index_->BuildIndex(data_num_, sorted_keys_.get());
        LOG_TRACE("OutputToHeader(): Successfully built pgm index.");
    }
    // 3. finish
    ++output_part_progress_;
    // now output_part_progress_ = output_part_num_ + 1
    index_head->loaded_ = true;
    LOG_TRACE(fmt::format("OutputToHeader(), output_row_progress_: {}, data_num_: {}.", output_row_progress_, data_num_));
}

UniquePtr<SecondaryIndexDataBuilderBase>
GetSecondaryIndexDataBuilder(const SharedPtr<DataType> &data_type, u32 data_num, u32 input_block_max_row, u32 part_capacity) {
    if (!(data_type->CanBuildSecondaryIndex())) {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
        return {};
    }
    switch (data_type->type()) {
        case LogicalType::kTinyInt: {
            return MakeUnique<SecondaryIndexDataBuilder<TinyIntT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kSmallInt: {
            return MakeUnique<SecondaryIndexDataBuilder<SmallIntT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kInteger: {
            return MakeUnique<SecondaryIndexDataBuilder<IntegerT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kBigInt: {
            return MakeUnique<SecondaryIndexDataBuilder<BigIntT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kFloat: {
            return MakeUnique<SecondaryIndexDataBuilder<FloatT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kDouble: {
            return MakeUnique<SecondaryIndexDataBuilder<DoubleT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kDate: {
            return MakeUnique<SecondaryIndexDataBuilder<DateT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kTime: {
            return MakeUnique<SecondaryIndexDataBuilder<TimeT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kDateTime: {
            return MakeUnique<SecondaryIndexDataBuilder<DateTimeT>>(data_num, input_block_max_row, part_capacity);
        }
        case LogicalType::kTimestamp: {
            return MakeUnique<SecondaryIndexDataBuilder<TimestampT>>(data_num, input_block_max_row, part_capacity);
        }
        default: {
            UnrecoverableError(fmt::format("Need to add secondary index support for data type: {}", data_type->ToString()));
            return {};
        }
    }
}

void SecondaryIndexDataHead::SaveIndexInner(FileHandler &file_handler) const {
    if (!loaded_) {
        UnrecoverableError("SaveIndexInner(): error: SecondaryIndexDataHead is not loaded");
    }
    file_handler.Write(&part_capacity_, sizeof(part_capacity_));
    file_handler.Write(&part_num_, sizeof(part_num_));
    file_handler.Write(&data_num_, sizeof(data_num_));
    file_handler.Write(&data_type_raw_, sizeof(data_type_raw_));
    file_handler.Write(&data_type_key_, sizeof(data_type_key_));
    file_handler.Write(&data_type_offset_, sizeof(data_type_offset_));
    // pgm
    pgm_index_->SaveIndex(file_handler);
    LOG_TRACE("SaveIndexInner() done.");
}

void SecondaryIndexDataHead::ReadIndexInner(FileHandler &file_handler) {
    if (loaded_) {
        UnrecoverableError("SecondaryIndexDataHead is already loaded");
    }
    file_handler.Read(&part_capacity_, sizeof(part_capacity_));
    file_handler.Read(&part_num_, sizeof(part_num_));
    file_handler.Read(&data_num_, sizeof(data_num_));
    file_handler.Read(&data_type_raw_, sizeof(data_type_raw_));
    file_handler.Read(&data_type_key_, sizeof(data_type_key_));
    file_handler.Read(&data_type_offset_, sizeof(data_type_offset_));
    // pgm
    pgm_index_->LoadIndex(file_handler);
    // update loaded_
    loaded_ = true;
    LOG_TRACE("ReadIndexInner() done.");
}

void SecondaryIndexDataPart::SaveIndexInner(FileHandler &file_handler) const {
    if (!loaded_) {
        UnrecoverableError("SaveIndexInner(): error: SecondaryIndexDataPart is not loaded");
    }
    file_handler.Write(&part_id_, sizeof(part_id_));
    file_handler.Write(&part_size_, sizeof(part_size_));
    file_handler.Write(&data_type_key_, sizeof(data_type_key_));
    file_handler.Write(&data_type_offset_, sizeof(data_type_offset_));
    // key
    if (u32 key_cnt = column_key_->Size(); key_cnt != part_size_) {
        UnrecoverableError("SaveIndexInner(): error: column_key_ size != part_size_.");
    }
    file_handler.Write(column_key_->data(), part_size_ * (column_key_->data_type_size_));
    // offset
    if (u32 offset_cnt = column_offset_->Size(); offset_cnt != part_size_) {
        UnrecoverableError("SaveIndexInner(): error: column_offset_ size != part_size_.");
    }
    file_handler.Write(column_offset_->data(), part_size_ * (column_offset_->data_type_size_));
    LOG_TRACE(fmt::format("SaveIndexInner() done. part_id_: {}.", part_id_));
}

void SecondaryIndexDataPart::ReadIndexInner(FileHandler &file_handler) {
    if (loaded_) {
        UnrecoverableError("SecondaryIndexDataPart is already loaded");
    }
    file_handler.Read(&part_id_, sizeof(part_id_));
    file_handler.Read(&part_size_, sizeof(part_size_));
    // key type
    file_handler.Read(&data_type_key_, sizeof(data_type_key_));
    auto data_type_key = MakeShared<DataType>(data_type_key_);
    // offset type
    file_handler.Read(&data_type_offset_, sizeof(data_type_offset_));
    if (data_type_offset_ != LogicalType::kInteger) {
        UnrecoverableError("ReadIndexInner(): data_type_offset_ != LogicalType::kInteger");
    }
    auto data_type_offset = MakeShared<DataType>(data_type_offset_);
    // key
    column_key_ = MakeUnique<ColumnVector>(std::move(data_type_key));
    column_key_->Initialize();
    file_handler.Read(column_key_->data(), part_size_ * (column_key_->data_type_size_));
    column_key_->Finalize(part_size_);
    // offset
    column_offset_ = MakeUnique<ColumnVector>(std::move(data_type_offset));
    column_offset_->Initialize();
    file_handler.Read(column_offset_->data(), part_size_ * (column_offset_->data_type_size_));
    column_offset_->Finalize(part_size_);
    // update loaded_
    loaded_ = true;
    LOG_TRACE(fmt::format("ReadIndexInner() done. part_id_: {}.", part_id_));
}

} // namespace infinity