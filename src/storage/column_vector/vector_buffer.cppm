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

export module vector_buffer;

import stl;
import global_resource_usage;
import heap_chunk;
import fix_heap;
import buffer_handle;
import var_buffer;
import data_type;
import sparse_util;
import sparse_info;
import internal_types;

namespace infinity {

class BufferManager;
class BlockColumnEntry;

export enum class VectorBufferType {
    kInvalid,
    kStandard,
    kHeap,
    kCompactBit,
    kVarBuffer, // new varchar
};

export class VectorBuffer {
public:
    static SharedPtr<VectorBuffer> Make(SizeT data_type_size, SizeT capacity, VectorBufferType buffer_type);

    static SharedPtr<VectorBuffer>
    Make(BufferManager *buffer_mgr, BlockColumnEntry *block_column_entry, SizeT data_type_size, SizeT capacity, VectorBufferType buffer_types);

public:
    explicit VectorBuffer() {
        GlobalResourceUsage::IncrObjectCount("VectorBuffer");
    }

    ~VectorBuffer() {
        GlobalResourceUsage::DecrObjectCount("VectorBuffer");
    }

    void Initialize(SizeT type_size, SizeT capacity);

    void InitializeCompactBit(SizeT capacity);

    void InitializeCompactBit(BufferManager *buffer_mgr, BlockColumnEntry *block_column_entry, SizeT capacity);

    void Initialize(BufferManager *buffer_mgr, BlockColumnEntry *block_column_entry, SizeT type_size, SizeT capacity);

    void ResetToInit(VectorBufferType type);

    void Copy(ptr_t input, SizeT size);

    [[nodiscard]] ptr_t GetDataMut() {
        if (std::holds_alternative<UniquePtr<char[]>>(ptr_)) {
            return std::get<UniquePtr<char[]>>(ptr_).get();
        } else {
            return static_cast<ptr_t>(std::get<BufferHandle>(ptr_).GetDataMut());
        }
    }

    [[nodiscard]] const_ptr_t GetData() const {
        if (std::holds_alternative<UniquePtr<char[]>>(ptr_)) {
            return std::get<UniquePtr<char[]>>(ptr_).get();
        } else {
            return static_cast<const_ptr_t>(std::get<BufferHandle>(ptr_).GetData());
        }
    }

    [[nodiscard]] bool GetCompactBit(SizeT idx) const;

    void SetCompactBit(SizeT idx, bool val);

    [[nodiscard]] static bool RawPointerGetCompactBit(const u8 *src_ptr_u8, SizeT idx);

    static void RawPointerSetCompactBit(u8 *dst_ptr_u8, SizeT idx, bool val);

    static bool CompactBitIsSame(const SharedPtr<VectorBuffer> &lhs, SizeT lhs_cnt, const SharedPtr<VectorBuffer> &rhs, SizeT rhs_cnt);

    static void CopyCompactBits(u8 *dst, const u8 *src, SizeT dst_start_id, SizeT src_start_id, SizeT count);

private:
    bool initialized_{false};

    std::variant<UniquePtr<char[]>, BufferHandle> ptr_;

    SizeT data_size_{0};
    SizeT capacity_{0};

public:
    VectorBufferType buffer_type_{VectorBufferType::kInvalid};

    UniquePtr<FixHeapManager> fix_heap_mgr_{nullptr};

public:
    void Reset() {
        fix_heap_mgr_ = nullptr;
        var_buffer_mgr_ = nullptr;
    }

    SizeT TotalSize(const DataType *data_type) const;

    void WriteAdv(char *&ptr, const DataType *data_type) const;

    void ReadAdv(char *&ptr, const DataType *data_type);

    const char *GetVarchar(SizeT offset, SizeT len) const;

    SizeT AppendVarchar(const char *data, SizeT len);

    Pair<const char *, const char *> GetSparseRaw(SizeT offset, SizeT nnz, const SparseInfo *sparse_info) const;

    SizeT AppendSparseRaw(const char *raw_data, const char *raw_idx, SizeT nnz, const SparseInfo *sparse_info);

    template <typename DataType, typename IdxType>
    SparseVecRef<DataType, IdxType> GetSparse(SizeT offset, SizeT nnz) const;

    template <typename DataType, typename IdxType>
    SizeT AppendSparse(const SparseVecRef<DataType, IdxType> &sparse_vec);

    const char *GetMultiVectorRaw(SizeT offset, SizeT size) const;

    SizeT AppendMultiVectorRaw(const char *raw_data, SizeT size);

    const char *GetTensorRaw(SizeT offset, SizeT size) const;

    SizeT AppendTensorRaw(const char *raw_data, SizeT size);

    const char *GetTensorArrayMeta(SizeT offset, SizeT array_num) const;

    SizeT AppendTensorArrayMeta(Span<const TensorT> tensor_metas) const;

private:
    UniquePtr<VarBufferManager> var_buffer_mgr_{nullptr};
};

template <typename DataType, typename IdxType>
SparseVecRef<DataType, IdxType> VectorBuffer::GetSparse(SizeT offset, SizeT nnz) const {
    if (nnz == 0) {
        return {0, nullptr, nullptr};
    }
    SizeT indice_size = SparseInfo::IndiceSize<IdxType>(nnz);
    SizeT data_size = SparseInfo::DataSize<DataType>(nnz);
    const char *raw_indice = var_buffer_mgr_->Get(offset, indice_size);
    const char *raw_data = nullptr;
    if (data_size > 0) {
        raw_data = var_buffer_mgr_->Get(offset + indice_size, data_size);
    }
    return {static_cast<i32>(nnz), reinterpret_cast<const IdxType *>(raw_indice), reinterpret_cast<const DataType *>(raw_data)};
}

template <typename DataType, typename IdxType>
SizeT VectorBuffer::AppendSparse(const SparseVecRef<DataType, IdxType> &sparse_vec) {
    SizeT indice_size = SparseInfo::IndiceSize<IdxType>(sparse_vec.nnz_);
    SizeT data_size = SparseInfo::DataSize<DataType>(sparse_vec.nnz_);
    SizeT file_offset = var_buffer_mgr_->Append(reinterpret_cast<const char *>(sparse_vec.indices_), indice_size);
    if (data_size > 0) {
        var_buffer_mgr_->Append(reinterpret_cast<const char *>(sparse_vec.data_), data_size);
    }
    return file_offset;
}

} // namespace infinity
