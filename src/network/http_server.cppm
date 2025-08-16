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

export module infinity_core:http_server;

import :infinity_type;

import std;

import third_party;

namespace infinity {

enum struct HTTPServerStatus : u8 {
    kStopped = 0,
    kStarting,
    kRunning,
    kStopping,
};

export class HTTPServer {
public:
    std::thread Start(const std::string &server_address, u16 port);
    void Shutdown();

private:
    std::atomic<HTTPServerStatus> status_{HTTPServerStatus::kStopped};
    std::shared_ptr<HttpRouter> router_{};
    std::shared_ptr<WebServer> server_{};

    std::shared_ptr<HttpConnectionProvider> connection_provider_{};
    std::shared_ptr<HttpConnectionHandler> connection_handler_{};
};

} // namespace infinity
