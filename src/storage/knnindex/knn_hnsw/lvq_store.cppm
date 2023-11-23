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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

import stl;
import hnsw_common;
import file_system;
import plain_store;

export module lvq_store;

namespace infinity {

export template <typename T, typename CompressType>
class LVQStore;

export template <typename T, typename CompressType>
struct LVQ {
    const SizeT vec_idx_;
    const LVQStore<T, CompressType> *const data_store_;

    void DecompressForTest(T *result) const {
        SizeT compressed_n = data_store_->base_.cur_vec_num();
        if (vec_idx_ < compressed_n) {
            data_store_->Decompress(vec_idx_, result);
        } else {
            const T *p = data_store_->plain_data_.GetVec(vec_idx_ - compressed_n);
            std::copy(p, p + data_store_->dim(), result);
        }
    }
};

export template <typename T, typename CompressType>
class LVQStore {
public:
    // assert CompressType to specific type
    static_assert(std::is_unsigned_v<CompressType> && std::is_integral_v<CompressType> && sizeof(CompressType) <= 2);

    friend class LVQ<T, CompressType>;

    using This = LVQStore<T, CompressType>;
    using InitArgs = Pair<SizeT, bool>; // first is buffer size, second is whether to make padding
    using DataType = T;
    using RtnType = const LVQ<T, CompressType>;

    static constexpr SizeT ERR_IDX = IndexAllocator<T>::ERR_IDX;

private:
    constexpr static SizeT MAX_BUCKET_IDX = std::numeric_limits<CompressType>::max(); // 255 for u8

    IndexAllocator<T> base_;

    const SizeT buffer_plain_size_;

    const SizeT mean_offset_;
    const SizeT min_offset_;
    const SizeT max_offset_;
    const SizeT compressed_vec_size_;
    const SizeT compressed_vecs_offset_;

    char *const ptr_;
    PlainStore<T> plain_data_;

private:
    const DataType *GetMean() const { return reinterpret_cast<const DataType *>(ptr_ + mean_offset_); }
    DataType *GetMeanMut() { return reinterpret_cast<DataType *>(ptr_ + mean_offset_); }

    const CompressType *GetCompressedVec(SizeT vec_idx) const {
        return reinterpret_cast<const CompressType *>(ptr_ + compressed_vecs_offset_ + vec_idx * compressed_vec_size_);
    }
    CompressType *GetCompressedVecMut(SizeT vec_idx) {
        return reinterpret_cast<CompressType *>(ptr_ + compressed_vecs_offset_ + vec_idx * compressed_vec_size_);
    }

    Pair<DataType, DataType> GetLowerUpper(SizeT vec_idx) const {
        const char *compressed_vec = ptr_ + compressed_vecs_offset_ + vec_idx * compressed_vec_size_;
        return {*reinterpret_cast<const DataType *>(compressed_vec + min_offset_), *reinterpret_cast<const DataType *>(compressed_vec + max_offset_)};
    }
    Pair<DataType *, DataType *> GetLowerUpperMut(SizeT vec_idx) {
        char *compressed_vec = ptr_ + compressed_vecs_offset_ + vec_idx * compressed_vec_size_;
        return {reinterpret_cast<DataType *>(compressed_vec + min_offset_), reinterpret_cast<DataType *>(compressed_vec + max_offset_)};
    }

private:
    // the caller is responsible for allocate the `result`
    void Decompress(SizeT idx, T *result) const {
        const T *mean = GetMean();
        const CompressType *compressed_vec = GetCompressedVec(idx);
        auto [lower, upper] = GetLowerUpper(idx);
        DataType bucket_range = (upper - lower) / MAX_BUCKET_IDX;
        for (SizeT j = 0; j < dim(); ++j) {
            result[j] = bucket_range * compressed_vec[j] + lower + mean[j];
        }
    }

    void CompressVec(const T *vec, const T *mean, SizeT idx) {
        T lower = std::numeric_limits<T>::max();
        T upper = -std::numeric_limits<T>::max();
        for (SizeT j = 0; j < dim(); ++j) {
            T x = vec[j] - mean[j];
            lower = std::min(lower, x);
            upper = std::max(upper, x);
        }
        auto [lp, up] = GetLowerUpperMut(idx);
        *lp = lower;
        *up = upper;

        CompressType *tgt = GetCompressedVecMut(idx);
        DataType bucket_range = (upper - lower) / MAX_BUCKET_IDX;
        for (SizeT j = 0; j < dim(); ++j) {
            DataType tmp = bucket_range == 0 ? 0 : std::floor((vec[j] - mean[j] - lower) / bucket_range + 0.5);
            assert(tmp <= std::numeric_limits<CompressType>::max() && tmp >= 0);
            tgt[j] = tmp;
        }
    }

