module;

#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>

import stl;
import file_system;

export module plain_data;

namespace infinity {

export template <typename Store>
concept DataStoreConcept = requires(Store s) {
    typename Store::DataType;
    typename Store::RtnType;
    { s.AddVec(std::declval<SizeT>(), std::declval<const typename Store::DataType *>()) } -> std::same_as<SizeT>;
    { s.AddBatchVec(std::declval<const typename Store::DataType *>(), std::declval<SizeT>()) } -> std::same_as<SizeT>;
    { s.GetVec(std::declval<SizeT>()) } -> std::same_as<typename Store::RtnType>;
};

export template <typename T>
class DataStore {
public:
    static constexpr SizeT ERR_IDX = std::numeric_limits<SizeT>::max();

private:
    SizeT cur_vec_num_;

public:
    const SizeT max_vec_num_;
    const SizeT dim_;

public:
    DataStore(SizeT max_vec_num, SizeT dim) : cur_vec_num_(0), max_vec_num_(max_vec_num), dim_(dim) {}

    DataStore(DataStore &&other) = default;
    DataStore &operator=(DataStore &&other) {
        cur_vec_num_ = other.cur_vec_num_;
        const_cast<SizeT &>(max_vec_num_) = other.max_vec_num_;
        const_cast<SizeT &>(dim_) = other.dim_;
        return *this;
    }

    SizeT AllocateVec(SizeT alloc_n) {
        if (cur_vec_num_ + alloc_n > max_vec_num_) {
            return ERR_IDX;
        }
        SizeT ret = cur_vec_num_;
        cur_vec_num_ += alloc_n;
        return ret;
    }

    void Save(FileHandler &file_handler) const {
        file_handler.Write(&cur_vec_num_, sizeof(SizeT));
        file_handler.Write(&max_vec_num_, sizeof(SizeT));
        file_handler.Write(&dim_, sizeof(SizeT));
    }

    static DataStore<T> Load(FileHandler &file_handler, SizeT new_vec_n) {
        SizeT cur_vec_num, max_vec_num, dim;
        file_handler.Read(&cur_vec_num, sizeof(SizeT));
        file_handler.Read(&max_vec_num, sizeof(SizeT));
        if (new_vec_n != 0) {
            assert(new_vec_n >= cur_vec_num);
            max_vec_num = new_vec_n;
        }
        file_handler.Read(&dim, sizeof(SizeT));
        DataStore<T> ret(max_vec_num, dim);
        ret.cur_vec_num_ = cur_vec_num;
        return ret;
    }

public:
    SizeT cur_vec_num() const { return cur_vec_num_; }
};

export template <typename T>
class PlainStore {
public:
    using DataType = T;
    using RtnType = const T *;

private:
    DataStore<T> base_;
    const UniquePtr<T[]> ptr_;

public:
    static PlainStore<T> Make(SizeT max_vec_num, SizeT dim) {
        DataStore<T> data_store(max_vec_num, dim);
        return PlainStore<T>(Move(data_store));
    }

    PlainStore(DataStore<T> base) : base_(Move(base)), ptr_(MakeUnique<T[]>(base_.max_vec_num_ * base_.dim_)) {}

    PlainStore(PlainStore &&other) : base_(Move(other.base_)), ptr_(Move(const_cast<UniquePtr<T[]> &>(other.ptr_))) {}
    PlainStore &operator=(PlainStore &&other) {
        base_ = Move(other.base_);
        const_cast<UniquePtr<T[]> &>(ptr_) = Move(const_cast<UniquePtr<T[]> &>(other.ptr_));
        return *this;
    }

    ~PlainStore() = default;

    SizeT cur_vec_num() const { return base_.cur_vec_num(); }
    SizeT max_vec_num() const { return base_.max_vec_num_; }
    SizeT dim() const { return base_.dim_; }

    static SizeT err_idx() { return DataStore<T>::ERR_IDX; }

public:
    SizeT AddVec(const DataType *vec) { return AddBatchVec(vec, 1); }

    SizeT AddBatchVec(const DataType *vecs, SizeT vec_num) {
        SizeT new_idx = base_.AllocateVec(vec_num);
        if (new_idx != err_idx()) {
            T *ptr = ptr_.get() + new_idx * dim();
            std::copy(vecs, vecs + vec_num * dim(), ptr);
        }
        return new_idx;
    }

    const RtnType GetVec(SizeT vec_idx) const {
        assert(vec_idx < cur_vec_num());
        return ptr_.get() + vec_idx * dim();
    }

    void Save(FileHandler &file_handler) const {
        base_.Save(file_handler);
        file_handler.Write(ptr_.get(), sizeof(T) * cur_vec_num() * dim());
    }

    static PlainStore Load(FileHandler &file_handler, SizeT max_vec_num = 0) {
        DataStore<T> data_store = DataStore<T>::Load(file_handler, max_vec_num);
        PlainStore ret(Move(data_store));
        file_handler.Read(ret.ptr_.get(), sizeof(T) * ret.cur_vec_num() * ret.dim());
        return ret;
    }
};
} // namespace infinity