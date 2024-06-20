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

#include <string>
#include <vector>

module bmp_util;

import logger;
import third_party;

namespace infinity {

BmpSearchOptions BMPUtil::ParseBmpSearchOptions(const Vector<UniquePtr<InitParameter>> &opt_params) {
    BmpSearchOptions options;
    for (const auto &opt_param : opt_params) {
        if (opt_param->param_name_ == "alpha") {
            f32 alpha = std::stof(opt_param->param_value_);
            if (alpha < 0.0 || alpha > 1.0) {
                LOG_WARN("Invalid alpha value, should be in [0, 1]");
                continue;
            }
            options.alpha_ = alpha;
        } else if (opt_param->param_name_ == "beta") {
            f32 beta = std::stof(opt_param->param_value_);
            if (beta < 0.0 || beta > 1.0) {
                LOG_WARN("Invalid beta value, should be in [0, 1]");
                continue;
            }
            options.beta_ = beta;
        } else if (opt_param->param_name_ == "use_tail") {
            if (!IsEqual(opt_param->param_value_, "true") && !IsEqual(opt_param->param_value_, "false")) {
                LOG_WARN("Invalid use_tail value, should be true or false");
                continue;
            }
            options.use_tail_ = IsEqual(opt_param->param_value_, "true");
        } else if (opt_param->param_name_ == "use_lock") {
            if (!IsEqual(opt_param->param_value_, "true") && !IsEqual(opt_param->param_value_, "false")) {
                LOG_WARN("Invalid use_lock value, should be true or false");
                continue;
            }
            options.use_lock_ = IsEqual(opt_param->param_value_, "true");
        }
    }
    return options;
};

Optional<BMPOptimizeOptions> BMPUtil::ParseBMPOptimizeOptions(const Vector<UniquePtr<InitParameter>> &opt_params) {
    BMPOptimizeOptions options;
    for (const auto &opt_param : opt_params) {
        if (opt_param->param_name_ == "topk") {
            i32 topk = std::stoi(opt_param->param_value_);
            if (topk <= 0) {
                continue;
            }
            if (topk > 1000) {
                LOG_WARN(fmt::format("topk value is large {}", topk));
            }
            options.topk_ = topk;
        }
    }
    if (options.topk_ == 0) {
        return None;
    }
    return options;
}

} // namespace infinity