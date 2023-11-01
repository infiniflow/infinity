//
// Created by jinhai on 23-10-30.
//

export module database;

import stl;
import query_result;
import query_options;
import table;
import table_collection_detail;
import session;
import std;
import parser;

namespace infinity {

export class Database {
public:
    explicit Database(const String &db_name, SharedPtr<EmbeddedSession> session) : db_name_(db_name), session_(std::move(session)) {}

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
    SharedPtr<EmbeddedSession> session_{};
};

} // namespace infinity
