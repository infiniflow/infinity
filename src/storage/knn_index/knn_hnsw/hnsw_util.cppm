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

#include <vector>

export module hnsw_util;

import stl;
import statement_common;
import infinity_exception;
import status;

namespace infinity {

export struct HnswOptimizeOptions {
    bool compress_to_lvq = false;
    bool lvq_avg = false;
};

export struct HnswUtil {
    static Optional<HnswOptimizeOptions> ParseOptimizeOptions(const Vector<UniquePtr<InitParameter>> &opt_params) {
        HnswOptimizeOptions options;
        for (const auto &param : opt_params) {
            if (IsEqual(param->param_name_, "compress_to_lvq")) {
                options.compress_to_lvq = true;
            } else if (IsEqual(param->param_name_, "lvq_avg")) {
                options.lvq_avg = true;
            }
        }
        if (options.compress_to_lvq && options.lvq_avg) {
            RecoverableError(Status::InvalidIndexParam("compress_to_lvq and lvq_avg cannot be set at the same time"));
        }
        if (!options.compress_to_lvq && !options.lvq_avg) {
            return None;
        }
        return options;
    }
};

} // namespace infinity