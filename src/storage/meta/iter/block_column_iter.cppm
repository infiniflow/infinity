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

#include <utility>

export module block_column_iter;

import stl;
import buffer_handle;
import block_column_entry;
import catalog;
import buffer_manager;
import column_vector;
import block_entry;
import sparse_util;
import multivector_util;
import internal_types;
import column_vector;

namespace infinity {

export template <bool CheckTS = true>
class BlockColumnIter {
public:
    BlockColumnIter(BlockColumnEntry *entry, BufferManager *buffer_mgr, TxnTimeStamp iterate_ts, SizeT row_count)
        : block_entry_(entry->block_entry()), column_vector_(MakeShared<ColumnVector>(entry->GetConstColumnVector(buffer_mgr, row_count))),
          ele_size_(entry->column_type()->Size()), iterate_ts_(iterate_ts), offset_(0), read_end_(0) {}
    // TODO: Does `ColumnVector` implements the move constructor?

    Optional<Pair<const void *, BlockOffset>> Next() {
        // FIXME: because no non-copy way to get data from `ColumnVector`, use data() tmply
        if (offset_ == read_end_) {
            auto [begin, end] = block_entry_->GetVisibleRange(iterate_ts_, read_end_);
            if (begin == end) {
                return None;
            }
            read_end_ = end;
            offset_ = begin;
        }
        auto ret = column_vector_->data() + offset_ * ele_size_;
        return std::make_pair(ret, offset_++);
    }

    const void *data() const { return column_vector_->data(); }

    const SharedPtr<ColumnVector> &column_vector() const { return column_vector_; }

    BlockOffset offset() const { return offset_; }

private:
    const BlockEntry *const block_entry_;
    const SharedPtr<ColumnVector> column_vector_;
    const SizeT ele_size_;
    const TxnTimeStamp iterate_ts_;

    BlockOffset offset_;
    BlockOffset read_end_;
};

export template <>
class BlockColumnIter<false> {
public:
    BlockColumnIter(BlockColumnEntry *entry, BufferManager *buffer_mgr, TxnTimeStamp, SizeT row_count)
        : block_entry_(entry->block_entry()), column_vector_(MakeShared<ColumnVector>(entry->GetConstColumnVector(buffer_mgr, row_count))),
          ele_size_(entry->column_type()->Size()), size_(block_entry_->row_count()), offset_(0) {}

    Optional<Pair<const void *, BlockOffset>> Next() {
        if (offset_ == size_) {
            return None;
        }
        auto ret = column_vector_->data() + offset_ * ele_size_;
        return std::make_pair(ret, offset_++);
    }

    const void *data() const { return column_vector_->data(); }

    const SharedPtr<ColumnVector> &column_vector() const { return column_vector_; }

    BlockOffset offset() const { return offset_; }

private:
    const BlockEntry *const block_entry_;
    const SharedPtr<ColumnVector> column_vector_;
    const SizeT ele_size_;
    const BlockOffset size_;

    BlockOffset offset_;
};

export template <typename DataType>
class MemIndexInserterIter {
public:
    using ValueType = const DataType *;

    MemIndexInserterIter(SegmentOffset block_offset, BlockColumnEntry *entry, BufferManager *buffer_mgr, SizeT offset, SizeT size)
        : block_offset_(block_offset), column_vector_(MakeShared<ColumnVector>(entry->GetConstColumnVector(buffer_mgr, offset + size))),
          ele_size_(entry->column_type()->Size()), cur_(offset), end_(offset + size) {}

    Optional<Pair<const DataType *, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        const void *ret = column_vector_->data() + cur_ * ele_size_;
        const auto *v_ptr = reinterpret_cast<const DataType *>(ret);
        return std::make_pair(v_ptr, block_offset_ + cur_++);
    }

    SharedPtr<ColumnVector> column_vector() const { return column_vector_; }

private:
    SegmentOffset block_offset_;
    SharedPtr<ColumnVector> column_vector_;
    SizeT ele_size_;
    SizeT cur_;
    SizeT end_;
};

export template <typename ElementT>
class MemIndexInserterIter<MultiVectorRef<ElementT>> {
public:
    using ValueType = const ElementT *;

    MemIndexInserterIter(SegmentOffset block_offset, BlockColumnEntry *entry, BufferManager *buffer_mgr, SizeT offset, SizeT size)
        : block_offset_(block_offset), column_vector_(MakeShared<ColumnVector>(entry->GetConstColumnVector(buffer_mgr, offset + size))),
          ele_size_(entry->column_type()->type_info()->Size()), cur_(offset), end_(offset + size) {}

