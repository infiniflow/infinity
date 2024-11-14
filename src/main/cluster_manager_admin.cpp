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

module cluster_manager;

import stl;
import status;

namespace infinity {

void ClusterManager::InitAsAdmin() {
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    current_node_role_ = NodeRole::kAdmin;
}

Status ClusterManager::UnInitFromAdminNoLock() {
    current_node_role_ = NodeRole::kUnInitialized;
    return Status::OK();
}

} // namespace infinity