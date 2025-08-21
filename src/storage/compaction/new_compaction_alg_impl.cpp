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

module infinity_core:new_compaction_alg.impl;

import :new_compaction_alg;
import :default_values;
import :infinity_exception;

import std;
import std.compat;

namespace infinity {

class DBTConfig {
public:
    DBTConfig(size_t m, size_t c, size_t s) : m_(m), c_(c), s_(s) {
        if (m <= 0 || c < m || s <= 0) {
            UnrecoverableError("Invalid compaction parameters");
        }
    }

    int CalculateLayer(size_t row_cnt) const { return std::max(0, static_cast<int>(std::log(row_cnt / s_) / std::log(c_))); }

    size_t Upperbound(int layer) const { return s_ * std::pow(c_, layer) - 1; }

    // size_t LowerBound(int layer) const { return layer >= 1 ? s_ * std::pow(c_, layer - 1) : 0; }

    const size_t m_; // the max node cnt in one layer
private:
    const size_t c_; // the exponent of capacity in one layer
    const size_t s_; // the size of the first layer
    // layer i has capacity: c^i * s, (i >= 0)
};

class DBTCompactionAlg : public NewCompactionAlg {
public:
    DBTCompactionAlg(int m, int c, int s, size_t segment_capacity) : config_(m, c, s), segment_capacity_(segment_capacity) {}

    void AddSegment(SegmentID segment_id, size_t segment_row_cnt) override {
        int layer = config_.CalculateLayer(segment_row_cnt);
        if (layer >= static_cast<int>(segment_layers_.size())) {
            segment_layers_.resize(layer + 1);
        }
        segment_layers_[layer].emplace_back(segment_id, segment_row_cnt);
    }

    std::vector<SegmentID> GetCompactableSegments() override {
        int cur_layer_n = segment_layers_.size();
        for (int layer = 0; layer < cur_layer_n; ++layer) {
            auto &segment_layer = segment_layers_[layer];
            if (segment_layer.size() < config_.m_) {
                continue;
            }

            std::vector<SegmentID> ret;
            size_t total_row_cnt = 0;
            for (size_t i = 0; i < config_.m_; ++i) {
                ret.push_back(segment_layer[i].first);
                total_row_cnt += segment_layer[i].second;
            }
            if (total_row_cnt > segment_capacity_) {
                return {};
            }

            segment_layer.erase(segment_layer.begin(), segment_layer.begin() + config_.m_);
            return ret;
        }
        return {};
    }

private:
    DBTConfig config_;
    size_t segment_capacity_ = 0;

    std::vector<std::vector<std::pair<SegmentID, size_t>>> segment_layers_;
};

std::unique_ptr<NewCompactionAlg> NewCompactionAlg::GetInstance() {
    return std::make_unique<DBTCompactionAlg>(DBT_COMPACTION_M, DBT_COMPACTION_C, DBT_COMPACTION_S, DEFAULT_SEGMENT_CAPACITY);
}

} // namespace infinity
