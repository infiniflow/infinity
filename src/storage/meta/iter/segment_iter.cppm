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

export module segment_iter;

import block_iter;
import stl;
import segment_entry;
import block_iter;
import buffer_manager;
import default_values;
import infinity_exception;
import block_entry;
import logger;
import sparse_util;
import multivector_util;
import internal_types;
import column_vector;
import fix_heap;

namespace infinity {

export class BlockEntryIter {
public:
    BlockEntryIter(const SegmentEntry *segment) : segment_(segment), cur_block_idx_(0) {}

    BlockEntry *Next();

private:
    const SegmentEntry *const segment_;

    BlockID cur_block_idx_;
};

export template <bool CheckTS>
class SegmentIter {
public:
    SegmentIter(const SegmentEntry *entry, BufferManager *buffer_mgr, Vector<ColumnID> &&column_ids, TxnTimeStamp iterate_ts)
        : entry_(entry), buffer_mgr_(buffer_mgr), column_ids_(std::move(column_ids)), iterate_ts_(iterate_ts), block_entry_iter_(entry) {
        auto *block_entry = block_entry_iter_.Next();
        if (block_entry->block_id() != 0) {
            String error_message = "First block id is not 0";
            UnrecoverableError(error_message);
        }
        if (block_entry == nullptr) {
            block_iter_ = None;
        } else {
            block_iter_ = BlockIter<CheckTS>(block_entry, buffer_mgr, column_ids_, iterate_ts_);
        }
    }

    Optional<Pair<Vector<const void *>, SegmentOffset>> Next() {
        if (!block_iter_.has_value()) {
            return None;
        }
        if (auto ret = block_iter_->Next(); ret) {
            auto &[vec, offset] = *ret;
            // FIXME: segment_entry should store the block capacity
            return std::make_pair(std::move(vec), static_cast<SegmentOffset>(offset + block_idx_ * DEFAULT_BLOCK_CAPACITY));
        }
        block_idx_++;
        auto *block_entry = block_entry_iter_.Next();
        if (block_entry == nullptr) {
            block_iter_ = None;
            return None;
        }
        if (block_entry->block_id() != block_idx_) {
            String error_message = "Block id is not continuous";
            UnrecoverableError(error_message);
        }
        block_iter_ = BlockIter<CheckTS>(block_entry, buffer_mgr_, column_ids_, iterate_ts_);
        return Next();
    }

    const SharedPtr<ColumnVector> &column_vector(SizeT col_id) const { return block_iter_->column_vector(col_id); }

    SizeT offset() const { return block_idx_ * DEFAULT_BLOCK_CAPACITY + block_iter_->offset(); }

private:
    const SegmentEntry *const entry_;
    BufferManager *const buffer_mgr_;
    const Vector<ColumnID> column_ids_;
    const TxnTimeStamp iterate_ts_;

    BlockEntryIter block_entry_iter_;
    BlockID block_idx_ = 0;
    Optional<BlockIter<CheckTS>> block_iter_;
};

export template <typename DataType, bool CheckTS = true>
class OneColumnIterator {
public:
    OneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts)
        : segment_iter_(entry, buffer_mgr, Vector<ColumnID>{column_id}, iterate_ts) {}

    Optional<Pair<const DataType *, SegmentOffset>> Next() {
        if (auto ret = segment_iter_.Next(); ret) {
            auto &[vec, offset] = *ret;
            const auto *v_ptr = reinterpret_cast<const DataType *>(vec[0]);
            return std::make_pair(v_ptr, offset);
        }
        return None;
    }

    SizeT offset() const { return segment_iter_.offset(); }

private:
    SegmentIter<CheckTS> segment_iter_;
};

export template <typename DataType, bool CheckTS>
class CappedOneColumnIterator : public OneColumnIterator<DataType, CheckTS> {
public:
    CappedOneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts, SizeT cap)
        : OneColumnIterator<DataType, CheckTS>(entry, buffer_mgr, column_id, iterate_ts), cap_(cap) {}

    Optional<Pair<const DataType *, SegmentOffset>> Next() {
        if (this->offset() >= cap_) {
            return None;
        }
        return OneColumnIterator<DataType, CheckTS>::Next();
    }

