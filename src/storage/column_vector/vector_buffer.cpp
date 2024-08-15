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

module vector_buffer;

import stl;
import fix_heap;
import buffer_obj;
import buffer_manager;
import buffer_handle;
import infinity_exception;
import block_column_entry;
import default_values;
import logger;
import third_party;
import serialize;

namespace infinity {

SharedPtr<VectorBuffer> VectorBuffer::Make(const SizeT data_type_size, const SizeT capacity, VectorBufferType buffer_type) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
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

SharedPtr<VectorBuffer> VectorBuffer::Make(BufferManager *buffer_mgr,
                                           BlockColumnEntry *block_column_entry,
                                           const SizeT data_type_size,
                                           const SizeT capacity,
                                           const VectorBufferType buffer_type) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
    buffer_ptr->buffer_type_ = buffer_type;
    switch (buffer_type) {
        case VectorBufferType::kCompactBit: {
            buffer_ptr->InitializeCompactBit(buffer_mgr, block_column_entry, capacity);
            break;
        }
        default: {
            buffer_ptr->Initialize(buffer_mgr, block_column_entry, data_type_size, capacity);
            break;
        }
    }
    return buffer_ptr;
}

void VectorBuffer::InitializeCompactBit(SizeT capacity) {
    if (initialized_) {
        String error_message = "Vector buffer is already initialized.";
        UnrecoverableError(error_message);
    }
    SizeT data_size = (capacity + 7) / 8;
    if (data_size > 0) {
        ptr_ = MakeUniqueForOverwrite<char[]>(data_size);
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::Initialize(SizeT type_size, SizeT capacity) {
    if (initialized_) {
        String error_message = "Vector buffer is already initialized.";
        UnrecoverableError(error_message);
    }
    SizeT data_size = type_size * capacity;
    if (data_size > 0) {
        ptr_ = MakeUniqueForOverwrite<char[]>(data_size);
    }
    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = MakeUnique<VarBufferManager>();
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::InitializeCompactBit(BufferManager *buffer_mgr, BlockColumnEntry *block_column_entry, SizeT capacity) {
    if (initialized_) {
        String error_message = "Vector buffer is already initialized.";
        UnrecoverableError(error_message);
    }
    SizeT data_size = (capacity + 7) / 8;
    auto *buffer_obj = block_column_entry->buffer();
    if (buffer_obj == nullptr) {
        String error_message = "Buffer object is nullptr.";
        UnrecoverableError(error_message);
    }
    if (buffer_obj->GetBufferSize() != data_size) {
        String error_message = "Buffer object size is not equal to data size.";
        UnrecoverableError(error_message);
    }
    ptr_ = buffer_obj->Load();
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::Initialize(BufferManager *buffer_mgr, BlockColumnEntry *block_column_entry, SizeT type_size, SizeT capacity) {
    if (initialized_) {
        String error_message = "Vector buffer is already initialized.";
        UnrecoverableError(error_message);
    }
    SizeT data_size = type_size * capacity;
    auto *buffer_obj = block_column_entry->buffer();
    if (buffer_obj == nullptr) {
        String error_message = "Buffer object is nullptr.";
        UnrecoverableError(error_message);
    }
    if (buffer_obj->GetBufferSize() != data_size) {
        String error_message = "Buffer object size is not equal to data size.";
        UnrecoverableError(error_message);
    }
    ptr_ = buffer_obj->Load();
    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = MakeUnique<VarBufferManager>(block_column_entry, buffer_mgr);
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::ResetToInit(VectorBufferType type) {
    if (type == VectorBufferType::kVarBuffer) {
        if (var_buffer_mgr_.get() != nullptr) {
            String error_message = "Vector heap should be null.";
            UnrecoverableError(error_message);
        }
    }

    if (buffer_type_ == VectorBufferType::kVarBuffer) {
        var_buffer_mgr_ = MakeUnique<VarBufferManager>();
    }
}

void VectorBuffer::Copy(ptr_t input, SizeT size) {
    if (data_size_ < size) {
        String error_message = "Attempt to copy an amount of data that cannot currently be accommodated";
        UnrecoverableError(error_message);
    }
    // std::memcpy(data_.get(), input, size);
    std::memcpy(GetDataMut(), input, size);
}

bool VectorBuffer::RawPointerGetCompactBit(const u8 *src_ptr_u8, SizeT idx) {
    SizeT byte_idx = idx / 8;
    SizeT bit_idx = idx % 8;
    return (src_ptr_u8[byte_idx] & (u8(1) << bit_idx)) != 0;
}

bool VectorBuffer::GetCompactBit(SizeT idx) const {
    if (idx >= capacity_) {
        String error_message = "Index out of range.";
        UnrecoverableError(error_message);
    }
    return VectorBuffer::RawPointerGetCompactBit(reinterpret_cast<const u8 *>(GetData()), idx);
}

void VectorBuffer::RawPointerSetCompactBit(u8 *dst_ptr_u8, SizeT idx, bool val) {
    SizeT byte_idx = idx / 8;
    SizeT bit_idx = idx % 8;
    if (val) {
        dst_ptr_u8[byte_idx] |= (u8(1) << bit_idx);
    } else {
        dst_ptr_u8[byte_idx] &= ~(u8(1) << bit_idx);
    }
}

void VectorBuffer::SetCompactBit(SizeT idx, bool val) {
    if (idx >= capacity_) {
        String error_message = "Index out of range.";
        UnrecoverableError(error_message);
    }
    VectorBuffer::RawPointerSetCompactBit(reinterpret_cast<u8 *>(GetDataMut()), idx, val);
}

bool VectorBuffer::CompactBitIsSame(const SharedPtr<VectorBuffer> &lhs, SizeT lhs_cnt, const SharedPtr<VectorBuffer> &rhs, SizeT rhs_cnt) {
    if (lhs_cnt != rhs_cnt) {
        return false;
    }
    if (lhs_cnt == 0) {
        return true;
    }
    SizeT full_byte_cnt = lhs_cnt / 8;
    SizeT last_byte_cnt = lhs_cnt % 8;
    auto lhs_data = reinterpret_cast<const u8 *>(lhs->GetData());
    auto rhs_data = reinterpret_cast<const u8 *>(rhs->GetData());
    for (SizeT idx = 0; idx < full_byte_cnt; ++idx) {
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
void VectorBuffer::CopyCompactBits(u8 *dst_ptr_u8, const u8 *src_ptr_u8, SizeT dest_start_idx, SizeT source_start_idx, SizeT count) {
    if ((source_start_idx % 8) == (dest_start_idx % 8)) {
        SizeT start_offset = source_start_idx % 8;
        SizeT start_todo = 8 - start_offset;
        SizeT extra;
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
        if (SizeT start_offset = dest_start_idx % 8; start_offset != 0) {
            // head part
            SizeT start_todo = std::min(count, 8 - start_offset);
            for (SizeT i = 0; i < start_todo; ++i) {
                RawPointerSetCompactBit(dst_ptr_u8, dest_start_idx++, RawPointerGetCompactBit(src_ptr_u8, source_start_idx++));
            }
            count -= start_todo;
        }
        SizeT byte_count = count / 8;
        SizeT dst_byte_start_idx = dest_start_idx / 8;
        SizeT source_byte_start_idx = source_start_idx / 8;
        SizeT source_bit_start_idx = source_start_idx % 8;
        u8 mask_1 = u8(0xff) << source_bit_start_idx;
        u8 mask_2 = ~mask_1;
        for (SizeT i = 0; i < byte_count; ++i) {
            // dst should get 2 parts from 2 different src bytes.
            u8 src_1 = src_ptr_u8[source_byte_start_idx + i];
            u8 src_2 = src_ptr_u8[source_byte_start_idx + i + 1];
            dst_ptr_u8[dst_byte_start_idx + i] = (src_1 & mask_1) | (src_2 & mask_2);
        }
        // tail part
        if (SizeT tail_count = count % 8; tail_count) {
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

SizeT VectorBuffer::TotalSize(const DataType *data_type) const {
    SizeT size = 0;
    if (const auto data_t = data_type->type(); data_t == kVarchar || data_t == kSparse || data_t == kTensor or data_t == kTensorArray) {
        size += sizeof(i32) + var_buffer_mgr_->TotalSize();
    }
    return size;
}

void VectorBuffer::WriteAdv(char *&ptr, const DataType *data_type) const {
    if (const auto data_t = data_type->type(); data_t == kVarchar || data_t == kSparse || data_t == kTensor or data_t == kTensorArray) {
        SizeT heap_len = var_buffer_mgr_->TotalSize();
        WriteBufAdv<i32>(ptr, heap_len);
        SizeT write_n = var_buffer_mgr_->Write(ptr);
        if (write_n != heap_len) {
            String error_message = "Failed to write var buffer";
            UnrecoverableError(error_message);
        }
        ptr += heap_len;
    }
}

void VectorBuffer::ReadAdv(char *&ptr, const DataType *data_type) {
    if (const auto data_t = data_type->type(); data_t == kVarchar || data_t == kSparse || data_t == kTensor or data_t == kTensorArray) {
        SizeT heap_len = ReadBufAdv<i32>(ptr);
        [[maybe_unused]] SizeT offset = this->AppendVarchar(ptr, heap_len);
        ptr += heap_len;
    }
}

const char *VectorBuffer::GetVarchar(SizeT offset, SizeT len) const { return var_buffer_mgr_->Get(offset, len); }

SizeT VectorBuffer::AppendVarchar(const char *data, SizeT len) { return var_buffer_mgr_->Append(data, len); }

Pair<const char *, const char *> VectorBuffer::GetSparseRaw(SizeT offset, SizeT nnz, const SparseInfo *sparse_info) const {
    if (nnz == 0) {
        return {nullptr, nullptr};
    }
    SizeT indice_size = sparse_info->IndiceSize(nnz);
    SizeT data_size = sparse_info->DataSize(nnz);
    const char *raw_indice_ptr = var_buffer_mgr_->Get(offset, indice_size);
    const char *raw_data_ptr = nullptr;
    if (data_size > 0) {
        raw_data_ptr = var_buffer_mgr_->Get(offset + indice_size, data_size);
    }
    return {raw_data_ptr, raw_indice_ptr};
}

SizeT VectorBuffer::AppendSparseRaw(const char *raw_data, const char *raw_idx, SizeT nnz, const SparseInfo *sparse_info) {
    SizeT indice_size = sparse_info->IndiceSize(nnz);
    SizeT data_size = sparse_info->DataSize(nnz);
    SizeT file_offset = var_buffer_mgr_->Append(raw_idx, indice_size);
    if (raw_data != nullptr) {
        var_buffer_mgr_->Append(raw_data, data_size);
    }
    return file_offset;
}

const char *VectorBuffer::GetTensorRaw(SizeT offset, SizeT size) const { return var_buffer_mgr_->Get(offset, size); }

SizeT VectorBuffer::AppendTensorRaw(const char *raw_data, SizeT size) { return var_buffer_mgr_->Append(raw_data, size); }

const TensorT *VectorBuffer::GetTensorArrayMeta(SizeT offset, SizeT arraw_size) const {
    SizeT bytes = arraw_size * sizeof(TensorT);
    return reinterpret_cast<const TensorT *>(var_buffer_mgr_->Get(offset, bytes));
}

SizeT VectorBuffer::AppendTensorArrayMeta(Span<const TensorT> tensor_metas) const {
    SizeT tensor_meta_size = tensor_metas.size() * sizeof(TensorT);
    return var_buffer_mgr_->Append(reinterpret_cast<const char *>(tensor_metas.data()), tensor_meta_size);
}

} // namespace infinity
