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

#include <cassert>
#include <fstream>
#include <ostream>
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module diskann_mem_data_store;

import stl;
import file_system;
import diskann_dist_func;
import diskann_utils;
import infinity_exception;
import local_file_handle;

namespace infinity {
export template <typename DataType>
class DiskAnnMemDataStore {
public:
    using This = DiskAnnMemDataStore<DataType>;
    using DistanceFunc = DiskAnnDistance<DataType>;
    using UniquePtrDataTypeAligned = std::unique_ptr<DataType[], decltype([](DataType *p) { std::free(p); })>;

    DiskAnnMemDataStore() : capacity_(0), dim_(0) {};
    DiskAnnMemDataStore(SizeT num_points, SizeT dim, DistanceFunc distance_fn)
        : capacity_(num_points), dim_(dim), distance_fn_(std::move(distance_fn)) {
        aligned_dim_ = RoundUp(dim, distance_fn.GetRequiredAlignment());
        DataType *ptr;
        AllocAligned((void **)&ptr, this->capacity_ * aligned_dim_ * sizeof(DataType), 8 * sizeof(DataType));
        // auto ptr = std::aligned_alloc(8 * sizeof(DataType), this->capacity_ * aligned_dim_ * sizeof(DataType));
        data_unique_ = UniquePtrDataTypeAligned(ptr);
        data_ = data_unique_.get();
        memset(data_, 0, this->capacity_ * aligned_dim_ * sizeof(DataType));
    }

    ~DiskAnnMemDataStore() {
        // data_ is freed in the destructor of UniquePtr
        // AlignedFree(data_);
    }

    DiskAnnMemDataStore(const This &&other)
        : capacity_(std::exchange(other.capacity_, 0)), dim_(std::exchange(other.dim_, 0)), distance_fn_(std::move(other.distance_fn_)),
          aligned_dim_(std::exchange(other.aligned_dim_, 0)), data_unique_(std::move(other.data_unique_)), data_(std::move(other.data_)),
          pre_computed_norms_(std::move(other.pre_computed_norms_)) {}

    DiskAnnMemDataStore &operator=(This &&other) {
        if (this != &other) {
            this->capacity_ = std::exchange(other.capacity_, 0);
            this->dim_ = std::exchange(other.dim_, 0);
            this->distance_fn_ = std::move(other.distance_fn_);
            this->aligned_dim_ = std::exchange(other.aligned_dim_, 0);
            this->data_unique_ = std::move(other.data_unique_);
            this->data_ = std::move(other.data_);
            this->pre_computed_norms_ = std::move(other.pre_computed_norms_);
        }
        return *this;
    }

    static This Make(SizeT num_points, SizeT dim, DiskAnnMetricType metric) {
        DistanceFunc distance_fn = DistanceFunc::Make(metric);
        return This(num_points, dim, distance_fn);
    }

    SizeT GetAlignedDim() const { return aligned_dim_; }

    SizeT GetAlignmentFactor() const { return distance_fn_.GetRequiredAlignment(); }

    SizeT Capacity() const { return capacity_; }

    SizeT GetDims() const { return dim_; }

    SizeT Resize(SizeT new_num_points) {
        if (new_num_points > capacity_) {
            return Expand(new_num_points);
        } else if (new_num_points < capacity_) {
            return Shrink(new_num_points);
        } else {
            return capacity_;
        }
    }

    void Load(LocalFileHandle &load_file_handle) {}
    void Save(LocalFileHandle &save_file_handle) {}

    void PopulateData(DataType *vectors, SizeT num_pts) {
        memset(data_, 0, num_pts * aligned_dim_ * sizeof(DataType));
        for (SizeT i = 0; i < num_pts; i++) {
            std::memmove(data_ + i * aligned_dim_, vectors + i * dim_, dim_ * sizeof(DataType));
        }

        if (distance_fn_.PreprocessingRequired()) {
            distance_fn_.PreprocessBasePoints(data_, this->_aligned_dim, num_pts);
        }
    }

