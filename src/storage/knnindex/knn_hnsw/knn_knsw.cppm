module;

import std;
import stl;
import parser;
import knn_distance;
import dist_func;

export module knn_knsw;

namespace infinity {

export template <typename DataType>
class KnnHnsw {
    static SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

    using LabelType = u32;

    using VertexType = i32;

    using PDV = Pair<DataType, VertexType>;

    struct CompareByFirst {
        constexpr bool operator()(const PDV &lhs, const PDV &rhs) const { return lhs.first < rhs.first; }
    };

    using DistHeap = Heap<PDV, CompareByFirst>;

    using DistTypeLabel = MaxHeap<Pair<DataType, LabelType>>;

    using VertexListSize = i32;
    using LayerSize = i32;

private:
    const SizeT max_vertex_;
    const SizeT M_;
    const SizeT Mmax_;
    const SizeT Mmax0_;
    SizeT ef_;
    const SizeT ef_construction_;
    const SizeT dim_;
    const DistFunc<DataType> dist_func_{};

    SizeT cur_vertex_n_{};
    i32 max_layer_{};
    VertexType enterpoint_{};

    // 1 / log(1.0 * M_)
    const double mult_;
    std::default_random_engine level_rng_{};

    // The offset of data structure. Init not in initialize list. const
    SizeT data_offset_{};
    SizeT neighbors0_offset_{};
    SizeT neighbor_n0_offset_{};
    SizeT layers_offset_{};
    SizeT layer_n_offset_{};
    SizeT label_offset_{};
    SizeT level0_size_{};

    SizeT neighbors1_offset_{};
    SizeT neighbor_n1_offset_{};
    SizeT layer_size_{};
    char *graph_;

    Vector<bool> visited_{};

private:
    const DataType *GetData(VertexType vertex_idx) const {
        return reinterpret_cast<const DataType *>(graph_ + vertex_idx * level0_size_ + data_offset_);
    }
    DataType *GetDataMut(VertexType vertex_idx) { return reinterpret_cast<DataType *>(graph_ + vertex_idx * level0_size_ + data_offset_); }

    const char *GetLayers(VertexType vertex_idx) const {
        return *reinterpret_cast<const char **>(graph_ + vertex_idx * level0_size_ + layers_offset_);
    }
    char **GetLayersMut(VertexType vertex_idx) { return reinterpret_cast<char **>(graph_ + vertex_idx * level0_size_ + layers_offset_); }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_idx, i32 layer_idx) const {
        char *vertex_p = graph_ + vertex_idx * level0_size_;
        if (layer_idx) {
            const char *layer = GetLayers(vertex_idx) + (layer_idx - 1) * layer_size_;
            return {reinterpret_cast<const VertexType *>(layer + neighbors1_offset_),
                    *reinterpret_cast<const VertexListSize *>(layer + neighbor_n1_offset_)};
        } else {
            return {reinterpret_cast<const VertexType *>(vertex_p + neighbors0_offset_),
                    *reinterpret_cast<const VertexListSize *>(vertex_p + neighbor_n0_offset_)};
        }
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_idx, i32 layer_idx) {
        char *vertex_p = graph_ + vertex_idx * level0_size_;
        if (layer_idx) {
            char *layer = *GetLayersMut(vertex_idx) + (layer_idx - 1) * layer_size_;
            return {reinterpret_cast<VertexType *>(layer + neighbors1_offset_), reinterpret_cast<VertexListSize *>(layer + neighbor_n1_offset_)};
        } else {
            return {reinterpret_cast<VertexType *>(vertex_p + neighbors0_offset_),
                    reinterpret_cast<VertexListSize *>(vertex_p + neighbor_n0_offset_)};
        }
    }

    LayerSize GetLayerN(VertexType vertex_idx) const {
        return *reinterpret_cast<const LayerSize *>(graph_ + vertex_idx * level0_size_ + layer_n_offset_);
    }
    LayerSize *GetLayerNMut(VertexType vertex_idx) { return reinterpret_cast<LayerSize *>(graph_ + vertex_idx * level0_size_ + layer_n_offset_); }