private:
    SizeT cap_;
};

export template <typename ElementT, bool CheckTS>
class OneColumnIterator<MultiVectorRef<ElementT>, CheckTS> {
public:
    OneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts, SizeT ele_size)
        : segment_iter_(entry, buffer_mgr, Vector<ColumnID>{column_id}, iterate_ts), ele_size_(ele_size) {}

    Optional<Pair<const ElementT *, SegmentOffset>> Next() {
        while (multi_vector_cur_ == multi_vector_ref_.embedding_num()) {
            auto ret = segment_iter_.Next();
            if (!ret) {
                return None;
            }
            const auto &[vec, offset] = *ret;
            multi_vector_segment_offset_ = offset;
            const auto block_offset = offset % DEFAULT_BLOCK_CAPACITY;
            const auto &column_vector = segment_iter_.column_vector(0);
            multi_vector_ref_ = column_vector->GetMultiVectorRaw(block_offset);
            multi_vector_cur_ = 0;
        }
        const auto *ret = multi_vector_ref_.raw_data().data() + multi_vector_cur_ * ele_size_;
        ++multi_vector_cur_;
        const auto *v_ptr = reinterpret_cast<const ElementT *>(ret);
        return std::make_pair(v_ptr, multi_vector_segment_offset_);
    }

    SizeT offset() const { return segment_iter_.offset(); }

private:
    SegmentIter<CheckTS> segment_iter_;
    SizeT ele_size_;
    SegmentOffset multi_vector_segment_offset_ = {};
    MultiVectorRef<ElementT> multi_vector_ref_ = {};
    SizeT multi_vector_cur_ = 0;
};

export template <typename DataType, typename IdxType, bool CheckTS>
class OneColumnIterator<SparseVecRef<DataType, IdxType>, CheckTS> {
public:
    OneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts)
        : segment_iter_(entry, buffer_mgr, Vector<ColumnID>{column_id}, iterate_ts) {}

    Optional<Pair<SparseVecRef<DataType, IdxType>, SegmentOffset>> Next() {
        auto ret = segment_iter_.Next();
        if (!ret) {
            return None;
        }
        auto &[vec, offset] = *ret;
        const auto *v_ptr = reinterpret_cast<const SparseT *>(vec[0]);
        if (v_ptr->nnz_ == 0) {
            SparseVecRef<DataType, IdxType> sparse_vec_ref(0, nullptr, nullptr);
            return std::make_pair(sparse_vec_ref, offset);
        }

        const auto &column_vector = segment_iter_.column_vector(0);
        SparseVecRef<DataType, IdxType> sparse_vec_ref =
            column_vector->buffer_->template GetSparse<DataType, IdxType>(v_ptr->file_offset_, v_ptr->nnz_);

        return std::make_pair(sparse_vec_ref, offset);
    }

    SizeT offset() const { return segment_iter_.offset(); }

private:
    SegmentIter<CheckTS> segment_iter_;
};

export template <typename DataType, typename IdxType, bool CheckTS>
class CappedOneColumnIterator<SparseVecRef<DataType, IdxType>, CheckTS> : public OneColumnIterator<SparseVecRef<DataType, IdxType>, CheckTS> {
public:
    CappedOneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts, SizeT cap)
        : OneColumnIterator<SparseVecRef<DataType, IdxType>, CheckTS>(entry, buffer_mgr, column_id, iterate_ts), cap_(cap) {}

    Optional<Pair<SparseVecRef<DataType, IdxType>, SegmentOffset>> Next() {
        if (this->offset() >= cap_) {
            return None;
        }
        return OneColumnIterator<SparseVecRef<DataType, IdxType>, CheckTS>::Next();
    }

private:
    SizeT cap_;
};

} // namespace infinity