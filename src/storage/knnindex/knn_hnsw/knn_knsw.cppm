module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

import stl;
import parser;
import knn_distance;
import dist_func;

export module knn_knsw;

namespace infinity {

export template <typename DataType>
class KnnHnsw {
    using LabelType = i32;

    using VertexType = i32;

    using PDV = Pair<DataType, VertexType>;

    struct CompareByFirst {
        constexpr bool operator()(const PDV &lhs, const PDV &rhs) const { return lhs.first < rhs.first; }
    };

    using DistHeap = Heap<PDV, CompareByFirst>;

    using DistTypeLabel = MaxHeap<Pair<DataType, LabelType>>;

    struct Vertex {
        Vector<DataType> data_;
        Vector<Vector<VertexType>> neighbor_layers_;
        LabelType label_; // rename
    };

private:
    const SizeT M_;
    const SizeT Mmax_;
    const SizeT Mmax0_;
    SizeT ef_;
    const SizeT ef_construction_;
    const SizeT dim_;
    const DistFunc<DataType> dist_func_{};

    i32 max_layer_{};
    VertexType enterpoint_{};

    Vector<Vertex> graph_{};

    // 1 / log(1.0 * M_)
    const double mult_;
    std::default_random_engine level_rng_{};

private:
    LabelType GetLabel(VertexType vertex_idx) const { return graph_[vertex_idx].label_; }

    const DataType *GetData(VertexType vertex_idx) const { return graph_[vertex_idx].data_.data(); }

    const Vector<VertexType> &GetNeighborsByLayer(VertexType vertex_idx, i32 layer_idx) const {
        return graph_[vertex_idx].neighbor_layers_[layer_idx];
    }

    Vector<VertexType> &GetNeighborsByLayerMut(VertexType vertex_idx, i32 layer_idx) { return graph_[vertex_idx].neighbor_layers_[layer_idx]; }

    void SetVertex(VertexType vertex_idx, i32 layer_idx, const DataType *data, LabelType label) {
        Vertex vertex;
        graph_[vertex_idx].data_.assign(data, data + dim_);
        graph_[vertex_idx].label_ = label;
        graph_[vertex_idx].neighbor_layers_.resize(layer_idx + 1);
        for (auto &neighbors : graph_[vertex_idx].neighbor_layers_) {
            neighbors.reserve(Mmax_ + 1);
        }
    }

private:
    // >= 0
    i32 GenerateRandomLayer() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double r = -log(distribution(level_rng_)) * mult_;
        return static_cast<i32>(r);
    }

    VertexType GenerateNewVertexIdx() {
        SizeT current_vertex_idx = graph_.size();
        graph_.emplace_back();
        return current_vertex_idx;
    }

    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    // return value is a max heap of distance
    DistHeap SearchLayer(VertexType enter_point, const DataType *query, i32 layer_idx, SizeT candidate_n) const {
        Vector<bool> visited(graph_.size(), false);
        DistHeap result;
        DistHeap candidate;

        DataType dist = dist_func_(query, GetData(enter_point), dim_);

        candidate.emplace(-dist, enter_point);
        result.emplace(dist, enter_point);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (-minus_c_dist > result.top().first && result.size() == candidate_n) {
                break;
            }
            for (const auto &n_idx : GetNeighborsByLayer(c_idx, layer_idx)) {
                if (visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                dist = dist_func_(query, GetData(n_idx), dim_);
                if (dist < result.top().first || result.size() < candidate_n) {
                    candidate.emplace(-dist, n_idx);
                    result.emplace(dist, n_idx);
                    if (result.size() > candidate_n) {
                        result.pop();
                    }
                }
            }
        }
        return result;
    }

    VertexType SearchLayerNearest(VertexType enter_point, const DataType *query, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        DataType cur_dist = dist_func_(query, GetData(enter_point), dim_);
        bool check = true;
        while (check) {
            check = false;
            const auto &neighbors = GetNeighborsByLayer(cur_p, layer_idx);
            for (const auto &n_idx : neighbors) {
                DataType n_dist = dist_func_(query, GetData(n_idx), dim_);
                if (n_dist < cur_dist) {
                    cur_p = n_idx;
                    cur_dist = n_dist;
                    check = true;
                }
            }
        }
        return cur_p;
    }