    LabelType GetLabel(VertexType vertex_idx) const {
        return *reinterpret_cast<const LabelType *>(graph_ + vertex_idx * level0_size_ + label_offset_);
    }
    LabelType *GetLabelMut(VertexType vertex_idx) { return reinterpret_cast<LabelType *>(graph_ + vertex_idx * level0_size_ + label_offset_); }

    void ReleaseVisited() { std::fill(visited_.begin(), visited_.begin() + cur_vertex_n_, false); }

public:
    template <typename SpaceType>
        requires SpaceConcept<DataType, SpaceType>
    KnnHnsw(SizeT max_vertex, SpaceType space, SizeT M = 16, SizeT ef_construction = 200, SizeT random_seed = 100)
        : max_vertex_(max_vertex), M_(M), Mmax_(M_), Mmax0_(2 * Mmax_), ef_(10), ef_construction_(Max(M_, ef_construction)), dim_(space.Dimension()),
          dist_func_(space.template DistFuncPtr<DataType>()), cur_vertex_n_(0), max_layer_(-1), enterpoint_(-1), mult_(1 / std::log(1.0 * M_)),
          visited_(max_vertex_, false) {
        level_rng_.seed(random_seed);

        data_offset_ = 0;
        neighbors0_offset_ = AlignTo(sizeof(DataType) * dim_, sizeof(VertexType));
        neighbor_n0_offset_ = AlignTo(neighbors0_offset_ + sizeof(VertexType) * Mmax0_, sizeof(VertexListSize));
        layers_offset_ = AlignTo(neighbor_n0_offset_ + sizeof(VertexListSize), sizeof(void *));
        layer_n_offset_ = AlignTo(layers_offset_ + sizeof(void *), sizeof(LayerSize));
        label_offset_ = AlignTo(layer_n_offset_ + sizeof(LayerSize), sizeof(LabelType));
        level0_size_ = AlignTo(label_offset_ + sizeof(LabelType), 8);

        neighbors1_offset_ = 0;
        neighbor_n1_offset_ = AlignTo(sizeof(VertexType) * Mmax_, sizeof(VertexListSize));
        layer_size_ = AlignTo(neighbor_n1_offset_ + sizeof(VertexListSize), 4);

        graph_ = new char[max_vertex_ * level0_size_];
    }

    ~KnnHnsw() {
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n_; ++vertex_idx) {
            delete[] GetLayers(vertex_idx);
        }
        delete[] graph_;
    }

