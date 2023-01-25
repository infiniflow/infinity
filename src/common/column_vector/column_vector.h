//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "vector_buffer/vector_buffer.h"
#include "bitmask.h"
#include "common/types/value.h"
#include "common/default_values.h"
#include "main/stats/global_resource_usage.h"
#include "selection.h"

namespace infinity {

enum class ColumnVectorType : i8 {
    kInvalid,
    kFlat,  // Stand without any encode
    kConstant, // All vector has same type and value
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
struct ColumnVector {
public:
    ColumnVectorType vector_type_ {ColumnVectorType::kInvalid};

    DataType data_type_;

    SizeT data_type_size_{0};

    // this buffer is holding the data
    SharedPtr<VectorBuffer> buffer_{nullptr};

    // Only a pointer to the real data in vector buffer
    ptr_t data_ptr_ {nullptr};

    // A bitmap to indicate the null information
    SharedPtr<Bitmask> nulls_ptr_{nullptr};

    SizeT capacity_{0};

    SizeT tail_index_ {0};

    bool initialized {false};

public:
    // Construct a column vector without initialization;
    explicit
    ColumnVector(DataType data_type)
            : data_type_(std::move(data_type)), vector_type_(ColumnVectorType::kInvalid) {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~ColumnVector() {
        Reset();
        GlobalResourceUsage::DecrObjectCount();
    }

    void
    Initialize(const ColumnVector& other, const Selection& input_select);

    void
    Initialize(SizeT capacity = DEFAULT_VECTOR_SIZE, ColumnVectorType vector_type = ColumnVectorType::kFlat);

    [[nodiscard]] String
    ToString() const;

    [[nodiscard]] String
    ToString(SizeT row_index) const;

    // Return the <index> of the vector
    // Since it will construct a new Value object, this function shouldn't be used in vectorized computation.
    // Directly uses data_ptr in vectorized computation.
    [[nodiscard]] Value
    GetValue(idx_t index) const;

    // Set the <index> element of the vector to the specified value.
    void
    SetValue(idx_t index, const Value& Value);

    void
    AppendValue(const Value& value);

    void
    ShallowCopy(const ColumnVector &other);

    // Enlarge the column vector capacity.
    void
    Reserve(SizeT new_capacity);

    void
    SetVectorType(ColumnVectorType vector_type) {
        TypeAssert(!initialized, "Column Vector is initialized")
        TypeAssert(vector_type != ColumnVectorType::kInvalid, "Attempt to set invalid column vector type.")
        if(vector_type_ == vector_type) {
            return ;
        }
        this->Reset();
        this->Initialize(DEFAULT_VECTOR_SIZE, vector_type);
    }

    void
    SetDataType(const DataType& data_type) {
        TypeAssert(!initialized, "Column Vector is initialized")
        data_type_ = data_type;
    }

    void
    Reset();

private:
    template<typename DataT>
    inline void
    CopyFrom(const_ptr_t __restrict src,
             ptr_t __restrict dst,
             SizeT count,
             const Selection& input_select);

public:
    [[nodiscard]] const inline ColumnVectorType&
    vector_type() const {
        return vector_type_;
    }

    [[nodiscard]] const inline DataType&
    data_type() const {
        return data_type_;
    }

    [[nodiscard]] inline ptr_t data() const {
        return data_ptr_;
    }

    [[nodiscard]] inline SizeT capacity() const {
        return capacity_;
    }

    [[nodiscard]] inline SizeT Size() const {
        return tail_index_;
    }
};

template<typename DataT>
inline void
ColumnVector::CopyFrom(const_ptr_t __restrict src,
                       ptr_t __restrict dst,
                       SizeT count,
                       const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];
        ((DataT *) (dst))[idx] = ((const DataT *) (src))[row_id];
    }
}

template<>
inline void
ColumnVector::CopyFrom<VarcharT>(const_ptr_t __restrict src,
                                 ptr_t __restrict dst,
                                 SizeT count,
                                 const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        VarcharT *dst_ptr = &(((VarcharT *) dst)[idx]);
        const VarcharT *src_ptr = &(((const VarcharT *) src)[row_id]);

        u16 varchar_len = src_ptr->length;
        if(varchar_len <= VarcharType::INLINE_LENGTH) {
            // Only prefix is enough to contain all string data.
            memcpy(dst_ptr->prefix, src_ptr->prefix, varchar_len);
        } else {
            memcpy(dst_ptr->prefix, src_ptr->prefix, VarcharType::PREFIX_LENGTH);
            ptr_t ptr = this->buffer_->heap_mgr_->Allocate(varchar_len);
            memcpy(ptr, src_ptr->ptr, varchar_len);
            dst_ptr->ptr = ptr;
        }
        dst_ptr->length = varchar_len;
    }
}

