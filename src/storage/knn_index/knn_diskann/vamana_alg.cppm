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

export module vamana_alg;

import stl;
import logger;
import index_base;
import local_file_handle;
import file_system_type;
import infinity_exception;
import knn_result_handler;
import logical_type;
import third_party;
import diskann_mem_data_store;
import diskann_mem_graph_store;
import diskann_dist_func;
import diskann_utils;
import default_values;

namespace infinity {

export template <typename VectorDataType, typename LabelType, MetricType metric>
class MemVamana {
public:
    using This = MemVamana<VectorDataType, LabelType, metric>;
    using InMemQueryScratch = InMemQueryScratch<VectorDataType>;

    MemVamana(SizeT L, SizeT R, SizeT dim, SizeT point_num) : L_(L), R_(R), dim_(dim), nd_(point_num) {
        max_points_ = point_num;
        locks_ = Vector<std::mutex>(point_num);

        DiskAnnMetricType dist_metric;
        if constexpr (metric == MetricType::kMetricCosine) {
            dist_metric = DiskAnnMetricType::Cosine;
        } else if constexpr (metric == MetricType::kMetricL2) {
            dist_metric = DiskAnnMetricType::L2;
        } else if constexpr (metric == MetricType::kMetricInnerProduct) {
            dist_metric = DiskAnnMetricType::IP;
        } else {
            UnrecoverableError("vamana_alg::MemVamana(): Unknown Metric type!");
        }

        if (dynamic_index_ && num_frozen_pts_ == 0) { // init medoid as first frozen point
            num_frozen_pts_ = 1;
        }

        data_store_ = DiskAnnMemDataStore<VectorDataType>::Make(point_num, dim, dist_metric);
        graph_store_ = DiskAnnMemGraphStore::Make(point_num, R);
    }

public:
    MemVamana() : L_(0), R_(0) {}
    MemVamana(This &&other)
        : L_(std::exchange(other.L_, 0)), R_(std::exchange(other.R_, 0)), dim_(std::exchange(other.dim_, 0)), nd_(std::exchange(other.nd_, 0)) {}
    This &operator=(This &&other) {
        if (this != &other) {
            this->max_points_ = std::exchange(other.max_points_, 0);
            this->locks_ = std::move(other.locks_);
            this->data_store_ = std::move(other.data_store_);
            this->graph_store_ = std::move(other.graph_store_);
        }
        return *this;
    }
    ~MemVamana() = default;

    static This Make(SizeT L, SizeT R, SizeT dim, SizeT point_num) {
        This ret(L, R, dim, point_num);
        return ret;
    }

    SizeT GetSizeInBytes() const {}

    void Save(LocalFileHandle &file_handle) {
        // graph_store_.Save(file_handler, nd_ + num_frozen_pts_, num_frozen_pts_, start_);
        // data_store_.Save(file_handler);
        // SaveLabels(file_handler);
    }

    void SaveGraph(LocalFileHandle &file_handle) { graph_store_.Save(file_handle, nd_ + num_frozen_pts_, num_frozen_pts_, start_); }

    static UniquePtr<This> Load(LocalFileHandle &file_handle) {}

    void Build(LocalFileHandle &data_file_handle, SizeT num_points, Vector<LabelType> &labels) {
        // https://github.com/microsoft/DiskANN/issues/53
        // graph initialization not be necessary if you shuffle the data before building it
        // graph_store_.InitRandomKnn(num_points);

        // load data
        this->nd_ = num_points;
        data_store_.PopulateData(data_file_handle, num_points);

        // build graph
        BuildWithDataPopulated(labels);
    }

private:
    void BuildWithDataPopulated(Vector<LabelType> &labels) {
        // load labels
        if (labels.size() != nd_) {
            UnrecoverableError("vamana_alg::Build(): Wrong number of labels!");
        }
        for (SizeT i = 0; i < nd_; i++) {
            tag_to_location_[labels[i]] = i;
            location_to_tag_[i] = labels[i];
        }

        if (query_scratch_.Size() == 0) {
            u32 num_threads = 1; // single thread for now
            InitializeQueryScratch(num_threads + 5, L_, L_, R_, maxc_, data_store_.GetAlignedDim());
        }

        Link(); // build vamana graph in 2 rounds

        has_built_ = true;
    }

