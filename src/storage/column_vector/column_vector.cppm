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

#include <compare>
#include <concepts>
#include <sstream>

export module column_vector;

import stl;
import global_resource_usage;
import vector_buffer;
import bitmask;
import selection;
import default_values;
import value;
import status;
import third_party;
import infinity_exception;
import fix_heap;
import internal_types;
import data_type;
import embedding_info;
import sparse_info;
import constant_expr;
import logger;
import column_def;
import logical_type;
import var_buffer;
import sparse_util;

namespace infinity {

class BufferManager;
class BlockColumnEntry;

export enum class ColumnVectorTipe : i8 {
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
    static inline SharedPtr<ColumnVector> Make(SharedPtr<DataType> data_type) { return MakeShared<ColumnVector>(std::move(data_type)); }

public:
    SizeT data_type_size_{0};

    // this buffer is holding the data
    SharedPtr<VectorBuffer> buffer_{nullptr};

    // A bitmap to indicate the null information
    SharedPtr<Bitmask> nulls_ptr_{nullptr};

    bool initialized{false};

private:
    ColumnVectorType vector_type_{ColumnVectorType::kInvalid};

    SharedPtr<DataType> data_type_;

    // Only a pointer to the real data in vector buffer
    ptr_t data_ptr_{nullptr};

    SizeT capacity_{0};

    SizeT tail_index_{0};

public:
    ColumnVector() : vector_type_(ColumnVectorType::kInvalid) {}

    // Construct a column vector without initialization;
    explicit ColumnVector(SharedPtr<DataType> data_type) : vector_type_(ColumnVectorType::kInvalid), data_type_(std::move(data_type)) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
    }

    // used in BatchInvertTask::BatchInvertTask, keep ObjectCount correct
    ColumnVector(const ColumnVector &right)
        : data_type_size_(right.data_type_size_), buffer_(right.buffer_), nulls_ptr_(right.nulls_ptr_), initialized(right.initialized),
          vector_type_(right.vector_type_), data_type_(right.data_type_), data_ptr_(right.data_ptr_), capacity_(right.capacity_),
          tail_index_(right.tail_index_) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
    }

    // used in BlockColumnIter, keep ObjectCount correct
    ColumnVector(ColumnVector &&right)
        : data_type_size_(right.data_type_size_), buffer_(std::move(right.buffer_)), nulls_ptr_(std::move(right.nulls_ptr_)),
          initialized(right.initialized), vector_type_(right.vector_type_), data_type_(std::move(right.data_type_)), data_ptr_(right.data_ptr_),
          capacity_(right.capacity_), tail_index_(right.tail_index_) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("ColumnVector");
#endif
    }

    ColumnVector &operator=(ColumnVector &&right) {
        if (this != &right) {
            data_type_size_ = right.data_type_size_;
            std::swap(buffer_, right.buffer_);
            std::swap(nulls_ptr_, right.nulls_ptr_);
            initialized = right.initialized;
            vector_type_ = right.vector_type_;
            std::swap(data_type_, right.data_type_);
            std::swap(data_ptr_, right.data_ptr_);
            capacity_ = right.capacity_;
            tail_index_ = right.tail_index_;
        }
        return *this;
    }

    ~ColumnVector() {
        // Reset(); // TODO: overload copy constructor and move constructor TO PREVENT USING `Reset`
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("ColumnVector");
#endif
    }

    String ToString() const {
        std::stringstream ss;
        for (SizeT idx = 0; idx < tail_index_; ++idx) {
            ss << ToString(idx) << std::endl;
        }
        return ss.str();
    }

    void AppendWith(const ColumnVector &other) { return AppendWith(other, 0, other.Size()); }

    void AppendValue(const Value &value) {
        if (!initialized) {
            String error_message = "Column vector isn't initialized.";
            UnrecoverableError(error_message);
        }
        if (vector_type_ == ColumnVectorType::kConstant) {
            if (tail_index_ >= 1) {
                String error_message = "Constant column vector will only have 1 value.";
                UnrecoverableError(error_message);
            }
        }

        if (tail_index_ >= capacity_) {
            String error_message = fmt::format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_);
            UnrecoverableError(error_message);
        }
        SetValue(tail_index_++, value);
    }

    void SetVectorType(ColumnVectorType vector_type) {
        if (initialized) {
            String error_message = "Column vector isn't initialized.";
            UnrecoverableError(error_message);
        }
        if (vector_type == ColumnVectorType::kInvalid) {
            String error_message = "Invalid column vector type.";
            UnrecoverableError(error_message);
        }
        if (vector_type_ == vector_type) {
            return;
        }
        this->Reset();
        this->Initialize(vector_type, DEFAULT_VECTOR_SIZE);
    }

