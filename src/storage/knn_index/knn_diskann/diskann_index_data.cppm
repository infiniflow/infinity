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
#include <set>
#include <string.h>

export module diskann_index_data;

import stl;
import index_base;
import search_top_1;
import kmeans_partition;
import infinity_exception;
import logger;
import third_party;
import status;
import default_values;
import diskann_partition_and_pq;
import vamana_alg;
import diskann_dist_func;
import local_file_handle;
import virtual_store;

namespace infinity {
export template <typename VectorDataType, typename LabelType, MetricType metric>
class DiskAnnIndexData {
    using MemVamana = MemVamana<VectorDataType, LabelType, metric>;
    using This = DiskAnnIndexData<VectorDataType, LabelType, metric>;

private:
    Atomic<bool> loaded_{false};
    MetricType metric_{metric};
    u32 dimension_{};
    SizeT data_num_{};
    u32 R_{};             // max degree
    u32 L_{};             // length of candidates list
    u32 num_pq_chunks_{}; // pq chunks num
    u32 num_parts_{};     // Vamana slices num
    u32 num_centers_{};   // kmeans centers num

    bool use_disk_pq_{false}; // store pq vector on disk
    f64 p_val_{};             // Sampling ratio
    Vector<LabelType> labels_;
    u32 train_size_{}; // size of sample training set

    SharedPtr<f32[]> full_pivot_data_; // pivot data for all chunks, every num_pq_chunks chunks are combined into a dim vector [num_centers * dim]
    SharedPtr<f32[]> centroid_;        // centroids after zero-mean, used for an option of L2 distance
    Vector<u32> rearrangement_;        // the dimensions in the order of chunks
    Vector<u32> chunk_offsets_;        // the starting index of each chunk in rearrangement_

    Path data_file_path_;
    Path mem_index_file_path_;
    Path index_file_path_;
    Path pqCompressed_data_file_path_;
    Path sample_data_file_path_;
    Path pq_pivot_file_path_;

private:
    // build vamana graph and merge graph with num_parts_
    void BuildMergedVamanaIndex(LocalFileHandle &data_file_handle, LocalFileHandle &mem_index_file_handle, Vector<LabelType> &labels) {
        if (num_parts_ == 1) {
            // MemVamana mem_index = MemVamana::Make(L_, R_, dimension_, data_num_);
            MemVamana mem_index(L_, R_, dimension_, data_num_);
            mem_index.Build(data_file_handle, data_num_, labels);

            mem_index.SaveGraph(mem_index_file_handle);
        } else if (num_parts_ > 1) {
            Status status = Status::NotSupport("DiskAnn()::BuildMergedVamanaIndex(): num_parts > 1 not supported");
            RecoverableError(status);
        } else {
            Status status = Status::NotSupport("num_parts should be greater than 0");
            RecoverableError(status);
        }
    }