    void Link() {
        Vector<SizeT> visit_order;
        // Vector<Neighbor> pool, tmp;
        // std::unordered_set<SizeT> visited;
        visit_order.resize(nd_ + num_frozen_pts_);
        std::iota(visit_order.begin(), visit_order.begin() + nd_, 0);
        if (num_frozen_pts_ > 0) {
            std::iota(visit_order.begin() + nd_, visit_order.end(), max_points_);
        }

        if (num_frozen_pts_ > 0) {
            LOG_TRACE(fmt::format("init node start_ = {}", start_));
            start_ = max_points_; // start from the first frozen point
        } else {
            start_ = CalculateEntryPoint(); // calculate medoid as enterpoint
        }

        for (SizeT node_ctr = 0; node_ctr < visit_order.size(); node_ctr++) {
            SizeT node = visit_order[node_ctr];
            // Avoid the case where the first Prune node is exactly start_ resulting in a pruned_list of none
            if (node == start_ && node_ctr == 0) {
                continue;
            }
            // LOG_TRACE(fmt::format("start_: {}, Link node: {}",start_, node));
            ScratchStoreManager<InMemQueryScratch> manager(query_scratch_); // get new scratch
            auto scratch = manager.ScratchSpace();
            Vector<SizeT> pruned_list;
            SearchForPointAndPrune(node, L_, pruned_list, scratch);
            assert(pruned_list.size() > 0);
            {
                std::lock_guard<std::mutex> guard(locks_[node]);
                graph_store_.SetNeighbours(node, pruned_list);
                assert(graph_store_.GetNeighbours(node).size() <= R_);
            }

            // add the reverse edge of the nodes in pruned_list to node
            InterInsert(node, pruned_list, scratch);
        }

        // start final cleanup
        for (SizeT node_ctr = 0; node_ctr < visit_order.size(); node_ctr++) {
            auto node = visit_order[node_ctr];
            if (graph_store_.GetNeighbours(node).size() > R_) {
                ScratchStoreManager<InMemQueryScratch> manager(query_scratch_);
                auto scratch = manager.ScratchSpace();

                std::unordered_set<SizeT> dummy_visited;
                Vector<Neighbor> dummy_pool(0);
                Vector<SizeT> new_out_neighbors;

                for (auto cur_nbr : graph_store_.GetNeighbours(node)) {
                    if (dummy_visited.find(cur_nbr) == dummy_visited.end() && cur_nbr != node) {
                        f32 dist = data_store_.GetDistance(node, cur_nbr);
                        dummy_pool.emplace_back(Neighbor(cur_nbr, dist));
                        dummy_visited.insert(cur_nbr);
                    }
                }
                PruneNeighbors(node, dummy_pool, new_out_neighbors, scratch);

                graph_store_.ClearNeighbours(node);
                graph_store_.SetNeighbours(node, new_out_neighbors);
            }
        }
    }

private:
    void InitializeQueryScratch(u32 num_threads, u32 search_l, u32 indexing_l, u32 r, u32 maxc, u32 dim) {
        for (u32 i = 0; i < num_threads; i++) {
            auto scratch = new InMemQueryScratch(search_l, indexing_l, r, maxc, dim, data_store_.GetAlignedDim(), data_store_.GetAlignmentFactor());
            query_scratch_.Push(scratch);
        }
    }

    SizeT CalculateEntryPoint() {
        // calculate global centroid
        return data_store_.CalculateMedoid();
    }

    void SearchForPointAndPrune(SizeT location, u32 Lindex, Vector<SizeT> &pruned_list, InMemQueryScratch *scratch) {
        Vector<SizeT> init_ids = GetInitIds();
        data_store_.GetVector(location, scratch->AlignedQuery());
        IterateToFixedPoint(scratch, Lindex, init_ids);

        auto &pool = scratch->Pool();             // pool is the expanded list of IterateToFixedPoint()
        for (SizeT i = 0; i < pool.size(); i++) { // remove self from pool
            if (pool[i].id == location) {
                pool.erase(pool.begin() + i);
                LOG_TRACE(fmt::format("SearchForPointAndPrune(): remove self from pool: {}", location));
                i--;
            }
        }

        assert(pool.size() > 0);
        assert(pruned_list.size() == 0);
        PruneNeighbors(location, pool, pruned_list, scratch);
        assert(!pruned_list.empty());
        assert(graph_store_.GetTotalPoints() == max_points_ + num_frozen_pts_);
    }

