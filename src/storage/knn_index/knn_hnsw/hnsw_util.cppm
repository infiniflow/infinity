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

export module infinity_core:hnsw_util;

import :infinity_exception;
import :status;

import std;

import statement_common;

namespace infinity {

export struct HnswOptimizeOptions {
    bool compress_to_rabitq = false;
    bool compress_to_lvq = false;
    bool lvq_avg = false;
};

export struct HnswUtil {
    static std::optional<HnswOptimizeOptions> ParseOptimizeOptions(const std::vector<std::unique_ptr<InitParameter>> &opt_params) {
        HnswOptimizeOptions options;
        for (const auto &param : opt_params) {
            if (param->param_name_ == "compress_to_rabitq") {
                options.compress_to_rabitq = true;
            } else if (param->param_name_ == "compress_to_lvq") {
                options.compress_to_lvq = true;
            } else if (param->param_name_ == "lvq_avg") {
                options.lvq_avg = true;
            }
        }
        if (options.compress_to_rabitq && (options.compress_to_lvq || options.lvq_avg)) {
            RecoverableError(Status::InvalidIndexParam("multiple compress type: rabitq and lvq cannot be set at the same time"));
        }
        if (options.compress_to_lvq && options.lvq_avg) {
            RecoverableError(Status::InvalidIndexParam("compress_to_lvq and lvq_avg cannot be set at the same time"));
        }
        if (!options.compress_to_rabitq && !options.compress_to_lvq && !options.lvq_avg) {
            return std::nullopt;
        }
        return options;
    }
};

} // namespace infinity