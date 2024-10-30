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
#include <ostream>
#include <random>
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module pq_flash_index;

import stl;
import logger;
import index_base;
import local_file_handle;
import status;
import infinity_exception;
import logical_type;
import third_party;
import diskann_dist_func;
import diskann_utils;
import default_values;
import diskann_partition_and_pq;
import virtual_store;

namespace infinity {

export template <typename VectorDataType, typename LabelType>
class PqFlashIndex {
    using This = PqFlashIndex<VectorDataType, LabelType>;

public:
    PqFlashIndex(UniquePtr<AlignedFileReader> &fileReader, DiskAnnMetricType metric, u64 data_dim, u64 num_points, u64 num_pq_chunks)
        : data_dim_(data_dim), num_points_(num_points), n_chunks_(num_pq_chunks), metric_(metric), reader_(std::move(fileReader)) {
        this->aligned_dim_ = RoundUp(data_dim, 8);
        dist_cmp_ = MakeShared<DiskAnnDistance<VectorDataType>>(metric);
        dist_cmp_float_ = MakeShared<DiskAnnDistance<f32>>(metric);
        pq_table_ = MakeUnique<FixedChunkPQTable>(data_dim, n_chunks_);
    }

    PqFlashIndex(This &&other)
        : data_dim_(other.data_dim_), num_points_(other.num_points_), n_chunks_(other.n_chunks_), metric_(other.metric_), reader_(other.reader_) {
        this->aligned_dim_ = other.aligned_dim_;
        this->dist_cmp_ = std::move(other.dist_cmp_);
        this->dist_cmp_float_ = std::move(other.dist_cmp_float_);
        this->pq_table_ = std::move(other.pq_table_);
    }

    ~PqFlashIndex() {
        if (this->load_flag_) {
            this->reader_->Close();
            ScratchStoreManager<SsdQueryScratch<VectorDataType>> manager(this->ssd_query_data_);
            manager.Destroy();
        }
    }

    static UniquePtr<This> Make(DiskAnnMetricType metric, u64 data_dim, u64 num_points, u64 num_pq_chunks) {
        auto fileReader = AlignedFileReader::Make();
        return MakeUnique<This>(fileReader, metric, data_dim, num_points, num_pq_chunks);
    }

    // First step
    int Load(std::string index_prefix, u32 num_threads = 1) {
        std::string pq_table_bin = index_prefix + "/pq_pivot.bin";
        std::string pq_compressed_vectors = index_prefix + "/pqCompressed_data.bin";
        std::string disk_index_file = index_prefix + "/index.bin";
        this->disk_index_file_ = disk_index_file;

        // 1. load pq compressed vector
        LoadPqCompressedVec(pq_compressed_vectors);

        // 2. load PQ table
        pq_table_->LoadPqCentroidBin(pq_table_bin, n_chunks_);

        // 3. load disk index meta data
        LoadDiskIndexMetaData(disk_index_file);

        // 4. open reader for the disk index file
        reader_->Open(disk_index_file);
        this->max_nthreads_ = num_threads;
        this->SetupThreadData(num_threads);

        // 5. load medoids node ids as enter point has finished in LoadDiskIndexMetaData()

        // 6. load medoids data as centroids
        this->UseMedoidsDataAsCentroids();

        this->load_flag_ = true;
        return 0;
    }

