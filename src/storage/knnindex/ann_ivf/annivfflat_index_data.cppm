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

import stl;
import index_def;
import file_system;
import file_system_type;
import search_top_k;
import kmeans_partition;
import infinity_exception;

export module annivfflat_index_data;

namespace infinity {

export template <typename CentroidsDataType, typename VectorDataType = CentroidsDataType>
struct AnnIVFFlatIndexData {
    MetricType metric_{MetricType::kInvalid};
    u32 dimension_{};
    u32 partition_num_{};
    u32 data_num_{};
    Vector<CentroidsDataType> centroids_;
    Vector<Vector<u32>> ids_;
    Vector<Vector<VectorDataType>> vectors_;
    AnnIVFFlatIndexData(MetricType metric, u32 dimension, u32 partition_num)
        : metric_(metric), dimension_(dimension), partition_num_(partition_num), centroids_(partition_num_ * dimension_), ids_(partition_num_),
          vectors_(partition_num_) {}

    void train_centroids(u32 dimension,
                         u32 vector_count,
                         const VectorDataType *vectors_ptr,
                         u32 iteration_max = 0,
                         u32 min_points_per_centroid = 32,
                         u32 max_points_per_centroid = 256) {
        if (dimension != dimension_) {
            Error<StorageException>("Dimension not match");
        }
        if (metric_ != MetricType::kMerticL2 && metric_ != MetricType::kMerticInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Error<StorageException>("Metric type not implemented");
            } else {
                Error<StorageException>("Metric type not supported");
            }
            return;
        }
        k_means_partition_only_centroids<f32>(metric_,
                                              dimension,
                                              vector_count,
                                              vectors_ptr,
                                              centroids_.data(),
                                              partition_num_,
                                              iteration_max,
                                              min_points_per_centroid,
                                              max_points_per_centroid);
    }

    void insert_data(i32 dimension, u64 vector_count, const VectorDataType *vectors_ptr, u32 id_begin = 0) {
        if (dimension != dimension_) {
            Error<StorageException>("Dimension not match");
        }
        if (metric_ != MetricType::kMerticL2 && metric_ != MetricType::kMerticInnerProduct) {
            if (metric_ != MetricType::kInvalid) {
                Error<StorageException>("Metric type not implemented");
            } else {
                Error<StorageException>("Metric type not supported");
            }
            return;
        }
        add_data_to_partition(dimension, vector_count, vectors_ptr, this, id_begin);
    }

    void SaveIndexInner(FileHandler &file_handler) {
        file_handler.Write(&metric_, sizeof(metric_));
        file_handler.Write(&dimension_, sizeof(dimension_));
        file_handler.Write(&partition_num_, sizeof(partition_num_));
        file_handler.Write(&data_num_, sizeof(data_num_));
        file_handler.Write(centroids_.data(), sizeof(CentroidsDataType) * dimension_ * partition_num_);
        u32 vector_element_num;
        for (u32 i = 0; i < partition_num_; ++i) {
            vector_element_num = ids_[i].size();
            file_handler.Write(&vector_element_num, sizeof(vector_element_num));
            file_handler.Write(ids_[i].data(), sizeof(u32) * vector_element_num);
            file_handler.Write(vectors_[i].data(), sizeof(VectorDataType) * dimension_ * vector_element_num);
        }
    }

    void SaveIndex(const String &file_path, UniquePtr<FileSystem> fs) {
        u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        UniquePtr<FileHandler> file_handler = fs->OpenFile(file_path, file_flags, FileLockType::kWriteLock);
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
    }

    static UniquePtr<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>> LoadIndexInner(FileHandler &file_handler) {
        auto index_data = MakeUnique<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>>(MetricType::kInvalid, 0, 0);
        index_data->ReadIndexInner(file_handler);
        return index_data;
    }

    static UniquePtr<AnnIVFFlatIndexData<CentroidsDataType, VectorDataType>> LoadIndex(const String &file_path, UniquePtr<FileSystem> fs) {
        u8 file_flags = FileFlags::READ_FLAG;
        UniquePtr<FileHandler> file_handler = fs->OpenFile(file_path, file_flags, FileLockType::kReadLock);
        auto index_data = LoadIndexInner(*file_handler);
        file_handler->Close();
        return index_data;
    }
};

export template <typename ElemType, typename CentroidsDataType, typename VectorDataType>
void add_data_to_partition(u32 dimension,
                           u32 vector_count,
                           const ElemType *vectors_ptr,
                           AnnIVFFlatIndexData<CentroidsDataType, VectorDataType> *index_data,
                           u32 id_begin = 0) {
    if (vector_count <= 0 || index_data == nullptr) {
        Error<StorageException>("vector_count <= 0 || index_data == nullptr");
        return;
    }
    if (index_data->dimension_ != dimension) {
        Error<StorageException>("index_data->dimension_ != dimension");
        return;
    }
    if (id_begin == 0)
        id_begin = index_data->data_num_;
    u32 partition_num = index_data->partition_num_;
    const auto &centroids = index_data->centroids_;
    auto &vectors = index_data->vectors_;
    auto &ids = index_data->ids_;
    Vector<u32> assigned_partition_id(vector_count);

    // Classify vectors
    // search_top_1
    search_top_1_without_dis<f32>(dimension, vector_count, vectors_ptr, partition_num, centroids.data(), assigned_partition_id.data());

    Vector<u32> partition_element_count(partition_num);
    // calculate partition_element_count
    for (auto i : assigned_partition_id)
        ++partition_element_count[i];
    // Reserve space
    for (u32 i = 0; i < partition_num; ++i) {
        vectors[i].reserve(vectors[i].size() + partition_element_count[i] * dimension);
        ids[i].reserve(ids[i].size() + partition_element_count[i]);
    }

    // insert vectors to partition
    for (u32 i = 0; i < vector_count; ++i) {
        auto vector_pos_i = vectors_ptr + i * dimension;
        auto partition_of_i = assigned_partition_id[i];
        vectors[partition_of_i].insert(vectors[partition_of_i].end(), vector_pos_i, vector_pos_i + dimension);
        ids[partition_of_i].push_back(id_begin + i);
    }
    // update data_num_
    index_data->data_num_ += vector_count;
}

} // namespace infinity