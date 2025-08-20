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

export module infinity_core:snapshot;

import :status;
import :snapshot_info;

namespace infinity {
class QueryContext;

export class Snapshot {
public:
    static Status CreateTableSnapshot(QueryContext *query_context, const std::string &snapshot_name, const std::string &table_name);
    static Status RestoreTableSnapshot(QueryContext *query_context, const std::string &snapshot_name);
    static Status CreateDatabaseSnapshot(QueryContext *query_context, const std::string &snapshot_name, const std::string &db_name);
    static Status RestoreDatabaseSnapshot(QueryContext *query_context, const std::string &snapshot_name);
    static Status DropSnapshot(QueryContext *query_context, const std::string &snapshot_name);
};

} // namespace infinity
