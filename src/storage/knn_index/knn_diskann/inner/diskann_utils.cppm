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

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <cstdlib>

import stl;
import third_party;
import default_values;
import infinity_exception;
import file_system;
import logger;
import third_party;

export module diskann_utils;

namespace infinity {
export void FileHandlerSeek(FileHandler &fh, i64 offset) {
    auto &fs = fh.file_system_;
    fs.Seek(fh, offset);
}

export inline void AllocAligned(void **ptr, SizeT size, SizeT align) {
    *ptr = nullptr;
    if ((u64)size % (u64)align != 0) {
        UnrecoverableError(fmt::format("AllocAligned(): size {} is not aligned to {}", size, align));
    }

    *ptr = std::aligned_alloc(align, size);
    if (*ptr == nullptr) {
        UnrecoverableError("AllocAligned(): Failed to allocate memory");
    }
}

export inline void AlignedFree(void *ptr) {
    if (ptr == nullptr)
        return;

    free(ptr);
}

export struct Neighbor {
    u32 id;
    f32 distance;
    bool expanded;

    Neighbor() : id(-1), distance(std::numeric_limits<f32>::max()), expanded(false) {};
    // Neighbor() = default;

    Neighbor(u32 id, f32 distance) : id(id), distance(distance), expanded(false) {}

    inline bool operator<(const Neighbor &other) const { return distance < other.distance || (distance == other.distance && id < other.id); }

    inline bool operator==(const Neighbor &other) const { return (id == other.id); }
};

export class NeighborPriorityQueue {
public:
    NeighborPriorityQueue() : size_(0), capacity_(0), cur_(0) {}

    NeighborPriorityQueue(SizeT capacity) : size_(0), capacity_(capacity), cur_(0), data_(capacity + 1) {}

    void Insert(const Neighbor &nbr) {
        if (size_ == capacity_ && data_[size_ - 1] < nbr)
            return;
        
        auto it = std::lower_bound(data_.begin(), data_.begin() + size_, nbr);
        // if (it != data_.start()+size_ && (*it).id == nbr.id) {
        //     return;
        // }

        SizeT lo = it - data_.begin();
        if (lo < capacity_) {
            std::memmove(&data_[lo + 1], &data_[lo], (size_ - lo) * sizeof(Neighbor));
        }
        data_[lo] = {nbr.id, nbr.distance};

        if (size_ < capacity_)
            size_++;
        if (lo < cur_)
            cur_ = lo;
    }

    Neighbor ClosestUnexpanded() {
        data_[cur_].expanded = true;
        SizeT pre = cur_;
        while (cur_ < size_ && data_[cur_].expanded) { // find the second-closest unexpanded neighbor
            cur_++;
        }
        return data_[pre];
    }

    bool HasUnexpandedNode() const { return cur_ < size_; }

    SizeT Size() const { return size_; }

    SizeT Capacity() const { return capacity_; }

    void Reserve(SizeT capacity) {
        if (capacity + 1 > data_.size()) {
            data_.resize(capacity + 1);
        }
        capacity_ = capacity;
    }

    Neighbor &operator[](SizeT i) { return data_[i]; }

    const Neighbor operator[](SizeT i) const { return data_[i]; }

    void Clear() {
        size_ = 0;
        cur_ = 0;
    }

private:
    SizeT size_;
    SizeT capacity_;
    SizeT cur_; // closest unexpanded neighbor
    Vector<Neighbor> data_;
};

export template <typename DataType>
class PQScratch {
public:
    f32 *aligned_pqtable_dist_scratch_ = nullptr;
    f32 *aligned_dist_scratch_ = nullptr;
    u8 *aligned_pq_coord_scratch_ = nullptr;
    f32 *rotated_query_ = nullptr;
    f32 *aligned_query_float_ = nullptr;

    PQScratch(SizeT graph_degree, SizeT aligned_dim) {
        AllocAligned((void **)&aligned_pq_coord_scratch_, graph_degree * DISKANN_MAX_PQ_CHUNKS * sizeof(u8), 256);
        AllocAligned((void **)&aligned_pqtable_dist_scratch_, 256 * DISKANN_MAX_PQ_CHUNKS * sizeof(float), 256);
        AllocAligned((void **)&aligned_dist_scratch_, graph_degree * sizeof(f32), 256);
        AllocAligned((void **)&rotated_query_, aligned_dim * sizeof(f32), 8 * sizeof(f32));

        memset(aligned_query_float_, 0, aligned_dim * sizeof(f32));
        memset(rotated_query_, 0, aligned_dim * sizeof(f32));
    }
    void Initialize(SizeT dim, const DataType *query, const f32 norm = 1.0f) {
        for (SizeT d = 0; d < dim; ++d) {
            if (norm != 1.0f)
                rotated_query_[d] = aligned_query_float_[d] = static_cast<float>(query[d]) / norm;
            else
                rotated_query_[d] = aligned_query_float_[d] = static_cast<float>(query[d]);
        }
    }

