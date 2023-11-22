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