    void CreateDiskLayout(LocalFileHandle &data_file_handle, LocalFileHandle &mem_index_file_handle, LocalFileHandle &index_file_handle) {
        SizeT npts = data_num_;
        SizeT ndims = dimension_;
        SizeT actual_file_size = 0;
        u32 width;
        SizeT medoid, vamana_frozen_num, vamana_frozen_loc = 0;
        mem_index_file_handle.Read(&actual_file_size, sizeof(SizeT));
        mem_index_file_handle.Read(&width, sizeof(u32));
        mem_index_file_handle.Read(&medoid, sizeof(SizeT));
        mem_index_file_handle.Read(&vamana_frozen_num, sizeof(SizeT));

        if (vamana_frozen_num == 1)
            vamana_frozen_loc = medoid;
        // node structure: [neighbor_size(u32), neighbor_id(SizeT)*]
        u64 max_node_len = (width) * sizeof(SizeT) + sizeof(u32) + ndims * sizeof(VectorDataType); // graph node + vector
        u32 nnodes_per_sector = DISKANN_SECTOR_LEN / max_node_len; // 0 if max_node_len > SECTOR_LEN(multi-sector for a node)

        UniquePtr<char[]> sector_buf = MakeUnique<char[]>(DISKANN_SECTOR_LEN);                             // sector buffer
        UniquePtr<char[]> multisector_buf = MakeUnique<char[]>(RoundUp(max_node_len, DISKANN_SECTOR_LEN)); // multi-sector buffer
        UniquePtr<char[]> node_buf = MakeUnique<char[]>(max_node_len);                                     // node buffer
        UniquePtr<VectorDataType[]> cur_node_coord = MakeUnique<VectorDataType[]>(ndims);                  // vector buf
        u32 &nnbrs = *(u32 *)(node_buf.get() + ndims * sizeof(VectorDataType));                            // neighbor num of node_buf
        u32 *nhood_buf = (u32 *)(node_buf.get() + ndims * sizeof(VectorDataType) + sizeof(u32));           // neighbor id ptr of node_buf

        u64 n_sectors = nnodes_per_sector > 0 ? RoundUp(npts, nnodes_per_sector) / nnodes_per_sector
                                              : npts * DivRoundUp(max_node_len, DISKANN_SECTOR_LEN); // total sectors num
        u64 disk_index_file_size = (n_sectors + 1) * DISKANN_SECTOR_LEN;                             // disk index file size

        // meta data for first sector
        Vector<u64> output_file_meta(DISKANN_SECTOR_LEN / sizeof(u64), 0);
        output_file_meta[0] = npts;
        output_file_meta[1] = ndims;
        output_file_meta[2] = medoid;
        output_file_meta[3] = max_node_len;
        output_file_meta[4] = nnodes_per_sector;
        output_file_meta[5] = n_sectors;
        output_file_meta[6] = vamana_frozen_num;
        output_file_meta[7] = vamana_frozen_loc;
        u64 append_reorder_data = 0; // not used for now
        output_file_meta[8] = append_reorder_data;
        output_file_meta[9] = disk_index_file_size;
        index_file_handle.Append(output_file_meta.data(), DISKANN_SECTOR_LEN);

        u64 cur_node_id = 0;
        if (nnodes_per_sector > 0) { // Write multiple nodes per sector
            for (u64 sector = 0; sector < n_sectors; sector++) {
                std::memset(sector_buf.get(), 0, DISKANN_SECTOR_LEN);

                for (u64 sector_node_id = 0; sector_node_id < nnodes_per_sector && cur_node_id < npts; sector_node_id++) {
                    std::memset(node_buf.get(), 0, max_node_len);

                    mem_index_file_handle.Read(&nnbrs, sizeof(u32)); // read neighbor num
                    assert(nnbrs <= width && nnbrs > 0);
                    mem_index_file_handle.Read(nhood_buf, nnbrs * sizeof(SizeT));                // read nnbrs * neighbor_id
                    data_file_handle.Read(cur_node_coord.get(), ndims * sizeof(VectorDataType)); // read vector

                    // node_buf: [vector(VectorDataType)*, neighbor_num(u32), neighbor_id(SizeT)*]
                    memcpy(node_buf.get(), cur_node_coord.get(), ndims * sizeof(VectorDataType)); // copy vector to node_buf
                    memcpy(node_buf.get() + ndims * sizeof(VectorDataType), &nnbrs, sizeof(u32)); // copy neighbor_num to node_buf
                    memcpy(node_buf.get() + ndims * sizeof(VectorDataType) + sizeof(u32),
                           nhood_buf,
                           nnbrs * sizeof(SizeT)); // copy neighbor_id to node_buf

                    char *sector_node_buf = sector_buf.get() + (sector_node_id * max_node_len); // node offset in sector
                    memcpy(sector_node_buf, node_buf.get(), max_node_len);                      // copy node_buf to sector_buf
                    cur_node_id++;
                }

                // write sector to disk
                index_file_handle.Append(sector_buf.get(), DISKANN_SECTOR_LEN);
            }
        } else { // Write multi-sector per node
            u64 nsectors_per_node = DivRoundUp(max_node_len, DISKANN_SECTOR_LEN);
            for (u64 i = 0; i < npts; i++) {
                memset(multisector_buf.get(), 0, DISKANN_SECTOR_LEN * nsectors_per_node);
                memset(node_buf.get(), 0, max_node_len);

                mem_index_file_handle.Read(&nnbrs, sizeof(u32)); // read neighbor num
                assert(nnbrs <= width && nnbrs > 0);
                mem_index_file_handle.Read(nhood_buf, nnbrs * sizeof(SizeT));                // read nnbrs * neighbor_id
                data_file_handle.Read(cur_node_coord.get(), ndims * sizeof(VectorDataType)); // read vector

                memcpy(multisector_buf.get(), cur_node_coord.get(), ndims * sizeof(VectorDataType)); // copy vector to multisector_buf
                memcpy(multisector_buf.get() + ndims * sizeof(VectorDataType), &nnbrs, sizeof(u32)); // copy neighbor_num to multisector_buf
                memcpy(multisector_buf.get() + ndims * sizeof(VectorDataType) + sizeof(u32),
                       nhood_buf,
                       nnbrs * sizeof(SizeT)); // copy neighbor_id to multisector_buf

                index_file_handle.Append(multisector_buf.get(), DISKANN_SECTOR_LEN * nsectors_per_node);
            }
        }
    }

