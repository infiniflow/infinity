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
import constant_expr;
import logger;

namespace infinity {

class BufferManager;
class BlockColumnEntry;

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
            UnrecoverableError("Column vector isn't initialized.");
        }
        if (vector_type_ == ColumnVectorType::kConstant) {
            if (tail_index_ >= 1) {
                UnrecoverableError("Constant column vector will only have 1 value.");
            }
        }

        if (tail_index_ >= capacity_) {
            UnrecoverableError(fmt::format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
        }
        SetValue(tail_index_++, value);
    }

    void SetVectorType(ColumnVectorType vector_type) {
        if (initialized) {
            UnrecoverableError("Column vector is initialized");
        }
        if (vector_type == ColumnVectorType::kInvalid) {
            UnrecoverableError("Invalid column vector type.");
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

    void AppendByStringView(std::string_view sv, char delimiter);

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

private:
    template <typename T>
    static void CopyValue(ColumnVector &dst, const ColumnVector &src, SizeT from, SizeT count) {
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
    void AppendEmbedding(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
        for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
            T value = DataType::StringToValue<T>(ele_str_view);
            ((T *)(data_ptr_ + dst_off))[i] = value;
            ++i;
        }
    }

    template <>
    void AppendEmbedding<BooleanT>(const Vector<std::string_view> &ele_str_views, SizeT dst_off) {
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
    void AppendTensor(const Vector<std::string_view> &ele_str_views, SizeT dst_off, SizeT unit_embedding_dim) {
        auto &[embedding_num, chunk_id, chunk_offset] = ((TensorT *)data_ptr_)[dst_off];
        const auto total_elememt_count = ele_str_views.size();
        const auto input_bytes = total_elememt_count * sizeof(T);
        if (input_bytes > DEFAULT_FIXLEN_TENSOR_CHUNK_SIZE) {
            Status status = Status::SyntaxError("Tensor size exceeds the limit.");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        embedding_num = total_elememt_count / unit_embedding_dim;
        auto tmp_data = MakeUniqueForOverwrite<T[]>(total_elememt_count);
        for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
            T value = DataType::StringToValue<T>(ele_str_view);
            tmp_data[i] = value;
            ++i;
        }
        std::tie(chunk_id, chunk_offset) = buffer_->fix_heap_mgr_->AppendToHeap(reinterpret_cast<const char *>(tmp_data.get()), input_bytes);
    }

    template <>
    void AppendTensor<BooleanT>(const Vector<std::string_view> &ele_str_views, SizeT dst_off, SizeT unit_embedding_dim) {
        auto &[embedding_num, chunk_id, chunk_offset] = ((TensorT *)data_ptr_)[dst_off];
        const auto total_elememt_count = ele_str_views.size();
        embedding_num = total_elememt_count / unit_embedding_dim;
        const auto bit_bytes = (total_elememt_count + 7) / 8;
        if (bit_bytes > DEFAULT_FIXLEN_TENSOR_CHUNK_SIZE) {
            Status status = Status::SyntaxError("Tensor size exceeds the limit.");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        auto tmp_data = MakeUnique<u8[]>(bit_bytes);
        for (SizeT i = 0; auto &ele_str_view : ele_str_views) {
            if (const auto value = DataType::StringToValue<float>(ele_str_view); value) {
                tmp_data[i / 8] |= (1u << (i % 8));
            }
            ++i;
        }
        std::tie(chunk_id, chunk_offset) = buffer_->fix_heap_mgr_->AppendToHeap(reinterpret_cast<const char *>(tmp_data.get()), bit_bytes);
    }

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

        u32 varchar_len = src_ptr->length_;
        if (src_ptr->IsInlined()) {
            // Only prefix is enough to contain all string data.
            std::memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
        } else {
            std::memcpy(dst_ptr->vector_.prefix_, src_ptr->vector_.prefix_, VARCHAR_PREFIX_LEN);
            auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(),
                                                                                       src_ptr->vector_.chunk_id_,
                                                                                       src_ptr->vector_.chunk_offset_,
                                                                                       varchar_len);
            dst_ptr->vector_.chunk_id_ = chunk_id;
            dst_ptr->vector_.chunk_offset_ = chunk_offset;
        }

        dst_ptr->length_ = varchar_len;
    }
}

template <>
inline void ColumnVector::CopyFrom<TensorT>(const VectorBuffer *__restrict src_buf,
                                            VectorBuffer *__restrict dst_buf,
                                            SizeT count,
                                            const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        TensorT *dst_ptr = &(((TensorT *)dst)[idx]);
        const TensorT *src_ptr = &(((const TensorT *)src)[row_id]);
        dst_ptr->embedding_num_ = src_ptr->embedding_num_;
        const auto tensor_bytes = src_ptr->embedding_num_ * (data_type()->type_info()->Size());
        std::tie(dst_ptr->chunk_id_, dst_ptr->chunk_offset_) =
            this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(), src_ptr->chunk_id_, src_ptr->chunk_offset_, tensor_bytes);
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

        u32 varchar_len = src_ptr->length_;
        if (src_ptr->IsInlined()) {
            // Only prefix is enough to contain all string data.
            std::memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
        } else {
            std::memcpy(dst_ptr->vector_.prefix_, src_ptr->vector_.prefix_, VARCHAR_PREFIX_LEN);
            auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(dst_buf->fix_heap_mgr_.get(),
                                                                                       src_ptr->vector_.chunk_id_,
                                                                                       src_ptr->vector_.chunk_offset_,
                                                                                       varchar_len);
            dst_ptr->vector_.chunk_id_ = chunk_id;
            dst_ptr->vector_.chunk_offset_ = chunk_offset;
        }

        dst_ptr->length_ = varchar_len;
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
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        const TensorT &src_tensor = ((const TensorT *)src)[idx];
        TensorT &dst_tensor = ((TensorT *)dst)[dest_start_idx];
        dst_tensor.embedding_num_ = src_tensor.embedding_num_;
        const auto tensor_bytes = src_tensor.embedding_num_ * (data_type()->type_info()->Size());
        std::tie(dst_tensor.chunk_id_, dst_tensor.chunk_offset_) =
            this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(), src_tensor.chunk_id_, src_tensor.chunk_offset_, tensor_bytes);
        ++dest_start_idx;
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

    u32 varchar_len = src_ptr->length_;
    if (src_ptr->IsInlined()) {
        // Only prefix is enough to contain all string data.
        std::memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
    } else {
        std::memcpy(dst_ptr->vector_.prefix_, src_ptr->vector_.prefix_, VARCHAR_PREFIX_LEN);
        auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(),
                                                                                   src_ptr->vector_.chunk_id_,
                                                                                   src_ptr->vector_.chunk_offset_,
                                                                                   varchar_len);
        dst_ptr->vector_.chunk_id_ = chunk_id;
        dst_ptr->vector_.chunk_offset_ = chunk_offset;
    }

    dst_ptr->length_ = varchar_len;
}

