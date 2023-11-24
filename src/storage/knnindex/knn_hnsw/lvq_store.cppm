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
#include <type_traits>

import stl;
import hnsw_common;
import file_system;
import plain_store;

export module lvq_store;

namespace infinity {

export template <typename T, typename CompressType>
class LVQStore {
public:
    // assert CompressType to specific type
    static_assert(std::is_same<CompressType, u8>());

    using MeanType = double;
    using BoundType = double;
    using Norm1Type = u16;
    using Norm2SqType = u32;

    using This = LVQStore<T, CompressType>;
    using InitArgs = Pair<SizeT, bool>; // first is buffer size, second is whether to make padding
    using DataType = T;

    static constexpr SizeT ERR_IDX = IndexAllocator::ERR_IDX;

    class LVQ {
    public:
        BoundType alpha_;
        BoundType lower_;
        Norm1Type norm1_;
        Norm2SqType norm2sq_;
        CompressType *const c_;

        LVQ(BoundType alpha, BoundType lower, Norm1Type norm1, Norm2SqType norm2sq, CompressType *c)
            : alpha_(alpha), lower_(lower), norm1_(norm1), norm2sq_(norm2sq), c_(c) {}
    };

    using RtnType = const LVQ;

    friend class LVQ;

private:
    constexpr static SizeT MAX_BUCKET_IDX = std::numeric_limits<CompressType>::max(); // 255 for u8

    IndexAllocator base_;

    const SizeT buffer_plain_size_;

    const SizeT mean_offset_;
    const SizeT compressed_data_offset_;

    const SizeT min_offset_;
    const SizeT max_offset_;
    const SizeT norm1_offset_;
    const SizeT norm2sq_offset_;
    const SizeT compressed_data_size_;

    char *const ptr_;
    PlainStore<T> plain_data_;

    const UniquePtr<CompressType[]> convert_space_;

public:
    const char *GetCompressedData(SizeT vec_idx) const { return ptr_ + compressed_data_offset_ + vec_idx * compressed_data_size_; }
    char *GetCompressedDataMut(SizeT vec_idx) { return ptr_ + compressed_data_offset_ + vec_idx * compressed_data_size_; }

    const MeanType *GetMean() const { return reinterpret_cast<const MeanType *>(ptr_ + mean_offset_); }
    MeanType *GetMeanMut() { return reinterpret_cast<MeanType *>(ptr_ + mean_offset_); }

    const CompressType *GetCompressedVec(SizeT vec_idx) const { return reinterpret_cast<const CompressType *>(GetCompressedData(vec_idx)); }
    CompressType *GetCompressedVecMut(SizeT vec_idx) { return reinterpret_cast<CompressType *>(GetCompressedDataMut(vec_idx)); }

    Pair<BoundType, BoundType> GetLowerRange(SizeT vec_idx) const {
        const char *compressed_data = GetCompressedData(vec_idx);
        return {*reinterpret_cast<const BoundType *>(compressed_data + min_offset_),
                *reinterpret_cast<const BoundType *>(compressed_data + max_offset_)};
    }
    Pair<BoundType *, BoundType *> GetLowerRangeMut(SizeT vec_idx) {
        char *compressed_data = GetCompressedDataMut(vec_idx);
        return {reinterpret_cast<BoundType *>(compressed_data + min_offset_), reinterpret_cast<BoundType *>(compressed_data + max_offset_)};
    }

    Pair<Norm1Type, Norm2SqType> GetNorm(SizeT vec_idx) const {
        const char *compressed_data = GetCompressedData(vec_idx);
        return {*reinterpret_cast<const Norm1Type *>(compressed_data + norm1_offset_),
                *reinterpret_cast<const Norm2SqType *>(compressed_data + norm2sq_offset_)};
    }
    Pair<Norm1Type *, Norm2SqType *> GetNormMut(SizeT vec_idx) {
        char *compressed_data = GetCompressedDataMut(vec_idx);
        return {reinterpret_cast<Norm1Type *>(compressed_data + norm1_offset_), reinterpret_cast<Norm2SqType *>(compressed_data + norm2sq_offset_)};
    }

    void DecompressForTest(const LVQ &lvq, T *result) const {
        const MeanType *mean = GetMean();
        for (SizeT j = 0; j < dim(); ++j) {
            result[j] = lvq.alpha_ * lvq.c_[j] + lvq.lower_ + mean[j];
        }
    }

private:
    // the caller is responsible for allocate the `result`
    void Decompress(SizeT idx, T *result) const {
        const MeanType *mean = GetMean();
        const CompressType *compressed_vec = GetCompressedVec(idx);
        auto [lower, range] = GetLowerRange(idx);
        for (SizeT j = 0; j < dim(); ++j) {
            result[j] = range * compressed_vec[j] + lower + mean[j];
        }
    }

    void CompressVec(const T *vec, const MeanType *mean, SizeT idx) {
        auto [lower, range] = GetLowerRangeMut(idx);
        auto [norm1, norm2sq] = GetNormMut(idx);
        CompressType *compressed_vec = GetCompressedVecMut(idx);
        CompressVec(vec, mean, compressed_vec, lower, range, norm1, norm2sq);
    }

