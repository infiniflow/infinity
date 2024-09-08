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

module peer_thrift_client;

import stl;
import third_party;
import logger;
import peer_server_thrift_types;
import status;
import thrift;

namespace infinity {

PeerClient PeerClient::Connect(const String &ip_address, i64 port) {
    SharedPtr<TSocket> socket = MakeShared<TSocket>(ip_address, port);
    SharedPtr<TBufferedTransport> transport = MakeShared<TBufferedTransport>(socket);
    SharedPtr<TBinaryProtocol> protocol = MakeShared<TBinaryProtocol>(transport);
    UniquePtr<PeerServiceClient> client = MakeUnique<PeerServiceClient>(protocol);
    transport->open();
    return {socket, transport, protocol, std::move(client)};
}

/// TODO: comment
Status PeerClient::Disconnect() {
    transport_->close();
    return Status::OK();
}

} // namespace infinity