    void PopulateData(LocalFileHandle &file_handle, SizeT num_pts) {
        if (num_pts > this->capacity_) {
            UnrecoverableError("DiskAnnMemDataStore::PopulateData(): num_pts > capacity_");
        }

        for (SizeT i = 0; i < num_pts; i++) {
            file_handle.Read(data_ + i * aligned_dim_, dim_ * sizeof(DataType));
            memset(data_ + i * aligned_dim_ + dim_, 0, (aligned_dim_ - dim_) * sizeof(DataType));
        }
        if (distance_fn_.PreprocessingRequired()) {
            distance_fn_.PreprocessBasePoints(data_, this->aligned_dim_, num_pts);
        }
    }

    void GetVector(SizeT idx, DataType *vector) const { memcpy(vector, data_ + idx * aligned_dim_, dim_ * sizeof(DataType)); }

    void SetVector(SizeT loc, DataType *vector) {
        SizeT offset = loc * aligned_dim_;
        memset(data_ + offset, 0, aligned_dim_ * sizeof(DataType));
        memcpy(data_ + offset, vector, dim_ * sizeof(DataType));
        if (distance_fn_.PreprocessingRequired()) {
            distance_fn_.preprocessBasePoint(data_ + offset, aligned_dim_, 1);
        }
    }

    void PrefetchVector(SizeT idx) {
        SizeT max_prefetch_size = (aligned_dim_ * sizeof(DataType) / 64) * 64;
        for (SizeT i = 0; i < max_prefetch_size; i += 64) {
            _mm_prefetch(((const char *)data_ + idx * aligned_dim_ * sizeof(DataType)) + i, _MM_HINT_T0);
        }
    }

    void PreprocessQuery(DataType *query, AbstractScratch<DataType> *query_scratch) const {
        if (query_scratch != nullptr) {
            memcpy(query_scratch->AlignedQueryT(), query, sizeof(DataType) * dim_);
        } else {
            UnrecoverableError("DiskAnnMemDataStore::PreprocessQuery(): query_scratch is null");
        }
    }

    f32 GetDistance(DataType *query, SizeT loc) const { return distance_fn_.Compare(query, data_ + loc * aligned_dim_, aligned_dim_); }

    void GetDistance(DataType *query, SizeT *locations, const SizeT location_count, f32 *distances, AbstractScratch<DataType> *query_scratch) const {
        for (SizeT i = 0; i < location_count; i++) {
            distances[i] = distance_fn_.Compare(query, data_ + locations[i] * aligned_dim_, aligned_dim_);
        }
    }

    f32 GetDistance(const SizeT loc1, const SizeT loc2) const {
        return distance_fn_.Compare(data_ + loc1 * aligned_dim_, data_ + loc2 * aligned_dim_, aligned_dim_);
    }

    void GetDistance(DataType *preprocessed_query, Vector<SizeT> &ids, Vector<f32> &distances, AbstractScratch<DataType> *query_scratch) const {
        if (distances.size() < ids.size()) {
            distances.resize(ids.size());
        }
        for (SizeT i = 0; i < ids.size(); i++) {
            distances[i] = distance_fn_.Compare(preprocessed_query, data_ + ids[i] * aligned_dim_, aligned_dim_);
        }
    }

    SizeT Expand(const SizeT new_size) {
        if (new_size == this->capacity_) {
            return this->capacity_;
        } else if (new_size < this->capacity_) {
            UnrecoverableError("DiskAnnMemDataStore::Expand(): cannot Expand to smaller size");
        }

        DataType *new_data = nullptr;
        AllocAligned((void **)&new_data, new_size * aligned_dim_ * sizeof(DataType), 8 * sizeof(DataType));
        memcpy(new_data, data_, this->capacity_ * aligned_dim_ * sizeof(DataType));
        data_unique_.reset(new_data);
        data_ = data_unique_.get();
        this->capacity_ = new_size;
        return this->capacity_;
    }

