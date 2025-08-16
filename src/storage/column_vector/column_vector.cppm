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

export module infinity_core:column_vector;

import :vector_buffer;
import :roaring_bitmap;
import :selection;
import :default_values;
import :value;
import :status;
import :infinity_exception;
import :var_buffer;
import :sparse_util;

import std;
import third_party;

import internal_types;
import data_type;
import embedding_info;
import sparse_info;
import array_info;
import constant_expr;
import logical_type;
import global_resource_usage;

namespace infinity {

class BufferManager;
class BufferObj;

export enum class ColumnVectorMode : i8 {
    kReadWrite,
    kReadOnly,
};

export enum class ColumnVectorType : i8 {
    kInvalid,
    kFlat,          // Stand without any encode
    kConstant,      // All vector has same type and value
    kCompactBit,    // Compact bit encoding
                    //    kDictionary, // There is a dictionary zone
                    //    kRLE, // Run length encoding
                    //    kSequence,
                    //    kBias,
                    //
                    //    kCollectionFlat, // May have missing
                    //    kCollectionDictionary, // May have missing
    kHeterogeneous, // May have missing
};

// Basic unit of column data vector
export struct ColumnVector {
public:
    static std::shared_ptr<ColumnVector> Make(std::shared_ptr<DataType> data_type);

public:
    size_t data_type_size_{0};

    // this buffer is holding the data
    std::shared_ptr<VectorBuffer> buffer_{nullptr};

    // A bitmap to indicate the null information
    // true: row is not null
    // false: row is null
    // initial state: all true
    std::shared_ptr<Bitmask> nulls_ptr_{nullptr};

    bool initialized{false};

private:
    ColumnVectorType vector_type_{ColumnVectorType::kInvalid};

    std::shared_ptr<DataType> data_type_{};

    // Only a pointer to the real data in vector buffer
    char *data_ptr_{nullptr};

    size_t capacity_{0};

    std::atomic<size_t> tail_index_{0};

public:
    ColumnVector();

    // Construct a column vector without initialization;
    explicit ColumnVector(std::shared_ptr<DataType> data_type);

    // used in BatchInvertTask::BatchInvertTask, keep ObjectCount correct
    ColumnVector(const ColumnVector &right);

    // used in BlockColumnIter, keep ObjectCount correct
    ColumnVector(ColumnVector &&right) noexcept;

    ColumnVector &operator=(ColumnVector &&right) noexcept;

    ~ColumnVector();

    std::string ToString() const;

    void AppendWith(const ColumnVector &other);

    void AppendValue(const Value &value);

    void SetVectorType(ColumnVectorType vector_type);

    static VectorBufferType GetVectorBufferType(const DataType &data_type);

private:
    VectorBufferType InitializeHelper(ColumnVectorType vector_type, size_t capacity);

public:
    void Initialize(ColumnVectorType vector_type = ColumnVectorType::kFlat, size_t capacity = DEFAULT_VECTOR_SIZE);

    void Initialize(BufferObj *buffer_obj,
                    BufferObj *outline_buffer_obj,
                    size_t current_row_count,
                    ColumnVectorMode vector_tipe = ColumnVectorMode::kReadWrite,
                    ColumnVectorType vector_type = ColumnVectorType::kFlat,
                    size_t capacity = DEFAULT_VECTOR_SIZE);

    void SetToCatalog(BufferObj *buffer_obj, BufferObj *outline_buffer_obj, ColumnVectorMode vector_tipe);

    void Initialize(const ColumnVector &other, const Selection &input_select);

    void Initialize(ColumnVectorType vector_type, const ColumnVector &other, size_t start_idx, size_t end_idx);

    void Initialize(const ColumnVector &other, size_t start_idx, size_t end_idx) { Initialize(other.vector_type_, other, start_idx, end_idx); }

    std::string ToString(size_t row_index) const;

    // Return the <index> of the vector
    // Since it will construct a new Value object, this function shouldn't be used in vectorized computation.
    // Directly uses data_ptr in vectorized computation.
    Value GetValueByIndex(size_t index) const;

    // Set the <index> element of the vector to the specified value.
    void SetValueByIndex(size_t index, const Value &value);

    void Finalize(size_t index);

    void AppendByPtr(const char *value_ptr);

    void AppendByStringView(std::string_view sv);

    void AppendByConstantExpr(const ConstantExpr *const_expr);

    void AppendWith(const ColumnVector &other, size_t start_row, size_t count);