private:
    VectorBufferType InitializeHelper(ColumnVectorType vector_type, SizeT capacity);

public:
    void Initialize(ColumnVectorType vector_type = ColumnVectorType::kFlat, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void Initialize(BufferManager *buffer_mgr,
                    BlockColumnEntry *block_column_entry,
                    SizeT current_row_count,
                    ColumnVectorTipe vector_tipe = ColumnVectorTipe::kReadWrite,
                    ColumnVectorType vector_type = ColumnVectorType::kFlat,
                    SizeT capacity = DEFAULT_VECTOR_SIZE);

    void Initialize(const ColumnVector &other, const Selection &input_select);

    void Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx);

    void Initialize(const ColumnVector &other, SizeT start_idx, SizeT end_idx) { Initialize(other.vector_type_, other, start_idx, end_idx); }

    String ToString(SizeT row_index) const;

    // Return the <index> of the vector
    // Since it will construct a new Value object, this function shouldn't be used in vectorized computation.
    // Directly uses data_ptr in vectorized computation.
    Value GetValue(SizeT index) const;

    // Set the <index> element of the vector to the specified value.
    void SetValue(SizeT index, const Value &Value);

    void Finalize(SizeT index);

    void AppendByPtr(const_ptr_t value_ptr);

    void AppendByStringView(std::string_view sv);

    void AppendByConstantExpr(const ConstantExpr *const_expr);

    void AppendWith(const ColumnVector &other, SizeT start_row, SizeT count);

    // input parameter:
    // from - start RowID
    // count - total row count to be copied. These rows shall be in the same BlockEntry.
    // return value: appended rows actually
    SizeT AppendWith(RowID from, SizeT count);

    void ShallowCopy(const ColumnVector &other);

    void Reset();

    bool operator==(const ColumnVector &other) const;
    bool operator!=(const ColumnVector &other) const { return !(*this == other); }

    // Estimated serialized size in bytes
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;

    // Read from a serialized version
    static SharedPtr<ColumnVector> ReadAdv(char *&ptr, i32 maxbytes);

public:
    static void SetMultiVector(MultiVectorT &dest_multi_vec, VectorBuffer *dest_buffer, Span<const char> data, const EmbeddingInfo *embedding_info);

    static Pair<Span<const char>, SizeT> GetMultiVector(const MultiVectorT &multi_vec, const VectorBuffer *buffer, const EmbeddingInfo *embedding_info);

    static void SetTensor(TensorT &dest_tensor, VectorBuffer *dest_buffer, Span<const char> data, const EmbeddingInfo *embedding_info);

    static Pair<Span<const char>, SizeT> GetTensor(const TensorT &tensor, const VectorBuffer *buffer, const EmbeddingInfo *embedding_info);

    static void SetTensorArrayMeta(TensorArrayT &dest_tensor_array, VectorBuffer *dest_buffer, Span<const TensorT> tensors);

    static Span<const TensorT> GetTensorArrayMeta(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer);

    static void SetTensorArray(TensorArrayT &dest_tensor_array,
                               VectorBuffer *dest_buffer,
                               const Vector<Span<const char>> &data,
                               const EmbeddingInfo *embedding_info);

    static Vector<Pair<Span<const char>, SizeT>>
    GetTensorArray(const TensorArrayT &src_tensor_array, const VectorBuffer *src_buffer, const EmbeddingInfo *embedding_info);

    Pair<Span<const char>, SizeT> GetMultiVectorRaw(SizeT idx) const;

    Pair<Span<const char>, SizeT> GetTensorRaw(SizeT idx) const;

    Vector<Pair<Span<const char>, SizeT>> GetTensorArrayRaw(SizeT idx) const;

