module;

#include <iostream>

import stl;

export module hnsw_profiler;

namespace infinity {
export class HnswProfiler {
    Vector<Vector<SizeT>> cal_nums_;
    Vector<SizeT> cal_sums_;
    Vector<SizeT> cal_cnts_;
    const SizeT bucket_size_;

public:
    explicit HnswProfiler(i32 max_layer, SizeT bucket_size)
        : cal_nums_(max_layer + 1), cal_sums_(max_layer + 1, 0), cal_cnts_(max_layer + 1, 0), bucket_size_(bucket_size) {}

    void AddSearch(i32 layer_idx, SizeT cal_num) {
        SizeT idx = cal_num / bucket_size_;
        if (cal_nums_[layer_idx].size() <= idx) {
            cal_nums_[layer_idx].resize(idx + 1);
        }
        ++cal_nums_[layer_idx][idx];
        cal_sums_[layer_idx] += cal_num;
        ++cal_cnts_[layer_idx];
    }

    void PrintResult() {
        SizeT max_layer = cal_nums_.size() - 1;
        for (i32 layer_idx =  max_layer; layer_idx >= 0; --layer_idx) {
            const auto &layer = cal_nums_[layer_idx];
            std::cout << "Layer " << layer_idx << ": " << std::endl;
            for (int i = 0; i < layer.size(); ++i) {
                std::cout << i * bucket_size_ << "\t";
            }
            std::cout << std::endl;
            for (int i = 0; i < layer.size(); ++i) {
                std::cout << layer[i] << "\t";
            }
            std::cout << std::endl;
            std::cout << "sum: " << cal_sums_[layer_idx] << std::endl;
            std::cout << "avg: " << double(cal_sums_[layer_idx]) / cal_cnts_[layer_idx] << std::endl;
        }
    }
};
} // namespace infinity