    // input parameter:
    // from - start RowID
    // count - total row count to be copied. These rows shall be in the same BlockEntry.
    // return value: appended rows actually
    size_t AppendWith(RowID from, size_t count);

    void ShallowCopy(const ColumnVector &other);

    void Reset();

    bool operator==(const ColumnVector &other) const;
    bool operator!=(const ColumnVector &other) const { return !(*this == other); }

    // Estimated serialized size in bytes
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;

    // Read from a serialized version
    static std::shared_ptr<ColumnVector> ReadAdv(const char *&ptr, i32 maxbytes);

public:
    static void
    SetMultiVector(MultiVectorT &dest_multi_vec, VectorBuffer *dest_buffer, std::span<const char> data, const EmbeddingInfo *embedding_info);

    static std::pair<std::span<const char>, size_t>
    GetMultiVector(const MultiVectorT &multi_vec, const VectorBuffer *buffer, const EmbeddingInfo *embedding_info);

    static void SetTensor(TensorT &dest_tensor, VectorBuffer *dest_buffer, std::span<const char> data, const EmbeddingInfo *embedding_info);

    static std::pair<std::span<const char>, size_t> GetTensor(const TensorT &tensor, const VectorBuffer *buffer, const EmbeddingInfo *embedding_info);

    static void SetTensorArrayMeta(TensorArrayT &dest_tensor_array, VectorBuffer *dest_buffer, std::span<const TensorT> tensors);

    static std::vector<TensorT> GetTensorArrayMeta(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer);

    static void SetTensorArray(TensorArrayT &dest_tensor_array,
                               VectorBuffer *dest_buffer,
                               const std::vector<std::span<const char>> &data,
                               const EmbeddingInfo *embedding_info);

    static std::vector<std::pair<std::span<const char>, size_t>>
    GetTensorArray(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info);

    static std::pair<std::span<const char>, size_t> GetArray(const ArrayT &src_array, const VectorBuffer *src_buffer, const ArrayInfo *array_info);

    std::pair<std::span<const char>, size_t> GetMultiVectorRaw(size_t idx) const;

    std::pair<std::span<const char>, size_t> GetTensorRaw(size_t idx) const;

    std::vector<std::pair<std::span<const char>, size_t>> GetTensorArrayRaw(size_t idx) const;

    Value GetArrayValue(const ArrayT &source) const;

    void SetArrayValue(ArrayT &target, const Value &value);

    bool AppendUnnestArray(const ColumnVector &other, size_t offset, size_t &array_offset);

private:
    Value GetArrayValueRecursively(const DataType &data_type, const char *data_ptr) const;

    void SetArrayValueRecursively(const Value &value, char *dst_ptr);

    template <typename T>
    static void CopyValue(ColumnVector &dst, const ColumnVector &src, size_t from, size_t count);

    template <typename T>
    void AppendEmbedding(const std::vector<std::string_view> &ele_str_views, size_t dst_off);