private:
    template <typename T>
    static void CopyValue(ColumnVector &dst, const ColumnVector &src, SizeT from, SizeT count);

    template <typename T>
    void AppendEmbedding(const Vector<std::string_view> &ele_str_views, SizeT dst_off);

    template <typename T>
    void AppendMultiVector(const Vector<std::string_view> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendTensor(const Vector<std::string_view> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendTensorArray(const Vector<Vector<std::string_view>> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info);

    template <typename T>
    void AppendSparse(const Vector<std::string_view> &ele_str_views, SizeT dst_off);

    template <typename DataT, typename IdxT>
    void AppendSparse(const Vector<std::string_view> &ele_str_views, SizeT dst_off);

public:
    template <typename DataT, typename IdxT>
    void AppendSparseInner(SizeT nnz, const DataT *data, const IdxT *index, SparseT &sparse);

    template <typename DataT, typename IdxT>
    void AppendSparseInner(SizeT nnz, const DataT *data, const IdxT *index, SizeT dst_off) {
        auto &sparse = reinterpret_cast<SparseT *>(data_ptr_)[dst_off];
        AppendSparseInner(nnz, data, index, sparse);
    }

    template <typename DataT, typename IdxT>
    void AppendSparse(SizeT nnz, const DataT *data, const IdxT *index) {
        SizeT dst_off = tail_index_++;
        AppendSparseInner(nnz, data, index, dst_off);
    }

    void AppendSparseRaw(const char *raw_data_ptr, const char *raw_index_ptr, SizeT nnz, SizeT dst_off);

    Tuple<Span<const char>, Span<const char>, SizeT> GetSparseRaw(SizeT index) const;

public:
    void AppendVarcharInner(Span<const char> data, VarcharT &varchar);

    void AppendVarcharInner(Span<const char> data, SizeT dst_off);

    void AppendVarchar(Span<const char> data);

    Span<const char> GetVarcharInner(const VarcharT &varchar) const;

    Span<const char> GetVarchar(SizeT index) const;

private:
    // Used by Append by Ptr
    void SetByRawPtr(SizeT index, const_ptr_t raw_ptr);

    void CopyRow(const ColumnVector &other, SizeT dst_idx, SizeT src_idx);

    template <typename DataT>
    inline void CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT count, const Selection &input_select);

    template <typename DataT>
    inline void
    CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT source_start_idx, SizeT dest_start_idx, SizeT count);

    template <typename DataT>
    inline void CopyRowFrom(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx);

public:
    [[nodiscard]] const inline ColumnVectorType &vector_type() const { return vector_type_; }

    [[nodiscard]] const inline SharedPtr<DataType> data_type() const { return data_type_; }

    [[nodiscard]] inline ptr_t data() const { return data_ptr_; }

    [[nodiscard]] inline SizeT capacity() const { return capacity_; }

    [[nodiscard]] inline SizeT Size() const { return tail_index_; }
};

template <typename T>
void ColumnVector::CopyValue(ColumnVector &dst, const ColumnVector &src, SizeT from, SizeT count) {
    auto *src_ptr = (T *)(src.data_ptr_);
    T *dst_ptr = &((T *)(dst.data_ptr_))[dst.tail_index_];
    if (src.vector_type() == ColumnVectorType::kConstant && src.tail_index_ == 1) {
        for (SizeT idx = 0; idx < count; ++idx) {
            dst_ptr[idx] = src_ptr[from];
        }
    } else {
        for (SizeT idx = 0; idx < count; ++idx) {
            dst_ptr[idx] = src_ptr[from + idx];
        }
    }
}

template <typename T>
void ColumnVector::AppendEmbedding(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
    for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
        T value = DataType::StringToValue<T>(ele_str_view);
        ((T *)(data_ptr_ + dst_off))[i] = value;
        ++i;
    }
}

