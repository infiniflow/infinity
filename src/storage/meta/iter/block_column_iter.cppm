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

export module infinity_core:block_column_iter;

import :stl;
import :buffer_handle;
import :buffer_manager;
import :column_vector;
import :sparse_util;
import :multivector_util;
import internal_types;
import :column_vector;

namespace infinity {
export template <typename DataType>
class MemIndexInserterIter1 {
public:
    using This = MemIndexInserterIter1<DataType>;
    using Split = Vector<This>;
    using ValueType = const DataType *;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->Size()), cur_(offset), end_(offset + row_cnt), row_count_(row_cnt) {}

    Optional<Pair<ValueType, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        const void *ret = col_.data() + cur_ * ele_size_;
        const auto *v_ptr = reinterpret_cast<ValueType>(ret);
        return std::make_pair(v_ptr, block_offset_ + cur_++);
    }

    SizeT GetRowCount() const { return row_count_; }

    const ColumnVector *column_vector() const { return &col_; }

    Split split() && {
        Split res;
        SizeT vec_num = 0;
        BlockOffset head = cur_;
        while (cur_ != end_) {
            if (vec_num == DEFAULT_ITER_BATCH_SIZE) {
                res.emplace_back(block_offset_, col_, head, DEFAULT_ITER_BATCH_SIZE);
                vec_num = 0;
                head = cur_;
            }
            ++cur_;
            ++vec_num;
        }
        res.emplace_back(block_offset_, col_, head, vec_num);
        return res;
    }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
    SizeT row_count_ = 0;
};

export template <typename ElementT>
class MemIndexInserterIter1<MultiVectorRef<ElementT>> {
public:
    using ValueType = const ElementT *;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->type_info()->Size()), cur_(offset), end_(offset + row_cnt),
          row_count_(row_cnt) {}

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

    SizeT GetRowCount() const { return row_count_; }

    const ColumnVector *column_vector() const { return &col_; }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
    MultiVectorRef<ElementT> multi_vector_ref_ = {};
    SizeT multi_vector_cur_ = 0;
    SizeT row_count_ = 0;
};

export template <typename DataType, typename IdxType>
class MemIndexInserterIter1<SparseVecRef<DataType, IdxType>> {
public:
    using ValueType = SparseVecRef<DataType, IdxType>;

    MemIndexInserterIter1(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt)
        : block_offset_(block_offset), col_(col), ele_size_(col.data_type()->Size()), cur_(offset), end_(offset + row_cnt), row_count_(row_cnt) {}

    Optional<Pair<SparseVecRef<DataType, IdxType>, SegmentOffset>> Next() {
        if (cur_ == end_) {
            return None;
        }
        auto [data_span, index_span, nnz] = col_.GetSparseRaw(cur_++);
        auto *data_ptr = reinterpret_cast<const DataType *>(data_span.data());
        auto *index_ptr = reinterpret_cast<const IdxType *>(index_span.data());
        return std::make_pair(SparseVecRef<DataType, IdxType>(nnz, index_ptr, data_ptr), block_offset_ + cur_ - 1);
    }

    SizeT GetRowCount() const { return row_count_; }

private:
    SegmentOffset block_offset_;
    const ColumnVector &col_;
    SizeT ele_size_;
    BlockOffset cur_;
    BlockOffset end_;
    SizeT row_count_ = 0;
};

} // namespace infinity