    virtual ~PQScratch() {
        AlignedFree((void *)aligned_pq_coord_scratch_);
        AlignedFree((void *)aligned_pqtable_dist_scratch_);
        AlignedFree((void *)aligned_dist_scratch_);
        AlignedFree((void *)aligned_query_float_);
        AlignedFree((void *)rotated_query_);
    }
};

export template <typename DataType>
class AbstractScratch {
public:
    AbstractScratch() = default;
    virtual ~AbstractScratch() = default;

    AbstractScratch(const AbstractScratch &) = delete;
    AbstractScratch &operator=(const AbstractScratch &) = delete;

    DataType *AlignedQueryT() { return aligned_query_T_; }
    PQScratch<DataType> *PqScratch() { return pq_scratch_; }

protected:
    DataType *aligned_query_T_;       // aligned query vector
    PQScratch<DataType> *pq_scratch_; // used for SSD index
};

// AbstractScratch space for in-memory index based search
export template <typename DataType>
class InMemQueryScratch : public AbstractScratch<DataType> {
public:
    InMemQueryScratch(u32 search_l,
                      u32 indexing_l,
                      u32 r,
                      u32 maxc,
                      SizeT dim,
                      SizeT aligned_dim,
                      SizeT alignment_factor,
                      bool init_pq_scratch = false)
        : L_(0), R_(r), maxc_(maxc) {
        if (search_l == 0 || indexing_l == 0 || r == 0 || dim == 0) {
            UnrecoverableError("InMemQueryScratch(): Invalid input parameters");
        }

        AllocAligned((void **)&this->aligned_query_T_, sizeof(DataType) * aligned_dim, alignment_factor * sizeof(DataType));
        memset(this->aligned_query_T_, 0, sizeof(DataType) * aligned_dim);

        if (init_pq_scratch)
            this->pq_scratch_ = new PQScratch<DataType>(DISKANN_MAX_GRAPH_DEGREE, aligned_dim);
        else
            this->pq_scratch_ = nullptr;

        occlude_factor_.reserve(maxc);
        inserted_into_pool_bs_ = new boost::dynamic_bitset<>();
        id_scratch_.reserve((SizeT)std::ceil(1.5 * DISKANN_GRAPH_SLACK_FACTOR * R_));
        dist_scratch_.reserve((SizeT)std::ceil(1.5 * DISKANN_GRAPH_SLACK_FACTOR * R_));

        ResizeForNewL(std::max(search_l, indexing_l));
    }
    ~InMemQueryScratch() {
        if (this->aligned_query_T_ != nullptr) {
            AlignedFree(this->aligned_query_T_);
            this->aligned_query_T_ = nullptr;
        }

        delete this->pq_scratch_;
        delete this->inserted_into_pool_bs_;
    }

    void ResizeForNewL(uint32_t new_l) {
        if (new_l > L_) {
            L_ = new_l;
            pool_.reserve(3 * L_ + R_);
            best_l_nodes_.Reserve(L_);

            inserted_into_pool_rs_.reserve(20 * L_);
        }
    }

    void Clear() {
        pool_.clear();
        best_l_nodes_.Clear();
        occlude_factor_.clear();

        inserted_into_pool_rs_.clear();
        inserted_into_pool_bs_->reset();

        id_scratch_.clear();
        dist_scratch_.clear();

        expanded_nodes_set_.clear();
        expanded_nghrs_vec_.clear();
        occlude_list_output_.clear();
    }

    inline u32 GetL() { return L_; }
    inline u32 GetR() { return R_; }
    inline u32 GetMaxc() { return maxc_; }
    inline DataType *AlignedQuery() { return this->aligned_query_T_; }
    inline PQScratch<DataType> *PqScratch() { return this->pq_scratch_; }
    inline Vector<Neighbor> &Pool() { return this->pool_; }
    inline NeighborPriorityQueue &BestLNodes() { return this->best_l_nodes_; }
    inline Vector<f32> &OccludeFactor() { return this->occlude_factor_; }
    inline std::unordered_set<u32> &InsertedIntoPoolRs() { return this->inserted_into_pool_rs_; }
    inline boost::dynamic_bitset<> &InsertedIntoPoolBs() { return *this->inserted_into_pool_bs_; }
    inline Vector<SizeT> &IdScratch() { return this->id_scratch_; }
    inline Vector<f32> &DistScratch() { return this->dist_scratch_; }
    inline std::unordered_set<u32> &ExpandedNodesSet() { return this->expanded_nodes_set_; }
    inline Vector<Neighbor> &ExpandedNghrsVec() { return this->expanded_nghrs_vec_; }
    inline Vector<u32> &OccludeListOutput() { return this->occlude_list_output_; }

private:
    u32 L_;
    u32 R_;
    u32 maxc_;

