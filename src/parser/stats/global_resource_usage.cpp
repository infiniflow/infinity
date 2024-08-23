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

#include "global_resource_usage.h"

namespace infinity {

std::atomic_bool GlobalResourceUsage::initialized_ = false;

int64_t GlobalResourceUsage::object_count_ = 0;
std::unordered_map<std::string, int64_t> GlobalResourceUsage::object_map_;

int64_t GlobalResourceUsage::raw_memory_count_ = 0;
std::unordered_map<std::string, int64_t> GlobalResourceUsage::raw_memory_map_;

std::mutex GlobalResourceUsage::object_mutex_{};

std::mutex GlobalResourceUsage::raw_memory_mutex_{};

} // namespace infinity