template <>
void ColumnVector::AppendEmbedding<BooleanT>(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
    const auto bit_bytes = (ele_str_views.size() + 7) / 8;
    auto data_ptr = reinterpret_cast<u8 *>(data_ptr_ + dst_off);
    std::fill_n(data_ptr, bit_bytes, 0);
    for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
        if (const auto value = DataType::StringToValue<float>(ele_str_view); value) {
            data_ptr[i / 8] |= (1u << (i % 8));
        }
        ++i;
    }
}

template <typename T>
Pair<UniquePtr<char[]>, SizeT> StrToTensor(const Vector<std::string_view> &ele_str_views, const EmbeddingInfo *embedding_info) {
    SizeT total_element_count = ele_str_views.size();
    auto tmp_data = MakeUniqueForOverwrite<char[]>(total_element_count * sizeof(T));
    auto *ptr = reinterpret_cast<T *>(tmp_data.get());
    for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
        ptr[i] = DataType::StringToValue<T>(ele_str_view);
        ++i;
    }
    return {std::move(tmp_data), total_element_count * sizeof(T)};
}

template <>
Pair<UniquePtr<char[]>, SizeT> StrToTensor<bool>(const Vector<std::string_view> &ele_str_views, const EmbeddingInfo *embedding_info) {
    SizeT total_element_count = ele_str_views.size();
    SizeT bit_bytes = (total_element_count + 7) / 8;
    auto tmp_data = MakeUnique<char[]>(bit_bytes);
    for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
        if (const auto value = DataType::StringToValue<float>(ele_str_view); value) {
            tmp_data[i / 8] |= (1u << (i % 8));
        }
        ++i;
    }
    return {std::move(tmp_data), bit_bytes};
}

template <typename T>
void ColumnVector::AppendMultiVector(const Vector<std::string_view> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info) {
    MultiVectorT &target_multivector = reinterpret_cast<MultiVectorT *>(data_ptr_)[dst_off];
    auto [data, data_bytes] = StrToTensor<T>(ele_str_views, embedding_info);
    Span<const char> data_span(data.get(), data_bytes);
    ColumnVector::SetMultiVector(target_multivector, buffer_.get(), data_span, embedding_info);
}

template <typename T>
void ColumnVector::AppendTensor(const Vector<std::string_view> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info) {
    TensorT &target_tensor = reinterpret_cast<TensorT *>(data_ptr_)[dst_off];
    auto [data, data_bytes] = StrToTensor<T>(ele_str_views, embedding_info);
    Span<const char> data_span(data.get(), data_bytes);
    ColumnVector::SetTensor(target_tensor, buffer_.get(), data_span, embedding_info);
}

template <typename T>
void ColumnVector::AppendTensorArray(const Vector<Vector<std::string_view>> &ele_str_views, SizeT dst_off, const EmbeddingInfo *embedding_info) {
    TensorArrayT &target_tensor_array = reinterpret_cast<TensorArrayT *>(data_ptr_)[dst_off];
    SizeT total_tensor_count = ele_str_views.size();
    Vector<UniquePtr<char[]>> tensors(total_tensor_count);
    Vector<Span<const char>> tensor_spans(total_tensor_count);
    for (SizeT i = 0; i < total_tensor_count; ++i) {
        auto [data, data_bytes] = StrToTensor<T>(ele_str_views[i], embedding_info);
        tensors[i] = std::move(data);
        tensor_spans[i] = Span<const char>(tensors[i].get(), data_bytes);
    }
    ColumnVector::SetTensorArray(target_tensor_array, buffer_.get(), tensor_spans, embedding_info);
}

template <typename T>
void ColumnVector::AppendSparse(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
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
            String error_message = "Unsupported sparse index type.";
            UnrecoverableError(error_message);
        }
    }
}

