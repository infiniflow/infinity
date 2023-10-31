//
// Created by jinhai on 23-10-14.
//
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

export class Infinity : public DatabaseObject {
public:
    Infinity();

    static SharedPtr<Infinity> Connect(const String &path);

    void Disconnect();

    QueryResult CreateDatabase(const String &db_name, const CreateDatabaseOptions &options);

    QueryResult DropDatabase(const String &db_name, const DropDatabaseOptions &options);

    QueryResult ListDatabases();

    QueryResult DescribeDatabase(const String &db_name);

    SharedPtr<Database> GetDatabase(const String &db_name);

    ~Infinity();

private:
    UniquePtr<EmbeddedSession> session_{};

    bool initialized_{false};
};

} // namespace infinity