    // Second step
    // nodes ids to cache in the bfs level order which starting from the medoids node
    void CacheBfsLevels(u64 num_nodes_to_cache, Vector<SizeT> &node_list, const bool shuffle = false) {
        std::random_device rng;
        std::mt19937 urng(rng());
        std::unordered_set<SizeT> node_set;

        u64 tenp_nodes = (u64)(std::round(this->num_points_ * 0.1));
        if (num_nodes_to_cache > tenp_nodes) {
            Status status = Status::NotSupport("DiskAnn(): num_nodes_to_cache should be less than 10 percent of num_points");
            RecoverableError(status);
        }

        UniquePtr<std::unordered_set<SizeT>> cur_level, prev_level;
        cur_level = MakeUnique<std::unordered_set<SizeT>>();
        prev_level = MakeUnique<std::unordered_set<SizeT>>();

        for (u64 miter = 0; miter < this->num_medoids_ && cur_level->size() < num_nodes_to_cache; miter++) {
            cur_level->insert(this->medoids_[miter]);
        }

        // filter support, not used for now
        if ((filter_to_medoid_ids_.size() > 0) && (cur_level->size() < num_nodes_to_cache)) {
            for (auto &x : filter_to_medoid_ids_) {
                for (auto &y : x.second) {
                    cur_level->insert(y);
                    if (cur_level->size() == num_nodes_to_cache)
                        break;
                }
                if (cur_level->size() == num_nodes_to_cache)
                    break;
            }
        }

        u64 lvl = 1;
        while ((node_set.size() + cur_level->size() < num_nodes_to_cache) && cur_level->size() != 0) {
            std::swap(cur_level, prev_level);
            cur_level->clear();

            Vector<SizeT> nodes_to_expand; // nodes to expand in this level

            for (const SizeT &id : *prev_level) {
                if (node_set.find(id) != node_set.end()) {
                    continue;
                }
                node_set.insert(id);
                nodes_to_expand.push_back(id);
            }
            LOG_DEBUG(fmt::format("CacheBfsLevels(): Expand {} nodes in level {}", nodes_to_expand.size(), lvl));
            if (shuffle) {
                std::shuffle(nodes_to_expand.begin(), nodes_to_expand.end(), urng);
            } else {
                std::sort(nodes_to_expand.begin(), nodes_to_expand.end());
            }

            // read expanded nodes data in this level
            bool finish_flag = false;
            SizeT BLOCK_SIZE = 1024;
            SizeT nblocks = DivRoundUp(nodes_to_expand.size(), BLOCK_SIZE);
            for (SizeT block = 0; block < nblocks && !finish_flag; block++) {
                SizeT start = block * BLOCK_SIZE;
                SizeT end = std::min((block + 1) * BLOCK_SIZE, nodes_to_expand.size());

                Vector<SizeT> nodes_to_read;
                Vector<VectorDataType *> coord_buffers(end - start, nullptr); // nullptr for not reading coord in ReadNodes()
                Vector<Pair<u32, SizeT *>> nbr_buffers;
                for (SizeT cur_pt = start; cur_pt < end; cur_pt++) {
                    nodes_to_read.push_back(nodes_to_expand[cur_pt]);
                    nbr_buffers.emplace_back(MakePair(0, new SizeT[max_degree_ + 1]));
                }

                auto read_status = ReadNodes(nodes_to_read, coord_buffers, nbr_buffers);
                // process nhood buffer of each readed expanded node
                for (u32 i = 0; i < read_status.size(); i++) {
                    if (read_status[i] == false) {
                        continue;
                    } else {
                        u32 nnbrs = nbr_buffers[i].first;
                        SizeT *nbrs = nbr_buffers[i].second;
                        for (u32 j = 0; j < nnbrs && !finish_flag; j++) {
                            if (node_set.find(nbrs[j]) == node_set.end()) {
                                cur_level->insert(nbrs[j]);
                            }
                            if (cur_level->size() + node_set.size() >= num_nodes_to_cache) {
                                finish_flag = true;
                            }
                        }
                    }
                    delete[] nbr_buffers[i].second;
                }
            }
            lvl++;
        }

        // check if all nodes are cached or cur_level has no unexpaned nodes
        assert(node_set.size() + cur_level->size() == num_nodes_to_cache || cur_level->size() == 0);
        node_list.clear();
        node_list.reserve(node_set.size() + cur_level->size());
        for (auto node : node_set) {
            node_list.push_back(node);
        }
        for (auto node : *cur_level) {
            node_list.push_back(node);
        }
        LOG_DEBUG(fmt::format("CacheBfsLevels(): Finish cached {} nodes in BFS {} level order", node_list.size(), lvl));
    }