template <typename DataT, typename IdxT>
void ColumnVector::AppendSparse(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
    SizeT total_element_count = ele_str_views.size();
    Vector<IdxT> index_vec;
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
        Vector<DataT> data_vec;
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
void ColumnVector::AppendSparseInner(SizeT nnz, const DataT *data, const IdxT *index, SparseT &target_sparse) {
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
        UniquePtr<IdxT[]> tmp_index;
        if (to_sort) {
            tmp_index = MakeUniqueForOverwrite<IdxT[]>(nnz);
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
        UniquePtr<IdxT[]> tmp_index;
        UniquePtr<DataT[]> tmp_data;
        if (to_sort) {
            tmp_index = MakeUniqueForOverwrite<IdxT[]>(nnz);
            tmp_data = MakeUniqueForOverwrite<DataT[]>(nnz);
            Vector<Pair<IdxT, DataT>> index_value_pairs(nnz);
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

template <>
void ColumnVector::CopyValue<BooleanT>(ColumnVector &dst, const ColumnVector &src, SizeT from, SizeT count) {
    auto dst_tail = dst.tail_index_;
    const VectorBuffer *src_buffer = src.buffer_.get();
    auto dst_buffer = dst.buffer_.get();
    if (dst_tail % 8 == 0 && from % 8 == 0) {
        SizeT dst_byte_offset = dst_tail / 8;
        SizeT src_byte_offset = from / 8;
        SizeT byte_count = (count + 7) / 8; // copy to tail
        std::memcpy(dst_buffer->GetDataMut() + dst_byte_offset, src_buffer->GetData() + src_byte_offset, byte_count);
    } else {
        for (SizeT idx = 0; idx < count; ++idx) {
            dst_buffer->SetCompactBit(dst_tail + idx, src_buffer->GetCompactBit(from + idx));
        }
    }
}

template <typename DataT>
inline void
ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT count, const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        ((DataT *)(dst))[idx] = ((const DataT *)(src))[row_id];
    }
}

template <>
inline void ColumnVector::CopyFrom<BooleanT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT count,
                                             const Selection &input_select) {
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        dst_buf->SetCompactBit(idx, src_buf->GetCompactBit(row_id));
    }
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT count,
                                             const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        VarcharT *dst_ptr = &(((VarcharT *)dst)[idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[row_id]);
        CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
    }
}

template <>
inline void ColumnVector::CopyFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 SizeT count,
                                                 const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        MultiVectorT *dst_ptr = &(((MultiVectorT *)dst)[idx]);
        const MultiVectorT *src_ptr = &(((const MultiVectorT *)src)[row_id]);
        CopyMultiVector(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT count,
                                            const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        TensorT *dst_ptr = &(((TensorT *)dst)[idx]);
        const TensorT *src_ptr = &(((const TensorT *)src)[row_id]);
        CopyTensor(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 SizeT count,
                                                 const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        TensorArrayT *dst_ptr = &(((TensorArrayT *)dst)[idx]);
        const TensorArrayT *src_ptr = &(((const TensorArrayT *)src)[row_id]);
        CopyTensorArray(*dst_ptr, dst_buf, *src_ptr, src_buf, embedding_info);
    }
}

template <>
inline void ColumnVector::CopyFrom<SparseT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT count,
                                            const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT dest_idx = input_select[idx];

        auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dest_idx;
        const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + idx;

        CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
    }
}

#if 0

template <>
inline void
ColumnVector::CopyFrom<PathT>(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT count, const Selection &input_select) {

    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        PathT *dst_ptr = &(((PathT *)dst)[idx]);
        const PathT *src_ptr = &(((const PathT *)src)[row_id]);

        u32 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
        std::memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->closed = src_ptr->closed;
    }
}

template <>
inline void ColumnVector::CopyFrom<PolygonT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT count,
                                             const Selection &input_select) {
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        PolygonT *dst_ptr = &(((PolygonT *)dst)[idx]);
        const PolygonT *src_ptr = &(((const PolygonT *)src)[row_id]);

        u64 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
        std::memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->bounding_box = src_ptr->bounding_box;
    }
}

template <>
inline void ColumnVector::CopyFrom<BitmapT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT count,
                                            const Selection &input_select) {
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        BitmapT *dst_ptr = &(((BitmapT *)dst)[idx]);
        const BitmapT *src_ptr = &(((const BitmapT *)src)[row_id]);

        u64 bit_count = src_ptr->count;
        u64 unit_count = BitmapT::UnitCount(bit_count);

        SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
        std::memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

        dst_ptr->ptr = (u64 *)ptr;
        dst_ptr->count = bit_count;
    }
}

