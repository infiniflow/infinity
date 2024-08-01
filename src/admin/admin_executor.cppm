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
export module admin_executor;

import admin_statement;
import query_context;
import query_result;
import stl;
import wal_entry;

namespace infinity {

export class AdminExecutor {
public:
    static QueryResult Execute(QueryContext* query_context, const AdminStatement* admin_statement);
private:
    static QueryResult ListLogFiles(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowLogFile(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListLogIndexes(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowLogIndex(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListCatalogs(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowCatalog(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListDatabases(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowDatabase(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListTables(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowTable(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListSegments(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowSegment(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListBlocks(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowBlock(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListColumns(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowColumn(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListIndexes(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowIndex(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ListIndexSegments(QueryContext* query_context, const AdminStatement* admin_statement);
    static QueryResult ShowIndexSegment(QueryContext* query_context, const AdminStatement* admin_statement);

    static Vector<SharedPtr<WalEntry>> GetAllCheckpointEntries(QueryContext* query_context, const AdminStatement* admin_statement);
};

}