    // Third step
    // load cache list from disk index
    void LoadCacheList(Vector<SizeT> &node_list) {
        SizeT num_cached_nodes = node_list.size();

        // nhood cache buffer
        nhood_cache_buf_ = MakeUnique<SizeT[]>(num_cached_nodes * (max_degree_ + 1));
        memset(nhood_cache_buf_.get(), 0, sizeof(SizeT) * num_cached_nodes * (max_degree_ + 1));

        // coord cache buffer
        SizeT coord_cache_buf_len = num_cached_nodes * aligned_dim_;
        char *tmp_coord_cache_ptr = nullptr;
        AllocAligned((void **)&tmp_coord_cache_ptr, coord_cache_buf_len * sizeof(VectorDataType), 8 * sizeof(VectorDataType));
        this->coord_cache_buf_.reset(reinterpret_cast<VectorDataType *>(tmp_coord_cache_ptr));
        memset(coord_cache_buf_.get(), 0, coord_cache_buf_len * sizeof(VectorDataType));

        // process block by block for caching
        SizeT BLOCK_SIZE = 8;
        SizeT num_blocks = DivRoundUp(num_cached_nodes, BLOCK_SIZE);
        for (SizeT block = 0; block < num_blocks; block++) {
            SizeT start_idx = block * BLOCK_SIZE;
            SizeT end_idx = std::min((block + 1) * BLOCK_SIZE, num_cached_nodes);

            Vector<SizeT> nodes_to_read;
            Vector<VectorDataType *> coord_buffers;
            Vector<Pair<u32, SizeT *>> nbr_buffers;
            for (SizeT node_idx = start_idx; node_idx < end_idx; node_idx++) {
                nodes_to_read.push_back(node_list[node_idx]);
                coord_buffers.push_back(coord_cache_buf_.get() + node_idx * aligned_dim_);
                nbr_buffers.emplace_back(0, nhood_cache_buf_.get() + node_idx * (max_degree_ + 1));
            }

            auto read_status = ReadNodes(nodes_to_read, coord_buffers, nbr_buffers);

            for (SizeT i = 0; i < read_status.size(); i++) {
                if (read_status[i] == true) {
                    coord_cache_.insert(std::make_pair(nodes_to_read[i], coord_buffers[i]));
                    nhood_cache_.insert(std::make_pair(nodes_to_read[i], nbr_buffers[i])); // use MakePair compiler error
                }
            }
        }
    }