    SizeT Shrink(SizeT new_size) {
        if (new_size == this->capacity_) {
            return this->capacity_;
        } else if (new_size > this->capacity_) {
            UnrecoverableError("DiskAnnMemDataStore::Shrink(): cannot Shrink to larger size");
        }

        DataType *new_data = nullptr;
        AllocAligned((void **)&new_data, new_size * aligned_dim_ * sizeof(DataType), 8 * sizeof(DataType));
        memcpy(new_data, data_, new_size * aligned_dim_ * sizeof(DataType));
        data_unique_.reset(new_data);
        data_ = data_unique_.get();
        this->capacity_ = new_size;
        return this->capacity_;
    }

    void MoveVectors(u32 old_location_start, u32 new_location_start, u32 num_locations) {
        if (num_locations == 0 || old_location_start == new_location_start) {
            return;
        }

        u32 mem_clear_loc_start = old_location_start;
        u32 mem_clear_loc_end_limit = old_location_start + num_locations;

        // If ranges are overlapping, make sure not to clear the newly copied data.
        if (new_location_start < old_location_start) {
            if (mem_clear_loc_start < new_location_start + num_locations) {
                // Clear only after the end of the new range.
                mem_clear_loc_start = new_location_start + num_locations;
            }
        } else {
            if (mem_clear_loc_end_limit > new_location_start) {
                // Clear only up to the beginning of the new range.
                mem_clear_loc_end_limit = new_location_start;
            }
        }

        CopyVectors(old_location_start, new_location_start, num_locations);
        memset(data_ + aligned_dim_ * mem_clear_loc_start, 0, sizeof(DataType) * aligned_dim_ * (mem_clear_loc_end_limit - mem_clear_loc_start));
    }

    void CopyVectors(SizeT from_loc, SizeT to_loc, SizeT num_points) {
        assert(from_loc < this->capacity_);
        assert(to_loc < this->capacity_);
        assert(num_points < this->capacity_);
        memmove(data_ + to_loc * aligned_dim_, data_ + from_loc * aligned_dim_, num_points * aligned_dim_ * sizeof(DataType));
    }

    // retun the index of min distance point from the center as the enter point
    SizeT CalculateMedoid() const {
        UniquePtr<f32[]> center = MakeUnique<f32[]>(aligned_dim_);
        for (SizeT i = 0; i < aligned_dim_; i++)
            center[i] = 0.0f;

        for (SizeT i = 0; i < capacity_; i++) {
            for (SizeT j = 0; j < aligned_dim_; j++) {
                center[j] += data_[i * aligned_dim_ + j];
            }
        }

        for (SizeT j = 0; j < aligned_dim_; j++) {
            center[j] /= capacity_;
        }

        // find the min distance point from the center
        UniquePtr<f32[]> distances = MakeUnique<f32[]>(capacity_);
        for (SizeT i = 0; i < capacity_; i++) {
            f32 &dist = distances[i];
            const DataType *cur_vec = data_ + i * aligned_dim_;
            dist = L2Distance<f32, DataType, f32>(cur_vec, center.get(), static_cast<u32>(aligned_dim_));
        }
        u32 min_idx = 0;
        f32 min_dist = distances[0];
        for (SizeT i = 1; i < capacity_; i++) {
            if (distances[i] < min_dist) {
                min_idx = i;
                min_dist = distances[i];
            }
        }
        return min_idx;
    }

    DistanceFunc getDistFn() const { return this->distance_fn_; }

private:
    SizeT capacity_;
    SizeT dim_;

    UniquePtrDataTypeAligned data_unique_;
    DataType *data_;
    SizeT aligned_dim_;
    DistanceFunc distance_fn_;
    SharedPtr<f32[]> pre_computed_norms_; // in case we need to save vector norms for optimization
};

} // namespace infinity