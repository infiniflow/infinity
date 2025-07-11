// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core;

import base_statement;
import admin_statement;

namespace infinity {

void ClusterManager::InitAsStandalone() {
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    current_node_role_ = NodeRole::kStandalone;
}

Status ClusterManager::UnInitFromStandaloneNoLock() {
    current_node_role_ = NodeRole::kUnInitialized;
    return Status::OK();
}

} // namespace infinity