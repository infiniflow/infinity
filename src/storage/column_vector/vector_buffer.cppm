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

export module infinity_core:vector_buffer;

import :var_buffer;
import :sparse_util;

import sparse_info;
import internal_types;
import data_type;
import global_resource_usage;

namespace infinity {

export enum class VectorBufferType {
    kInvalid,
    kStandard,
    kHeap,
    kCompactBit,
    kVarBuffer, // new varchar
};

export class VectorBuffer {
public:
    static std::shared_ptr<VectorBuffer> Make(size_t data_type_size, size_t capacity, VectorBufferType buffer_type);

    static std::shared_ptr<VectorBuffer>
    Make(FileWorker *file_worker, FileWorker *var_file_worker, size_t data_type_size, size_t capacity, VectorBufferType buffer_type);

public:
    explicit VectorBuffer() {}

    ~VectorBuffer() {}

    void Initialize(size_t type_size, size_t capacity);

    void InitializeCompactBit(size_t capacity);

    void InitializeCompactBit(FileWorker *file_worker, size_t capacity);

    void Initialize(FileWorker *file_worker, FileWorker *var_file_worker, size_t type_size, size_t capacity);

    void SetToCatalog(FileWorker *file_worker, FileWorker *var_file_worker);

    void ResetToInit(VectorBufferType type);

    void Copy(char *input, size_t size);

    [[nodiscard]] char *GetData() const {
        if (std::holds_alternative<std::unique_ptr<char[]>>(ptr_)) {
            return std::get<std::unique_ptr<char[]>>(ptr_).get();
        }
        void *data{};
        std::get<FileWorker *>(ptr_)->Read(data);
        return static_cast<char *>(data);
    }

    [[nodiscard]] bool GetCompactBit(size_t idx) const;

    void SetCompactBit(size_t idx, bool val);

    [[nodiscard]] static bool RawPointerGetCompactBit(const u8 *src_ptr_u8, size_t idx);

    static void RawPointerSetCompactBit(u8 *dst_ptr_u8, size_t idx, bool val);

    static bool CompactBitIsSame(const std::shared_ptr<VectorBuffer> &lhs, size_t lhs_cnt, const std::shared_ptr<VectorBuffer> &rhs, size_t rhs_cnt);

    static void CopyCompactBits(u8 *dst, const u8 *src, size_t dst_start_id, size_t src_start_id, size_t count);

private:
    bool initialized_{false};

    std::variant<std::unique_ptr<char[]>, FileWorker *> ptr_;

    size_t data_size_{0};
    size_t capacity_{0};

public:
    VectorBufferType buffer_type_{VectorBufferType::kInvalid};

public:
    void Reset() { var_buffer_mgr_ = nullptr; }

    size_t TotalSize(const DataType *data_type) const;

    void WriteAdv(char *&ptr, const DataType *data_type) const;

    void ReadAdv(const char *&ptr, const DataType *data_type);

    const char *GetVarchar(size_t offset, size_t len) const;

    size_t AppendVarchar(const char *data, size_t len);

    std::pair<const char *, const char *> GetSparseRaw(size_t offset, size_t nnz, const SparseInfo *sparse_info) const;

    size_t AppendSparseRaw(const char *raw_data, const char *raw_idx, size_t nnz, const SparseInfo *sparse_info);

    template <typename DataType, typename IdxType>
    SparseVecRef<DataType, IdxType> GetSparse(size_t offset, size_t nnz) const;

    template <typename DataType, typename IdxType>
    size_t AppendSparse(const SparseVecRef<DataType, IdxType> &sparse_vec);

    const char *GetMultiVectorRaw(size_t offset, size_t size) const;

    size_t AppendMultiVectorRaw(const char *raw_data, size_t size);

    const char *GetTensorRaw(size_t offset, size_t size) const;

    size_t AppendTensorRaw(const char *raw_data, size_t size);

    const char *GetTensorArrayMeta(size_t offset, size_t array_num) const;

    size_t AppendTensorArrayMeta(std::span<const TensorT> tensor_metas) const;

    const char *GetArrayRaw(size_t offset, size_t size) const;

    size_t AppendArrayRaw(const char *raw_data, size_t size) const;

    VarBufferManager *var_buffer_mgr() const { return var_buffer_mgr_.get(); }

private:
    std::unique_ptr<VarBufferManager> var_buffer_mgr_{nullptr};
};

template <typename DataType, typename IdxType>
SparseVecRef<DataType, IdxType> VectorBuffer::GetSparse(size_t offset, size_t nnz) const {
    if (nnz == 0) {
        return {0, nullptr, nullptr};
    }
    size_t indice_size = SparseInfo::IndiceSize<IdxType>(nnz);
    size_t data_size = SparseInfo::DataSize<DataType>(nnz);
    const char *raw_indice = var_buffer_mgr_->Get(offset, indice_size);
    const char *raw_data = nullptr;
    if (data_size > 0) {
        raw_data = var_buffer_mgr_->Get(offset + indice_size, data_size);
    }
    return {static_cast<i32>(nnz), reinterpret_cast<const IdxType *>(raw_indice), reinterpret_cast<const DataType *>(raw_data)};
}

template <typename DataType, typename IdxType>
size_t VectorBuffer::AppendSparse(const SparseVecRef<DataType, IdxType> &sparse_vec) {
    size_t indice_size = SparseInfo::IndiceSize<IdxType>(sparse_vec.nnz_);
    size_t data_size = SparseInfo::DataSize<DataType>(sparse_vec.nnz_);
    size_t file_offset = var_buffer_mgr_->Append(reinterpret_cast<const char *>(sparse_vec.indices_), indice_size);
    if (data_size > 0) {
        var_buffer_mgr_->Append(reinterpret_cast<const char *>(sparse_vec.data_), data_size);
    }
    return file_offset;
}

} // namespace infinity
