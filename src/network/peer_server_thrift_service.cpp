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

module peer_server_thrift_service;

import stl;
import third_party;
import logger;
import peer_server_thrift_types;

namespace infinity {

void PeerServerThriftService::Register(infinity_peer_server::RegisterResponse &response, const infinity_peer_server::RegisterRequest &request) {
    return ;
}

void PeerServerThriftService::UnRegister(infinity_peer_server::UnRegisterResponse &response, const infinity_peer_server::UnRegisterRequest &request) {
    return ;
}
void PeerServerThriftService::SendHeartBeat(infinity_peer_server::HeartBeatResponse &response, const infinity_peer_server::HeartBeatRequest &request) {
    return ;
}
void PeerServerThriftService::SyncLog(infinity_peer_server::SyncLogResponse &response, const infinity_peer_server::SyncLogRequest &request) {
    return ;
}
void PeerServerThriftService::ChangeRole(infinity_peer_server::ChangeRoleResponse &response, const infinity_peer_server::ChangeRoleRequest &request) {
    return ;
}
void PeerServerThriftService::NewLeader(infinity_peer_server::NewLeaderResponse &response, const infinity_peer_server::NewLeaderRequest &request) {
    return ;
}

} // namespace infinity