    template <typename T>
    void AppendMultiVector(const std::vector<std::string_view> &ele_str_views, size_t dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendTensor(const std::vector<std::string_view> &ele_str_views, size_t dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendTensorArray(const std::vector<std::vector<std::string_view>> &ele_str_views, size_t dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendSparse(const std::vector<std::string_view> &ele_str_views, size_t dst_off);

    template <typename DataT, typename IdxT>
    void AppendSparse(const std::vector<std::string_view> &ele_str_views, size_t dst_off);

public:
    template <typename DataT, typename IdxT>
    void AppendSparseInner(size_t nnz, const DataT *data, const IdxT *index, SparseT &sparse);

    template <typename DataT, typename IdxT>
    void AppendSparseInner(size_t nnz, const DataT *data, const IdxT *index, size_t dst_off) {
        auto &sparse = reinterpret_cast<SparseT *>(data_ptr_)[dst_off];
        AppendSparseInner(nnz, data, index, sparse);
    }

    template <typename DataT, typename IdxT>
    void AppendSparse(size_t nnz, const DataT *data, const IdxT *index) {
        size_t dst_off = tail_index_.fetch_add(1);
        AppendSparseInner(nnz, data, index, dst_off);
    }

    void AppendSparseRaw(const char *raw_data_ptr, const char *raw_index_ptr, size_t nnz, size_t dst_off);

    std::tuple<std::span<const char>, std::span<const char>, size_t> GetSparseRaw(size_t index) const;

public:
    void AppendVarcharInner(std::span<const char> data, VarcharT &varchar);

    void AppendVarcharInner(std::span<const char> data, size_t dst_off);

    void AppendVarchar(std::span<const char> data);

    std::span<const char> GetVarcharInner(const VarcharT &varchar) const;

    std::span<const char> GetVarchar(size_t index) const;

    char *GetRawPtr(size_t index);

private:
    // Used by Append by Ptr
    void SetByRawPtr(size_t index, const char *raw_ptr);

    void CopyRow(const ColumnVector &other, size_t dst_idx, size_t src_idx);

    template <typename DataT>
    inline void CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, size_t count, const Selection &input_select);

    template <typename DataT>
    inline void
    CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, size_t source_start_idx, size_t dest_start_idx, size_t count);

    template <typename DataT>
    inline void CopyRowFrom(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx);

public:
    [[nodiscard]] const inline ColumnVectorType &vector_type() const { return vector_type_; }

    [[nodiscard]] const inline std::shared_ptr<DataType> data_type() const { return data_type_; }

    [[nodiscard]] inline char *data() const { return data_ptr_; }

    [[nodiscard]] inline size_t capacity() const { return capacity_; }

    [[nodiscard]] inline size_t Size() const { return tail_index_.load(); }
};

template <typename T>
void ColumnVector::CopyValue(ColumnVector &dst, const ColumnVector &src, size_t from, size_t count) {
    auto *src_ptr = (T *)(src.data_ptr_);
    T *dst_ptr = &((T *)(dst.data_ptr_))[dst.tail_index_.load()];
    if (src.vector_type() == ColumnVectorType::kConstant && src.tail_index_.load() == 1) {
        for (size_t idx = 0; idx < count; ++idx) {
            dst_ptr[idx] = src_ptr[from];
        }
    } else {
        for (size_t idx = 0; idx < count; ++idx) {
            dst_ptr[idx] = src_ptr[from + idx];
        }
    }
}

template <typename T>
void ColumnVector::AppendEmbedding(const std::vector<std::string_view> &ele_str_views, size_t dst_off) {
    for (size_t i = 0; auto &ele_str_view : ele_str_views) {
        T value = DataType::StringToValue<T>(ele_str_view);
        ((T *)(data_ptr_ + dst_off))[i] = value;
        ++i;
    }
}

template <>
void ColumnVector::AppendEmbedding<BooleanT>(const std::vector<std::string_view> &ele_str_views, size_t dst_off) {
    const auto bit_bytes = (ele_str_views.size() + 7) / 8;
    auto data_ptr = reinterpret_cast<u8 *>(data_ptr_ + dst_off);
    std::fill_n(data_ptr, bit_bytes, 0);
    for (size_t i = 0; auto &ele_str_view : ele_str_views) {
        if (const auto value = DataType::StringToValue<float>(ele_str_view); value) {
            data_ptr[i / 8] |= (1u << (i % 8));
        }
        ++i;
    }
}

template <typename T>
std::pair<std::unique_ptr<char[]>, size_t> StrToTensor(const std::vector<std::string_view> &ele_str_views, const EmbeddingInfo *embedding_info) {
    size_t total_element_count = ele_str_views.size();
    auto tmp_data = std::make_unique_for_overwrite<char[]>(total_element_count * sizeof(T));
    auto *ptr = reinterpret_cast<T *>(tmp_data.get());
    for (size_t i = 0; auto &ele_str_view : ele_str_views) {
        ptr[i] = DataType::StringToValue<T>(ele_str_view);
        ++i;
    }
    return {std::move(tmp_data), total_element_count * sizeof(T)};
}

template <>
std::pair<std::unique_ptr<char[]>, size_t> StrToTensor<bool>(const std::vector<std::string_view> &ele_str_views,
                                                             const EmbeddingInfo *embedding_info) {
    size_t total_element_count = ele_str_views.size();
    size_t bit_bytes = (total_element_count + 7) / 8;
    auto tmp_data = std::make_unique<char[]>(bit_bytes);
    for (size_t i = 0; auto &ele_str_view : ele_str_views) {
        if (const auto value = DataType::StringToValue<float>(ele_str_view); value) {
            tmp_data[i / 8] |= (1u << (i % 8));
        }
        ++i;
    }
    return {std::move(tmp_data), bit_bytes};
}

template <typename T>
void ColumnVector::AppendMultiVector(const std::vector<std::string_view> &ele_str_views, size_t dst_off, const EmbeddingInfo *embedding_info) {
    MultiVectorT &target_multivector = reinterpret_cast<MultiVectorT *>(data_ptr_)[dst_off];
    auto [data, data_bytes] = StrToTensor<T>(ele_str_views, embedding_info);
    std::span<const char> data_span(data.get(), data_bytes);
    ColumnVector::SetMultiVector(target_multivector, buffer_.get(), data_span, embedding_info);
}

template <typename T>
void ColumnVector::AppendTensor(const std::vector<std::string_view> &ele_str_views, size_t dst_off, const EmbeddingInfo *embedding_info) {
    TensorT &target_tensor = reinterpret_cast<TensorT *>(data_ptr_)[dst_off];
    auto [data, data_bytes] = StrToTensor<T>(ele_str_views, embedding_info);
    std::span<const char> data_span(data.get(), data_bytes);
    ColumnVector::SetTensor(target_tensor, buffer_.get(), data_span, embedding_info);
}

template <typename T>
void ColumnVector::AppendTensorArray(const std::vector<std::vector<std::string_view>> &ele_str_views,
                                     size_t dst_off,
                                     const EmbeddingInfo *embedding_info) {
    TensorArrayT &target_tensor_array = reinterpret_cast<TensorArrayT *>(data_ptr_)[dst_off];
    size_t total_tensor_count = ele_str_views.size();
    std::vector<std::unique_ptr<char[]>> tensors(total_tensor_count);
    std::vector<std::span<const char>> tensor_spans(total_tensor_count);
    for (size_t i = 0; i < total_tensor_count; ++i) {
        auto [data, data_bytes] = StrToTensor<T>(ele_str_views[i], embedding_info);
        tensors[i] = std::move(data);
        tensor_spans[i] = std::span<const char>(tensors[i].get(), data_bytes);
    }
    ColumnVector::SetTensorArray(target_tensor_array, buffer_.get(), tensor_spans, embedding_info);
}

template <typename T>
void ColumnVector::AppendSparse(const std::vector<std::string_view> &ele_str_views, size_t dst_off) {
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            AppendSparse<T, TinyIntT>(ele_str_views, dst_off);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            AppendSparse<T, SmallIntT>(ele_str_views, dst_off);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            AppendSparse<T, IntegerT>(ele_str_views, dst_off);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            AppendSparse<T, BigIntT>(ele_str_views, dst_off);
            break;
        }
        default: {
            UnrecoverableError("Unsupported sparse index type.");
        }
    }
}

