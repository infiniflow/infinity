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

module infinity_core:vector_buffer.impl;

import :vector_buffer;
import :fileworker_manager;
import :infinity_exception;
import :default_values;

import third_party;
import serialize;
import internal_types;
import logical_type;
import sparse_info;
import data_type;

namespace infinity {

std::shared_ptr<VectorBuffer> VectorBuffer::Make(const size_t data_type_size, const size_t capacity, VectorBufferType buffer_type) {
    std::shared_ptr<VectorBuffer> buffer_ptr = std::make_shared<VectorBuffer>();
    buffer_ptr->buffer_type_ = buffer_type;
    switch (buffer_type) {
        case VectorBufferType::kCompactBit: {
            buffer_ptr->InitializeCompactBit(capacity);
            break;
        }
        default: {
            buffer_ptr->Initialize(data_type_size, capacity);
            break;
        }
    }
    return buffer_ptr;
}

std::shared_ptr<VectorBuffer>
VectorBuffer::Make(FileWorker *buffer_obj, FileWorker *outline_buffer_obj, size_t data_type_size, size_t capacity, VectorBufferType buffer_type) {
    std::shared_ptr<VectorBuffer> buffer_ptr = std::make_shared<VectorBuffer>();
    buffer_ptr->buffer_type_ = buffer_type;
    switch (buffer_type) {
        case VectorBufferType::kCompactBit: {
            buffer_ptr->InitializeCompactBit(buffer_obj, capacity);
            break;
        }
        default: {
            buffer_ptr->Initialize(buffer_obj, outline_buffer_obj, data_type_size, capacity);
            break;
        }
    }
    return buffer_ptr;
}

void VectorBuffer::InitializeCompactBit(size_t capacity) {
    if (initialized_) {
        UnrecoverableError("std::vector buffer is already initialized.");
    }
    size_t data_size = (capacity + 7) / 8;
    if (data_size > 0) {
        ptr_ = std::make_unique_for_overwrite<char[]>(data_size);
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::Initialize(size_t type_size, size_t capacity) {
    if (initialized_) {
        UnrecoverableError("std::vector buffer is already initialized.");
    }
    size_t data_size = type_size * capacity;
    if (data_size > 0) {
        ptr_ = std::make_unique_for_overwrite<char[]>(data_size);
    }
    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = std::make_unique<VarBufferManager>();
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::InitializeCompactBit(FileWorker *buffer_obj, size_t capacity) {
    if (initialized_) {
        UnrecoverableError("std::vector buffer is already initialized.");
    }
    size_t data_size = (capacity + 7) / 8;
    if (buffer_obj == nullptr) {
        UnrecoverableError("Buffer object is nullptr.");
    }
    // if (buffer_obj->GetBufferSize() != data_size) {
    //     UnrecoverableError("Buffer object size is not equal to data size.");
    // }
    // ptr_ = buffer_obj->Load();
    ptr_ = buffer_obj;
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::Initialize(FileWorker *buffer_obj, FileWorker *outline_buffer_obj, size_t type_size, size_t capacity) {
    if (initialized_) {
        UnrecoverableError("std::vector buffer is already initialized.");
    }
    size_t data_size = type_size * capacity;
    if (buffer_obj == nullptr) {
        UnrecoverableError("Buffer object is nullptr.");
    }
    // if (buffer_obj->GetBufferSize() != data_size) {
    //     UnrecoverableError("Buffer object size is not equal to data size.");
    // }
    // ptr_ = buffer_obj->Load();
    ptr_ = buffer_obj;
    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = std::make_unique<VarBufferManager>(outline_buffer_obj);
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::SetToCatalog(FileWorker *buffer_obj, FileWorker *outline_buffer_obj) {
    if (!std::holds_alternative<std::unique_ptr<char[]>>(ptr_)) {
        UnrecoverableError("Cannot convert to new catalog");
    }

    void *src_ptr = std::get<std::unique_ptr<char[]>>(ptr_).release();
    buffer_obj->SetData(src_ptr);

    // ptr_ = buffer_obj->Load();
    ptr_ = buffer_obj;
    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_->SetToCatalog(outline_buffer_obj);
    }
}

void VectorBuffer::ResetToInit(VectorBufferType type) {
    if (type == VectorBufferType::kVarBuffer) {
        if (var_buffer_mgr_.get() != nullptr) {
            UnrecoverableError("std::vector heap should be null.");
        }
    }

    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = std::make_unique<VarBufferManager>();
    }
}

void VectorBuffer::Copy(char *input, size_t size) {
    if (data_size_ < size) {
        UnrecoverableError("Attempt to copy an amount of data that cannot currently be accommodated");
    }
    // std::memcpy(data_.get(), input, size);
    std::memcpy(GetDataMut(), input, size);
}

bool VectorBuffer::RawPointerGetCompactBit(const u8 *src_ptr_u8, size_t idx) {
    size_t byte_idx = idx / 8;
    size_t bit_idx = idx % 8;
    return (src_ptr_u8[byte_idx] & (u8(1) << bit_idx)) != 0;
}

bool VectorBuffer::GetCompactBit(size_t idx) const {
    if (idx >= capacity_) {
        UnrecoverableError("Index out of range.");
    }
    return VectorBuffer::RawPointerGetCompactBit(reinterpret_cast<const u8 *>(GetData()), idx);
}

void VectorBuffer::RawPointerSetCompactBit(u8 *dst_ptr_u8, size_t idx, bool val) {
    size_t byte_idx = idx / 8;
    size_t bit_idx = idx % 8;
    if (val) {
        dst_ptr_u8[byte_idx] |= (u8(1) << bit_idx);
    } else {
        dst_ptr_u8[byte_idx] &= ~(u8(1) << bit_idx);
    }
}

void VectorBuffer::SetCompactBit(size_t idx, bool val) {
    if (idx >= capacity_) {
        UnrecoverableError("Index out of range.");
    }
    VectorBuffer::RawPointerSetCompactBit(reinterpret_cast<u8 *>(GetDataMut()), idx, val);
}

bool VectorBuffer::CompactBitIsSame(const std::shared_ptr<VectorBuffer> &lhs,
                                    size_t lhs_cnt,
                                    const std::shared_ptr<VectorBuffer> &rhs,
                                    size_t rhs_cnt) {
    if (lhs_cnt != rhs_cnt) {
        return false;
    }
    if (lhs_cnt == 0) {
        return true;
    }
    size_t full_byte_cnt = lhs_cnt / 8;
    size_t last_byte_cnt = lhs_cnt % 8;
    auto lhs_data = reinterpret_cast<const u8 *>(lhs->GetData());
    auto rhs_data = reinterpret_cast<const u8 *>(rhs->GetData());
    for (size_t idx = 0; idx < full_byte_cnt; ++idx) {
        if (lhs_data[idx] != rhs_data[idx]) {
            return false;
        }
    }
    if (last_byte_cnt > 0) {
        // las_byte_cnt is in [1, 7]
        u8 mask = (1 << last_byte_cnt) - 1;
        if ((lhs_data[full_byte_cnt] & mask) != (rhs_data[full_byte_cnt] & mask)) {
            return false;
        }
    }
    return true;
}

// TODO: Verify this function by unittests.
void VectorBuffer::CopyCompactBits(u8 *dst_ptr_u8, const u8 *src_ptr_u8, size_t dest_start_idx, size_t source_start_idx, size_t count) {
    if ((source_start_idx % 8) == (dest_start_idx % 8)) {
        size_t start_offset = source_start_idx % 8;
        size_t start_todo = 8 - start_offset;
        size_t extra;
        if (start_offset == 0) {
            // Copy by byte when the start index is aligned.
            std::memcpy(dst_ptr_u8 + dest_start_idx / 8, src_ptr_u8 + source_start_idx / 8, count / 8);
            // Copy the last bits.
            extra = count % 8;
        } else {
            u8 src_1 = src_ptr_u8[source_start_idx / 8];
            u8 &dst_1 = dst_ptr_u8[dest_start_idx / 8];
            if (count <= start_todo) {
                u8 source_mask = (u8(0xff) << start_offset) & (~(u8(0xff) << (start_offset + count)));
                dst_1 = (dst_1 & (~source_mask)) | (src_1 & source_mask);
                return;
            } else {
                u8 source_mask = u8(0xff) << start_offset;
                dst_1 = (dst_1 & (~source_mask)) | (src_1 & source_mask);
                std::memcpy(dst_ptr_u8 + (dest_start_idx / 8) + 1, src_ptr_u8 + (source_start_idx / 8) + 1, (count - start_todo) / 8);
                extra = (count - start_todo) % 8;
            }
        }
        if (extra != 0) {
            u8 src = src_ptr_u8[(source_start_idx + count) / 8];
            u8 &dst = dst_ptr_u8[(dest_start_idx + count) / 8];
            u8 extra_mask = ~(u8(0xff) << extra);
            dst = (dst & (~extra_mask)) | (src & extra_mask);
        }
    } else {
        if (size_t start_offset = dest_start_idx % 8; start_offset != 0) {
            // head part
            size_t start_todo = std::min(count, 8 - start_offset);
            for (size_t i = 0; i < start_todo; ++i) {
                RawPointerSetCompactBit(dst_ptr_u8, dest_start_idx++, RawPointerGetCompactBit(src_ptr_u8, source_start_idx++));
            }
            count -= start_todo;
        }
        size_t byte_count = count / 8;
        size_t dst_byte_start_idx = dest_start_idx / 8;
        size_t source_byte_start_idx = source_start_idx / 8;
        size_t source_bit_start_idx = source_start_idx % 8;
        u8 mask_1 = u8(0xff) << source_bit_start_idx;
        u8 mask_2 = ~mask_1;
        for (size_t i = 0; i < byte_count; ++i) {
            // dst should get 2 parts from 2 different src bytes.
            u8 src_1 = src_ptr_u8[source_byte_start_idx + i];
            u8 src_2 = src_ptr_u8[source_byte_start_idx + i + 1];
            dst_ptr_u8[dst_byte_start_idx + i] = (src_1 & mask_1) | (src_2 & mask_2);
        }
        // tail part
        if (size_t tail_count = count % 8; tail_count) {
            u8 src_1 = src_ptr_u8[source_byte_start_idx + byte_count];
            u8 src_2 = src_ptr_u8[source_byte_start_idx + byte_count + 1];
            u8 tail_full = (src_1 & mask_1) | (src_2 & mask_2);
            u8 tail_keep = u8(0xff) << tail_count;
            u8 tail_set = ~tail_keep;
            u8 &dst = dst_ptr_u8[dst_byte_start_idx + byte_count];
            dst = (dst & tail_keep) | (tail_full & tail_set);
        }
    }
}

size_t VectorBuffer::TotalSize(const DataType *data_type) const {
    size_t size = 0;
    switch (data_type->type()) {
        case LogicalType::kArray:
        case LogicalType::kVarchar:
        case LogicalType::kSparse:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            size += sizeof(i32) + var_buffer_mgr_->TotalSize();
            break;
        }
        default: {
            break;
        }
    }
    return size;
}

void VectorBuffer::WriteAdv(char *&ptr, const DataType *data_type) const {
    switch (data_type->type()) {
        case LogicalType::kArray:
        case LogicalType::kVarchar:
        case LogicalType::kSparse:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            const auto heap_len = var_buffer_mgr_->TotalSize();
            WriteBufAdv<i32>(ptr, heap_len);
            if (const auto write_n = var_buffer_mgr_->Write(ptr); write_n != heap_len) {
                UnrecoverableError("Failed to write var buffer");
            }
            ptr += heap_len;
            break;
        }
        default: {
            break;
        }
    }
}

void VectorBuffer::ReadAdv(const char *&ptr, const DataType *data_type) {
    switch (data_type->type()) {
        case LogicalType::kArray:
        case LogicalType::kVarchar:
        case LogicalType::kSparse:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            const auto heap_len = ReadBufAdv<i32>(ptr);
            [[maybe_unused]] size_t offset = this->AppendVarchar(ptr, heap_len);
            ptr += heap_len;
            break;
        }
        default: {
            break;
        }
    }
}

const char *VectorBuffer::GetVarchar(size_t offset, size_t len) const { return var_buffer_mgr_->Get(offset, len); }

size_t VectorBuffer::AppendVarchar(const char *data, size_t len) { return var_buffer_mgr_->Append(data, len); }

std::pair<const char *, const char *> VectorBuffer::GetSparseRaw(size_t offset, size_t nnz, const SparseInfo *sparse_info) const {
    if (nnz == 0) {
        return {nullptr, nullptr};
    }
    size_t indice_size = sparse_info->IndiceSize(nnz);
    size_t data_size = sparse_info->DataSize(nnz);
    const char *raw_indice_ptr = var_buffer_mgr_->Get(offset, indice_size);
    const char *raw_data_ptr = nullptr;
    if (data_size > 0) {
        raw_data_ptr = var_buffer_mgr_->Get(offset + indice_size, data_size);
    }
    return {raw_data_ptr, raw_indice_ptr};
}

size_t VectorBuffer::AppendSparseRaw(const char *raw_data, const char *raw_idx, size_t nnz, const SparseInfo *sparse_info) {
    size_t indice_size = sparse_info->IndiceSize(nnz);
    size_t data_size = sparse_info->DataSize(nnz);
    size_t file_offset = var_buffer_mgr_->Append(raw_idx, indice_size);
    if (raw_data != nullptr) {
        var_buffer_mgr_->Append(raw_data, data_size);
    }
    return file_offset;
}

const char *VectorBuffer::GetMultiVectorRaw(size_t offset, size_t size) const { return var_buffer_mgr_->Get(offset, size); }

size_t VectorBuffer::AppendMultiVectorRaw(const char *raw_data, size_t size) { return var_buffer_mgr_->Append(raw_data, size); }

const char *VectorBuffer::GetTensorRaw(size_t offset, size_t size) const { return var_buffer_mgr_->Get(offset, size); }

size_t VectorBuffer::AppendTensorRaw(const char *raw_data, size_t size) { return var_buffer_mgr_->Append(raw_data, size); }

const char *VectorBuffer::GetTensorArrayMeta(size_t offset, size_t array_num) const {
    size_t bytes = array_num * sizeof(TensorT);
    return var_buffer_mgr_->Get(offset, bytes);
}

size_t VectorBuffer::AppendTensorArrayMeta(std::span<const TensorT> tensor_metas) const {
    size_t tensor_meta_size = tensor_metas.size() * sizeof(TensorT);
    return var_buffer_mgr_->Append(reinterpret_cast<const char *>(tensor_metas.data()), tensor_meta_size);
}

const char *VectorBuffer::GetArrayRaw(size_t offset, size_t size) const { return var_buffer_mgr_->Get(offset, size); }

size_t VectorBuffer::AppendArrayRaw(const char *raw_data, size_t size) const { return var_buffer_mgr_->Append(raw_data, size); }

} // namespace infinity
