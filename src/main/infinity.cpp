//
// Created by jinhai on 23-10-14.
//

module;

module infinity;

import stl;
import logger;
import config;
import resource_manager;
import fragment_scheduler;
import storage;
import local_file_system;
import std;
import third_party;
import query_options;
import query_result;
import database;
import infinity_context;
import session;
import query_context;
import database_object;

namespace infinity {

Infinity::Infinity() : DatabaseObject(ObjectType::kDatabase), session_(std::move(MakeUnique<EmbeddedSession>())) {}

Infinity::~Infinity() { Disconnect(); }

SharedPtr<Infinity> Infinity::Connect(const String &path) {
    LocalFileSystem fs;
    if (!fs.Exists(path)) {
        std::cerr << path << " doesn't exist." << std::endl;
    }

    SharedPtr<String> config_path = MakeShared<String>(path + "/infinity_conf.toml");
    if (!fs.Exists(*config_path)) {
        std::cerr << *config_path << "doesn't exists. Generating it ..." << std::endl;
        // Generate the infinity_conf.toml and serialize it.
    }

    SharedPtr<Infinity> infinity_ptr = MakeShared<Infinity>();
    InfinityContext::instance().Init(config_path);

    //    infinity_ptr->Init(config_path);
    Printf("Connect to database at: {}\n", path);
    infinity_ptr->session_ = MakeUnique<EmbeddedSession>();
    return infinity_ptr;
}

void Infinity::Disconnect() {
    Printf("To disconnect the database.\n");
    InfinityContext::instance().UnInit();
}

QueryResult Infinity::CreateDatabase(const String &db_name, const CreateDatabaseOptions &options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    query_context_ptr->set_current_schema(session_->current_database());
}

QueryResult Infinity::DropDatabase(const String &db_name, const CreateDatabaseOptions &options) {}

QueryResult Infinity::ListDatabases() {}

QueryResult Infinity::DescribeDatabase(const String &db_name) {}

SharedPtr<Database> Infinity::GetDatabase(const String &db_name) {}

} // namespace infinity
