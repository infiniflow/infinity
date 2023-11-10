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
module;

export module infinity;

import stl;
import config;
import resource_manager;
import fragment_scheduler;
import storage;
import database;
import query_result;
import query_options;
import infinity_context;
import session;
import database_object;

namespace infinity {

export class Infinity {
public:
    Infinity();

    ~Infinity() = default;

    static SharedPtr<Infinity> RemoteConnect();

    u64 GetSessionId();

    void RemoteDisconnect();

    static SharedPtr<Infinity> LocalConnect(const String &path);

    void LocalDisconnect();

    QueryResult CreateDatabase(const String &db_name, const CreateDatabaseOptions &options);

    QueryResult DropDatabase(const String &db_name, const DropDatabaseOptions &options);

    QueryResult ListDatabases();

    SharedPtr<Database> GetDatabase(const String &db_name);

    // For embedded sqllogictest
    QueryResult Query(const String& query_text);

private:
    SharedPtr<SessionBase> session_{};
};

} // namespace infinity