    // DistHeap is the min heap whose key is the minus distance to query
    // result distance is increasing **NOW**
    Vector<VertexType> SelectNeighborsHeuristic(DistHeap &candidates, SizeT M, bool extend_candidates = false, bool keey_pruned_candidates = false) {
        Vector<VertexType> result;
        if (SizeT c_size = candidates.size(); c_size < M) {
            result.resize(c_size);
            SizeT i = c_size - 1;
            while (!candidates.empty()) {
                result[i--] = candidates.top().second;
                candidates.pop();
            }
            return result;
        }
        while (!candidates.empty() && result.size() < M) {
            const auto [minus_c_dist, c_idx] = candidates.top();
            candidates.pop();
            const DataType *c_data = GetData(c_idx);
            bool check = true;
            for (VertexType r_idx : result) {
                DataType cr_dist = dist_func_(c_data, GetData(r_idx), dim_);
                if (cr_dist < -minus_c_dist) {
                    check = false;
                    break;
                }
            }
            if (check) {
                result.emplace_back(c_idx);
            }
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    void ConnectNeighbors(VertexType vertex_idx, Vector<VertexType> neighbors, i32 layer_idx) {
        auto &q_neighbors = GetNeighborsByLayerMut(vertex_idx, layer_idx);
        q_neighbors = Move(neighbors);
        for (VertexType n_idx : q_neighbors) {
            auto &n_neighbors = GetNeighborsByLayerMut(n_idx, layer_idx);
            n_neighbors.emplace_back(vertex_idx);
            SizeT Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
            if (n_neighbors.size() > Mmax) {
                DistHeap candidates;
                for (const auto &nn_idx : n_neighbors) {
                    DataType nn_dist = dist_func_(GetData(nn_idx), GetData(n_idx), dim_);
                    candidates.emplace(-nn_dist, nn_idx);
                }
                Vector<VertexType> shrink_neighbors = SelectNeighborsHeuristic(candidates, Mmax);
                n_neighbors = Move(shrink_neighbors);
            }
        }
    }

public:
    template <typename SpaceType>
        requires SpaceConcept<DataType, SpaceType>
    KnnHnsw(SpaceType space, SizeT M = 16, SizeT ef_construction = 200, SizeT random_seed = 100)
        : M_(M), Mmax_(M_), Mmax0_(2 * Mmax_), ef_(10), ef_construction_(Max(M_, ef_construction)), dim_(space.Dimension()),
          dist_func_(space.template DistFuncPtr<DataType>()), max_layer_(i64_max), enterpoint_(-1), mult_(1 / log(1.0 * M_)) {
        level_rng_.seed(random_seed);
    }

    void Insert(const DataType *query, LabelType label) {
        VertexType q_vertex = GenerateNewVertexIdx();
        i32 q_layer = GenerateRandomLayer();
        SetVertex(q_vertex, q_layer, query, label);
        VertexType ep = enterpoint_;
        if (max_layer_ != -1) {
            for (i32 cur_layer = max_layer_; cur_layer > q_layer; --cur_layer) {
                ep = SearchLayerNearest(ep, query, cur_layer);
            }
            for (i32 cur_layer = Min(q_layer, max_layer_); cur_layer >= 0; --cur_layer) {
                DistHeap search_result = SearchLayer(ep, query, cur_layer, ef_construction_);
                DistHeap candidates;
                while (!search_result.empty()) {
                    const auto &[dist, idx] = search_result.top();
                    candidates.emplace(-dist, idx);
                    search_result.pop();
                }
                Vector<VertexType> neighbors = SelectNeighborsHeuristic(candidates, M_);
                ep = neighbors.back();
                ConnectNeighbors(q_vertex, Move(neighbors), cur_layer);
            }
        }
        if (q_layer > max_layer_) {
            max_layer_ = q_layer;
            enterpoint_ = q_vertex;
        }
    }

    DistTypeLabel KnnSearch(const DataType *query, SizeT k) {
        VertexType ep = enterpoint_;
        for (i32 cur_layer = max_layer_; cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest(ep, query, cur_layer);
        }
        DistHeap search_result = SearchLayer(ep, query, 0, Max(k, ef_));
        while (search_result.size() > k) {
            search_result.pop();
        }
        DistTypeLabel result;
        while (!search_result.empty()) {
            const auto &[dist, idx] = search_result.top();
            result.emplace(dist, GetLabel(idx));
            search_result.pop();
        }
        return result;
    }

    void SetEf(SizeT ef) { ef_ = ef; }

    // Following is the tmp debug function.

    void DumpGraph(const String filename) {
        std::fstream fo(filename, std::ios::out);
        fo << std::endl << "---------------------------------------------" << std::endl;
        fo << "M_: " << M_ << std::endl;
        fo << "Mmax_: " << Mmax_ << std::endl;
        fo << "Mmax0_: " << Mmax0_ << std::endl;
        fo << "ef_: " << ef_ << std::endl;
        fo << "ef_construction_: " << ef_construction_ << std::endl;
        fo << "dim_: " << dim_ << std::endl;
        fo << std::endl;

        fo << "current element number: " << graph_.size() << std::endl;
        fo << "max layer: " << max_layer_ << std::endl;
        fo << std::endl;

        Vector<Vector<VertexType>> layer2vertex(max_layer_ + 1);
        for (VertexType v = 0; v < graph_.size(); ++v) {
            const Vertex &vertex = graph_[v];
            i32 v_max_layer = vertex.neighbor_layers_.size();
            for (i32 layer = 0; layer < v_max_layer; ++layer) {
                layer2vertex[layer].emplace_back(v);
            }
        }

        for (i32 layer = 0; layer <= max_layer_; ++layer) {
            fo << "layer " << layer << std::endl;
            const auto &vertex_idxes = layer2vertex[layer];
            for (VertexType v : vertex_idxes) {
                fo << v << ": ";
                const Vertex &vertex = graph_[v];
                for (VertexType neighbor : vertex.neighbor_layers_[layer]) {
                    fo << neighbor << ", ";
                }
                fo << std::endl;
            }
        }
        fo << "---------------------------------------------" << std::endl;
    }

    void TmpSave(const String &file_name) {
        std::fstream fo(file_name, std::ios::out);
        fo << max_layer_ << std::endl;
        fo << enterpoint_ << std::endl;
        fo << graph_.size() << std::endl;
        for (const auto &vertex : graph_) {
            fo << vertex.label_ << std::endl;
            for (const auto &data : vertex.data_) {
                fo << data << " ";
            }
            fo << vertex.neighbor_layers_.size() << std::endl;
            for (const auto &layer : vertex.neighbor_layers_) {
                fo << layer.size() << std::endl;
                for (const auto &neighbor : layer) {
                    fo << neighbor << " ";
                }
                fo << std::endl;
            }
            fo << std::endl;
        }
    }

    template <typename SpaceType>
        requires SpaceConcept<DataType, SpaceType>
    KnnHnsw(SpaceType space, const String &file_name)
        : M_(16), Mmax_(M_), Mmax0_(2 * Mmax_), ef_(10), ef_construction_(200), dim_(space.Dimension()),
          dist_func_(space.template DistFuncPtr<DataType>()), mult_(1 / log(1.0 * M_)) {
        level_rng_.seed(100);
        std::fstream fo(file_name, std::ios::in);
        fo >> max_layer_;
        fo >> enterpoint_;
        int vertex_num = -1;
        fo >> vertex_num;
        graph_.resize(vertex_num);
        for (int i = 0; i < vertex_num; i++) {
            fo >> graph_[i].label_;
            graph_[i].data_.resize(dim_);
            for (int j = 0; j < dim_; j++) {
                fo >> graph_[i].data_[j];
            }
            int layer_num = -1;
            fo >> layer_num;
            graph_[i].neighbor_layers_.resize(layer_num);
            for (int j = 0; j < layer_num; j++) {
                int neighbor_num = -1;
                fo >> neighbor_num;
                graph_[i].neighbor_layers_[j].resize(neighbor_num);
                for (int k = 0; k < neighbor_num; k++) {
                    fo >> graph_[i].neighbor_layers_[j][k];
                }
            }
        }
    }
};
} // namespace infinity