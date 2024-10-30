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
#include <string>
module merge_knn;

import stl;
import statement_common;

namespace infinity {

Optional<f32> GetKnnThreshold(const Vector<InitParameter> &opt_params) {
    Optional<f32> knn_threshold;
    for (const auto &opt_param : opt_params) {
        if (opt_param.param_name_ == "threshold") {
            knn_threshold = std::stof(opt_param.param_value_);
        }
    }
    return knn_threshold;
}

Optional<f32> GetKnnThreshold(const Vector<UniquePtr<InitParameter>> &opt_params) {
    Optional<f32> knn_threshold;
    for (const auto &opt_param : opt_params) {
        if (opt_param->param_name_ == "threshold") {
            knn_threshold = std::stof(opt_param->param_value_);
        }
    }
    return knn_threshold;
}

} // namespace infinity