    void SaveSampleData(LocalFileHandle &train_data_handle,
                        LocalFileHandle &train_ids_handle,
                        SharedPtr<VectorDataType[]> &train_data,
                        SharedPtr<SizeT[]> &train_data_ids) {
        train_data_handle.Append(&this->train_size_, sizeof(u32));
        train_data_handle.Append(&this->dimension_, sizeof(u32));
        train_ids_handle.Append(&this->train_size_, sizeof(u32));
        u32 const_one = 1;
        train_ids_handle.Append(&const_one, sizeof(u32));
        for (u32 i = 0; i < this->train_size_; i++) {
            train_data_handle.Append(train_data.get() + i * this->dimension_, sizeof(VectorDataType) * this->dimension_);
            train_ids_handle.Append(train_data_ids.get() + i, sizeof(SizeT));
        }
    }

public:
    DiskAnnIndexData() = default;
    DiskAnnIndexData(u32 dimension, SizeT data_num, u32 R, u32 L, u32 num_pq_chunks, u32 num_parts, u32 num_centers = DISKANN_NUM_CENTERS)
        : dimension_(dimension), data_num_{data_num}, R_(R), L_(L), num_pq_chunks_(num_pq_chunks), num_parts_(num_parts), num_centers_(num_centers) {}

    static UniquePtr<This>
    Make(u32 dimension, SizeT data_num, u32 R, u32 L, u32 num_pq_chunks, u32 num_parts, u32 num_centers = DISKANN_NUM_CENTERS) {
        return MakeUnique<This>(dimension, data_num, R, L, num_pq_chunks, num_parts, num_centers);
    }