    // Fourth step
    void CachedBeamSearch(const VectorDataType *query1,
                          const u64 k_search,
                          const u64 l_search,
                          u64 *indices,
                          f32 *distances,
                          const u64 beam_width,
                          const bool use_filter = false,
                          const LabelType &filter_labels = 0,
                          const u32 io_limit = std::numeric_limits<u32>::max(),
                          const bool use_reorder_data = false,
                          QueryStats *stats = nullptr) {
        if (!this->load_flag_) {
            Status status = Status::NotSupport("DiskAnn(): index not loaded");
            RecoverableError(status);
        }

        ScratchStoreManager<SsdQueryScratch<VectorDataType>> manager(this->ssd_query_data_);
        auto query_scratch = manager.ScratchSpace();
        auto pq_query_scratch = query_scratch->PqScratch();

        query_scratch->Reset();

        f32 query_norm = 0;
        VectorDataType *aligned_query_T = query_scratch->AlignedQueryT();
        f32 *query_float = pq_query_scratch->aligned_query_float_;
        f32 *query_rotated = pq_query_scratch->rotated_query_;

        // normalize query
        // cosine: normalize the query
        // IP: normalize the first d-1 dims, and add a 0 for last dim, convert MIPS to L2 search
        if (metric_ == DiskAnnMetricType::IP || metric_ == DiskAnnMetricType::Cosine) {
            u64 inherent_dim = (metric_ == DiskAnnMetricType::Cosine) ? this->data_dim_ : this->data_dim_ - 1;
            for (u64 i = 0; i < inherent_dim; i++) {
                aligned_query_T[i] = query1[i];
                query_norm += query1[i] * query1[i];
            }
            if (metric_ == DiskAnnMetricType::IP) {
                aligned_query_T[this->data_dim_ - 1] = 0;
            }
            query_norm = std::sqrt(query_norm);

            for (u64 i = 0; i < inherent_dim; i++) {
                aligned_query_T[i] = (VectorDataType)(aligned_query_T[i] / query_norm);
            }
            pq_query_scratch->Initialize(this->data_dim_, aligned_query_T);
        } else { // L2: do nothing
            for (u64 i = 0; i < this->data_dim_; i++) {
                aligned_query_T[i] = query1[i];
            }
            pq_query_scratch->Initialize(this->data_dim_, aligned_query_T);
        }

        // pointers to buffers for data
        VectorDataType *data_buf = query_scratch->coord_scratch_;
        _mm_prefetch((char *)(data_buf), _MM_HINT_T1);

        // sector scratch
        char *sector_scratch = query_scratch->sector_scratch_;
        u64 &sector_scratch_idx = query_scratch->sector_idx_;
        u64 num_sector_per_node = nnodes_per_sector_ > 0 ? 1 : DivRoundUp(max_node_len_, DISKANN_SECTOR_LEN);

        // query <-> PQ chunk centers distances
        pq_table_->PreprocessQuery(query_rotated);
        f32 *pq_dists =
            pq_query_scratch->aligned_pqtable_dist_scratch_;        // pq_dists stores query to each PQ chunk centers distances (n_chunks * n_centers)
        pq_table_->PopulateChunkDistances(query_rotated, pq_dists); // populate pq_dists

        // query <-> neighbor list
        f32 *dist_scratch = pq_query_scratch->aligned_dist_scratch_;
        u8 *pq_coord_scratch = pq_query_scratch->aligned_pq_coord_scratch_;

        // lamda to batch compute query<-> node distances in PQ space
        auto compute_dists = [this, pq_coord_scratch, pq_dists](const SizeT *ids, const SizeT n_ids, f32 *dists_out) {
            AggregateCoords(ids,
                            n_ids,
                            this->data_.get(),
                            this->n_chunks_,
                            pq_coord_scratch); // aggregate pq compressed coords of ids into pq_coord_scratch
            PqDistLookup(pq_coord_scratch, n_ids, this->n_chunks_, pq_dists, dists_out); // lookup distances in pq_dists
        };

        // bind query scratch data
        std::unordered_set<SizeT> &visited = query_scratch->visited_;
        NeighborPriorityQueue &retset = query_scratch->retset_; // priority queue for beam search, store pq distances to query
        retset.Reserve(l_search);
        Vector<Neighbor> &full_retset = query_scratch->full_retset_; // store all expanded nodes' full precision distances to query

        // compute best medoid as the enter point of beam search
        SizeT best_medoid = 0;
        f32 best_dist = std::numeric_limits<f32>::max();
        if (!use_filter) {
            for (u64 cur_m = 0; cur_m < this->num_medoids_; cur_m++) {
                f32 cur_expanded_dist = dist_cmp_float_->Compare(query_float, centroid_data_.get() + cur_m * aligned_dim_, (u32)this->data_dim_);
                if (cur_expanded_dist < best_dist) {
                    best_medoid = medoids_[cur_m];
                    best_dist = cur_expanded_dist;
                }
            }
        } else {
            Status status = Status::NotSupport("DiskAnn(): search with filter is not supported");
            RecoverableError(status);
        }

        // searching start from the best medoid
        compute_dists(&best_medoid, (SizeT)1, dist_scratch);
        retset.Insert(Neighbor(best_medoid, dist_scratch[0]));
        visited.insert(best_medoid);
        u32 cmps = 0;
        u32 hops = 0;
        u32 num_ios = 0;

        Vector<SizeT> frontier; // expanding nodes not found in cache in one iteration, needs to read from disk
        frontier.push_back(2 * beam_width);
        Vector<Pair<SizeT, char *>> frontier_nhoods; // store ptr in sector_scratch of frontier nodes' nhoods info read from disk
        frontier_nhoods.reserve(2 * beam_width);
        Vector<AlignedRead> frontier_read_reqs;
        frontier_read_reqs.reserve(2 * beam_width);
        Vector<Pair<SizeT, Pair<u32, SizeT *>>> cached_nhoods; // cached id and neighbors while searching, [node_id, (num_nbrs, nbrs*)]
        cached_nhoods.reserve(2 * beam_width);

        while (retset.HasUnexpandedNode() && num_ios < io_limit) {
            // clear iterative state
            frontier.clear();
            frontier_nhoods.clear();
            frontier_read_reqs.clear();
            cached_nhoods.clear();
            sector_scratch_idx = 0;

            // 1. find beam_width top unexpanded nodes from retset
            u32 num_seen = 0;
            while (retset.HasUnexpandedNode() && frontier.size() < beam_width && num_seen < beam_width) {
                // expand beam_width nodes within one iteration
                auto nbr = retset.ClosestUnexpanded();
                num_seen++;
                // find ClosestUnexpanded node in cache
                auto iter = nhood_cache_.find(nbr.id);
                if (iter != nhood_cache_.end()) { // found in cache, push to cached_nhoods
                    cached_nhoods.push_back(std::make_pair(nbr.id, iter->second));
                    if (stats != nullptr) {
                        stats->n_cache_hits_++;
                    }
                } else { // not found in cache, push to frontier
                    frontier.push_back(nbr.id);
                }
            }

            //  2. process not cached nodes, read data and nhoods of frontier
            if (!frontier.empty()) {
                if (stats != nullptr) {
                    stats->n_hops_++;
                }
                for (u64 i = 0; i < frontier.size(); i++) {
                    auto id = frontier[i];
                    Pair<SizeT, char *> fnhood;
                    fnhood.first = id;
                    fnhood.second = sector_scratch + num_sector_per_node * sector_scratch_idx * DISKANN_SECTOR_LEN; // read buf
                    sector_scratch_idx++;
                    frontier_nhoods.push_back(fnhood);
                    frontier_read_reqs.emplace_back(GetNodeSector(id) * DISKANN_SECTOR_LEN, num_sector_per_node * DISKANN_SECTOR_LEN, fnhood.second);
                    if (stats != nullptr) {
                        stats->n_4k_++;
                        stats->n_ios_++;
                    }
                    num_ios++;
                }
                reader_->Read(frontier_read_reqs);
            }

            // 3. process cached nodes' nhoods
            for (auto &cached_nhood : cached_nhoods) {
                auto global_cache_iter = coord_cache_.find(cached_nhood.first); // get coord data of cached node
                VectorDataType *node_fp_coords_copy = global_cache_iter->second;
                f32 cur_expanded_dist;
                cur_expanded_dist = dist_cmp_->Compare(aligned_query_T, node_fp_coords_copy, (u32)this->data_dim_);
                full_retset.push_back(Neighbor(cached_nhood.first, cur_expanded_dist));

                u64 nnbrs = cached_nhood.second.first;
                SizeT *node_nbrs = cached_nhood.second.second;
                compute_dists(node_nbrs, nnbrs, dist_scratch); // compute cached node nbrs' pq distances to query
                if (stats != nullptr) {
                    stats->n_cmps_ += nnbrs;
                }

                // add nbrs and their pq distances to retset
                for (u64 m = 0; m < nnbrs; m++) {
                    u32 id = node_nbrs[m];
                    if (visited.insert(id).second) {
                        // filter
                        if (!use_filter && this->dummy_pts_.find(id) != this->dummy_pts_.end()) {
                            continue;
                        }
                        if (use_filter) {
                            Status status = Status::NotSupport("DiskAnn(): search with filter is not supported");
                            RecoverableError(status);
                        }

                        cmps++;
                        f32 dist = dist_scratch[m];
                        Neighbor nn(id, dist);
                        retset.Insert(nn);
                    }
                }
            }

            // 4. process frontier nodes' nhoods which have been readed from disk
            for (auto &frontier_nhood : frontier_nhoods) {
                char *node_disk_buf = OffsetToNode(frontier_nhood.second, frontier_nhood.first);
                u32 *node_buf = OffsetToNodeNhood(node_disk_buf);
                u64 nnbrs = (u64)(*node_buf);
                VectorDataType *node_fp_coords = OffsetToNodeCoords(node_disk_buf);
                memcpy(data_buf, node_fp_coords, disk_bytes_per_point_);
                f32 cur_expanded_dist;
                cur_expanded_dist = dist_cmp_->Compare(aligned_query_T, data_buf, (u32)this->aligned_dim_);

                full_retset.push_back(Neighbor(frontier_nhood.first, cur_expanded_dist));
                SizeT *node_nbrs = (SizeT *)(node_buf + 1);
                // compute node_nbrs <-> query dist in PQ space
                compute_dists(node_nbrs, nnbrs, dist_scratch);
                if (stats != nullptr) {
                    stats->n_cmps_ += nnbrs;
                }

                // process prefetched nhood
                for (u64 m = 0; m < nnbrs; m++) {
                    SizeT id = node_nbrs[m];
                    if (visited.insert(id).second) { // if not visited, insert successfullly
                        // filter
                        if (!use_filter && this->dummy_pts_.find(id) != this->dummy_pts_.end()) {
                            continue;
                        }
                        if (use_filter) {
                            Status status = Status::NotSupport("DiskAnn(): search with filter is not supported");
                            RecoverableError(status);
                        }

                        cmps++;
                        f32 dist = dist_scratch[m];
                        if (stats != nullptr) {
                            stats->n_cmps_++;
                        }
                        Neighbor nn(id, dist);
                        retset.Insert(nn);
                    }
                }
            }

            hops++;
        } // beam search end

        // LOG_DEBUG(fmt::format("Beam search hops {}: {} nodes expanded, {} cmps,  {} ios", hops, full_retset.size(), cmps, num_ios));
        // copy the top k results to the output buffer
        std::sort(full_retset.begin(), full_retset.end());
        for (u64 i = 0; i < k_search; i++) {
            indices[i] = full_retset[i].id;
            auto key = indices[i];
            // filter
            if (dummy_pts_.find(key) != dummy_pts_.end()) {
                indices[i] = this->dummy_to_real_map_[key];
            }

            if (distances != nullptr) {
                distances[i] = full_retset[i].distance;
                if (metric_ == DiskAnnMetricType::IP) {
                    // flip the sign to convert min to max
                    distances[i] = (-distances[i]);
                }
            }
        }

        // delete[] data;
    }

private:
    // read pq compressed vectors from disk to this->data_
    void LoadPqCompressedVec(std::string pq_compressed_vectors_path) {
        this->data_ = MakeUnique<u8[]>(this->num_points_ * this->n_chunks_);
        auto read_buf = MakeUnique<u32[]>(this->num_points_ * this->n_chunks_ * sizeof(u32));
        auto [pq_data_handle, status] = VirtualStore::Open(pq_compressed_vectors_path, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        pq_data_handle->Read(read_buf.get(), this->num_points_ * this->n_chunks_ * sizeof(u32));
        for (u64 i = 0; i < this->num_points_ * this->n_chunks_; i++) {
            this->data_[i] = static_cast<u8>(read_buf[i]);
        }
    }

    // read the data of the medoid nodes
    void UseMedoidsDataAsCentroids() {
        if (this->centroid_data_ != nullptr) {
            AlignedFree((void *)this->centroid_data_.get());
        }
        char *tmp_centroid_data_ptr = nullptr;
        AllocAligned((void **)&tmp_centroid_data_ptr, sizeof(f32) * this->aligned_dim_ * this->num_medoids_, 32);
        this->centroid_data_.reset(reinterpret_cast<f32 *>(tmp_centroid_data_ptr));
        memset(this->centroid_data_.get(), 0, sizeof(f32) * this->aligned_dim_ * this->num_medoids_);

        Vector<SizeT> nodes_to_read;          // node ids to read
        Vector<VectorDataType *> medoid_bufs; // store the data of the medoid nodes
        Vector<Pair<u32, SizeT *>> nbr_bufs;  // node's id and its neighbors' ids
        for (u64 i = 0; i < this->num_medoids_; i++) {
            nodes_to_read.push_back(this->medoids_[i]);
            medoid_bufs.push_back(new VectorDataType[this->data_dim_]);
            nbr_bufs.push_back(std::make_pair(0, nullptr)); // (0, nullptr) for not used in ReadNodes()
        }

        auto read_status = ReadNodes(nodes_to_read, medoid_bufs, nbr_bufs);

        for (u64 cur_m = 0; cur_m < this->num_medoids_; cur_m++) {
            if (read_status[cur_m]) {
                for (u32 i = 0; i < data_dim_; i++) {
                    centroid_data_[cur_m * aligned_dim_ + i] = medoid_bufs[cur_m][i];
                }
            } else {
                Status status = Status::IOError("DiskAnn(): read_status from ReadNodes() is false");
                RecoverableError(status);
            }
            delete[] medoid_bufs[cur_m];
        }
    }

    // process a batch of read requests of nodes data in disk index file
    // return a vector of bool indicating whether each node is read successfully or not
    Vector<bool> ReadNodes(const Vector<SizeT> &node_ids, Vector<VectorDataType *> &coord_buffers, Vector<Pair<u32, SizeT *>> &nbr_buffers) {
        Vector<AlignedRead> read_reqs;
        Vector<bool> retval(node_ids.size(), true);

        std::unique_ptr<char[], decltype([](char *p) { std::free(p); })> buf =
            nullptr; // the whole buffer of all read requests aligned to DISKANN_SECTOR_LEN
        auto num_sectors = nnodes_per_sector_ > 0 ? 1 : DivRoundUp(max_node_len_, DISKANN_SECTOR_LEN); // sector num to read for one node
        char *tmp_buf_ptr = nullptr;
        AllocAligned((void **)&tmp_buf_ptr, node_ids.size() * num_sectors * DISKANN_SECTOR_LEN, DISKANN_SECTOR_LEN);
        buf.reset(tmp_buf_ptr);

        for (SizeT i = 0; i < node_ids.size(); i++) {
            auto node_id = node_ids[i];

            AlignedRead read;
            read.len = num_sectors * DISKANN_SECTOR_LEN;
            read.buf = buf.get() + i * num_sectors * DISKANN_SECTOR_LEN;
            read.offset = GetNodeSector(node_id) * DISKANN_SECTOR_LEN;
            read_reqs.push_back(read);
        }

        reader_->Read(read_reqs);
        // reader reads all sectors of node in disk index, so it needs to extract node data
        for (u32 i = 0; i < read_reqs.size(); i++) {
            char *node_buf = OffsetToNode((char *)read_reqs[i].buf, node_ids[i]);
            // extract node data
            if (coord_buffers[i] != nullptr) { // if coord_buffers[i] is not used, it is nullptr
                VectorDataType *node_coords = OffsetToNodeCoords(node_buf);
                memcpy(coord_buffers[i], node_coords, disk_bytes_per_point_);
            }
            // extract node neighbors
            if (nbr_buffers[i].second != nullptr) { // if nbr_buffers[i] is not used, it is (0, nullptr)
                u32 *node_nhood = OffsetToNodeNhood(node_buf);
                u32 num_nbrs = *node_nhood; // the first u32 is the number of neighbors
                nbr_buffers[i].first = num_nbrs;
                memcpy(nbr_buffers[i].second, node_nhood + 1, num_nbrs * sizeof(SizeT));
            }
        }

        return retval;
    }

    // get the sector NO of a node in the disk index file
    inline SizeT GetNodeSector(SizeT node_id) {
        // +1 for the meta data in the first sector
        return 1 + (nnodes_per_sector_ > 0 ? node_id / nnodes_per_sector_ : node_id * DivRoundUp(max_node_len_, DISKANN_SECTOR_LEN));
    }

    // get the offset of a node in its start sector buffer
    inline char *OffsetToNode(char *sector_buf, SizeT node_id) {
        return sector_buf + (nnodes_per_sector_ == 0 ? 0 : (node_id % nnodes_per_sector_) * max_node_len_);
    }

    // locate the offset of the vector data from the start sector buffer(i.e. start of the buffer)
    inline VectorDataType *OffsetToNodeCoords(char *node_buf) { return (VectorDataType *)(node_buf); }

    // locate the offset of the neighbor data from the start sector buffer(i.e. skip the vector data)
    inline u32 *OffsetToNodeNhood(char *node_buf) { return (u32 *)(node_buf + disk_bytes_per_point_); }

    void LoadDiskIndexMetaData(const std::string &disk_index_file) {
        auto [index_file_handle, status] = VirtualStore::Open(disk_index_file, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        u64 disk_nnodes, disk_ndims;
        index_file_handle->Read(&disk_nnodes, sizeof(u64));
        index_file_handle->Read(&disk_ndims, sizeof(u64));
        disk_bytes_per_point_ = disk_ndims * sizeof(VectorDataType);
        if (disk_nnodes != num_points_ || disk_ndims != data_dim_) {
            UnrecoverableError("Index file does not match the PQ flash index");
        }

        u64 medoid_id_on_file; // medoid node id
        index_file_handle->Read(&medoid_id_on_file, sizeof(u64));
        this->num_medoids_ = 1;
        this->medoids_ = MakeUnique<SizeT[]>(1);
        this->medoids_[0] = medoid_id_on_file;
        LOG_DEBUG(fmt::format("LoadDiskIndexMetaData(): Loaded medoid node id: {}", medoid_id_on_file));

        index_file_handle->Read(&max_node_len_, sizeof(u64));
        index_file_handle->Read(&nnodes_per_sector_, sizeof(u64));
        index_file_handle->Read(&sector_num_, sizeof(u64));
        max_degree_ = (max_node_len_ - disk_bytes_per_point_) / sizeof(u64) - 1; // -1 for the neighbor count

        index_file_handle->Read(&num_frozen_points_, sizeof(u64));
        u64 file_frozen_id;
        index_file_handle->Read(&file_frozen_id, sizeof(u64));
        if (num_frozen_points_ == 1) {
            frozen_location_ = file_frozen_id;
        }
        index_file_handle->Read(&reorder_data_exists_, sizeof(u64));

        index_file_handle->Read(&disk_index_size_, sizeof(u64));
    }

    void SetupThreadData(u64 nthreads, u64 visited_reserve = 4096, bool async = false) {
        if (async == true) {
            // needs ssd search scratch and async read ctx
            Status status = Status::NotSupport("DiskAnn(): async read not supported");
            RecoverableError(status);
        } else {
            // only ssd search scratch, exclude async read ctx
            for (u64 thread = 0; thread < nthreads; thread++) {
                SsdQueryScratch<VectorDataType> *scratch = new SsdQueryScratch<VectorDataType>(this->aligned_dim_, visited_reserve);
                this->ssd_query_data_.Push(scratch);
            }
        }
    }

private:
    u64 data_dim_;
    u64 aligned_dim_;
    u64 disk_bytes_per_point_;    // bytes per vector in disk index
    u64 num_points_;              // number of points in the dataset
    std::string disk_index_file_; // path to disk index file
    bool load_flag_ = false;      // whether the index is loaded or not

    // PQ data
    UniquePtr<u8[]> data_;                  // npts * [u8 * n_chunks_], store pq compressed vector
    u64 n_chunks_;                          // num of pq chunks
    UniquePtr<FixedChunkPQTable> pq_table_; // store the PQ table

    u64 max_node_len_;      // max bytes of a node in the disk index
    u64 nnodes_per_sector_; // 0 for multi-sector nodes, >0 for multi-node sectors
    u64 max_degree_;        // max degree of a node in the disk index
    u64 sector_num_;        // num of sectors in the disk index
    u64 disk_index_size_;   // bytes of the disk index file

    UniquePtr<SizeT[]> medoids_;                                                   // one entry point by default
    SizeT num_medoids_;                                                            // defaults to 1
    std::unique_ptr<f32[], decltype([](f32 *p) { std::free(p); })> centroid_data_; // centroid data for the medoids node

    u64 num_frozen_points_; // number of frozen points in the disk index
    u64 frozen_location_;   // location of the frozen points in the disk index
    u64 reorder_data_exists_;

    // thread data
    u64 max_nthreads_; // max number of threads to use
    // ConcurrentQueue<SSDThreadData<VectorDataType> *> thread_data_; // thread data for async read and ssd search scratch, async read not used now
    ConcurrentQueue<SsdQueryScratch<VectorDataType> *> ssd_query_data_; // only ssd search scratch for now

    // nhood cache
    UniquePtr<SizeT[]> nhood_cache_buf_; // cache buffer for all cached node's nhood
    std::unordered_map<SizeT, Pair<u32, SizeT *>>
        nhood_cache_; // each node's ptr in nhood_cache_buf_, u32 is the number of neighbors, SizeT * is the ptr to the neighbors
    // coord cache
    std::unique_ptr<VectorDataType[], decltype([](VectorDataType *p) { std::free(p); })>
        coord_cache_buf_;                                     // cache buffer for all cached node's coord data
    std::unordered_map<SizeT, VectorDataType *> coord_cache_; // each node's ptr in coord_cache_buf_

    DiskAnnMetricType metric_;
    UniquePtr<AlignedFileReader> reader_; // reader for the disk index file

    SharedPtr<DiskAnnDistance<VectorDataType>> dist_cmp_;
    SharedPtr<DiskAnnDistance<f32>> dist_cmp_float_;

    // filter support
    std::unordered_map<LabelType, Vector<SizeT>> filter_to_medoid_ids_;
    std::unordered_set<SizeT> dummy_pts_;                // dummy points for filter
    std::unordered_map<SizeT, SizeT> dummy_to_real_map_; // map the dummy point to its real point
};

} // namespace infinity