private:
    // >= 0
    i32 GenerateRandomLayer() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double r = -std::log(distribution(level_rng_)) * mult_;
        return static_cast<i32>(r);
    }

    VertexType GenerateNewVertexIdx() { return cur_vertex_n_++; }

    void InitVertex(VertexType vertex_idx, i32 layer_n, const DataType *data, LabelType label) {
        DataType *vertex_data = GetDataMut(vertex_idx);
        std::copy(data, data + dim_, vertex_data);
        *GetNeighborsMut(vertex_idx, 0).second = 0;
        *GetLabelMut(vertex_idx) = label;
        *GetLayerNMut(vertex_idx) = layer_n;
        if (layer_n) {
            *GetLayersMut(vertex_idx) = new char[layer_size_ * layer_n];
            for (i32 layer_idx = 1; layer_idx <= layer_n; ++layer_idx) {
                *GetNeighborsMut(vertex_idx, layer_idx).second = 0;
            }
        }
    }

    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    // return value is a max heap of distance
    DistHeap SearchLayer(VertexType enter_point, const DataType *query, i32 layer_idx, SizeT candidate_n) {
        DistHeap result;
        DistHeap candidate;

        DataType dist = dist_func_(query, GetData(enter_point), dim_);

        candidate.emplace(-dist, enter_point);
        result.emplace(dist, enter_point);
        visited_[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (-minus_c_dist > result.top().first && result.size() == candidate_n) {
                break;
            }
            const auto [neighbors_p, neighbor_size] = GetNeighbors(c_idx, layer_idx);
            for (int i = 0; i < neighbor_size; ++i) {
                VertexType n_idx = neighbors_p[i];
                if (visited_[n_idx]) {
                    continue;
                }
                visited_[n_idx] = true;
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
        ReleaseVisited();
        return result;
    }

    VertexType SearchLayerNearest(VertexType enter_point, const DataType *query, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        DataType cur_dist = dist_func_(query, GetData(enter_point), dim_);
        bool check = true;
        while (check) {
            check = false;
            const auto [neighbors_p, neighbor_size] = GetNeighbors(cur_p, layer_idx);
            for (int i = 0; i < neighbor_size; ++i) {
                VertexType n_idx = neighbors_p[i];
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
    // result distance is increasing
    Vector<VertexType> SelectNeighborsHeuristic(DistHeap &candidates, SizeT M, bool extend_candidates = false, bool keey_pruned_candidates = false) {
        Vector<VertexType> result;
        if (SizeT c_size = candidates.size(); c_size < M) {
            result.reserve(c_size);
            while (!candidates.empty()) {
                result.emplace_back(candidates.top().second);
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
        return result;
    }

    void ConnectNeighbors(VertexType vertex_idx, const Vector<VertexType> &neighbors, i32 layer_idx) {
        const auto [q_neighbors_p, q_neighbor_size_p] = GetNeighborsMut(vertex_idx, layer_idx);
        VertexListSize q_neighbor_size = neighbors.size();
        *q_neighbor_size_p = q_neighbor_size;
        std::reverse_copy(neighbors.begin(), neighbors.end(), q_neighbors_p);
        for (int i = 0; i < q_neighbor_size; ++i) {
            VertexType n_idx = q_neighbors_p[i];
            auto [n_neighbors_p, n_neighbor_size_p] = GetNeighborsMut(n_idx, layer_idx);
            VertexListSize n_neighbor_size = *n_neighbor_size_p;
            SizeT Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
            if (n_neighbor_size < Mmax) {
                *(n_neighbors_p + n_neighbor_size) = vertex_idx;
                *n_neighbor_size_p = n_neighbor_size + 1;
                continue;
            }
            DistHeap candidates;
            const DataType *n_data = GetData(n_idx);
            DataType n_dist = dist_func_(n_data, GetData(vertex_idx), dim_);
            candidates.emplace(-n_dist, vertex_idx);
            for (int i = 0; i < n_neighbor_size; ++i) {
                VertexType nn_idx = n_neighbors_p[i];
                DataType nn_dist = dist_func_(GetData(nn_idx), n_data, dim_);
                candidates.emplace(-nn_dist, nn_idx);
            }
            Vector<VertexType> shrink_neighbors = SelectNeighborsHeuristic(candidates, Mmax);
            *n_neighbor_size_p = shrink_neighbors.size();
            std::reverse_copy(shrink_neighbors.begin(), shrink_neighbors.end(), n_neighbors_p);
        }
    }

public:
    void Insert(const DataType *query, LabelType label) {
        VertexType q_vertex = GenerateNewVertexIdx();
        i32 q_layer = GenerateRandomLayer();
        InitVertex(q_vertex, q_layer, query, label);
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
                ep = neighbors.front();
                ConnectNeighbors(q_vertex, neighbors, cur_layer);
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

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------

    // void CheckGraph() {
    //     assert(cur_vertex_n_ <= max_vertex_);
    //     int max_layer = -1;
    //     for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n_; ++vertex_idx) {
    //         int cur_max_layer = GetLayerN(vertex_idx);
    //         max_layer = Max(cur_max_layer, max_layer);
    //         assert(cur_max_layer >= 0 && cur_max_layer <= max_layer_);
    //         for (int layer_idx = 0; layer_idx <= cur_max_layer; ++layer_idx) {
    //             char *p = graph_ + vertex_idx * level0_size_;
    //             auto [neighbors, neighbor_n] = GetNeighbors(vertex_idx, layer_idx);
    //             VertexType Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
    //             assert(neighbor_n <= Mmax);
    //             for (int i = 0; i < neighbor_n; ++i) {
    //                 VertexType neighbor_idx = neighbors[i];
    //                 assert(neighbor_idx < cur_vertex_n_);
    //                 assert(neighbor_idx != vertex_idx);
    //                 auto [n_neighbors, n_neighbor_n] = GetNeighbors(neighbor_idx, layer_idx);
    //                 assert(n_neighbor_n <= Mmax);
    //             }
    //         }
    //     }
    //     assert(max_layer == max_layer_);
    // }

    // void DumpGraph(const String filename) {
    //     std::fstream fo(filename, std::ios::out);
    //     fo << std::endl << "---------------------------------------------" << std::endl;
    //     fo << "M_: " << M_ << std::endl;
    //     fo << "Mmax_: " << Mmax_ << std::endl;
    //     fo << "Mmax0_: " << Mmax0_ << std::endl;
    //     fo << "ef_: " << ef_ << std::endl;
    //     fo << "ef_construction_: " << ef_construction_ << std::endl;
    //     fo << "dim_: " << dim_ << std::endl;
    //     fo << std::endl;

    //     fo << "current element number: " << cur_vertex_n_ << std::endl;
    //     fo << "max layer: " << max_layer_ << std::endl;
    //     fo << std::endl;

    //     Vector<Vector<VertexType>> layer2vertex(max_layer_ + 1);
    //     for (VertexType v = 0; v < cur_vertex_n_; ++v) {
    //         i32 layer_n = GetLayerN(v);
    //         for (i32 layer_idx = 0; layer_idx <= layer_n; ++layer_idx) {
    //             layer2vertex[layer_idx].emplace_back(v);
    //         }
    //     }
    //     for (i32 layer = 0; layer <= max_layer_; ++layer) {
    //         fo << "layer " << layer << std::endl;
    //         for (VertexType v : layer2vertex[layer]) {
    //             fo << v << ": ";
    //             auto [neighbors, neighbor_n] = GetNeighbors(v, layer);
    //             for (int i = 0; i < neighbor_n; ++i) {
    //                 fo << neighbors[i] << ", ";
    //             }
    //             fo << std::endl;
    //         }
    //     }
    //     fo << "---------------------------------------------" << std::endl;
    // }

    // void TmpSave(const String &file_name) {
    //     std::fstream fo(file_name, std::ios::out);
    //     fo << max_layer_ << std::endl;
    //     fo << enterpoint_ << std::endl;
    //     fo << graph_.size() << std::endl;
    //     for (const auto &vertex : graph_) {
    //         fo << vertex.label_ << std::endl;
    //         for (const auto &data : vertex.data_) {
    //             fo << data << " ";
    //         }
    //         fo << vertex.neighbor_layers_.size() << std::endl;
    //         for (const auto &layer : vertex.neighbor_layers_) {
    //             fo << layer.size() << std::endl;
    //             for (const auto &neighbor : layer) {
    //                 fo << neighbor << " ";
    //             }
    //             fo << std::endl;
    //         }
    //         fo << std::endl;
    //     }
    // }

    // template <typename SpaceType>
    //     requires SpaceConcept<DataType, SpaceType>
    // KnnHnsw(SpaceType space, const String &file_name)
    //     : M_(16), Mmax_(M_), Mmax0_(2 * Mmax_), ef_(10), ef_construction_(200), dim_(space.Dimension()),
    //       dist_func_(space.template DistFuncPtr<DataType>()), mult_(1 / std::log(1.0 * M_)) {
    //     level_rng_.seed(100);
    //     std::fstream fo(file_name, std::ios::in);
    //     fo >> max_layer_;
    //     fo >> enterpoint_;
    //     int vertex_num = -1;
    //     fo >> vertex_num;
    //     graph_.resize(vertex_num);
    //     for (int i = 0; i < vertex_num; i++) {
    //         fo >> graph_[i].label_;
    //         graph_[i].data_.resize(dim_);
    //         for (int j = 0; j < dim_; j++) {
    //             fo >> graph_[i].data_[j];
    //         }
    //         int layer_num = -1;
    //         fo >> layer_num;
    //         graph_[i].neighbor_layers_.resize(layer_num);
    //         for (int j = 0; j < layer_num; j++) {
    //             int neighbor_num = -1;
    //             fo >> neighbor_num;
    //             graph_[i].neighbor_layers_[j].resize(neighbor_num);
    //             for (int k = 0; k < neighbor_num; k++) {
    //                 fo >> graph_[i].neighbor_layers_[j][k];
    //             }
    //         }
    //     }
    // }
};
} // namespace infinity