    // build index from data file
    void BuildIndex(const u32 dimension,
                    const u32 data_num,
                    Vector<LabelType> &labels,
                    Path data_file_path,
                    Path mem_index_file_path,
                    Path index_file_path,
                    Path pqCompressed_data_file_path,
                    Path sample_data_file_path,
                    Path pq_pivot_file_path) {
        if (loaded_) {
            String error_message = "DiskAnnIndexData::BuildIndex(): Index data already exists.";
            UnrecoverableError(error_message);
        }
        if (dimension != dimension_) {
            String error_message = "DiskAnnIndexData::BuildIndex(): Dimension not match";
            UnrecoverableError(error_message);
        }
        if (metric_ != MetricType::kMetricL2 && metric_ != MetricType::kMetricCosine) {
            if (metric_ != MetricType::kInvalid) {
                Status status = Status::NotSupport("Metric type not implemented");
                RecoverableError(status);
            } else {
                Status status = Status::NotSupport("Metric type is invalid");
                RecoverableError(status);
            }
            return;
        }
        if (data_num == 0) {
            LOG_TRACE("DiskAnnIndexData::BuildIndex(): Empty data, no need to build index");
            loaded_.store(true);
            return;
        }
        num_pq_chunks_ = std::max(num_pq_chunks_, 1u);
        num_pq_chunks_ = std::min(num_pq_chunks_, static_cast<u32>(dimension));
        num_pq_chunks_ = std::min(num_pq_chunks_, static_cast<u32>(DISKANN_MAX_PQ_CHUNKS));
        data_num_ = data_num;
        labels_ = std::move(labels);
        this->data_file_path_ = std::move(data_file_path);
        this->mem_index_file_path_ = std::move(mem_index_file_path);
        this->index_file_path_ = std::move(index_file_path);
        this->pqCompressed_data_file_path_ = std::move(pqCompressed_data_file_path);
        this->sample_data_file_path_ = std::move(sample_data_file_path);
        this->pq_pivot_file_path_ = std::move(pq_pivot_file_path);

        auto [data_file_handle, data_file_status] = LocalStore::Open(data_file_path_, FileAccessMode::kRead);
        if (!data_file_status.ok()) {
            UnrecoverableError(data_file_status.message());
        }

        SharedPtr<VectorDataType[]> train_data;
        SharedPtr<SizeT[]> train_data_ids;

        // step 1. generates random sample and sets it to train_data and train_size
        {
            u32 train_size;
            p_val_ = ((f64)DISKANN_TRAINING_SET_SIZE / (f64)data_num);
            if (data_num > DISKANN_TRAINING_SET_SIZE) {
                GenRandomSlice<VectorDataType>(*data_file_handle, p_val_, train_data, train_size, dimension_, data_num_, train_data_ids);
            } else {
                LOG_TRACE(fmt::format("data num is less than sample_size {}, using all data", DISKANN_TRAINING_SET_SIZE));
                train_size = data_num;
                train_data = MakeUnique<VectorDataType[]>(train_size * dimension_);
                train_data_ids = MakeUnique<SizeT[]>(train_size);
                data_file_handle->Read(train_data.get(), train_size * dimension_ * sizeof(VectorDataType));
                for (SizeT i = 0; i < train_size; i++) {
                    train_data_ids[i] = i;
                }
            }
            this->train_size_ = train_size;
            LOG_TRACE(fmt::format("Sample training size :{}", train_size));
        }

        // step 2. generate pq pivots and pq data
        {
            data_file_handle->Seek(0);
            auto [pqCompressed_data_file_handle, pq_data_file_status] = LocalStore::Open(pqCompressed_data_file_path_, FileAccessMode::kWrite);
            if (!pq_data_file_status.ok()) {
                UnrecoverableError(pq_data_file_status.message());
            }

            auto [pq_pivot_file_handle, pq_pivot_file_status] = LocalStore::Open(pq_pivot_file_path_, FileAccessMode::kWrite);
            if (!pq_pivot_file_status.ok()) {
                UnrecoverableError(pq_pivot_file_status.message());
            };

            bool make_zero_mean = true; // mean subtraction for centering, not for inner product
            if (metric_ == MetricType::kMetricInnerProduct)
                make_zero_mean = false;

            if (train_size_ > num_centers_) {
                GeneratePqPivots<VectorDataType>(*pq_pivot_file_handle,
                                                 train_data,
                                                 train_size_,
                                                 dimension_,
                                                 num_centers_,
                                                 num_pq_chunks_,
                                                 DISKANN_NUM_KMEANS_REPS,
                                                 make_zero_mean,
                                                 full_pivot_data_,
                                                 centroid_,
                                                 rearrangement_,
                                                 chunk_offsets_);
                GeneratePqDataFromPivots<VectorDataType>(*data_file_handle,
                                                         *pqCompressed_data_file_handle,
                                                         data_num_,
                                                         dimension_,
                                                         num_centers_,
                                                         num_pq_chunks_,
                                                         full_pivot_data_,
                                                         centroid_,
                                                         rearrangement_,
                                                         chunk_offsets_);
                LOG_TRACE(fmt::format("generate Pq data done"));
            } else {
                Status status = Status::NotSupport("training size is less than centers num");
                RecoverableError(status);
                // TODO
                //  LOG_TRACE("Train size is less than number of centers, using train data as pivots");
            }
            LOG_TRACE(fmt::format("Finished generating pq pivots and pq data"));
        }

        // step 3. build vamana graph
        {
            data_file_handle->Seek(0);
            auto [mem_index_file_handle, mem_index_write_status] = LocalStore::Open(mem_index_file_path_, FileAccessMode::kWrite);
            if (!mem_index_write_status.ok()) {
                UnrecoverableError(mem_index_write_status.message());
            }

            BuildMergedVamanaIndex(*data_file_handle, *mem_index_file_handle, labels_);
            LOG_TRACE(fmt::format("Finished building and merging Vamana graph"));
        }

        // step 4. create disk layout
        {
            data_file_handle->Seek(0);
            auto [mem_index_file_handle, mem_index_read_status] = LocalStore::Open(mem_index_file_path_, FileAccessMode::kRead);
            if (!mem_index_read_status.ok()) {
                UnrecoverableError(mem_index_read_status.message());
            }

            auto [index_file_handle, index_file_status] = LocalStore::Open(index_file_path_, FileAccessMode::kWrite);
            if (!index_file_status.ok()) {
                UnrecoverableError(index_file_status.message());
            }

            CreateDiskLayout(*data_file_handle, *mem_index_file_handle, *index_file_handle);
            LOG_TRACE(fmt::format("Finished creating disk layout"));
        }

        // step 5. save sample data and clean mem vamana
        {
            data_file_handle->Seek(0);
            Path train_data_path = sample_data_file_path_ / "train_data.bin";
            Path train_data_ids_path = sample_data_file_path_ / "train_ids.bin";
            auto [train_data_handle, train_data_status] = LocalStore::Open(train_data_path, FileAccessMode::kWrite);
            if (!train_data_status.ok()) {
                UnrecoverableError(train_data_status.message());
            }

            auto [train_data_ids_handle, train_data_ids_status] = LocalStore::Open(train_data_ids_path, FileAccessMode::kWrite);
            if (!train_data_ids_status.ok()) {
                UnrecoverableError(train_data_ids_status.message());
            }

            SaveSampleData(*train_data_handle, *train_data_ids_handle, train_data, train_data_ids);
            // train_data.reset();
            // train_data_ids.reset();
            LocalStore::DeleteFile(mem_index_file_path_); // delete mem index file
            LOG_TRACE(fmt::format("Finished saving sample data"));
        }

        loaded_.store(true);
    }