    SizeT MergeCompress(const T *vecs, SizeT vec_num) {
        SizeT compressed_n = cur_vec_num();
        if (auto ret = base_.AllocateVec(vec_num + plain_data_.cur_vec_num()); ret > max_vec_num()) {
            return err_idx();
        }
        auto decompressed = MakeUnique<T[]>(dim() * compressed_n);
        for (SizeT i = 0; i < compressed_n; ++i) {
            Decompress(i, decompressed.get() + i * dim());
        }
        T *mean = GetMeanMut();
        for (SizeT i = 0; i < dim(); ++i) {
            mean[i] *= compressed_n;
        }
        for (SizeT i = 0; i < plain_data_.cur_vec_num(); ++i) {
            const T *vec = plain_data_.GetVec(i);
            for (SizeT j = 0; j < dim(); ++j) {
                mean[j] += vec[j];
            }
        }
        for (SizeT i = 0; i < vec_num; ++i) {
            const T *vec = vecs + i * dim();
            for (SizeT j = 0; j < dim(); ++j) {
                mean[j] += vec[j];
            }
        }
        for (SizeT i = 0; i < dim(); ++i) {
            mean[i] /= cur_vec_num();
        }

        for (SizeT i = 0; i < compressed_n; ++i) {
            CompressVec(decompressed.get() + i * dim(), mean, i);
        }
        for (SizeT i = 0; i < plain_data_.cur_vec_num(); ++i) {
            CompressVec(plain_data_.GetVec(i), mean, compressed_n + i);
        }
        SizeT start_idx = compressed_n + plain_data_.cur_vec_num();
        for (SizeT i = 0; i < vec_num; ++i) {
            CompressVec(vecs + i * dim(), mean, start_idx + i);
        }
        plain_data_ = PlainStore<T>::Make(Min(buffer_plain_size_, max_vec_num() - cur_vec_num()), dim());
        return start_idx;
    }

    LVQStore(IndexAllocator<T> base, This::InitArgs init_args)
        : base_(Move(base)),                                                                 //
          buffer_plain_size_(init_args.first),                                               //
          mean_offset_(0),                                                                   //
          min_offset_(AlignTo(sizeof(CompressType) * dim(), sizeof(T))),                     //
          max_offset_(AlignTo(min_offset_ + sizeof(T), sizeof(T))),                          //
          compressed_vec_size_(AlignTo(max_offset_ + sizeof(T), init_args.second ? 32 : 1)), //
          compressed_vecs_offset_(AlignTo(dim() * sizeof(T), init_args.second ? 32 : 1)),    //
          ptr_(new char[compressed_vecs_offset_ + compressed_vec_size_ * max_vec_num()]),    //
          plain_data_(PlainStore<T>::Make(0, dim()))                                         //
    {
        T *mean = GetMeanMut();
        std::fill(mean, mean + dim(), 0);
    }

public:
    SizeT cur_vec_num() const { return base_.cur_vec_num() + plain_data_.cur_vec_num(); }
    SizeT max_vec_num() const { return base_.max_vec_num_; }
    SizeT dim() const { return base_.dim_; }

    static SizeT err_idx() { return IndexAllocator<T>::ERR_IDX; }

    static LVQStore Make(SizeT max_vec_num, SizeT dim, This::InitArgs init_args) {
        IndexAllocator<T> data_store(max_vec_num, dim);
        return LVQStore(Move(data_store), Move(init_args));
    }

    LVQStore(const LVQStore &) = delete;
    LVQStore &operator=(const LVQStore &) = delete;

    LVQStore(LVQStore &&other)
        : base_(Move(other.base_)),                               //
          buffer_plain_size_(other.buffer_plain_size_),           //
          mean_offset_(other.mean_offset_),                       //
          min_offset_(other.min_offset_),                         //
          max_offset_(other.max_offset_),                         //
          compressed_vec_size_(other.compressed_vec_size_),       //
          compressed_vecs_offset_(other.compressed_vecs_offset_), //
          ptr_(other.ptr_),                                       //
          plain_data_(Move(other.plain_data_)) {
        const_cast<char *&>(other.ptr_) = nullptr;
    }
    LVQStore &operator=(LVQStore &&other) = delete;

    ~LVQStore() {
        if (ptr_ != nullptr) {
            delete[] ptr_;
        }
    }

    SizeT AddVec(const DataType *vec) { return AddBatchVec(vec, 1); }

    SizeT AddBatchVec(const DataType *vecs, SizeT vec_num) {
        if (SizeT idx = plain_data_.AddBatchVec(vecs, vec_num); idx != IndexAllocator<T>::ERR_IDX) {
            return base_.cur_vec_num() + idx;
        } else if (SizeT idx = MergeCompress(vecs, vec_num); idx != IndexAllocator<T>::ERR_IDX) {
            return idx;
        } else {
            return err_idx();
        }
    }

    const RtnType GetVec(SizeT vec_idx) const {
        assert(vec_idx < cur_vec_num());
        return LVQ<T, CompressType>{vec_idx, this};
    }

    void Compress() {
        SizeT ret = MergeCompress(nullptr, 0);
        assert(ret != IndexAllocator<T>::ERR_IDX);
    }

    void Save(FileHandler &file_handler) {
        Compress();
        base_.Save(file_handler);
        file_handler.Write(ptr_, compressed_vecs_offset_ + compressed_vec_size_ * cur_vec_num());
    }

    static LVQStore Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs init_args) {
        IndexAllocator<T> base = IndexAllocator<T>::Load(file_handler, max_vec_num);
        LVQStore ret(Move(base), Move(init_args));
        file_handler.Read(ret.ptr_, ret.compressed_vecs_offset_ + ret.compressed_vec_size_ * ret.cur_vec_num());
        return ret;
    }
};

} // namespace infinity