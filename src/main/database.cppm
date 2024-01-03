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

export module database;

import stl;
import query_result;
import query_options;
import table;
import table_detail;
import session;
import parser;

namespace infinity {

export class Database {
public:
    explicit Database(const String &db_name, SharedPtr<BaseSession> session) : db_name_(db_name), session_(Move(session)) {}

    QueryResult CreateTable(const String &table_name,
                            Vector<ColumnDef *> column_defs,
                            Vector<TableConstraint *> constraints,
                            const CreateTableOptions &create_table_options);

    QueryResult DropTable(const String &table_name, const DropTableOptions &drop_table_options);

    QueryResult ListTables();

    QueryResult DescribeTable(const String &table_name);

    SharedPtr<Table> GetTable(const String &table_name);

    [[nodiscard]] const String &db_name() const { return db_name_; }

private:
    String db_name_{};
    SharedPtr<BaseSession> session_{};
};

} // namespace infinity