    void UnitTest() {
        LOG_TRACE("DiskAnnIndexData::UnitTest()");

        LOG_TRACE("UnitTest(): Test train data and ids");
        {
            Path train_data_path = sample_data_file_path_ / "train_data.bin";
            Path train_data_ids_path = sample_data_file_path_ / "train_ids.bin";
            auto [train_data_handle, train_data_status] = LocalStore::Open(train_data_path, FileAccessMode::kRead);
            if (!train_data_status.ok()) {
                UnrecoverableError(train_data_status.message());
            }

            auto [train_data_ids_handle, train_data_ids_status] = LocalStore::Open(train_data_ids_path, FileAccessMode::kRead);
            if (!train_data_ids_status.ok()) {
                UnrecoverableError(train_data_ids_status.message());
            }

            UniquePtr<VectorDataType[]> train_data = MakeUnique<VectorDataType[]>(train_size_ * dimension_);
            UniquePtr<SizeT[]> train_data_ids = MakeUnique<SizeT[]>(train_size_);
            fmt::print("train_size: {}, dimension: {}\n", train_size_, dimension_);
            train_data_handle->Read(train_data.get(), train_size_ * dimension_ * sizeof(VectorDataType));
            train_data_ids_handle->Read(train_data_ids.get(), train_size_ * sizeof(SizeT));
            fmt::print("train_data_ids[0] {} :\n", train_data_ids[0]);
            fmt::print("train_data_ids[1] {} :\n", train_data_ids[1]);
            for (u32 j = 0; j < dimension_; j++) {
                fmt::print("{}, ", train_data[0 * dimension_ + j]);
            }
            fmt::print("\n");
            for (u32 j = 0; j < dimension_; j++) {
                fmt::print("{}, ", train_data[1 * dimension_ + j]);
            }
            fmt::print("\n");
        }

        LOG_TRACE("UnitTest(): Test Pq pivots and pq data");
        {

            fmt::print("centroid_: \n");
            for (u32 j = 0; j < dimension_; j++) {
                fmt::print("{} ,", centroid_[j]);
            }
            fmt::print("\n");
            for (u32 i = 0; i < 3; i++) {
                fmt::print("pivots[{}]: \n", i);
                for (u32 j = 0; j < num_centers_; j++) {
                    fmt::print("{} ", full_pivot_data_[i * dimension_ + j]);
                }
                fmt::print("\n");
            }
            auto [pqCompressed_data_file_handle, pq_data_file_status] = LocalStore::Open(pqCompressed_data_file_path_, FileAccessMode::kRead);
            if (!pq_data_file_status.ok()) {
                UnrecoverableError(pq_data_file_status.message());
            }

            UniquePtr<u32[]> pqdata = MakeUniqueForOverwrite<u32[]>(num_pq_chunks_ * data_num_);
            pqCompressed_data_file_handle->Read(pqdata.get(), num_pq_chunks_ * sizeof(u32) * data_num_);
            fmt::print("pqdata[0]: \n");
            for (u32 j = 0; j < num_pq_chunks_; j++) {
                fmt::print("{}, ", pqdata[j]);
            }
            fmt::print("\n");
        }
    }
};

} // namespace infinity