    // used in SearchForPointAndPrune()
    // abtain search path of location starting from init_ids
    Pair<u32, u32> IterateToFixedPoint(InMemQueryScratch *scratch, const u32 Lsize, Vector<SizeT> &init_ids, bool search_invocations = false) {
        Vector<Neighbor> &expanded_nodes = scratch->Pool();
        NeighborPriorityQueue &best_l_nodes = scratch->BestLNodes();
        best_l_nodes.Reserve(Lsize);
        std::unordered_set<u32> &inserted_into_pool_rs = scratch->InsertedIntoPoolRs();
        boost::dynamic_bitset<> &inserted_into_pool_bs = scratch->InsertedIntoPoolBs();
        Vector<SizeT> &id_scratch = scratch->IdScratch();
        Vector<f32> &dist_scratch = scratch->DistScratch();
        assert(id_scratch.size() == 0);

        VectorDataType *aligned_query = scratch->AlignedQuery();
        data_store_.PreprocessQuery(aligned_query, scratch);
        assert(expanded_nodes.size() == 0 && id_scratch.size() == 0);

        SizeT total_num_points = max_points_ + num_frozen_pts_;
        const SizeT MAX_POINTS_FOR_USING_BITSET = 10000000;                    // 10M points for using bitset
        bool fast_iterate = (total_num_points <= MAX_POINTS_FOR_USING_BITSET); // turn on fast iterate for small datasets(1e7)

        if (fast_iterate) {
            if (inserted_into_pool_bs.size() < total_num_points) {
                SizeT resize_size = 2 * total_num_points > MAX_POINTS_FOR_USING_BITSET ? MAX_POINTS_FOR_USING_BITSET : 2 * total_num_points;
                inserted_into_pool_bs.resize(resize_size);
            }
        }

        auto is_not_visited = [fast_iterate, &inserted_into_pool_bs, &inserted_into_pool_rs](const u32 id) {
            return fast_iterate ? inserted_into_pool_bs[id] == 0 : inserted_into_pool_rs.find(id) == inserted_into_pool_rs.end();
        };

        auto compute_dists = [this, scratch](Vector<SizeT> &ids, Vector<f32> &dists_out) {
            data_store_.GetDistance(scratch->AlignedQuery(), ids, dists_out, scratch);
        };

        // add initial nodes to the best candiates queue
        for (auto id : init_ids) {
            assert(id < max_points_ + num_frozen_pts_);

            if (is_not_visited(id)) {
                if (fast_iterate) {
                    inserted_into_pool_bs[id] = 1;
                } else {
                    inserted_into_pool_rs.insert(id);
                }

                f32 distance;
                SizeT ids[] = {(SizeT)id};
                f32 distances[] = {std::numeric_limits<f32>::max()};
                data_store_.GetDistance(aligned_query, ids, 1, distances, scratch);
                distance = distances[0];
                Neighbor nn = Neighbor(id, distance);
                best_l_nodes.Insert(nn);
            }
        }

        u32 hops = 0; // number of hops in the graph
        u32 cmps = 0; // number of comparisons

        // Gready search
        while (best_l_nodes.HasUnexpandedNode()) {
            auto nbr = best_l_nodes.ClosestUnexpanded();
            auto n = nbr.id;
            // std::cout<<"closest unexpanded node: "<<n<<std::endl;

            if (!search_invocations) {
                expanded_nodes.emplace_back(nbr);
            }

            id_scratch.clear();
            dist_scratch.clear();
            {
                locks_[n].lock();
                auto nbrs = graph_store_.GetNeighbours(n);
                locks_[n].unlock();
                for (auto id : nbrs) {
                    // std::cout<<"expanded node"<<n<<"'s neighbor: "<<id<<std::endl;
                    assert(id < max_points_ + num_frozen_pts_);
                    if (is_not_visited(id)) {
                        id_scratch.push_back(id); // add neighbors of expanded node
                    }
                }
            }

            for (auto id : id_scratch) {
                if (fast_iterate) {
                    inserted_into_pool_bs[id] = 1;
                } else {
                    inserted_into_pool_rs.insert(id);
                }
            }

            assert(dist_scratch.capacity() >= id_scratch.size());
            compute_dists(id_scratch, dist_scratch);
            cmps += id_scratch.size();

            for (SizeT i = 0; i < id_scratch.size(); i++) {
                best_l_nodes.Insert(Neighbor(id_scratch[i], dist_scratch[i]));
            }
        }
        assert(best_l_nodes.Size() != 0);

        return std::make_pair(hops, cmps);
    }