template <>
inline void
ColumnVector::CopyFrom<BlobT>(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT count, const Selection &input_select) {
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        BlobT *dst_ptr = &(((BlobT *)dst)[idx]);
        const BlobT *src_ptr = &(((const BlobT *)src)[row_id]);

        u64 blob_size = src_ptr->size;
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
        std::memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

        dst_ptr->ptr = ptr;
        dst_ptr->size = blob_size;
    }
}

#endif

template <>
inline void ColumnVector::CopyFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf,
                                               VectorBuffer *__restrict dst_buf,
                                               SizeT count,
                                               const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();

    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        const_ptr_t src_ptr = src + row_id * data_type_size_;
        ptr_t dst_ptr = dst + idx * data_type_size_;
        std::memcpy(dst_ptr, src_ptr, data_type_size_);
    }
}

template <typename DataT>
inline void ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf,
                                   VectorBuffer *__restrict dst_buf,
                                   SizeT source_start_idx,
                                   SizeT dest_start_idx,
                                   SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();

    SizeT source_end_idx = source_start_idx + count;

    Copy(((const DataT *)(src)) + source_start_idx, ((const DataT *)(src)) + source_end_idx, ((DataT *)(dst)) + dest_start_idx);
}

template <>
inline void ColumnVector::CopyFrom<BooleanT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT source_start_idx,
                                             SizeT dest_start_idx,
                                             SizeT count) {
    VectorBuffer::CopyCompactBits(reinterpret_cast<u8 *>(dst_buf->GetDataMut()),
                                  reinterpret_cast<const u8 *>(src_buf->GetData()),
                                  dest_start_idx,
                                  source_start_idx,
                                  count);
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT source_start_idx,
                                             SizeT dest_start_idx,
                                             SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();

    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        VarcharT *dst_ptr = &(((VarcharT *)dst)[dest_start_idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[idx]);
        CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 SizeT source_start_idx,
                                                 SizeT dest_start_idx,
                                                 SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    SizeT source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        const MultiVectorT &src_multivector = ((const MultiVectorT *)src)[idx];
        MultiVectorT &dst_multivector = ((MultiVectorT *)dst)[dest_start_idx];
        CopyMultiVector(dst_multivector, dst_buf, src_multivector, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT source_start_idx,
                                            SizeT dest_start_idx,
                                            SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    SizeT source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        const TensorT &src_tensor = ((const TensorT *)src)[idx];
        TensorT &dst_tensor = ((TensorT *)dst)[dest_start_idx];
        CopyTensor(dst_tensor, dst_buf, src_tensor, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf,
                                                 VectorBuffer *__restrict dst_buf,
                                                 SizeT source_start_idx,
                                                 SizeT dest_start_idx,
                                                 SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    SizeT source_end_idx = source_start_idx + count;
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        const TensorArrayT &src_ref = ((const TensorArrayT *)src)[idx];
        TensorArrayT &dst_ref = ((TensorArrayT *)dst)[dest_start_idx];
        CopyTensorArray(dst_ref, dst_buf, src_ref, src_buf, embedding_info);
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<SparseT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT source_start_idx,
                                            SizeT dest_start_idx,
                                            SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx, dst_idx = dest_start_idx; idx < source_end_idx; ++idx, ++dst_idx) {
        const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + idx;
        auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dst_idx;

        CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
    }
}

#if 0
template <>
inline void ColumnVector::CopyFrom<PathT>(const VectorBuffer *__restrict src_buf,
                                          VectorBuffer *__restrict dst_buf,
                                          SizeT source_start_idx,
                                          SizeT dest_start_idx,
                                          SizeT count) {
    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        PathT *dst_ptr = &(((PathT *)dst)[dest_start_idx]);
        const PathT *src_ptr = &(((const PathT *)src)[idx]);

        u32 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
        std::memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->closed = src_ptr->closed;

        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<PolygonT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT source_start_idx,
                                             SizeT dest_start_idx,
                                             SizeT count) {
    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        PolygonT *dst_ptr = &(((PolygonT *)dst)[dest_start_idx]);
        const PolygonT *src_ptr = &(((const PolygonT *)src)[idx]);

        u64 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
        std::memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->bounding_box = src_ptr->bounding_box;
        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<BitmapT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT source_start_idx,
                                            SizeT dest_start_idx,
                                            SizeT count) {
    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {

        BitmapT *dst_ptr = &(((BitmapT *)dst)[dest_start_idx]);
        const BitmapT *src_ptr = &(((const BitmapT *)src)[idx]);

        u64 bit_count = src_ptr->count;
        u64 unit_count = BitmapT::UnitCount(bit_count);

        SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
        std::memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

        dst_ptr->ptr = (u64 *)ptr;
        dst_ptr->count = bit_count;

        ++dest_start_idx;
    }
}

template <>
inline void ColumnVector::CopyFrom<BlobT>(const VectorBuffer *__restrict src_buf,
                                          VectorBuffer *__restrict dst_buf,
                                          SizeT source_start_idx,
                                          SizeT dest_start_idx,
                                          SizeT count) {
    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {

        BlobT *dst_ptr = &(((BlobT *)dst)[dest_start_idx]);
        const BlobT *src_ptr = &(((const BlobT *)src)[idx]);

        u64 blob_size = src_ptr->size;
        ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
        std::memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

        dst_ptr->ptr = ptr;
        dst_ptr->size = blob_size;

        ++dest_start_idx;
    }
}

#endif

template <>
inline void ColumnVector::CopyFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf,
                                               VectorBuffer *__restrict dst_buf,
                                               SizeT source_start_idx,
                                               SizeT dest_start_idx,
                                               SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();

    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {

        const_ptr_t src_ptr = src + idx * data_type_size_;
        ptr_t dst_ptr = dst + dest_start_idx * data_type_size_;
        std::memcpy(dst_ptr, src_ptr, data_type_size_);

        ++dest_start_idx;
    }
}

template <typename DataT>
inline void ColumnVector::CopyRowFrom(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();

    ((DataT *)(dst))[dst_idx] = ((const DataT *)(src))[src_idx];
}

template <>
inline void
ColumnVector::CopyRowFrom<BooleanT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    dst_buf->SetCompactBit(dst_idx, src_buf->GetCompactBit(src_idx));
}

