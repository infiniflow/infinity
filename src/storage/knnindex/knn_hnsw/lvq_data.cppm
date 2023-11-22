module;

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

import stl;
import hnsw_common;
import file_system;
import plain_data;

export module lvq_data;

namespace infinity {

export template <typename T, typename CompressType>
class LVQStore;

export template <typename T, typename CompressType>
struct LVQ {
    const SizeT vec_idx_;
    const LVQStore<T, CompressType> *const data_store_;

    void DecompressForTest(T *result) const {
        if (vec_idx_ < data_store_->cur_vec_num()) {
            data_store_->Decompress(vec_idx_, result);
        } else {
            const T *p = data_store_->plain_data_.GetVec(vec_idx_ - data_store_->cur_vec_num());
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

    using DataType = T;
    using RtnType = LVQ<T, CompressType>;

    static constexpr bool DEFAULT_PADDING = true;

private:
    constexpr static SizeT MAX_BUCKET_IDX = std::numeric_limits<CompressType>::max(); // 255 for u8
    
    DataStore<T> base_;

    const SizeT buffer_plain_size_;

    const SizeT mean_offset_;
    const SizeT min_offset_;
    const SizeT max_offset_;
    const SizeT compressed_vec_size_;
    const SizeT compressed_vecs_offset_;

    char *ptr_;
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

    LVQStore(DataStore<T> base, SizeT buffer_size, bool padding = DEFAULT_PADDING)
        : base_(Move(base)),                                                              //
          buffer_plain_size_(buffer_size),                                                //
          mean_offset_(0),                                                                //
          min_offset_(AlignTo(sizeof(CompressType) * dim(), sizeof(T))),                  //
          max_offset_(AlignTo(min_offset_ + sizeof(T), sizeof(T))),                       //
          compressed_vec_size_(AlignTo(max_offset_ + sizeof(T), padding ? 32 : 1)),       //
          compressed_vecs_offset_(AlignTo(dim() * sizeof(T), padding ? 32 : 1)),          //
          ptr_(new char[compressed_vecs_offset_ + compressed_vec_size_ * max_vec_num()]), //
          plain_data_(PlainStore<T>::Make(0, dim()))                                      //
    {
        T *mean = GetMeanMut();
        std::fill(mean, mean + dim(), 0);
    }

public:
    SizeT cur_vec_num() const { return base_.cur_vec_num(); }
    SizeT max_vec_num() const { return base_.max_vec_num_; }
    SizeT dim() const { return base_.dim_; }

    static SizeT err_idx() { return DataStore<T>::ERR_IDX; }

    static LVQStore Make(SizeT max_vec_num, SizeT dim, SizeT buffer_size, bool padding = DEFAULT_PADDING) {
        DataStore<T> data_store(max_vec_num, dim);
        return LVQStore(Move(data_store), buffer_size, padding);
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
        if (other.ptr_ != nullptr) {
            delete[] other.ptr_;
            other.ptr_ = nullptr;
        }
    }
    LVQStore &operator=(LVQStore &&other) = delete;

    ~LVQStore() {
        if (ptr_ != nullptr) {
            delete[] ptr_;
        }
    }

    SizeT AddVec(const DataType *vec) { return AddBatchVec(vec, 1); }

    SizeT AddBatchVec(const DataType *vecs, SizeT vec_num) {
        if (SizeT idx = plain_data_.AddBatchVec(vecs, vec_num); idx != DataStore<T>::ERR_IDX) {
            return cur_vec_num() + idx;
        } else if (SizeT idx = MergeCompress(vecs, vec_num); idx != DataStore<T>::ERR_IDX) {
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
        assert(ret != DataStore<T>::ERR_IDX);
    }

    void Save(FileHandler &file_handler) {
        Compress();
        base_.Save(file_handler);
        file_handler.Write(ptr_, compressed_vecs_offset_ + compressed_vec_size_ * cur_vec_num());
    }

    static LVQStore Load(FileHandler &file_handler, SizeT buffer_size, SizeT max_vec_num = 0, bool padding = DEFAULT_PADDING) {
        DataStore<T> base = DataStore<T>::Load(file_handler, max_vec_num);
        LVQStore ret(Move(base), buffer_size, padding);
        file_handler.Read(ret.ptr_, ret.compressed_vecs_offset_ + ret.compressed_vec_size_ * ret.cur_vec_num());
        return ret;
    }
};

} // namespace infinity