    Vector<SizeT> GetInitIds() {
        Vector<SizeT> init_ids;
        init_ids.reserve(1 + num_frozen_pts_);

        init_ids.emplace_back(start_);
        for (SizeT i = 0; i < num_frozen_pts_; i++) {
            if (i != start_)
                init_ids.emplace_back(max_points_ + i);
        }

        return init_ids;
    }

    void PruneNeighbors(SizeT location, Vector<Neighbor> &pool, Vector<SizeT> &pruned_list, InMemQueryScratch *scratch) {
        SizeT range = this->R_;
        SizeT max_candidate_size = this->maxc_;
        f32 alpha = this->alpha_;

        if (pool.size() == 0) {
            pruned_list.clear();
            return;
        }

        std::sort(pool.begin(), pool.end());
        pruned_list.clear();
        pruned_list.reserve(range);

        OccludeList(location, pool, alpha, range, max_candidate_size, pruned_list, scratch);
        assert(pruned_list.size() <= range);

        // saturate graph: sample extra points from the pool to reach the desired degree
        if (saturate_graph_ && alpha > 1) {
            for (const auto &node : pool) {
                if (pruned_list.size() >= range) {
                    break;
                }
                if ((std::find(pruned_list.begin(), pruned_list.end(), node.id) == pruned_list.end()) && (node.id != location)) {
                    pruned_list.push_back(node.id);
                }
            }
        }
    }

    void OccludeList(SizeT location,
                     Vector<Neighbor> &pool,
                     f32 alpha,
                     SizeT degree,
                     SizeT maxc,
                     Vector<SizeT> &result,
                     InMemQueryScratch *scratch,
                     std::unordered_set<SizeT> *delete_set_ptr = nullptr) {
        if (pool.size() == 0)
            return;

        assert(std::is_sorted(pool.begin(), pool.end()));
        assert(result.size() == 0);
        if (pool.size() > maxc) {
            pool.resize(maxc);
        }
        Vector<f32> &occlude_factor = scratch->OccludeFactor();
        occlude_factor.clear();
        occlude_factor.insert(occlude_factor.end(), pool.size(), 0.0f);

        f32 cur_alpha = 1;
        while (cur_alpha <= alpha && result.size() < degree) {
            f32 eps = cur_alpha + 0.01f;

            for (auto iter = pool.begin(); result.size() < degree && iter != pool.end(); iter++) {
                if (occlude_factor[iter - pool.begin()] > cur_alpha) {
                    continue;
                }
                occlude_factor[iter - pool.begin()] = std::numeric_limits<f32>::max();
                if (delete_set_ptr == nullptr || delete_set_ptr->find(iter->id) == delete_set_ptr->end()) {
                    if (iter->id != location) {
                        result.push_back(iter->id);
                    }
                }

                // Update occlude factor for points from iter+1 to pool.end()
                for (auto iter2 = iter + 1; iter2 != pool.end(); iter2++) {
                    auto t = iter2 - pool.begin();
                    if (occlude_factor[t] > alpha) {
                        continue;
                    }

                    // bool prune_allowed = true;
                    f32 djk = data_store_.GetDistance(iter2->id, iter->id);
                    if constexpr (metric == MetricType::kMetricCosine || metric == MetricType::kMetricL2) {
                        occlude_factor[t] = (djk == 0) ? std::numeric_limits<float>::max() : std::max(occlude_factor[t], iter2->distance / djk);
                    } else if constexpr (metric == MetricType::kMetricInnerProduct) {
                        f32 x = -(iter2->distance);
                        f32 y = -djk;
                        if (y > cur_alpha * x) {
                            occlude_factor[t] = std::max(occlude_factor[t], eps);
                        }
                    } else {
                        UnrecoverableError("vamana_alg::OccludeList(): Unknown Metric type!");
                    }
                }
            }

            cur_alpha *= 1.2f;
        }
    }

