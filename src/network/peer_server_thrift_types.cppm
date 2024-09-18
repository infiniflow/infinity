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

module ;

#include "peer_server_thrift/PeerService.h"

export module peer_server_thrift_types;

namespace infinity_peer_server {

export using infinity_peer_server::PeerServiceIf;
export using infinity_peer_server::PeerServiceClient;
export using infinity_peer_server::PeerServiceIfFactory;
export using infinity_peer_server::PeerServiceProcessorFactory;
export using infinity_peer_server::NodeStatus;
export using infinity_peer_server::NodeType;
export using infinity_peer_server::NodeInfo;
export using infinity_peer_server::RegisterRequest;
export using infinity_peer_server::RegisterResponse;
export using infinity_peer_server::UnregisterRequest;
export using infinity_peer_server::UnregisterResponse;
export using infinity_peer_server::HeartBeatRequest;
export using infinity_peer_server::HeartBeatResponse;
export using infinity_peer_server::SyncLogRequest;
export using infinity_peer_server::SyncLogResponse;
export using infinity_peer_server::ChangeRoleRequest;
export using infinity_peer_server::ChangeRoleResponse;
export using infinity_peer_server::NewLeaderRequest;
export using infinity_peer_server::NewLeaderResponse;
export using infinity_peer_server::to_string;
}
