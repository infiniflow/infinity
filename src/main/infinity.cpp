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
import parser;

namespace infinity {

Infinity::Infinity() : DatabaseObject(ObjectType::kDatabase), session_(std::move(MakeUnique<EmbeddedSession>())) {}

Infinity::~Infinity() { Disconnect(); }

SharedPtr<Infinity> Infinity::Connect(const String &path) {
    LocalFileSystem fs;
    if (!fs.Exists(path)) {
        std::cerr << path << " doesn't exist." << std::endl;
        return nullptr;
    }

    SharedPtr<String> config_path = MakeShared<String>(path + "/infinity_conf.toml");

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
    UniquePtr<CreateStatement> create_statement = MakeUnique<CreateStatement>();
    SharedPtr<CreateSchemaInfo> create_schema_info = MakeShared<CreateSchemaInfo>();
    create_schema_info->schema_name_ = db_name;
    create_statement->create_info_ = create_schema_info;
    QueryResponse response = query_context_ptr->QueryStatement(create_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    result.error_message_ = response.result_msg_;
    result.error_code_ = 0;
    return result;
}

QueryResult Infinity::DropDatabase(const String &db_name, const DropDatabaseOptions &options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    query_context_ptr->set_current_schema(session_->current_database());
    UniquePtr<DropStatement> drop_statement = MakeUnique<DropStatement>();
    SharedPtr<DropSchemaInfo> drop_schema_info = MakeShared<DropSchemaInfo>();
    drop_schema_info->schema_name_ = db_name;
    drop_statement->drop_info_ = drop_schema_info;
    QueryResponse response = query_context_ptr->QueryStatement(drop_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    result.error_message_ = response.result_msg_;
    result.error_code_ = 0;
    return result;
}

QueryResult Infinity::ListDatabases() {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    query_context_ptr->set_current_schema(session_->current_database());
    UniquePtr<ShowStatement> show_statement = MakeUnique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kDatabases;
    QueryResponse response = query_context_ptr->QueryStatement(show_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    result.error_message_ = response.result_msg_;
    result.error_code_ = 0;
    return result;
}

QueryResult Infinity::DescribeDatabase(const String &db_name) {}

SharedPtr<Database> Infinity::GetDatabase(const String &db_name) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
//    query_context_ptr->set_current_schema(session_->current_database());
    UniquePtr<CommandStatement> command_statement = MakeUnique<CommandStatement>();
    command_statement->command_info_ = MakeShared<UseCmd>(db_name.c_str());
    QueryResponse response = query_context_ptr->QueryStatement(command_statement.get());
    if(response.result_msg_.get() == nullptr) {
        return MakeShared<Database>(db_name);
    } else {
        return nullptr;
    }
}

} // namespace infinity