    void InterInsert(SizeT n, Vector<SizeT> &pruned_list, InMemQueryScratch *scratch) {
        const SizeT range = this->R_;
        const auto &src_pool = pruned_list;
        assert(!src_pool.empty());

        for (auto des : src_pool) {
            assert(des < max_points_ + num_frozen_pts_);
            Vector<SizeT> copy_of_neighbors;
            bool prune_needed = false;
            {
                std::lock_guard<std::mutex> guard(locks_[des]);
                auto &des_pool = graph_store_.GetNeighbours(des);
                if (std::find(des_pool.begin(), des_pool.end(), n) == des_pool.end()) {
                    if (des_pool.size() < range * DISKANN_GRAPH_SLACK_FACTOR) {
                        graph_store_.AddNeighbour(des, n);
                        prune_needed = false;
                    } else {
                        copy_of_neighbors.reserve(des_pool.size() + 1);
                        copy_of_neighbors = des_pool;
                        copy_of_neighbors.push_back(n);
                        prune_needed = true;
                    }
                }
            }

            if (prune_needed) {
                std::unordered_set<SizeT> dummy_visited;
                Vector<Neighbor> dummy_pool(0);

                SizeT reserveSize = (SizeT)(std::ceil(1.05 * DISKANN_GRAPH_SLACK_FACTOR * range));
                dummy_visited.reserve(reserveSize);
                dummy_pool.reserve(reserveSize);

                for (auto cur_nbr : copy_of_neighbors) {
                    if (dummy_visited.find(cur_nbr) == dummy_visited.end() && cur_nbr != des) {
                        f32 dist = data_store_.GetDistance(des, cur_nbr);
                        dummy_pool.emplace_back(Neighbor(cur_nbr, dist));
                        dummy_visited.insert(cur_nbr);
                    }
                }

                Vector<SizeT> new_out_neighbors;
                PruneNeighbors(des, dummy_pool, new_out_neighbors, scratch);
                {
                    std::lock_guard<std::mutex> guard(locks_[des]);
                    graph_store_.SetNeighbours(des, new_out_neighbors);
                }
            }
        }
    }

private:
    SizeT L_; // candidates list size when building
    SizeT R_; // max degree of a vertex

    // indexing parameters
    SizeT maxc_ = 750; // Maximum number of candidate neighbours to be considered
    f32 alpha_ = 1.2f; // alpha in RobustPrune
    ConcurrentQueue<InMemQueryScratch *> query_scratch_;
    u32 indexingThreads_ = 1; // number of threads for indexing
    // SizeT num_rnds_ = 2; // number of rounds in index building

    bool saturate_graph_ = true; // whether to saturate the graph, is true when use_filters is false
    bool has_built_ = false;
    bool dynamic_index_ = false;

    // VamanaStore vamana_store_;
    SizeT dim_;
    SizeT nd_;         // number of data points
    SizeT max_points_; // maximum number of data points
    // SizeT aligned_dim_; // in data_store_

    UniquePtr<f32[]> centroid_; // global centroid data to calculate enterpoint
    SizeT start_;               // enterpoint
    HashMap<LabelType, SizeT> tag_to_location_;
    HashMap<SizeT, LabelType> location_to_tag_;
    Vector<std::mutex> locks_; // Per node lock

    DiskAnnMemDataStore<VectorDataType> data_store_;
    DiskAnnMemGraphStore graph_store_;

    SizeT num_frozen_pts_ = 0; // number of frozen points
    // bool dynamic_index_ = false;
    // bool filtered_index = false;
};

} // namespace infinity