template <>
inline void
ColumnVector::CopyRowFrom<VarcharT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    VarcharT *dst_ptr = &(((VarcharT *)dst)[dst_idx]);
    const VarcharT *src_ptr = &(((const VarcharT *)src)[src_idx]);
    CopyVarchar(*dst_ptr, dst_buf, *src_ptr, src_buf);
}

template <>
inline void
ColumnVector::CopyRowFrom<MultiVectorT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const MultiVectorT &src_multivector = ((const MultiVectorT *)src)[src_idx];
    MultiVectorT &dst_multivector = ((MultiVectorT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyMultiVector(dst_multivector, dst_buf, src_multivector, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<TensorT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const TensorT &src_tensor = ((const TensorT *)src)[src_idx];
    TensorT &dst_tensor = ((TensorT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyTensor(dst_tensor, dst_buf, src_tensor, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<TensorArrayT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const TensorArrayT &src_ref = ((const TensorArrayT *)src)[src_idx];
    TensorArrayT &dst_ref = ((TensorArrayT *)dst)[dst_idx];
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type_->type_info().get());
    CopyTensorArray(dst_ref, dst_buf, src_ref, src_buf, embedding_info);
}

template <>
inline void
ColumnVector::CopyRowFrom<SparseT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const auto *sparse_info = static_cast<const SparseInfo *>(data_type_->type_info().get());

    const auto *src_sparse = reinterpret_cast<const SparseT *>(src) + src_idx;
    auto *dst_sparse = reinterpret_cast<SparseT *>(dst) + dst_idx;

    CopySparse(*dst_sparse, dst_buf, *src_sparse, src_buf, sparse_info);
}

#if 0

template <>
inline void ColumnVector::CopyRowFrom<PathT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    PathT *dst_ptr = &(((PathT *)dst)[dst_idx]);
    const PathT *src_ptr = &(((const PathT *)src)[src_idx]);

    u32 point_count = src_ptr->point_count;

    SizeT point_area_size = point_count * sizeof(PointT);
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
    std::memcpy(ptr, src_ptr->ptr, point_area_size);

    dst_ptr->ptr = ptr;
    dst_ptr->point_count = point_count;
    dst_ptr->closed = src_ptr->closed;
}

template <>
inline void
ColumnVector::CopyRowFrom<PolygonT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    PolygonT *dst_ptr = &(((PolygonT *)dst)[dst_idx]);
    const PolygonT *src_ptr = &(((const PolygonT *)src)[src_idx]);

    u64 point_count = src_ptr->point_count;

    SizeT point_area_size = point_count * sizeof(PointT);
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
    std::memcpy(ptr, src_ptr->ptr, point_area_size);

    dst_ptr->ptr = ptr;
    dst_ptr->point_count = point_count;
    dst_ptr->bounding_box = src_ptr->bounding_box;
}

template <>
inline void
ColumnVector::CopyRowFrom<BitmapT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    BitmapT *dst_ptr = &(((BitmapT *)dst)[dst_idx]);
    const BitmapT *src_ptr = &(((const BitmapT *)src)[src_idx]);

    u64 bit_count = src_ptr->count;
    u64 unit_count = BitmapT::UnitCount(bit_count);

    SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(bit_area_size);
    std::memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

    dst_ptr->ptr = (u64 *)ptr;
    dst_ptr->count = bit_count;
}

