//
// Created by jinhai on 23-11-1.
//

module;

module database;

import stl;
import std;
import query_options;
import query_result;
import table;
import infinity_context;
import query_context;
import parser;

namespace infinity {

QueryResult Database::CreateTable(const String &table_name,
                                  Vector<ColumnDef *> column_defs,
                                  Vector<TableConstraint *> constraints,
                                  const CreateTableOptions &create_table_options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<CreateStatement> create_statement = MakeUnique<CreateStatement>();
    SharedPtr<CreateTableInfo> create_table_info = MakeShared<CreateTableInfo>();
    create_table_info->table_name_ = table_name;
    create_table_info->column_defs_ = std::move(column_defs);
    create_table_info->constraints_ = std::move(constraints);
    create_statement->create_info_ = std::move(create_table_info);
    QueryResponse response = query_context_ptr->QueryStatement(create_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Database::DropTable(const String &table_name, const DropTableOptions &drop_table_options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<DropStatement> drop_statement = MakeUnique<DropStatement>();
    SharedPtr<DropTableInfo> drop_table_info = MakeShared<DropTableInfo>();
    drop_table_info->schema_name_ = db_name_;
    drop_table_info->table_name_ = table_name;
    drop_statement->drop_info_ = drop_table_info;
    QueryResponse response = query_context_ptr->QueryStatement(drop_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Database::ListTables() {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<ShowStatement> show_statement = MakeUnique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kTables;
    QueryResponse response = query_context_ptr->QueryStatement(show_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Database::DescribeTable(const String &db_name) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<ShowStatement> show_statement = MakeUnique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kColumns;
    QueryResponse response = query_context_ptr->QueryStatement(show_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
}

SharedPtr<Table> Database::GetTable(const String &table_name) {
        UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
        query_context_ptr->Init(InfinityContext::instance().config(),
                                InfinityContext::instance().fragment_scheduler(),
                                InfinityContext::instance().storage(),
                                InfinityContext::instance().resource_manager());
        UniquePtr<CommandStatement> command_statement = MakeUnique<CommandStatement>();
        command_statement->command_info_ = MakeShared<CheckTable>(table_name.c_str());
        QueryResponse response = query_context_ptr->QueryStatement(command_statement.get());
        if(response.result_msg_.get() == nullptr) {
            return MakeShared<Table>(table_name, session_);
        } else {
            return nullptr;
        }
}

} // namespace infinity
