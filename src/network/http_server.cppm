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

export module http_server;

import stl;
import third_party;

namespace infinity {

export class HTTPServer {
public:
    void Start(const String& server_address, u16 port);
    void Shutdown();
private:
    SharedPtr<HttpRouter> router_{};
    SharedPtr<WebServer> server_{};
    atomic_bool started_{false};
};

}

