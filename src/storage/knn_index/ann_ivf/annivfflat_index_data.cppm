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

export module annivfflat_index_data;

import stl;
import index_base;
import file_system;
import file_system_type;
import search_top_k;
import kmeans_partition;
import infinity_exception;
import logger;
import third_party;
import status;

namespace infinity {

export template <typename CentroidsDataType, typename VectorDataType = CentroidsDataType>
struct AnnIVFFlatIndexData {
    using CommonType = std::common_type_t<VectorDataType, CentroidsDataType>;
    bool loaded_{false};
    MetricType metric_{MetricType::kInvalid};
    u32 dimension_{};
    u32 partition_num_{};
    u32 data_num_{};
    Vector<CentroidsDataType> centroids_;
    Vector<Vector<u32>> ids_;
    Vector<Vector<VectorDataType>> vectors_;

    AnnIVFFlatIndexData() = default;
    AnnIVFFlatIndexData(MetricType metric, u32 dimension, u32 partition_num)
        : metric_(metric), dimension_(dimension), partition_num_(partition_num) {}

    // use existing vectors for training and insert
    // used in benchmark because there is no deleted rows
    void BuildIndex(const u32 dimension,
                    const u32 train_count,
                    const VectorDataType *train_ptr,
                    const u32 vector_count,
                    const VectorDataType *vectors_ptr,
                    const u32 min_points_per_centroid = 32,
                    const u32 max_points_per_centroid = 256) {
        if (loaded_) {
            String error_message = "AnnIVFFlatIndexData::BuildIndex(): Index data already exists.";
            UnrecoverableError(error_message);
        }
        if (dimension != dimension_) {
            String error_message = "Dimension not match";
            UnrecoverableError(error_message);
        }
        if (metric_ != MetricType::kMetricL2 && metric_ != MetricType::kMetricInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Status status = Status::NotSupport("Metric type not implemented");
                RecoverableError(status);
            } else {
                Status status = Status::NotSupport("Metric type not supported");
                RecoverableError(status);
            }
            return;
        }
        if (vector_count == 0 or train_count == 0) {
            LOG_TRACE("AnnIVFFlatIndexData::BuildIndex(): Empty data, no need to build index");
            loaded_ = true;
            return;
        }

        // step 1. train centroids
        TrainCentroids(train_count, train_ptr, min_points_per_centroid, max_points_per_centroid);

        // step 2. insert data to partitions
        struct {
            u32 operator[](u32 i) { return i; }
        } get_id;
        InsertData(vector_count, vectors_ptr, get_id);

        loaded_ = true;
    }

    // use iter for both training and insert
    // used when create index for a segment
    void BuildIndex(auto &&iter,
                    const u32 dimension,
                    const u32 full_row_count,
                    const u32 min_points_per_centroid = 32,
                    const u32 max_points_per_centroid = 256) {
        if (loaded_) {
            String error_message = "AnnIVFFlatIndexData::BuildIndex(): Index data already exists.";
            UnrecoverableError(error_message);
        }
        if (dimension != dimension_) {
            String error_message = "Dimension not match";
            UnrecoverableError(error_message);
        }
        if (metric_ != MetricType::kMetricL2 && metric_ != MetricType::kMetricInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Status status = Status::NotSupport("Metric type not implemented");
                RecoverableError(status);
            } else {
                Status status = Status::NotSupport("Metric type not supported");
                RecoverableError(status);
            }
            return;
        }

        // step 1. load input data

        // reserve space for vectors and ids
        Vector<VectorDataType> segment_column_data;
        segment_column_data.reserve(full_row_count * dimension);
        // offset without deleted rows
        Vector<SegmentOffset> segment_offset;
        segment_offset.reserve(full_row_count);

        // record input data count
        u32 cnt = 0;
        while (true) {
            auto pair_opt = iter.Next();
            if (!pair_opt) {
                break;
            }
            if (cnt >= full_row_count) {
                String error_message = "AnnIVFFlatIndexData::BuildIndex(): segment row count more than expected.";
                UnrecoverableError(error_message);
            }
            auto &[val_ptr, offset] = pair_opt.value(); // val_ptr is const VectorDataType * type, offset is SegmentOffset type
            // copy data of single embedding
            segment_column_data.insert(segment_column_data.end(), val_ptr, val_ptr + dimension);
            // copy offset of single embedding
            segment_offset.push_back(offset);
            // update cnt
            ++cnt;
        }
        if (cnt < full_row_count) {
            LOG_TRACE("AnnIVFFlatIndexData::BuildIndex(): segment has deleted rows");
        }
        if (cnt == 0) {
            loaded_ = true;
            return;
        }

        // step 2. train centroids
        TrainCentroids(cnt, segment_column_data.data(), min_points_per_centroid, max_points_per_centroid);

        // step 3. insert data to partitions, will update data_num_
        InsertData(cnt, segment_column_data.data(), segment_offset.data());

