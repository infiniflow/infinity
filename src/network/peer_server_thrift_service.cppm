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

export module peer_server_thrift_service;

import peer_server_thrift_types;
import stl;
import global_resource_usage;

namespace infinity {

export class PeerServerThriftService final : public infinity_peer_server::PeerServiceIf {

public:
    PeerServerThriftService() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PeerServerThriftService");
#endif
    }
    virtual ~PeerServerThriftService() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PeerServerThriftService");
#endif
    }

    void Register(infinity_peer_server::RegisterResponse &response, const infinity_peer_server::RegisterRequest &request) final;
    void Unregister(infinity_peer_server::UnregisterResponse &response, const infinity_peer_server::UnregisterRequest &request) final;
    void HeartBeat(infinity_peer_server::HeartBeatResponse &response, const infinity_peer_server::HeartBeatRequest &request) final;
    void SyncLog(infinity_peer_server::SyncLogResponse &response, const infinity_peer_server::SyncLogRequest &request) final;
    void ChangeRole(infinity_peer_server::ChangeRoleResponse &response, const infinity_peer_server::ChangeRoleRequest &request) final;
    void NewLeader(infinity_peer_server::NewLeaderResponse &response, const infinity_peer_server::NewLeaderRequest &request) final;

private:
};

} // namespace infinity