    Optional<Pair<const ElementT *, SegmentOffset>> Next() {
        // prepare multi-vector data
        while (multi_vector_cur_ == multi_vector_ref_.embedding_num()) {
            if (cur_ == end_) {
                return None;
            }
            multi_vector_ref_ = column_vector_->GetMultiVectorRaw(cur_++);
            multi_vector_cur_ = 0;
        }
        const auto *ret = multi_vector_ref_.raw_data().data() + multi_vector_cur_ * ele_size_;
        ++multi_vector_cur_;
        const auto *v_ptr = reinterpret_cast<const ElementT *>(ret);
        return std::make_pair(v_ptr, block_offset_ + cur_ - 1);
    }

    SharedPtr<ColumnVector> column_vector() const { return column_vector_; }

private:
    SegmentOffset block_offset_;
    SharedPtr<ColumnVector> column_vector_;
    SizeT ele_size_;
    SizeT cur_;
    SizeT end_;
    MultiVectorRef<ElementT> multi_vector_ref_ = {};
    SizeT multi_vector_cur_ = 0;
};

export template <typename DataType, typename IdxType>
class MemIndexInserterIter<SparseVecRef<DataType, IdxType>> {
public:
    using ValueType = SparseVecRef<DataType, IdxType>;

    MemIndexInserterIter(SegmentOffset block_offset, BlockColumnEntry *entry, BufferManager *buffer_mgr, SizeT offset, SizeT size)
        : block_offset_(block_offset), column_vector_(MakeShared<ColumnVector>(entry->GetConstColumnVector(buffer_mgr, offset + size))),
          ele_size_(entry->column_type()->Size()), cur_(offset), end_(offset + size) {}

    Optional<Pair<SparseVecRef<DataType, IdxType>, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        auto [data_span, index_span, nnz] = column_vector_->GetSparseRaw(cur_++);
        auto *data_ptr = reinterpret_cast<const DataType *>(data_span.data());
        auto *index_ptr = reinterpret_cast<const IdxType *>(index_span.data());
        return std::make_pair(SparseVecRef<DataType, IdxType>(nnz, index_ptr, data_ptr), block_offset_ + cur_ - 1);
    }

private:
    SegmentOffset block_offset_;
    SharedPtr<ColumnVector> column_vector_;
    SizeT ele_size_;
    SizeT cur_;
    SizeT end_;
};

export template <typename DataType>
class MemIndexInserterIter1 {
public:
    using ValueType = const DataType *;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->Size()), cur_(offset), end_(offset + row_cnt) {}

    Optional<Pair<const DataType *, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        const void *ret = col_.data() + cur_ * ele_size_;
        const auto *v_ptr = reinterpret_cast<const DataType *>(ret);
        return std::make_pair(v_ptr, block_offset_ + cur_++);
    }

    const ColumnVector *column_vector() const { return &col_; }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
};

export template <typename ElementT>
class MemIndexInserterIter1<MultiVectorRef<ElementT>> {
public:
    using ValueType = const ElementT *;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->type_info()->Size()), cur_(offset), end_(offset + row_cnt) {}

    Optional<Pair<const ElementT *, SegmentOffset>> Next() {
        // prepare multi-vector data
        while (multi_vector_cur_ == multi_vector_ref_.embedding_num()) {
            if (cur_ == end_) {
                return None;
            }
            multi_vector_ref_ = col_.GetMultiVectorRaw(cur_++);
            multi_vector_cur_ = 0;
        }
        const auto *ret = multi_vector_ref_.raw_data().data() + multi_vector_cur_ * ele_size_;
        ++multi_vector_cur_;
        const auto *v_ptr = reinterpret_cast<const ElementT *>(ret);
        return std::make_pair(v_ptr, block_offset_ + cur_ - 1);
    }

    const ColumnVector *column_vector() const { return &col_; }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
    MultiVectorRef<ElementT> multi_vector_ref_ = {};
    SizeT multi_vector_cur_ = 0;
};

export template <typename DataType, typename IdxType>
class MemIndexInserterIter1<SparseVecRef<DataType, IdxType>> {
public:
    using ValueType = SparseVecRef<DataType, IdxType>;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->Size()), cur_(offset), end_(offset + row_cnt) {}

    Optional<Pair<SparseVecRef<DataType, IdxType>, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        auto [data_span, index_span, nnz] = col_.GetSparseRaw(cur_++);
        auto *data_ptr = reinterpret_cast<const DataType *>(data_span.data());
        auto *index_ptr = reinterpret_cast<const IdxType *>(index_span.data());
        return std::make_pair(SparseVecRef<DataType, IdxType>(nnz, index_ptr, data_ptr), block_offset_ + cur_ - 1);
    }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
};

} // namespace infinity