//
// Created by jinhai on 23-10-30.
//

export module database;

import stl;
import query_result;
import query_options;
import table;
import table_collection_detail;

namespace infinity {

export class Database {
public:
    QueryResult CreateTable(const String &column_name, Vector<Pair<String, String>>, const CreateTableOptions &create_table_options);

    QueryResult DropTable(const String &table_name);

    QueryResult ListTables();

    QueryResult DescribeTable(const String &table_name);

    SharedPtr<Table> GetDatabase(const String &table_name);
};

} // namespace infinity