template <typename DataT, typename IdxT>
void ColumnVector::AppendSparse(const std::vector<std::string_view> &ele_str_views, size_t dst_off) {
    size_t total_element_count = ele_str_views.size();
    std::vector<IdxT> index_vec;
    index_vec.reserve(total_element_count);
    if constexpr (std::is_same_v<DataT, BooleanT>) {
        for (u32 i = 0; i < total_element_count; ++i) {
            auto index = DataType::StringToValue<IdxT>(ele_str_views[i]);
            if (index < 0) {
                RecoverableError(Status::InvalidDataType());
            }
            index_vec.push_back(index);
        }
        AppendSparseInner(total_element_count, static_cast<DataT *>(nullptr), index_vec.data(), dst_off);
    } else {
        std::vector<DataT> data_vec;
        data_vec.reserve(total_element_count);
        for (u32 i = 0; i < total_element_count; ++i) {
            auto [index, value] = DataType::StringToSparseValue<DataT, IdxT>(ele_str_views[i]);
            if (index < 0) {
                RecoverableError(Status::InvalidDataType());
            }
            index_vec.push_back(index);
            data_vec.push_back(value);
        }
        AppendSparseInner(total_element_count, data_vec.data(), index_vec.data(), dst_off);
    }
}

template <typename DataT, typename IdxT>
void ColumnVector::AppendSparseInner(size_t nnz, const DataT *data, const IdxT *index, SparseT &target_sparse) {
    target_sparse.nnz_ = nnz;
    if (nnz == 0) {
        target_sparse.file_offset_ = -1;
        return;
    }
    bool to_sort = true;
    auto *sparse_info = static_cast<SparseInfo *>(data_type_->type_info().get());
    if (sparse_info->StoreType() == SparseStoreType::kSorted) {
        to_sort = false;
    }

    if constexpr (std::is_same_v<DataT, BooleanT>) {
        if (data != nullptr) {
            RecoverableError(Status::InvalidDataType());
            return;
        }
        std::unique_ptr<IdxT[]> tmp_index;
        if (to_sort) {
            tmp_index = std::make_unique_for_overwrite<IdxT[]>(nnz);
            std::copy(index, index + nnz, tmp_index.get());
            std::sort(tmp_index.get(), tmp_index.get() + nnz);
            auto *index_end = std::unique(tmp_index.get(), tmp_index.get() + nnz);
            if (index_end != tmp_index.get() + nnz) {
                RecoverableError(Status::InvalidDataType());
                return;
            }
            index = tmp_index.get();
        }
        SparseVecRef<DataT, IdxT> sparse_vec_ref(nnz, index, nullptr);
        target_sparse.file_offset_ = buffer_->AppendSparse(sparse_vec_ref);
    } else {
        std::unique_ptr<IdxT[]> tmp_index;
        std::unique_ptr<DataT[]> tmp_data;
        if (to_sort) {
            tmp_index = std::make_unique_for_overwrite<IdxT[]>(nnz);
            tmp_data = std::make_unique_for_overwrite<DataT[]>(nnz);
            std::vector<std::pair<IdxT, DataT>> index_value_pairs(nnz);
            for (u32 i = 0; i < nnz; ++i) {
                index_value_pairs[i] = {index[i], data[i]};
            }
            std::sort(index_value_pairs.begin(), index_value_pairs.end(), [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });
            for (u32 i = 0; i < nnz; ++i) {
                tmp_index[i] = index_value_pairs[i].first;
                tmp_data[i] = index_value_pairs[i].second;
            }
            auto *index_end = std::unique(tmp_index.get(), tmp_index.get() + nnz);
            if (index_end != tmp_index.get() + nnz) {
                RecoverableError(Status::InvalidDataType());
                return;
            }
            index = tmp_index.get();
            data = tmp_data.get();
        }
        SparseVecRef<DataT, IdxT> sparse_vec_ref(nnz, index, data);
        target_sparse.file_offset_ = buffer_->AppendSparse(sparse_vec_ref);
    }
}