template <>
inline void
ColumnVector::CopyRowFrom<TensorT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetDataMut();
    const TensorT &src_tensor = ((const TensorT *)src)[src_idx];
    TensorT &dst_tensor = ((TensorT *)dst)[dst_idx];
    dst_tensor.embedding_num_ = src_tensor.embedding_num_;
    const auto tensor_bytes = src_tensor.embedding_num_ * (data_type()->type_info()->Size());
    std::tie(dst_tensor.chunk_id_, dst_tensor.chunk_offset_) =
        this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(), src_tensor.chunk_id_, src_tensor.chunk_offset_, tensor_bytes);
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
    explicit ColumnVectorPtrAndIdx(const SharedPtr<ColumnVector> &col)
        : data_ptr_(reinterpret_cast<const VarcharT *>(col->data())), fix_heap_mgr_(col->buffer_->fix_heap_mgr_.get()) {}
    auto &SetIndex(u32 index) {
        idx_ = index;
        return *this;
    }
    auto &operator[](u32 index) { return SetIndex(index); }
    // Does not check type.
    friend std::strong_ordering ThreeWayCompareReaderValue(const IteratorType &left, const IteratorType &right) {
        const VarcharT &left_value = left.data_ptr_[left.idx_];
        const VarcharT &right_value = right.data_ptr_[right.idx_];
        auto left_length = static_cast<u32>(left_value.length_);
        auto right_length = static_cast<u32>(right_value.length_);
        auto left_char_iterator = left.fix_heap_mgr_->GetNextCharIterator(left_value);
        auto right_char_iterator = right.fix_heap_mgr_->GetNextCharIterator(right_value);
        return CompareCharArray(left_char_iterator, left_length, right_char_iterator, right_length);
    }
    friend bool CheckReaderValueEquality(const IteratorType &left, const IteratorType &right) {
        const VarcharT &left_value = left.data_ptr_[left.idx_];
        const VarcharT &right_value = right.data_ptr_[right.idx_];
        auto left_length = static_cast<u32>(left_value.length_);
        auto right_length = static_cast<u32>(right_value.length_);
        if (left_length != right_length) {
            return false;
        }
        auto left_char_iterator = left.fix_heap_mgr_->GetNextCharIterator(left_value);
        auto right_char_iterator = right.fix_heap_mgr_->GetNextCharIterator(right_value);
        for (u32 i = 0; i < left_length; ++i) {
            if (left_char_iterator.GetNextChar() != right_char_iterator.GetNextChar()) {
                return false;
            }
        }
        return true;
    }

private:
    const VarcharT *data_ptr_ = nullptr;
    FixHeapManager *fix_heap_mgr_ = nullptr;
    u32 idx_ = {};
    static std::strong_ordering CompareCharArray(auto &left_char_iterator, u32 left_len, auto &right_char_iterator, u32 right_len) {
        for (u32 i = 0, com_len = std::min(left_len, right_len); i < com_len; ++i) {
            char left_char = left_char_iterator.GetNextChar();
            char right_char = right_char_iterator.GetNextChar();
            if (left_char < right_char) {
                return std::strong_ordering::less;
            } else if (left_char > right_char) {
                return std::strong_ordering::greater;
            }
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
