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

#include "network/grpc_server.h"
#include "network/grpc_server.cpp"
#include <grpcpp/grpcpp.h>
#include <grpc++/grpc++.h>

int main() {
    infinity::GrpcAsyncClient client(
        grpc::CreateChannel("localhost:50059", grpc::InsecureChannelCredentials()));

    auto resp_0 = client.Connect();

    auto resp_1 =  client.DisConnect(resp_0.session_id());

    return 0;
}