void CopyVarchar(VarcharT &dst_ref, VectorBuffer *dst_vec_buffer, const VarcharT &src_ref, const VectorBuffer *src_vec_buffer);

void CopyMultiVector(MultiVectorT &dst_ref,
                     VectorBuffer *dst_vec_buffer,
                     const MultiVectorT &src_ref,
                     const VectorBuffer *src_vec_buffer,
                     const EmbeddingInfo *embedding_info);

void CopyTensor(TensorT &dst_ref,
                VectorBuffer *dst_vec_buffer,
                const TensorT &src_ref,
                const VectorBuffer *src_vec_buffer,
                const EmbeddingInfo *embedding_info);

void CopyTensorArray(TensorArrayT &dst_ref,
                     VectorBuffer *dst_buffer,
                     const TensorArrayT &src_ref,
                     const VectorBuffer *src_buffer,
                     const EmbeddingInfo *embedding_info);

void CopySparse(SparseT &dst_sparse,
                VectorBuffer *dst_vec_buffer,
                const SparseT &src_sparse,
                const VectorBuffer *src_vec_buffer,
                const SparseInfo *sparse_info);

void CopyArray(ArrayT &dst_array,
               VectorBuffer *dst_vec_buffer,
               const ArrayT &src_array,
               const VectorBuffer *src_vec_buffer,
               const ArrayInfo *array_info);

template <>
void ColumnVector::CopyValue<BooleanT>(ColumnVector &dst, const ColumnVector &src, size_t from, size_t count) {
    auto dst_tail = dst.tail_index_.load();
    const VectorBuffer *src_buffer = src.buffer_.get();
    auto dst_buffer = dst.buffer_.get();
    if (dst_tail % 8 == 0 && from % 8 == 0) {
        size_t dst_byte_offset = dst_tail / 8;
        size_t src_byte_offset = from / 8;
        size_t byte_count = (count + 7) / 8; // copy to tail
        std::memcpy(dst_buffer->GetDataMut() + dst_byte_offset, src_buffer->GetData() + src_byte_offset, byte_count);
    } else {
        for (size_t idx = 0; idx < count; ++idx) {
            dst_buffer->SetCompactBit(dst_tail + idx, src_buffer->GetCompactBit(from + idx));
        }
    }
}

template <typename DataT>
inline void
ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, size_t count, const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        ((DataT *)(dst))[idx] = ((const DataT *)(src))[row_id];
    }
}

template <>
inline void ColumnVector::CopyFrom<BooleanT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             size_t count,
                                             const Selection &input_select) {
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        dst_buf->SetCompactBit(idx, src_buf->GetCompactBit(row_id));
    }
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             size_t count,
                                             const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        VarcharT *dst_ptr = &(((VarcharT *)dst)[idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[row_id]);
        CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
    }
}