        loaded_ = true;
    }

    inline void TrainCentroids(const u32 vector_count,
                               const VectorDataType *vector_data_ptr,
                               const u32 min_points_per_centroid,
                               const u32 max_points_per_centroid) {
        u32 iteration_max = 0;
        if (partition_num_ != 0 and partition_num_ > vector_count) {
            LOG_TRACE(fmt::format("AnnIVFFlatIndexData::TrainCentroids(): non-zero partition_num_ = {}, more than vector_count = {}",
                                  partition_num_,
                                  vector_count));
            partition_num_ = vector_count;
            LOG_TRACE(fmt::format("AnnIVFFlatIndexData::TrainCentroids(): Update partition_num_ to {}", partition_num_));
        }
        u32 real_partition_num = GetKMeansCentroids<CommonType>(metric_,
                                                                dimension_,
                                                                vector_count,
                                                                vector_data_ptr,
                                                                centroids_,
                                                                partition_num_,
                                                                iteration_max,
                                                                min_points_per_centroid,
                                                                max_points_per_centroid);
        if (real_partition_num != partition_num_) {
            LOG_TRACE(fmt::format("AnnIVFFlatIndexData::BuildIndex(): After K-means partition, real_partition_num = %u, partition_num_ = %u",
                                  real_partition_num,
                                  partition_num_));
            LOG_TRACE(fmt::format("AnnIVFFlatIndexData::BuildIndex(): Update partition_num_ to %u", real_partition_num));
            partition_num_ = real_partition_num;
        }
    }

    inline void InsertData(u32 vector_count, const VectorDataType *vector_data_ptr, auto &&get_offset) {
        // step 1. Classify vectors
        // search_top_1
        auto assigned_partition_id = MakeUniqueForOverwrite<u32[]>(vector_count);
        search_top_1_without_dis<CommonType>(dimension_,
                                             vector_count,
                                             vector_data_ptr,
                                             partition_num_,
                                             centroids_.data(),
                                             assigned_partition_id.get());

        // step 2. Reserve space
        Vector<u32> partition_element_count(partition_num_);
        // calculate partition_element_count
        for (u32 i = 0; i < vector_count; ++i)
            ++partition_element_count[assigned_partition_id[i]];
        ids_.resize(partition_num_);
        vectors_.resize(partition_num_);
        for (u32 i = 0; i < partition_num_; ++i) {
            ids_[i].reserve(partition_element_count[i]);
            vectors_[i].reserve(partition_element_count[i] * dimension_);
        }

        // step 3. Insert vectors into partitions
        for (u32 i = 0; i < vector_count; ++i) {
            auto vector_pos_i = vector_data_ptr + i * dimension_;
            auto partition_of_i = assigned_partition_id[i];
            vectors_[partition_of_i].insert(vectors_[partition_of_i].end(), vector_pos_i, vector_pos_i + dimension_);
            ids_[partition_of_i].push_back(get_offset[i]);
        }

        // step 4. Update data_num_
        data_num_ += vector_count;
    }

    void SaveIndexInner(FileHandler &file_handler) {
        if (!loaded_) {
            String error_message = "AnnIVFFlatIndexData::SaveIndexInner(): Index data not loaded.";
            UnrecoverableError(error_message);
        }
        file_handler.Write(&metric_, sizeof(metric_));
        file_handler.Write(&dimension_, sizeof(dimension_));
        file_handler.Write(&partition_num_, sizeof(partition_num_));
        file_handler.Write(&data_num_, sizeof(data_num_));
        if (!centroids_.empty()) {
            file_handler.Write(centroids_.data(), sizeof(CentroidsDataType) * dimension_ * partition_num_);
            u32 vector_element_num;
            for (u32 i = 0; i < partition_num_; ++i) {
                vector_element_num = ids_[i].size();
                file_handler.Write(&vector_element_num, sizeof(vector_element_num));
                file_handler.Write(ids_[i].data(), sizeof(u32) * vector_element_num);
                file_handler.Write(vectors_[i].data(), sizeof(VectorDataType) * dimension_ * vector_element_num);
            }
        }
    }

    void SaveIndex(const String &file_path, UniquePtr<FileSystem> fs) {
        u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        auto [file_handler, status] = fs->OpenFile(file_path, file_flags, FileLockType::kWriteLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        SaveIndexInner(*file_handler);
        file_handler->Close();
    }

    void ReadIndexInner(FileHandler &file_handler) {
        file_handler.Read(&metric_, sizeof(metric_));
        file_handler.Read(&dimension_, sizeof(dimension_));
        file_handler.Read(&partition_num_, sizeof(partition_num_));
        file_handler.Read(&data_num_, sizeof(data_num_));
        centroids_.resize(dimension_ * partition_num_);
        ids_.resize(partition_num_);
        vectors_.resize(partition_num_);
        file_handler.Read(centroids_.data(), sizeof(CentroidsDataType) * dimension_ * partition_num_);
        u32 vector_element_num;
        for (u32 i = 0; i < partition_num_; ++i) {
            file_handler.Read(&vector_element_num, sizeof(vector_element_num));
            ids_[i].resize(vector_element_num);
            file_handler.Read(ids_[i].data(), sizeof(u32) * vector_element_num);
            vectors_[i].resize(dimension_ * vector_element_num);
            file_handler.Read(vectors_[i].data(), sizeof(VectorDataType) * dimension_ * vector_element_num);
        }
        loaded_ = true;
    }

    static UniquePtr<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>> LoadIndexInner(FileHandler &file_handler) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>>(MetricType::kInvalid, 0, 0);
        index_data->ReadIndexInner(file_handler);
        return index_data;
    }

    static UniquePtr<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>> LoadIndex(const String &file_path, UniquePtr<FileSystem> fs) {
        u8 file_flags = FileFlags::READ_FLAG;
        auto [file_handler, status] = fs->OpenFile(file_path, file_flags, FileLockType::kReadLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        auto index_data = LoadIndexInner(*file_handler);
        file_handler->Close();
        return index_data;
    }
};

} // namespace infinity