    void CompressVec(const T *vec, const MeanType *mean, CompressType *tgt, BoundType *lp, BoundType *rp, Norm1Type *n1, Norm2SqType *n2) const {
        BoundType lower = std::numeric_limits<BoundType>::max();
        BoundType upper = -std::numeric_limits<BoundType>::max();
        for (SizeT j = 0; j < dim(); ++j) {
            BoundType x = vec[j] - mean[j];
            lower = std::min(lower, x);
            upper = std::max(upper, x);
        }
        *lp = lower;
        BoundType range = (upper - lower) / MAX_BUCKET_IDX;
        *rp = range;

        Norm1Type norm1 = 0;
        Norm2SqType norm2sq = 0;
        for (SizeT j = 0; j < dim(); ++j) {
            auto res = static_cast<CompressType>((range == 0) ? 0 : std::floor((vec[j] - mean[j] - lower) / range + 0.5)); // use multiple here
            assert(res <= std::numeric_limits<CompressType>::max() && res >= 0);
            tgt[j] = res;
            norm1 += res;
            norm2sq += res * res;
        }
        *n1 = norm1;
        *n2 = norm2sq;
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
        MeanType *mean = GetMeanMut();
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

    LVQStore(IndexAllocator base, This::InitArgs init_args)
        : base_(Move(base)),                                                                                //
          buffer_plain_size_(init_args.first),                                                              //
          mean_offset_(0),                                                                                  //
          compressed_data_offset_(AlignTo(dim() * sizeof(MeanType), init_args.second ? 32 : 1)),            //
          min_offset_(AlignTo(sizeof(CompressType) * dim(), sizeof(MeanType))),                             //
          max_offset_(AlignTo(min_offset_ + sizeof(BoundType), sizeof(BoundType))),                         //
          norm1_offset_(AlignTo(max_offset_ + sizeof(BoundType), sizeof(Norm1Type))),                       //
          norm2sq_offset_(AlignTo(norm1_offset_ + sizeof(Norm1Type), sizeof(Norm2SqType))),                 //
          compressed_data_size_(AlignTo(norm2sq_offset_ + sizeof(Norm2SqType), init_args.second ? 32 : 1)), //
          ptr_(new char[compressed_data_offset_ + compressed_data_size_ * max_vec_num()]),                  //
          plain_data_(PlainStore<T>::Make(0, dim())),                                                       //
          convert_space_(MakeUnique<CompressType[]>(dim()))                                                 //
    {
        MeanType *mean = GetMeanMut();
        std::fill(mean, mean + dim(), 0);
    }

public:
    SizeT cur_vec_num() const { return base_.cur_vec_num() + plain_data_.cur_vec_num(); }
    SizeT max_vec_num() const { return base_.max_vec_num_; }
    SizeT dim() const { return base_.dim_; }

    static SizeT err_idx() { return IndexAllocator::ERR_IDX; }

    static LVQStore Make(SizeT max_vec_num, SizeT dim, This::InitArgs init_args) {
        IndexAllocator data_store(max_vec_num, dim);
        return LVQStore(Move(data_store), Move(init_args));
    }

    LVQStore(const LVQStore &) = delete;
    LVQStore &operator=(const LVQStore &) = delete;

    LVQStore(LVQStore &&other)
        : base_(Move(other.base_)),                                                           //
          buffer_plain_size_(other.buffer_plain_size_),                                       //
          mean_offset_(other.mean_offset_),                                                   //
          min_offset_(other.min_offset_),                                                     //
          max_offset_(other.max_offset_),                                                     //
          norm1_offset_(other.norm1_offset_),                                                 //
          norm2sq_offset_(other.norm2sq_offset_),                                             //
          compressed_data_size_(other.compressed_data_size_),                                 //
          compressed_data_offset_(other.compressed_data_offset_),                             //
          ptr_(other.ptr_),                                                                   //
          plain_data_(Move(other.plain_data_)),                                               //
          convert_space_(Move(const_cast<UniquePtr<CompressType[]> &>(other.convert_space_))) //
    {
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
        if (SizeT idx = plain_data_.AddBatchVec(vecs, vec_num); idx != IndexAllocator::ERR_IDX) {
            return base_.cur_vec_num() + idx;
        } else if (SizeT idx = MergeCompress(vecs, vec_num); idx != IndexAllocator::ERR_IDX) {
            return idx;
        } else {
            return err_idx();
        }
    }

    RtnType GetVec(SizeT vec_idx) const {
        assert(vec_idx < cur_vec_num());
        auto [lower, range] = GetLowerRange(vec_idx);
        auto [norm1, norm2sq] = GetNorm(vec_idx);
        const CompressType *compressed_vec = GetCompressedVec(vec_idx);
        return LVQ(range, lower, norm1, norm2sq, const_cast<CompressType *>(compressed_vec));
    }

    RtnType Convert(const T *vec) const {
        const MeanType *mean = GetMean();
        BoundType lower, range;
        Norm1Type norm1;
        Norm2SqType norm2sq;
        CompressVec(vec, mean, convert_space_.get(), &lower, &range, &norm1, &norm2sq);
        return LVQ(range, lower, norm1, norm2sq, convert_space_.get());
    }

    void Compress() {
        SizeT ret = MergeCompress(nullptr, 0);
        assert(ret != IndexAllocator::ERR_IDX);
    }

    void Save(FileHandler &file_handler) {
        Compress();
        base_.Save(file_handler);
        file_handler.Write(ptr_, compressed_data_offset_ + compressed_data_size_ * cur_vec_num());
    }

    static LVQStore Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs init_args) {
        IndexAllocator base = IndexAllocator::Load(file_handler, max_vec_num);
        LVQStore ret(Move(base), Move(init_args));
        file_handler.Read(ret.ptr_, ret.compressed_data_offset_ + ret.compressed_data_size_ * ret.cur_vec_num());
        return ret;
    }
};

} // namespace infinity