template <>
inline void ColumnVector::CopyFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 size_t count,
                                                 const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        MultiVectorT *dst_ptr = &(((MultiVectorT *)dst)[idx]);
        const MultiVectorT *src_ptr = &(((const MultiVectorT *)src)[row_id]);
        CopyMultiVector(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            size_t count,
                                            const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        TensorT *dst_ptr = &(((TensorT *)dst)[idx]);
        const TensorT *src_ptr = &(((const TensorT *)src)[row_id]);
        CopyTensor(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 size_t count,
                                                 const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];
        TensorArrayT *dst_ptr = &(((TensorArrayT *)dst)[idx]);
        const TensorArrayT *src_ptr = &(((const TensorArrayT *)src)[row_id]);
        CopyTensorArray(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<SparseT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            size_t count,
                                            const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    for (size_t idx = 0; idx < count; ++idx) {
        size_t dest_idx = input_select[idx];

        auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dest_idx;
        const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + idx;

        CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<ArrayT>(const VectorBuffer *__restrict src_buf,
                                           VectorBuffer *__restrict dst_buf,
                                           size_t count,
                                           const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *array_info = static_cast<const ArrayInfo *>(data_type_->type_info().get());
    for (size_t idx = 0; idx < count; ++idx) {
        size_t dest_idx = input_select[idx];
        auto *dst_array = reinterpret_cast<ArrayT *>(dst) + dest_idx;
        const auto *src_array = reinterpret_cast<const ArrayT *>(src) + idx;
        CopyArray(*dst_array, dst_buf, *src_array, src_buf, array_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf,
                                               VectorBuffer *__restrict dst_buf,
                                               size_t count,
                                               const Selection &input_select) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();

    for (size_t idx = 0; idx < count; ++idx) {
        size_t row_id = input_select[idx];

        const char *src_ptr = src + row_id * data_type_size_;
        char *dst_ptr = dst + idx * data_type_size_;
        std::memcpy(dst_ptr, src_ptr, data_type_size_);
    }
}

template <typename DataT>
inline void ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf,
                                   VectorBuffer *__restrict dst_buf,
                                   size_t source_start_idx,
                                   size_t dest_start_idx,
                                   size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();

    size_t source_end_idx = source_start_idx + count;

    std::copy(((const DataT *)(src)) + source_start_idx, ((const DataT *)(src)) + source_end_idx, ((DataT *)(dst)) + dest_start_idx);
}

template <>
inline void ColumnVector::CopyFrom<BooleanT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             size_t source_start_idx,
                                             size_t dest_start_idx,
                                             size_t count) {
    VectorBuffer::CopyCompactBits(reinterpret_cast<u8 *>(dst_buf->GetDataMut()),
                                  reinterpret_cast<const u8 *>(src_buf->GetData()),
                                  dest_start_idx,
                                  source_start_idx,
                                  count);
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             size_t source_start_idx,
                                             size_t dest_start_idx,
                                             size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();

    size_t source_end_idx = source_start_idx + count;
    for (size_t idx = source_start_idx; idx < source_end_idx; ++idx) {
        VarcharT *dst_ptr = &(((VarcharT *)dst)[dest_start_idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[idx]);
        CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 size_t source_start_idx,
                                                 size_t dest_start_idx,
                                                 size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    size_t source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = source_start_idx; idx < source_end_idx; ++idx) {
        const MultiVectorT &src_multivector = ((const MultiVectorT *)src)[idx];
        MultiVectorT &dst_multivector = ((MultiVectorT *)dst)[dest_start_idx];
        CopyMultiVector(dst_multivector, dst_buf, src_multivector, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            size_t source_start_idx,
                                            size_t dest_start_idx,
                                            size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    size_t source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = source_start_idx; idx < source_end_idx; ++idx) {
        const TensorT &src_tensor = ((const TensorT *)src)[idx];
        TensorT &dst_tensor = ((TensorT *)dst)[dest_start_idx];
        CopyTensor(dst_tensor, dst_buf, src_tensor, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 size_t source_start_idx,
                                                 size_t dest_start_idx,
                                                 size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    size_t source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (size_t idx = source_start_idx; idx < source_end_idx; ++idx) {
        const TensorArrayT &src_ref = ((const TensorArrayT *)src)[idx];
        TensorArrayT &dst_ref = ((TensorArrayT *)dst)[dest_start_idx];
        CopyTensorArray(dst_ref, dst_buf, src_ref, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<SparseT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            size_t source_start_idx,
                                            size_t dest_start_idx,
                                            size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    size_t source_end_idx = source_start_idx + count;
    for (size_t idx = source_start_idx, dst_idx = dest_start_idx; idx < source_end_idx; ++idx, ++dst_idx) {
        const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + idx;
        auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dst_idx;

        CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<ArrayT>(const VectorBuffer *__restrict src_buf,
                                           VectorBuffer *__restrict dst_buf,
                                           size_t source_start_idx,
                                           size_t dest_start_idx,
                                           size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *array_info = static_cast<const ArrayInfo *>(data_type_->type_info().get());
    size_t source_end_idx = source_start_idx + count;
    for (size_t idx = source_start_idx, dst_idx = dest_start_idx; idx < source_end_idx; ++idx, ++dst_idx) {
        const auto *src_array = reinterpret_cast<const ArrayT *>(src) + idx;
        auto *dst_array = reinterpret_cast<ArrayT *>(dst) + dst_idx;
        CopyArray(*dst_array, dst_buf, *src_array, src_buf, array_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf,
                                               VectorBuffer *__restrict dst_buf,
                                               size_t source_start_idx,
                                               size_t dest_start_idx,
                                               size_t count) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();

    size_t source_end_idx = source_start_idx + count;
    for (size_t idx = source_start_idx; idx < source_end_idx; ++idx) {

        const char *src_ptr = src + idx * data_type_size_;
        char *dst_ptr = dst + dest_start_idx * data_type_size_;
        std::memcpy(dst_ptr, src_ptr, data_type_size_);

        ++dest_start_idx;
    }
}

template <typename DataT>
inline void ColumnVector::CopyRowFrom(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();

    ((DataT *)(dst))[dst_idx] = ((const DataT *)(src))[src_idx];
}

template <>
inline void
ColumnVector::CopyRowFrom<BooleanT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    dst_buf->SetCompactBit(dst_idx, src_buf->GetCompactBit(src_idx));
}

template <>
inline void
ColumnVector::CopyRowFrom<VarcharT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    VarcharT *dst_ptr = &(((VarcharT *)dst)[dst_idx]);
    const VarcharT *src_ptr = &(((const VarcharT *)src)[src_idx]);
    CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
}

template <>
inline void
ColumnVector::CopyRowFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const MultiVectorT &src_multivector = ((const MultiVectorT *)src)[src_idx];
    MultiVectorT &dst_multivector = ((MultiVectorT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyMultiVector(dst_multivector, dst_buf, src_multivector, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<TensorT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const TensorT &src_tensor = ((const TensorT *)src)[src_idx];
    TensorT &dst_tensor = ((TensorT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyTensor(dst_tensor, dst_buf, src_tensor, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const TensorArrayT &src_ref = ((const TensorArrayT *)src)[src_idx];
    TensorArrayT &dst_ref = ((TensorArrayT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyTensorArray(dst_ref, dst_buf, src_ref, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<SparseT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + src_idx;
    auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dst_idx;

    CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<ArrayT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const auto *array_info = static_cast<const ArrayInfo *>(data_type_->type_info().get());
    const auto *src_array = reinterpret_cast<const ArrayT *>(src) + src_idx;
    auto *dst_array = reinterpret_cast<ArrayT *>(dst) + dst_idx;
    CopyArray(*dst_array, dst_buf, *src_array, src_buf, array_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf, size_t src_idx, VectorBuffer *__restrict dst_buf, size_t dst_idx) {
    const char *src = src_buf->GetData();
    char *dst = dst_buf->GetDataMut();
    const char *src_ptr = src + src_idx * data_type_size_;
    char *dst_ptr = dst + dst_idx * data_type_size_;
    std::memcpy(dst_ptr, src_ptr, data_type_size_);
}

export template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

export template <typename ValueType>
concept PODValueType = IsAnyOf<ValueType,
                               TinyIntT,
                               SmallIntT,
                               IntegerT,
                               BigIntT,
                               HugeIntT,
                               Float16T,
                               BFloat16T,
                               FloatT,
                               DoubleT,
                               DecimalT,
                               DateT,
                               TimeT,
                               DateTimeT,
                               TimestampT,
                               IntervalT,
                               RowID,
                               UuidT,
                               VarcharT>;

export template <typename ValueType>
concept BinaryGenerateBoolean = PODValueType<ValueType> or IsAnyOf<ValueType, BooleanT>;

template <typename Unsupported>
class ColumnVectorPtrAndIdx {
    static_assert(false, "Unsupported type");
};

// Return Iterator for POD ColumnVector
// Now only support reading values from ColumnVector
// used in cases like "where x > y" ( (int > int) -> bool )
template <PODValueType FlatType>
class ColumnVectorPtrAndIdx<FlatType> {
public:
    explicit ColumnVectorPtrAndIdx(const std::shared_ptr<ColumnVector> &col) : data_ptr_(reinterpret_cast<const FlatType *>(col->data())) {}
    // Don't return reference
    // keep compatibility with "Run(TA left, TB right, TC &result)"
    FlatType operator[](u32 index) { return data_ptr_[index]; }

private:
    const FlatType *data_ptr_ = nullptr;
};

// Return Iterator for BooleanT ColumnVector
// Now support writing BooleanT results to ColumnVector and comparing BooleanT values
// used in cases like "where x > y" ( (int > int) -> bool )
template <>
class ColumnVectorPtrAndIdx<BooleanT> {
    using IteratorType = ColumnVectorPtrAndIdx<BooleanT>;

public:
    explicit ColumnVectorPtrAndIdx(const std::shared_ptr<ColumnVector> &col) : buffer_(col->buffer_.get()) {}
    auto &SetIndex(u32 index) {
        idx_ = index;
        return *this;
    }
    auto &operator[](u32 index) { return SetIndex(index); }
    void SetValue(bool value) { buffer_->SetCompactBit(idx_, value); }
    inline bool GetValue() const { return buffer_->GetCompactBit(idx_); }
    // Does not check type.
    friend std::strong_ordering ThreeWayCompareReaderValue(const IteratorType &left, const IteratorType &right) {
        bool left_value = left.GetValue();
        bool right_value = right.GetValue();
        return left_value <=> right_value;
    }

private:
    VectorBuffer *buffer_ = nullptr;
    u32 idx_ = {};
};

// Return Iterator for VarcharT ColumnVector
// Now only support reading Varchar from ColumnVector
// used in cases like "where c1 > c2" ( (varchar > varchar) -> bool )
template <>
class ColumnVectorPtrAndIdx<VarcharT> {
    using IteratorType = ColumnVectorPtrAndIdx<VarcharT>;

public:
    explicit ColumnVectorPtrAndIdx(const std::shared_ptr<ColumnVector> &col)
        : data_ptr_(reinterpret_cast<const VarcharT *>(col->data())), col_(col) {}
    auto &SetIndex(u32 index) {
        idx_ = index;
        return *this;
    }
    auto &operator[](u32 index) { return SetIndex(index); }
    // Does not check type.
    friend std::strong_ordering ThreeWayCompareReaderValue(const IteratorType &left, const IteratorType &right) {
        std::span<const char> left_v = left.col_->GetVarchar(left.idx_);
        std::span<const char> right_v = right.col_->GetVarchar(right.idx_);
        return CompareCharArray(left_v.data(), left_v.size(), right_v.data(), right_v.size());
    }
    friend bool CheckReaderValueEquality(const IteratorType &left, const IteratorType &right) {
        std::span<const char> left_v = left.col_->GetVarchar(left.idx_);
        std::span<const char> right_v = right.col_->GetVarchar(right.idx_);
        return left_v.size() == right_v.size() && std::strncmp(left_v.data(), right_v.data(), left_v.size()) == 0;
    }

    friend void GetReaderValue(const IteratorType &left, const char *&dst, size_t &dst_len) {
        std::span<const char> left_v = left.col_->GetVarchar(left.idx_);
        dst = left_v.data();
        dst_len = left_v.size();
    }

    friend void SetReaderValue(IteratorType &left, const char *dst, int dst_len) {
        std::span<const char> span(dst, dst_len);
        left.col_->AppendVarchar(span);
    }

private:
    const VarcharT *data_ptr_ = nullptr;
    // VectorBuffer *vec_buffer_ = nullptr;
    std::shared_ptr<ColumnVector> col_;
    u32 idx_ = {};

    static std::strong_ordering CompareCharArray(const char *left_data, u32 left_len, const char *right_data, u32 right_len) {
        int res = std::strncmp(left_data, right_data, std::min(left_len, right_len));
        if (res < 0) {
            return std::strong_ordering::less;
        } else if (res > 0) {
            return std::strong_ordering::greater;
        }
        return left_len <=> right_len;
    }
};

// BooleanColumnWriter does not check null, range and type.
export using BooleanColumnWriter = ColumnVectorPtrAndIdx<BooleanT>;

// ColumnValueReader does not check null, range and type.
export template <BinaryGenerateBoolean ColumnValueType>
using ColumnValueReader = ColumnVectorPtrAndIdx<ColumnValueType>;

} // namespace infinity
