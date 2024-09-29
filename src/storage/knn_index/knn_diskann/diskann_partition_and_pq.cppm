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

#include <cstring>
#include <random>
#include <cassert>
export module diskann_partition_and_pq;

import stl;
import third_party;
import file_system;
import file_system_type;
import infinity_exception;
import index_base;
import vector_distance;
import logger;
import simd_functions;
import diskann_dist_func;
import default_values;

namespace infinity {

void KmeansppSelectingPivots(f32 *data, u32 num_points, u32 dim,
                               f32 *pivot_data, u32 num_centers);

f32 RunLloyds(f32 *data, u32 num_points, u32 dim, f32 *centers,
                 const u32 num_centers, const u32 max_reps, u32 *closest_center);

f32 LloydsIter(f32 *data, SizeT num_points, SizeT dim, f32 *centers,
                  SizeT num_centers, Vector<SizeT> *closest_docs,
                  u32 *&closest_center);

template <typename InType, typename OutType>
void ConvertTypes(const InType *srcmat, OutType *destmat, size_t npts,
                   size_t dim);

// Generate random slice from existing data file
export template <typename VectorDataType>
void GenRandomSlice(FileHandler &data_file, f64 p_val, SharedPtr<VectorDataType[]> &sampled_data, 
                        u32 &slice_size, u32 &ndims, SizeT &npts, SharedPtr<SizeT[]> &sampled_ids) {
    Vector<Vector<f32>> sampled_vectors;
    Vector<SizeT> sampled_ids_vec;
    SharedPtr<VectorDataType[]> cur_vector_T = MakeUnique<VectorDataType[]>(ndims);
    p_val = p_val < 1 ? p_val : 1;

    std::random_device rd;
    u32 x = rd();
    std::mt19937 generator(x);
    std::uniform_real_distribution<f32> distribution(0, 1);

    for (u32 i = 0; i < npts; i++) {
        data_file.Read(cur_vector_T.get(), ndims * sizeof(VectorDataType));
        f32 rnd_val = distribution(generator);
        if (rnd_val < p_val) {
            Vector<f32> cur_vector_float;
            for (u32 d = 0; d < ndims; d++){
                cur_vector_float.push_back(cur_vector_T[d]);
            }
            sampled_vectors.push_back(cur_vector_float);
            sampled_ids_vec.push_back(i);
        }
    }
    slice_size = sampled_vectors.size();
    sampled_data = MakeShared<VectorDataType[]>(slice_size * ndims);
    sampled_ids = MakeShared<SizeT[]>(slice_size);

    std::copy(sampled_ids_vec.begin(), sampled_ids_vec.end(), sampled_ids.get());
    for (u32 i = 0; i < slice_size; i++) {
        std::copy(sampled_vectors[i].begin(), sampled_vectors[i].end(), sampled_data.get() + i * ndims);
    }
}

// Generate PQ pivots from train_data
// train_data in float32, and return full_pivot_data in float32
export template <typename VectorDataType>
void GeneratePqPivots(FileHandler &pq_pivot_file_handler, SharedPtr<VectorDataType[]>& origin_train_data, u32 num_train, u32 dim,
                      u32 num_centers, u32 num_pq_chunks, u32 max_k_means_reps,
                      bool make_zero_mean, SharedPtr<f32[]> &full_pivot_data,
                      SharedPtr<f32[]> &centroid, Vector<u32> &rearrangement,
                      Vector<u32> &chunk_offsets) {
    centroid = MakeShared<f32[]>(dim);
    UniquePtr<VectorDataType[]> train_data = MakeUnique<VectorDataType[]>(num_train * dim);
    memcpy(train_data.get(), origin_train_data.get(), num_train * dim * sizeof(VectorDataType));

    for (uint64_t d = 0; d < dim; d++) {
        centroid[d] = 0;
    }

    // mean centering
    if (make_zero_mean) {
        for (uint64_t p = 0; p < num_train; p++) {
            for (uint64_t d = 0; d < dim; d++) {
                centroid[d] += train_data[p * dim + d];
            }
        }
        for (uint64_t d = 0; d < dim; d++) {
            centroid[d] /= num_train;
        }

        for (uint64_t p = 0; p < num_train; p++) {
            for (uint64_t d = 0; d < dim; d++) {
                train_data[p * dim + d] -= centroid[d];
            }
        }
    }

    SizeT low_val = (SizeT)std::floor((double)dim / (double)num_pq_chunks); // size of small chunk
    SizeT high_val = (SizeT)std::ceil((double)dim / (double)num_pq_chunks); // size of big chunk
    SizeT max_num_high = dim - (low_val * num_pq_chunks); // number of big chunks
    Vector<Vector<u32> > bin_to_dims(num_pq_chunks); // which dimensions are in which chunk
    std::unordered_map<u32, u32> dim_to_bin; // which chunk does a dimension belong to

    // assign each dimension to a chunk in order(big chunks first)
    for (u32 cur_chunk=0; cur_chunk<max_num_high; cur_chunk++) {
        u32 start_d = high_val*cur_chunk;
        for (u32 d=start_d; d<start_d + high_val; d++) {
            bin_to_dims[cur_chunk].push_back(d);
            dim_to_bin[d] = cur_chunk;
        }
    }
    for (u32 cur_chunk=max_num_high; cur_chunk<num_pq_chunks; cur_chunk++) {
        u32 start_d = high_val*max_num_high + low_val*(cur_chunk-max_num_high);
        for (u32 d=start_d; d<start_d + low_val; d++) {
            bin_to_dims[cur_chunk].push_back(d);
            dim_to_bin[d] = cur_chunk;
        }
    }
    
    // rearrangement: the dimensions in the order of chunks
    // chunk_offsets: the starting index of each chunk in rearrangement
    chunk_offsets.push_back(0);
    for (u32 b = 0; b < num_pq_chunks; b++) {
        for (auto p : bin_to_dims[b]) {
            rearrangement.push_back(p);
        }
        if (b > 0)
            chunk_offsets.push_back(chunk_offsets[b - 1] + (unsigned)bin_to_dims[b - 1].size());
    }
    chunk_offsets.push_back(dim);


    // k-means clustering for each chunk
    full_pivot_data = MakeShared<f32[]>(num_centers * dim);
    for (u32 i = 0; i < num_pq_chunks; i++) { // for each chunk
        SizeT cur_chunk_size = chunk_offsets[i + 1] - chunk_offsets[i];

        if (cur_chunk_size == 0)
            continue;
        
        UniquePtr<f32[]> cur_pivot_data = MakeUnique<f32[]>(num_centers * cur_chunk_size);
        UniquePtr<f32[]> cur_data = MakeUnique<f32[]>(num_train * cur_chunk_size);
        UniquePtr<u32[]> closest_center = MakeUnique<u32[]>(num_train);
        for (u32 j = 0; j < num_train; j++) {
            std::memcpy(cur_data.get() + j * cur_chunk_size,
                  train_data.get() + j * dim + chunk_offsets[i],
                  cur_chunk_size * sizeof(f32));
        }

        // kmeans++ to select start pivots
        KmeansppSelectingPivots(cur_data.get(), num_train, cur_chunk_size,
                                      cur_pivot_data.get(), num_centers);
        // run lloyd's algorithm when using kmeans++ pivots as initial centroids 
        RunLloyds(cur_data.get(), num_train, cur_chunk_size, cur_pivot_data.get(), 
                       num_centers, max_k_means_reps, closest_center.get());

        for (u64 j = 0; j < num_centers; j++) {
            std::memcpy(full_pivot_data.get() + j * dim + chunk_offsets[i],
                        cur_pivot_data.get() + j * cur_chunk_size,
                        cur_chunk_size * sizeof(f32));
        }
    }

    // Write meta_data(first sector) followed by [file_size, full_pivot_data, centroid, chunk_offsets]
    UniquePtr<SizeT[]> meta_sector = MakeUnique<SizeT[]>(DISKANN_SECTOR_LEN / sizeof(SizeT));
    meta_sector[0] = DISKANN_SECTOR_LEN;
    meta_sector[1] = sizeof(f32) * num_centers * dim + meta_sector[0]; // full_pivot_data end offset
    meta_sector[2] = sizeof(f32) * dim + meta_sector[1]; // centroid end offset
    meta_sector[3] = sizeof(u32) * chunk_offsets.size() + meta_sector[2]; // chunk_offsets end offset
    pq_pivot_file_handler.Write(meta_sector.get(), DISKANN_SECTOR_LEN);
    pq_pivot_file_handler.Write(full_pivot_data.get(), sizeof(f32) * num_centers * dim);
    pq_pivot_file_handler.Write(centroid.get(), sizeof(f32) * dim);
    pq_pivot_file_handler.Write(chunk_offsets.data(), sizeof(u32) * chunk_offsets.size());
}

export template <typename VectorDataType> 
void GeneratePqDataFromPivots(FileHandler &data_file, FileHandler &pqCompressed_data_file, 
                            SizeT num_points, u32 dim, u32 num_centers,
                            u32 num_pq_chunks, SharedPtr<f32[]> &full_pivot_data,
                            SharedPtr<f32[]> &centroid, Vector<u32> &rearrangement,
                            Vector<u32> &chunk_offsets) {
    const SizeT BLOCK_SIZE = 5000000; // Chunking Processing Block Size
    SizeT block_size = num_points <= BLOCK_SIZE ? num_points : BLOCK_SIZE;

    UniquePtr<u32[]> block_compressed_base = MakeUnique<u32[]>(block_size * num_pq_chunks);
    memset(block_compressed_base.get(), 0, sizeof(u32) * block_size * num_pq_chunks);

    UniquePtr<VectorDataType[]> block_data_T = MakeUnique<VectorDataType[]>(block_size * dim);
    UniquePtr<f32[]> block_data_float = MakeUnique<f32[]>(block_size * dim);
    UniquePtr<f32[]> block_data_tmp = MakeUnique<f32[]>(block_size * dim);

    SizeT num_blocks = num_points / block_size + (num_points % block_size == 0 ? 0 : 1);
    // process block by block
    for (size_t block = 0; block < num_blocks; block++) {
        size_t start_id = block * block_size;
        size_t end_id = (std::min)((block + 1) * block_size, num_points);
        size_t cur_blk_size = end_id - start_id;

        data_file.Read(block_data_T.get(), cur_blk_size * dim * sizeof(VectorDataType));
        ConvertTypes<VectorDataType, f32>(block_data_T.get(), block_data_float.get(), cur_blk_size, dim);

        // mean centering
        for (SizeT p = 0; p < cur_blk_size; p++) {
            for (SizeT d = 0; d < dim; d++) {
                block_data_tmp[p * dim + d] -= centroid[d];
            }
        }
        // rearrange the data
        for (SizeT p = 0; p < cur_blk_size; p++) {
            for (SizeT d = 0; d < dim; d++) {
                block_data_float[p * dim + d] = block_data_tmp[p * dim + rearrangement[d]];
            }
        }

        // process chunk by chunk in a block
        for (SizeT i = 0; i < num_pq_chunks; i++) {
            SizeT cur_chunk_size = chunk_offsets[i + 1] - chunk_offsets[i];
            if (cur_chunk_size == 0)
                continue;
            UniquePtr<f32[]> cur_pivot_data = MakeUnique<f32[]>(num_centers * cur_chunk_size);
            UniquePtr<f32[]> cur_data = MakeUnique<f32[]>(cur_blk_size * cur_chunk_size);
            UniquePtr<u32[]> closest_center = MakeUnique<u32[]>(cur_blk_size);

            for (SizeT j = 0; j < cur_blk_size; j++) {
                for (SizeT k = 0; k < cur_chunk_size; k++)
                    cur_data[j * cur_chunk_size + k] = block_data_float[j * dim + chunk_offsets[i] + k];
            }

            for (SizeT j = 0; j < num_centers; j++) {
                memcpy(cur_pivot_data.get() + j * cur_chunk_size,
                        full_pivot_data.get() + j * dim + chunk_offsets[i],
                        cur_chunk_size * sizeof(f32));
            }

            ComputeClosestCenters(cur_data.get(), cur_blk_size, cur_chunk_size, cur_pivot_data.get(),
                                    num_centers, closest_center.get());

            for (SizeT j = 0; j < cur_blk_size; j++) {
                block_compressed_base[j * num_pq_chunks + i] = closest_center[j];
            }       
        }
        // write pq Compressed data file
        pqCompressed_data_file.Write(block_compressed_base.get(), cur_blk_size * num_pq_chunks * sizeof(u32));
    }

}


// Run Lloyd's algorithm to find the best centers store in closest_center
f32 RunLloyds(f32 *data, u32 num_points, u32 dim, f32 *centers,
                 const u32 num_centers, const u32 max_reps, u32 *closest_center) {
    f32 residual = std::numeric_limits<f32>::max();
    // UniquePtr<f32[]> docs_l2sq = MakeUnique<f32[]>(num_points);
    
    // for (u32 i = 0; i < num_points; i++) {
    //     docs_l2sq[i] = ComputL2sq<f32, u32>(docs_l2sq+i, data + i * dim, dim);
    // }

    // run lloyd's algorithm inner loop
    f32 old_residual;
    UniquePtr<Vector<SizeT>[]> closest_docs = MakeUnique<Vector<SizeT>[]>(num_centers);
    for (SizeT i = 0; i < max_reps; ++i) {
        old_residual = residual;
        residual = LloydsIter(data, num_points, dim, centers, num_centers,
                              closest_docs.get(), closest_center);
        // check for convergence
        if (((i != 0) && ((old_residual - residual) / residual) < 0.00001) ||
            (residual < std::numeric_limits<f32>::epsilon())) {
            break;
        }
    }

    return residual;
}


// Run one iteration of Lloyd's algorithm, return the residual 
f32 LloydsIter(f32 *data, SizeT num_points, SizeT dim, f32 *centers,
                  SizeT num_centers, Vector<SizeT> *closest_docs,
                  u32 *&closest_center) {
    assert(closest_center != nullptr);
    assert(closest_docs != nullptr);
    bool compute_residual = true;

    // compute  the closest centers
    ComputeClosestCenters(data, num_points, dim, centers, num_centers, 
                          closest_center, closest_docs);
    
    memset(centers, 0, sizeof(f32) * (SizeT)num_centers * (SizeT)dim);

    for (SizeT c = 0; c < num_centers; ++c) {
        f32 *center = centers + (SizeT)c * (SizeT)dim;
        UniquePtr <f64[]> cluster_sum = MakeUnique<f64[]>(dim);
        for (SizeT i = 0; i < dim; i++)
            cluster_sum[i] = 0.0;
        for (SizeT i = 0; i < closest_docs[c].size(); i++) {
            f32 *current = data + ((closest_docs[c][i]) * dim);
            for (SizeT j = 0; j < dim; j++) {
                cluster_sum[j] += (f64)current[j];
            }
        }
        if (closest_docs[c].size() > 0) {
            for (SizeT i = 0; i < dim; i++)
                center[i] = (f32)(cluster_sum[i] / ((f64)closest_docs[c].size()));
        }
    }

    f32 residual = 0.0;
    if (compute_residual) {
        SizeT BUF_PAD = 32;
        SizeT CHUNK_SIZE = 2 * 8192;
        SizeT nchunks =
            num_points / CHUNK_SIZE + (num_points % CHUNK_SIZE == 0 ? 0 : 1);
        Vector<f32> residuals(nchunks * BUF_PAD, 0.0);

        for (SizeT chunk = 0; chunk < nchunks; ++chunk)
        for (SizeT d = chunk * CHUNK_SIZE;
            d < num_points && d < (chunk + 1) * CHUNK_SIZE; ++d)
            residuals[chunk * BUF_PAD] += L2Distance<f32, f32, f32>(
                data + (d * dim), centers + (SizeT)closest_center[d] * (SizeT)dim, dim);

        for (SizeT chunk = 0; chunk < nchunks; ++chunk)
        residual += residuals[chunk * BUF_PAD];
    }

    return residual;

}


// Select pivots using kmeans++ to initialize the centroids
void KmeansppSelectingPivots(f32 *data, u32 num_points, u32 dim,
                               f32 *pivot_data, u32 num_centers) {
    assert(num_points < 1 << 23); // maximum is 8388608
    Vector<u32> picked;
    std::random_device rd;
    auto x = rd();
    std::mt19937 generator(x);
    std::uniform_real_distribution<> distribution(0, 1);
    std::uniform_int_distribution<u32> int_dist(0, num_points - 1);
    u32 init_id = int_dist(generator);
    u32 num_picked = 1;

    picked.push_back(init_id); // pick the first pivot randomly
    std::memcpy(pivot_data, data + init_id * dim, dim * sizeof(f32));

    UniquePtr<f32[]> dist = MakeUnique<f32[]>(num_points);
    for (u32 i = 0; i < num_points; i++) {
        dist[i] = L2Distance<f32, f32, f32>(data + i * dim, data + init_id * dim, dim);
    }

    f64 dart_val;
    u32 tmp_pivot;
    bool sum_flag = false;

    // select left num_centers-1 pivots
    while (num_picked < num_centers) {
        dart_val = distribution(generator);
        double sum = 0;
        for (SizeT i = 0; i < num_points; i++) {
            sum = sum + dist[i];
        }
        if (sum == 0)
            sum_flag = true;
        dart_val *= sum;

        f64 prefix_sum = 0;
        for (u32 i = 0; i < num_points; i++) {
            tmp_pivot = i;
            if (dart_val >= prefix_sum && dart_val < prefix_sum + dist[i]) {
                break;
            }
            prefix_sum += dist[i];
        }

        if (find(picked.begin(), picked.end(), tmp_pivot) != picked.end() && (sum_flag == false))
            continue;
        picked.push_back(tmp_pivot);
        memcpy(pivot_data + num_picked * dim, data + tmp_pivot * dim, dim * sizeof(f32));

        // update dist from new pivot
        for (u32 i = 0; i < num_points; i++) {
            dist[i] = (std::min)(dist[i], 
                                L2Distance<f32, f32, f32>(data + i * dim, data + tmp_pivot * dim, dim));
        }
        num_picked++;
    }
}

template <typename InType, typename OutType>
void ConvertTypes(const InType *srcmat, OutType *destmat, size_t npts,
                   size_t dim) {
    if (std::is_same<InType, OutType>::value) {
        return;
    }
    for (u64 i = 0; i < npts; i++) {
        for (u64 j = 0; j < dim; j++) {
            destmat[i * dim + j] = (OutType)srcmat[i * dim + j];
        }
    }
}

} // namespace infinity