template <>
inline void ColumnVector::CopyRowFrom<BlobT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    BlobT *dst_ptr = &(((BlobT *)dst)[dst_idx]);
    const BlobT *src_ptr = &(((const BlobT *)src)[src_idx]);

    u64 blob_size = src_ptr->size;
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
    std::memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

    dst_ptr->ptr = ptr;
    dst_ptr->size = blob_size;
}

#endif

template <>
inline void
ColumnVector::CopyRowFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const_ptr_t src_ptr = src + src_idx * data_type_size_;
    ptr_t dst_ptr = dst + dst_idx * data_type_size_;
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
                               UuidT>;

export template <typename ValueType>
concept BinaryGenerateBoolean = PODValueType<ValueType> or IsAnyOf<ValueType, BooleanT, VarcharT>;

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
    explicit ColumnVectorPtrAndIdx(const SharedPtr<ColumnVector> &col) : data_ptr_(reinterpret_cast<const FlatType *>(col->data())) {}
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
    explicit ColumnVectorPtrAndIdx(const SharedPtr<ColumnVector> &col) : buffer_(col->buffer_.get()) {}
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
    explicit ColumnVectorPtrAndIdx(const SharedPtr<ColumnVector> &col) : data_ptr_(reinterpret_cast<const VarcharT *>(col->data())), col_(col) {}
    auto &SetIndex(u32 index) {
        idx_ = index;
        return *this;
    }
    auto &operator[](u32 index) { return SetIndex(index); }
    // Does not check type.
    friend std::strong_ordering ThreeWayCompareReaderValue(const IteratorType &left, const IteratorType &right) {
        Span<const char> left_v = left.col_->GetVarchar(left.idx_);
        Span<const char> right_v = right.col_->GetVarchar(right.idx_);
        return CompareCharArray(left_v.data(), left_v.size(), right_v.data(), right_v.size());
    }
    friend bool CheckReaderValueEquality(const IteratorType &left, const IteratorType &right) {
        Span<const char> left_v = left.col_->GetVarchar(left.idx_);
        Span<const char> right_v = right.col_->GetVarchar(right.idx_);
        return left_v.size() == right_v.size() && std::strncmp(left_v.data(), right_v.data(), left_v.size()) == 0;
    }

private:
    const VarcharT *data_ptr_ = nullptr;
    // VectorBuffer *vec_buffer_ = nullptr;
    SharedPtr<ColumnVector> col_;
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