    Vector<Neighbor> pool_;              // store all neighbors explored from best_l_nodes_
    NeighborPriorityQueue best_l_nodes_; // store best L nodes
    Vector<f32> occlude_factor_;         // used in OccludeList()
    std::unordered_set<u32> inserted_into_pool_rs_;
    boost::dynamic_bitset<> *inserted_into_pool_bs_;

    Vector<SizeT> id_scratch_;
    Vector<f32> dist_scratch_;

    std::unordered_set<u32> expanded_nodes_set_;
    Vector<Neighbor> expanded_nghrs_vec_;
    Vector<u32> occlude_list_output_;
};

export template <typename T>
class ConcurrentQueue {
    typedef std::chrono::microseconds chrono_us_t;
    typedef std::unique_lock<std::mutex> mutex_locker;

    std::queue<T> q_;
    std::mutex mut_;
    std::mutex push_mut_;
    std::mutex pop_mut_;
    std::condition_variable push_cv_;
    std::condition_variable pop_cv_;
    T null_T_;

public:
    ConcurrentQueue() {}

    ConcurrentQueue(T nullT) { this->null_T_ = nullT; }

    ~ConcurrentQueue() {
        this->push_cv_.notify_all();
        this->pop_cv_.notify_all();
    }

    // queue stats
    uint64_t Size() {
        mutex_locker lk(this->mut_);
        uint64_t ret = q_.size();
        lk.unlock();
        return ret;
    }

    bool Empty() { return (this->size() == 0); }

    // PUSH BACK
    void Push(T &new_val) {
        mutex_locker lk(this->mut_);
        this->q_.push(new_val);
        lk.unlock();
    }

    template <class Iterator>
    void Insert(Iterator iter_begin, Iterator iter_end) {
        mutex_locker lk(this->mut_);
        for (Iterator it = iter_begin; it != iter_end; it++) {
            this->q_.push(*it);
        }
        lk.unlock();
    }

    // POP FRONT
    T Pop() {
        mutex_locker lk(this->mut_);
        if (this->q_.empty()) {
            lk.unlock();
            return this->null_T_;
        } else {
            T ret = this->q_.front();
            this->q_.pop();
            lk.unlock();
            return ret;
        }
    }

    // register for notifications
    void WaitForPushNotify(chrono_us_t wait_time = chrono_us_t{10}) {
        mutex_locker lk(this->push_mut_);
        this->push_cv_.wait_for(lk, wait_time);
        lk.unlock();
    }

    void WaitForPopNotify(chrono_us_t wait_time = chrono_us_t{10}) {
        mutex_locker lk(this->pop_mut_);
        this->pop_cv_.wait_for(lk, wait_time);
        lk.unlock();
    }

    // just notify functions
    void PushNotifyOne() { this->push_cv_.notify_one(); }
    void PushNotifyAll() { this->push_cv_.notify_all(); }
    void PopNotifyOne() { this->pop_cv_.notify_one(); }
    void PopNotifyAll() { this->pop_cv_.notify_all(); }
};

export template <typename T>
class ScratchStoreManager {
public:
    ScratchStoreManager(ConcurrentQueue<T *> &query_scratch) : scratch_pool_(query_scratch) {
        scratch_ = query_scratch.Pop();
        while (scratch_ == nullptr) {
            query_scratch.WaitForPushNotify();
            scratch_ = query_scratch.Pop();
        }
    }
    T *ScratchSpace() { return scratch_; }

    ~ScratchStoreManager() {
        scratch_->Clear();
        scratch_pool_.Push(scratch_);
        scratch_pool_.PushNotifyAll();
    }

    void Destroy() {
        while (!scratch_pool_.Empty()) {
            auto scratch = scratch_pool_.pop();
            while (scratch == nullptr) {
                scratch_pool_.WaitForPushNotify();
                scratch = scratch_pool_.pop();
            }
            delete scratch;
        }
    }

private:
    T *scratch_;
    ConcurrentQueue<T *> &scratch_pool_;
};

} // namespace infinity