template<>
inline void
ColumnVector::CopyFrom<CharT>(const_ptr_t __restrict src,
                              ptr_t __restrict dst,
                              SizeT count,
                              const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];
        const_ptr_t src_ptr = src + row_id * data_type_size_;
        ptr_t dst_ptr = dst + idx * data_type_size_;
        memcpy(dst_ptr, src_ptr, data_type_size_);
    }
}

template<>
inline void
ColumnVector::CopyFrom<PathT>(const_ptr_t __restrict src,
                              ptr_t __restrict dst,
                              SizeT count,
                              const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        PathT *dst_ptr = &(((PathT *) dst)[idx]);
        const PathT *src_ptr = &(((const PathT *) src)[row_id]);

        u32 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
        memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->closed = src_ptr->closed;
    }
}

template<>
inline void
ColumnVector::CopyFrom<PolygonT>(const_ptr_t __restrict src,
                                 ptr_t __restrict dst,
                                 SizeT count,
                                 const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        PolygonT *dst_ptr = &(((PolygonT *) dst)[idx]);
        const PolygonT *src_ptr = &(((const PolygonT *) src)[row_id]);

        u64 point_count = src_ptr->point_count;

        SizeT point_area_size = point_count * sizeof(PointT);
        ptr_t ptr = this->buffer_->heap_mgr_->Allocate(point_area_size);
        memcpy(ptr, src_ptr->ptr, point_area_size);

        dst_ptr->ptr = ptr;
        dst_ptr->point_count = point_count;
        dst_ptr->bounding_box = src_ptr->bounding_box;
    }
}

template<>
inline void
ColumnVector::CopyFrom<BitmapT>(const_ptr_t __restrict src,
                                ptr_t __restrict dst,
                                SizeT count,
                                const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        BitmapT *dst_ptr = &(((BitmapT *) dst)[idx]);
        const BitmapT *src_ptr = &(((const BitmapT *) src)[row_id]);

        u64 bit_count = src_ptr->count;
        u64 unit_count = BitmapT::UnitCount(bit_count);

        SizeT bit_area_size = unit_count * BitmapT::UNIT_BYTES;
        ptr_t ptr = this->buffer_->heap_mgr_->Allocate(bit_area_size);
        memcpy(ptr, (void*)(src_ptr->ptr), bit_area_size);

        dst_ptr->ptr = (u64*)ptr;
        dst_ptr->count = bit_count;
    }
}

template<>
inline void
ColumnVector::CopyFrom<BlobT>(const_ptr_t __restrict src,
                              ptr_t __restrict dst,
                              SizeT count,
                              const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        BlobT *dst_ptr = &(((BlobT *) dst)[idx]);
        const BlobT *src_ptr = &(((const BlobT *) src)[row_id]);

        u64 blob_size = src_ptr->size;
        ptr_t ptr = this->buffer_->heap_mgr_->Allocate(blob_size);
        memcpy(ptr, (void*)(src_ptr->ptr), blob_size);

        dst_ptr->ptr = ptr;
        dst_ptr->size = blob_size;
    }
}

template<>
inline void
ColumnVector::CopyFrom<EmbeddingT>(const_ptr_t __restrict src,
                                   ptr_t __restrict dst,
                                   SizeT count,
                                   const Selection& input_select) {
    for(SizeT idx = 0; idx < count; ++ idx) {
        SizeT row_id = input_select[idx];

        const_ptr_t src_ptr = src + row_id * data_type_size_;
        ptr_t dst_ptr = dst + idx * data_type_size_;
        memcpy(dst_ptr, src_ptr, data_type_size_);
    }
}

}
