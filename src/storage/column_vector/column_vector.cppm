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

#include <sstream>

import stl;
import parser;
import global_resource_usage;
import vector_buffer;
import bitmask;
import selection;
import default_values;
import value;
import column_buffer;
import third_party;
import infinity_exception;

export module column_vector;

namespace infinity {

export enum class ColumnVectorType : i8 {
    kInvalid,
    kFlat,          // Stand without any encode
    kConstant,      // All vector has same type and value
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
    static inline SharedPtr<ColumnVector> Make(SharedPtr<DataType> data_type) { return MakeShared<ColumnVector>(Move(data_type)); }

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
    explicit ColumnVector(SharedPtr<DataType> data_type) : vector_type_(ColumnVectorType::kInvalid), data_type_(Move(data_type)) {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~ColumnVector() {
        Reset();
        GlobalResourceUsage::DecrObjectCount();
    }

    void Initialize(const ColumnVector &other, SizeT start_idx, SizeT end_idx) { Initialize(other.vector_type_, other, start_idx, end_idx); }

    String ToString() const {
        std::stringstream ss;
        for (SizeT idx = 0; idx < tail_index_; ++idx) {
            ss << ToString(idx) << " ";
        }
        return ss.str();
    }

    void AppendWith(const ColumnVector &other) { return AppendWith(other, 0, other.Size()); }

    void AppendValue(const Value &value) {
        if (!initialized) {
            Error<StorageException>("Column vector isn't initialized.");
        }
        if (vector_type_ == ColumnVectorType::kConstant) {
            if (tail_index_ >= 1) {
                Error<StorageException>("Constant column vector will only have 1 value.");
            }
        }

        if (tail_index_ >= capacity_) {
            Error<StorageException>(Format("Exceed the column vector capacity.({}/{})", tail_index_, capacity_));
        }
        SetValue(tail_index_++, value);
    }

    void SetVectorType(ColumnVectorType vector_type) {
        if (initialized) {
            Error<TypeException>("Column vector is initialized");
        }
        if (vector_type == ColumnVectorType::kInvalid) {
            Error<TypeException>("Invalid column vector type.");
        }
        if (vector_type_ == vector_type) {
            return;
        }
        this->Reset();
        this->Initialize(vector_type, DEFAULT_VECTOR_SIZE);
    }

public:
    void Initialize(const ColumnVector &other, const Selection &input_select);

    void Initialize(ColumnVectorType vector_type = ColumnVectorType::kFlat, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void Initialize(ColumnVectorType vector_type, const ColumnVector &other, SizeT start_idx, SizeT end_idx);

    String ToString(SizeT row_index) const;

    // Return the <index> of the vector
    // Since it will construct a new Value object, this function shouldn't be used in vectorized computation.
    // Directly uses data_ptr in vectorized computation.
    Value GetValue(SizeT index) const;

    // Set the <index> element of the vector to the specified value.
    void SetValue(SizeT index, const Value &Value);

    void Finalize(SizeT index);

    void AppendByPtr(const_ptr_t value_ptr);

    // This two should merge into one function because `ColumnBuffer` will be removed.
    void AppendWith(const ColumnVector &other, SizeT start_row, SizeT count);

    // input parameter:
    // column_buffer - input column
    // start_row - start row number of ptr
    // row_count - total row count to be copied
    // return value: appended rows actually
    SizeT AppendWith(ColumnBuffer &column_buffer, SizeT start_row, SizeT row_count);

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
    static void CopyValue(const ColumnVector &src, const ColumnVector &dst, SizeT from, SizeT count) {
        auto *src_ptr = (T *)(dst.data_ptr_);
        T *dst_ptr = &((T *)(src.data_ptr_))[src.tail_index_];
        for (SizeT idx = 0; idx < count; ++idx) {
            dst_ptr[idx] = src_ptr[from + idx];
        }
    }

    // Used by Append by Ptr
    void SetByRawPtr(SizeT index, const_ptr_t raw_ptr);

    // Use by Append value
    void SetByPtr(SizeT index, const_ptr_t value_ptr);

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

template <typename DataT>
inline void
ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf, VectorBuffer *__restrict dst_buf, SizeT count, const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();
    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];
        ((DataT *)(dst))[idx] = ((const DataT *)(src))[row_id];
    }
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT count,
                                             const Selection &input_select) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();

    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        VarcharT *dst_ptr = &(((VarcharT *)dst)[idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[row_id]);

        u32 varchar_len = src_ptr->length_;
        if (src_ptr->IsInlined()) {
            // Only prefix is enough to contain all string data.
            Memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
        } else {
            Memcpy(dst_ptr->vector_.prefix_, src_ptr->value_.prefix_, VARCHAR_PREFIX_LEN);
            auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(dst_buf->fix_heap_mgr_.get(),
                                                                                       src_ptr->vector_.chunk_id_,
                                                                                       src_ptr->vector_.chunk_offset_,
                                                                                       varchar_len);
            dst_ptr->vector_.chunk_id_ = chunk_id;
            dst_ptr->vector_.chunk_offset_ = chunk_offset;
        }

        dst_ptr->length_ = varchar_len;
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
        Memcpy(ptr, src_ptr->ptr, point_area_size);

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
        Memcpy(ptr, src_ptr->ptr, point_area_size);

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
        Memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

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
        Memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

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
    ptr_t dst = dst_buf->GetData();

    for (SizeT idx = 0; idx < count; ++idx) {
        SizeT row_id = input_select[idx];

        const_ptr_t src_ptr = src + row_id * data_type_size_;
        ptr_t dst_ptr = dst + idx * data_type_size_;
        Memcpy(dst_ptr, src_ptr, data_type_size_);
    }
}

template <typename DataT>
inline void ColumnVector::CopyFrom(const VectorBuffer *__restrict src_buf,
                                   VectorBuffer *__restrict dst_buf,
                                   SizeT source_start_idx,
                                   SizeT dest_start_idx,
                                   SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();

    SizeT source_end_idx = source_start_idx + count;

    std::copy(((const DataT *)(src)) + source_start_idx, ((const DataT *)(src)) + source_end_idx, ((DataT *)(dst)) + dest_start_idx);
}

template <>
inline void ColumnVector::CopyFrom<VarcharT>(const VectorBuffer *__restrict src_buf,
                                             VectorBuffer *__restrict dst_buf,
                                             SizeT source_start_idx,
                                             SizeT dest_start_idx,
                                             SizeT count) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();

    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {
        VarcharT *dst_ptr = &(((VarcharT *)dst)[dest_start_idx]);
        const VarcharT *src_ptr = &(((const VarcharT *)src)[idx]);

        u32 varchar_len = src_ptr->length_;
        if (src_ptr->IsInlined()) {
            // Only prefix is enough to contain all string data.
            Memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
        } else {
            Memcpy(dst_ptr->vector_.prefix_, src_ptr->value_.prefix_, VARCHAR_PREFIX_LEN);
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
        Memcpy(ptr, src_ptr->ptr, point_area_size);

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
        Memcpy(ptr, src_ptr->ptr, point_area_size);

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
        Memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

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
        Memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

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
    ptr_t dst = dst_buf->GetData();

    SizeT source_end_idx = source_start_idx + count;
    for (SizeT idx = source_start_idx; idx < source_end_idx; ++idx) {

        const_ptr_t src_ptr = src + idx * data_type_size_;
        ptr_t dst_ptr = dst + dest_start_idx * data_type_size_;
        Memcpy(dst_ptr, src_ptr, data_type_size_);

        ++dest_start_idx;
    }
}

template <typename DataT>
inline void ColumnVector::CopyRowFrom(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();

    ((DataT *)(dst))[dst_idx] = ((const DataT *)(src))[src_idx];
}

template <>
inline void
ColumnVector::CopyRowFrom<VarcharT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {

    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();

    VarcharT *dst_ptr = &(((VarcharT *)dst)[dst_idx]);
    const VarcharT *src_ptr = &(((const VarcharT *)src)[src_idx]);

    u32 varchar_len = src_ptr->length_;
    if (src_ptr->IsInlined()) {
        // Only prefix is enough to contain all string data.
        Memcpy(dst_ptr->short_.data_, src_ptr->short_.data_, varchar_len);
    } else {
        Memcpy(dst_ptr->vector_.prefix_, src_ptr->value_.prefix_, VARCHAR_PREFIX_LEN);
        auto [chunk_id, chunk_offset] = this->buffer_->fix_heap_mgr_->AppendToHeap(src_buf->fix_heap_mgr_.get(),
                                                                                   src_ptr->vector_.chunk_id_,
                                                                                   src_ptr->vector_.chunk_offset_,
                                                                                   varchar_len);
        dst_ptr->vector_.chunk_id_ = chunk_id;
        dst_ptr->vector_.chunk_offset_ = chunk_offset;
    }

    dst_ptr->length_ = varchar_len;
}

#if 0

template <>
inline void ColumnVector::CopyRowFrom<PathT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    PathT *dst_ptr = &(((PathT *)dst)[dst_idx]);
    const PathT *src_ptr = &(((const PathT *)src)[src_idx]);

    u32 point_count = src_ptr->point_count;

    SizeT point_area_size = point_count * sizeof(PointT);
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(point_area_size);
    Memcpy(ptr, src_ptr->ptr, point_area_size);

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
    Memcpy(ptr, src_ptr->ptr, point_area_size);

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
    Memcpy(ptr, (void *)(src_ptr->ptr), bit_area_size);

    dst_ptr->ptr = (u64 *)ptr;
    dst_ptr->count = bit_count;
}

template <>
inline void ColumnVector::CopyRowFrom<BlobT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    BlobT *dst_ptr = &(((BlobT *)dst)[dst_idx]);
    const BlobT *src_ptr = &(((const BlobT *)src)[src_idx]);

    u64 blob_size = src_ptr->size;
    ptr_t ptr = this->buffer_->fix_heap_mgr_->Allocate(blob_size);
    Memcpy(ptr, (void *)(src_ptr->ptr), blob_size);

    dst_ptr->ptr = ptr;
    dst_ptr->size = blob_size;
}

#endif

template <>
inline void
ColumnVector::CopyRowFrom<EmbeddingT>(const VectorBuffer *__restrict src_buf, SizeT src_idx, VectorBuffer *__restrict dst_buf, SizeT dst_idx) {
    const_ptr_t src = src_buf->GetData();
    ptr_t dst = dst_buf->GetData();
    const_ptr_t src_ptr = src + src_idx * data_type_size_;
    ptr_t dst_ptr = dst + dst_idx * data_type_size_;
    Memcpy(dst_ptr, src_ptr, data_type_size_);
}

} // namespace infinity
