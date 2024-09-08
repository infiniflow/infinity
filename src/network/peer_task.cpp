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

module peer_task;

import stl;

namespace infinity {

String ToString(NodeRole role) {
    switch (role) {
        case NodeRole::kUnInitialized:
            return "uninitialized";
        case NodeRole::kAdmin:
            return "admin";
        case NodeRole::kStandalone:
            return "standalone";
        case NodeRole::kLeader:
            return "leader";
        case NodeRole::kFollower:
            return "follower";
        case NodeRole::kLearner:
            return "learner";
    }
}

String ToString(NodeStatus status) {
    switch (status) {
        case NodeStatus::kReady:
            return "ready";
        case NodeStatus::kConnected:
            return "connected";
        case NodeStatus::kTimeout:
            return "timeout";
        case NodeStatus::kInvalid:
            return "kInvalid";
    }
}


} // namespace infinity
