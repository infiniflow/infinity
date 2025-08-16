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

module infinity_core:infinity.impl;

import :infinity;
import :query_result;
import :infinity_context;
import :query_context;
import :session_manager;
import :utility;

import std;

import create_statement;
import drop_statement;
import create_schema_info;
import drop_schema_info;
import create_table_info;
import drop_table_info;
import show_statement;
import command_statement;
import flush_statement;
import compact_statement;
import column_def;
import statement_common;
import create_index_info;
import drop_index_info;
import insert_row_expr;
import insert_statement;
import copy_statement;
import parsed_expr;
import delete_statement;
import update_statement;
import explain_statement;
import search_expr;
import select_statement;
import table_reference;
import optimize_statement;
import alter_statement;
import admin_statement;

namespace infinity {

std::variant<std::unique_ptr<QueryContext>, QueryResult> Infinity::GetQueryContext(bool is_admin_stmt, bool is_admin_show_node) const {
    InfinityContext &context = InfinityContext::instance();
    if (!context.InfinityContextInited()) {
        QueryResult query_result;
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::InfinityIsIniting();
        return query_result;
    }
    bool check_start = !is_admin_stmt || (!context.IsAdminRole() && !is_admin_show_node); // refactor by old code
    bool error = check_start && !context.InfinityContextStarted();
    if (error) {
        QueryResult query_result;
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::InfinityIsStarting();
        return query_result;
    }
    std::unique_ptr<QueryContext> query_context_ptr = std::make_unique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager(),
                            InfinityContext::instance().persistence_manager());

    return query_context_ptr;
}

#define GET_QUERY_CONTEXT(result, query_context_ptr)                                                                                                 \
    if (std::holds_alternative<QueryResult>(result)) {                                                                                               \
        return std::get<QueryResult>(result);                                                                                                        \
    }                                                                                                                                                \
    query_context_ptr = std::move(std::get<std::unique_ptr<QueryContext>>(result));                                                                        \
    query_context_ptr->CreateQueryProfiler();

u64 Infinity::GetSessionId() { return session_->session_id(); }

void Infinity::Hello() { fmt::print("hello infinity\n"); }

void Infinity::LocalInit(const std::string &path, const std::string &config_path) {
    if (!config_path.empty() && VirtualStore::Exists(config_path)) {
        std::shared_ptr<std::string> config_path_ptr = std::make_shared<std::string>(config_path);
        InfinityContext::instance().InitPhase1(config_path_ptr);
    } else {
        LOG_WARN(fmt::format("Infinity::LocalInit cannot find config: {}", config_path));
        std::unique_ptr<DefaultConfig> default_config = std::make_unique<DefaultConfig>();
        default_config->default_log_dir_ = fmt::format("{}/log", path);
        default_config->default_data_dir_ = fmt::format("{}/data", path);
        default_config->default_catalog_dir_ = fmt::format("{}/catalog", path);
        default_config->default_wal_dir_ = fmt::format("{}/wal", path);
        default_config->default_temp_dir_ = fmt::format("{}/tmp", path);
        default_config->default_resource_dir_ = fmt::format("{}/resource", path);
        default_config->default_persistence_dir_ = fmt::format("{}/persistence", path);

        default_config->default_log_level_ = LogLevel::kInfo;
        default_config->default_log_to_stdout_ = false;
        InfinityContext::instance().InitPhase1(nullptr, default_config.get());
    }
    InfinityContext::instance().InitPhase2();
}

void Infinity::LocalUnInit() { InfinityContext::instance().UnInit(); }

std::shared_ptr<Infinity> Infinity::LocalConnect() {
    std::shared_ptr<Infinity> infinity_ptr = std::make_shared<Infinity>();

    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    infinity_ptr->session_ = session_mgr->CreateLocalSession();
    return infinity_ptr;
}

void Infinity::LocalDisconnect() {
    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    session_mgr->RemoveSessionByID(session_->session_id());
    session_.reset();
}

std::shared_ptr<Infinity> Infinity::RemoteConnect() {
    std::shared_ptr<Infinity> infinity_ptr = std::make_shared<Infinity>();
    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    std::shared_ptr<RemoteSession> remote_session = session_mgr->CreateRemoteSession();
    if (remote_session == nullptr) {
        return nullptr;
    }
    infinity_ptr->session_ = std::move(remote_session);
    return infinity_ptr;
}

void Infinity::RemoteDisconnect() {
    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    session_mgr->RemoveSessionByID(session_->session_id());
    session_.reset();
}

QueryResult Infinity::CreateDatabase(const std::string &schema_name, const CreateDatabaseOptions &create_db_options, const std::string &comment) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<CreateStatement> create_statement = std::make_unique<CreateStatement>();
    std::shared_ptr<CreateSchemaInfo> create_schema_info = std::make_shared<CreateSchemaInfo>();

    create_schema_info->schema_name_ = schema_name;
    ToLower(create_schema_info->schema_name_);
    create_statement->create_info_ = create_schema_info;
    create_statement->create_info_->conflict_type_ = create_db_options.conflict_type_;
    create_statement->create_info_->comment_ = comment;
    QueryResult query_result = query_context_ptr->QueryStatement(create_statement.get());
    return query_result;
}

QueryResult Infinity::DropDatabase(const std::string &schema_name, const DropDatabaseOptions &drop_database_options) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<DropStatement> drop_statement = std::make_unique<DropStatement>();
    std::shared_ptr<DropSchemaInfo> drop_schema_info = std::make_shared<DropSchemaInfo>();

    drop_schema_info->schema_name_ = schema_name;
    ToLower(drop_schema_info->schema_name_);

    drop_statement->drop_info_ = drop_schema_info;
    drop_statement->drop_info_->conflict_type_ = drop_database_options.conflict_type_;
    QueryResult result = query_context_ptr->QueryStatement(drop_statement.get());
    return result;
}

QueryResult Infinity::ListDatabases() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kDatabases;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::GetDatabase(const std::string &schema_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();

    std::string db_name = schema_name;
    ToLower(db_name);

    command_statement->command_info_ = std::make_unique<UseCmd>(db_name.c_str());
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::ShowDatabase(const std::string &schema_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kDatabase;

    show_statement->schema_name_ = schema_name;
    ToLower(show_statement->schema_name_);

    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::Query(const std::string &query_text) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::string query_text_internal = query_text;
    ToLower(query_text_internal);

    QueryResult result = query_context_ptr->Query(query_text_internal);
    return result;
}

QueryResult Infinity::Flush(const std::string &flush_type) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<FlushStatement> flush_statement = std::make_unique<FlushStatement>();

    if (flush_type == "data") {
        flush_statement->type_ = FlushType::kData;
    } else if (flush_type == "catalog") {
        flush_statement->type_ = FlushType::kCatalog;
    } else {
        QueryResult query_result;
        query_result.result_table_ = nullptr;
        query_result.status_ = Status::InvalidQueryOption(fmt::format("Unknown flush type: '{}'", flush_type));
        return query_result;
    }

    QueryResult result = query_context_ptr->QueryStatement(flush_statement.get());
    return result;
}

QueryResult Infinity::Compact(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    auto compact_statement = std::make_unique<ManualCompactStatement>(db_name, table_name);

    ToLower(compact_statement->db_name_);
    ToLower(compact_statement->table_name_);

    QueryResult result = query_context_ptr->QueryStatement(compact_statement.get());
    return result;
}

QueryResult Infinity::SetVariableOrConfig(const std::string &name, bool value, SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::string var_name = name;
    ToLower(var_name);

    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SetCmd>(scope, SetVarType::kBool, var_name, value);
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::SetVariableOrConfig(const std::string &name, i64 value, SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::string var_name = name;
    ToLower(var_name);

    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SetCmd>(scope, SetVarType::kInteger, var_name, value);
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::SetVariableOrConfig(const std::string &name, double value, SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::string var_name = name;
    ToLower(var_name);

    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SetCmd>(scope, SetVarType::kDouble, var_name, value);
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::SetVariableOrConfig(const std::string &name, std::string value, SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::string var_name = name;
    ToLower(var_name);

    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SetCmd>(scope, SetVarType::kString, var_name, value);
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::ShowVariable(const std::string &variable_name, SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->var_name_ = variable_name;
    ToLower(show_statement->var_name_);
    switch (scope) {
        case SetScope::kGlobal: {
            show_statement->show_type_ = ShowStmtType::kGlobalVariable;
            break;
        }
        case SetScope::kSession: {
            show_statement->show_type_ = ShowStmtType::kSessionVariable;
            break;
        }
        default: {
            UnrecoverableError("Invalid set scope.");
        }
    }

    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowVariables(SetScope scope) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    switch (scope) {
        case SetScope::kGlobal: {
            show_statement->show_type_ = ShowStmtType::kGlobalVariables;
            break;
        }
        case SetScope::kSession: {
            show_statement->show_type_ = ShowStmtType::kSessionVariables;
            break;
        }
        default: {
            UnrecoverableError("Invalid set scope.");
        }
    }

    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowConfig(const std::string &config_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->var_name_ = config_name;
    ToLower(show_statement->var_name_);

    show_statement->show_type_ = ShowStmtType::kConfig;

    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowConfigs() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kConfigs;

    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::CreateTable(const std::string &db_name,
                                  const std::string &table_name,
                                  std::vector<ColumnDef *> column_defs,
                                  std::vector<TableConstraint *> constraints,
                                  CreateTableOptions create_table_options) {
    DeferFn free_create_table([&]() {
        for (auto &column_def : column_defs) {
            delete column_def;
            column_def = nullptr;
        }
        for (auto &constraint : constraints) {
            delete constraint;
            constraint = nullptr;
        }
    });

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<CreateStatement> create_statement = std::make_unique<CreateStatement>();
    std::shared_ptr<CreateTableInfo> create_table_info = std::make_shared<CreateTableInfo>();
    create_table_info->schema_name_ = db_name;
    ToLower(create_table_info->schema_name_);

    create_table_info->table_name_ = table_name;
    ToLower(create_table_info->table_name_);

    create_table_info->column_defs_ = std::move(column_defs);
    for (ColumnDef *column_def_ptr : create_table_info->column_defs_) {
        ToLower(column_def_ptr->name_);
    }
    create_table_info->constraints_ = std::move(constraints);
    create_table_info->conflict_type_ = create_table_options.conflict_type_;
    create_table_info->properties_ = std::move(create_table_options.properties_);
    for (InitParameter *parameter_ptr : create_table_info->properties_) {
        ToLower(parameter_ptr->param_name_);
        ToLower(parameter_ptr->param_value_);
    }
    create_statement->create_info_ = std::move(create_table_info);
    QueryResult result = query_context_ptr->QueryStatement(create_statement.get());
    return result;
}

QueryResult Infinity::DropTable(const std::string &db_name, const std::string &table_name, const DropTableOptions &options) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<DropStatement> drop_statement = std::make_unique<DropStatement>();
    std::shared_ptr<DropTableInfo> drop_table_info = std::make_shared<DropTableInfo>();
    drop_table_info->schema_name_ = db_name;
    ToLower(drop_table_info->schema_name_);

    drop_table_info->table_name_ = table_name;
    ToLower(drop_table_info->table_name_);

    drop_table_info->conflict_type_ = options.conflict_type_;
    drop_statement->drop_info_ = drop_table_info;
    QueryResult result = query_context_ptr->QueryStatement(drop_statement.get());
    return result;
}

QueryResult Infinity::ListTables(const std::string &db_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);
    show_statement->show_type_ = ShowStmtType::kTables;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowTable(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->show_type_ = ShowStmtType::kTable;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowColumns(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->show_type_ = ShowStmtType::kColumns;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowTables(const std::string &db_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->show_type_ = ShowStmtType::kTables;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::GetTable(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr = std::make_unique<QueryContext>(session_.get());
    query_context_ptr->set_current_schema(db_name);
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager(),
                            InfinityContext::instance().persistence_manager());
    std::unique_ptr<CommandStatement> command_statement = std::make_unique<CommandStatement>();

    std::string table_name_internal = table_name;
    ToLower(table_name_internal);

    command_statement->command_info_ = std::make_unique<CheckTable>(table_name_internal.c_str());
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::ListTableIndexes(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->show_type_ = ShowStmtType::kIndexes;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::CreateIndex(const std::string &db_name,
                                  const std::string &table_name,
                                  const std::string &index_name,
                                  const std::string &index_comment,
                                  IndexInfo *index_info_ptr,
                                  const CreateIndexOptions &create_index_options) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    std::unique_ptr<CreateStatement> create_statement = std::make_unique<CreateStatement>();
    std::shared_ptr<CreateIndexInfo> create_index_info = std::make_shared<CreateIndexInfo>();

    create_index_info->schema_name_ = db_name;
    ToLower(create_index_info->schema_name_);

    create_index_info->table_name_ = table_name;
    ToLower(create_index_info->table_name_);

    create_index_info->index_name_ = index_name;
    ToLower(create_index_info->index_name_);

    create_index_info->comment_ = index_comment;
    ToLower(create_index_info->comment_);

    ToLower(index_info_ptr->column_name_);
    for (InitParameter *init_param_ptr : *index_info_ptr->index_param_list_) {
        ToLower(init_param_ptr->param_name_);
        ToLower(init_param_ptr->param_value_);
    }

    create_index_info->index_info_ = index_info_ptr;

    create_statement->create_info_ = create_index_info;
    create_statement->create_info_->conflict_type_ = create_index_options.conflict_type_;

    QueryResult result = query_context_ptr->QueryStatement(create_statement.get());
    return result;
}

QueryResult
Infinity::DropIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name, const DropIndexOptions &drop_index_options) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<DropStatement> drop_statement = std::make_unique<DropStatement>();
    std::shared_ptr<DropIndexInfo> drop_index_info = std::make_shared<DropIndexInfo>();

    drop_index_info->schema_name_ = db_name;
    ToLower(drop_index_info->schema_name_);

    drop_index_info->table_name_ = table_name;
    ToLower(drop_index_info->table_name_);

    drop_index_info->index_name_ = index_name;
    ToLower(drop_index_info->index_name_);

    drop_statement->drop_info_ = drop_index_info;

    drop_statement->drop_info_->conflict_type_ = drop_index_options.conflict_type_;

    QueryResult result = query_context_ptr->QueryStatement(drop_statement.get());
    return result;
}

QueryResult Infinity::ShowIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    std::string index_name_internal = index_name;
    ToLower(index_name_internal);

    show_statement->index_name_ = index_name_internal;

    show_statement->show_type_ = ShowStmtType::kIndex;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowIndexSegment(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    std::string index_name_internal = index_name;
    ToLower(index_name_internal);

    show_statement->index_name_ = index_name_internal;

    show_statement->segment_id_ = segment_id;
    show_statement->show_type_ = ShowStmtType::kIndexSegment;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult
Infinity::ShowIndexChunk(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id, ChunkID chunk_id) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    std::string index_name_internal = index_name;
    ToLower(index_name_internal);

    show_statement->index_name_ = index_name_internal;

    show_statement->segment_id_ = segment_id;
    show_statement->chunk_id_ = chunk_id;
    show_statement->show_type_ = ShowStmtType::kIndexChunk;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowSegment(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->segment_id_ = segment_id;
    show_statement->show_type_ = ShowStmtType::kSegment;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowSegments(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->show_type_ = ShowStmtType::kSegments;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowBlock(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id, const BlockID &block_id) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->segment_id_ = segment_id;
    show_statement->block_id_ = block_id;
    show_statement->show_type_ = ShowStmtType::kBlock;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowBlocks(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->segment_id_ = segment_id;
    show_statement->show_type_ = ShowStmtType::kBlocks;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowBlockColumn(const std::string &db_name,
                                      const std::string &table_name,
                                      const SegmentID &segment_id,
                                      const BlockID &block_id,
                                      const size_t &column_id) {

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->schema_name_ = db_name;
    ToLower(show_statement->schema_name_);

    show_statement->table_name_ = table_name;
    ToLower(show_statement->table_name_);

    show_statement->segment_id_ = segment_id;
    show_statement->block_id_ = block_id;
    show_statement->column_id_ = column_id;
    show_statement->show_type_ = ShowStmtType::kBlockColumn;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowBuffer() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kBuffer;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowProfiles() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kProfiles;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowMemindex() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kMemIndex;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowQueries() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kQueries;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowQuery(u64 query_index) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kQueries;
    show_statement->session_id_ = query_index;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowTransactions() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kQueries;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowLogs() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kLogs;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowObjects() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kPersistenceObjects;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowObject(const std::string &filename) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kPersistenceObject;
    show_statement->file_name_ = filename;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowFilesInObject() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kPersistenceFiles;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowMemory() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kMemory;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowMemoryObjects() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kMemoryObjects;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowMemoryAllocations() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kMemoryAllocation;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ShowFunction(const std::string &function_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ShowStatement> show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kFunction;
    show_statement->function_name_ = function_name;
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::Insert(const std::string &db_name, const std::string &table_name, std::vector<InsertRowExpr *> *&insert_rows) {
    DeferFn free_insert_rows([&]() {
        if (insert_rows != nullptr) {
            for (auto &insert_row : *insert_rows) {
                if (insert_row != nullptr) {
                    delete insert_row;
                    insert_row = nullptr;
                }
            }
            delete insert_rows;
            insert_rows = nullptr;
        }
    });
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<InsertStatement> insert_statement = std::make_unique<InsertStatement>();
    insert_statement->schema_name_ = db_name;
    ToLower(insert_statement->schema_name_);
    insert_statement->table_name_ = table_name;
    ToLower(insert_statement->table_name_);
    insert_statement->insert_rows_.reserve(insert_rows->size());
    for (auto &insert_row_expr_ptr : *insert_rows) {
        for (auto &column_name : insert_row_expr_ptr->columns_) {
            ToLower(column_name);
        }
        insert_statement->insert_rows_.emplace_back(insert_row_expr_ptr);
        insert_row_expr_ptr = nullptr;
    }
    delete insert_rows;
    insert_rows = nullptr;
    QueryResult result = query_context_ptr->QueryStatement(insert_statement.get());
    return result;
}

QueryResult Infinity::Import(const std::string &db_name, const std::string &table_name, const std::string &path, ImportOptions import_options) {

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<CopyStatement> import_statement = std::make_unique<CopyStatement>();

    import_statement->copy_from_ = true;
    import_statement->file_path_ = path;

    import_statement->schema_name_ = db_name;
    ToLower(import_statement->schema_name_);

    import_statement->table_name_ = table_name;
    ToLower(import_statement->table_name_);

    import_statement->header_ = import_options.header_;
    import_statement->copy_file_type_ = import_options.copy_file_type_;
    import_statement->delimiter_ = import_options.delimiter_;

    QueryResult result = query_context_ptr->QueryStatement(import_statement.get());
    return result;
}

QueryResult
Infinity::Export(const std::string &db_name, const std::string &table_name, std::vector<ParsedExpr *> *columns, const std::string &path, ExportOptions export_options) {
    DeferFn free_column_expressions([&]() {
        if (columns != nullptr) {
            for (auto &column_expr : *columns) {
                delete column_expr;
                column_expr = nullptr;
            }
            delete columns;
            columns = nullptr;
        }
    });

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<CopyStatement> export_statement = std::make_unique<CopyStatement>();

    export_statement->copy_from_ = false;
    export_statement->file_path_ = path;

    export_statement->schema_name_ = db_name;
    ToLower(export_statement->schema_name_);

    export_statement->table_name_ = table_name;
    ToLower(export_statement->table_name_);

    export_statement->expr_array_ = columns;

    export_statement->header_ = export_options.header_;
    export_statement->copy_file_type_ = export_options.copy_file_type_;
    export_statement->delimiter_ = export_options.delimiter_;
    export_statement->offset_ = export_options.offset_;
    export_statement->limit_ = export_options.limit_;
    export_statement->row_limit_ = export_options.row_limit_;

    columns = nullptr;

    QueryResult result = query_context_ptr->QueryStatement(export_statement.get());

    return result;
}

QueryResult Infinity::Delete(const std::string &db_name, const std::string &table_name, ParsedExpr *filter) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<DeleteStatement> delete_statement = std::make_unique<DeleteStatement>();

    delete_statement->schema_name_ = db_name;
    ToLower(delete_statement->schema_name_);

    delete_statement->table_name_ = table_name;
    ToLower(delete_statement->table_name_);

    // TODO: to lower expression identifier string
    delete_statement->where_expr_ = filter;
    QueryResult result = query_context_ptr->QueryStatement(delete_statement.get());
    return result;
}

QueryResult Infinity::Update(const std::string &db_name, const std::string &table_name, ParsedExpr *filter, std::vector<UpdateExpr *> *update_list) {
    DeferFn free_update_list([&]() {
        if (update_list != nullptr) {
            for (auto &update_expr : *update_list) {
                delete update_expr;
                update_expr = nullptr;
            }
            delete update_list;
            update_list = nullptr;
        }
    });

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<UpdateStatement> update_statement = std::make_unique<UpdateStatement>();

    update_statement->schema_name_ = db_name;
    update_statement->table_name_ = table_name;

    // TODO: to lower expression identifier string
    update_statement->where_expr_ = filter;
    update_statement->update_expr_array_ = update_list;
    for (UpdateExpr *update_expr_ptr : *update_statement->update_expr_array_) {
        ToLower(update_expr_ptr->column_name);
    }

    update_list = nullptr;
    QueryResult result = query_context_ptr->QueryStatement(update_statement.get());

    return result;
}

QueryResult Infinity::Explain(const std::string &db_name,
                              const std::string &table_name,
                              ExplainType explain_type,
                              SearchExpr *&search_expr,
                              ParsedExpr *filter,
                              ParsedExpr *limit,
                              ParsedExpr *offset,
                              std::vector<ParsedExpr *> *output_columns,
                              std::vector<ParsedExpr *> *highlight_columns,
                              std::vector<OrderByExpr *> *order_by_list,
                              std::vector<ParsedExpr *> *group_by_list,
                              ParsedExpr *having) {
    DeferFn free_output_columns([&]() {
        if (output_columns != nullptr) {
            for (auto &output_column : *output_columns) {
                delete output_column;
                output_column = nullptr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });
    DeferFn free_highlight_columns([&]() {
        if (highlight_columns != nullptr) {
            for (auto &highlight_column : *highlight_columns) {
                delete highlight_column;
                highlight_column = nullptr;
            }
            delete highlight_columns;
            highlight_columns = nullptr;
        }
    });
    DeferFn free_order_by_list([&]() {
        if (order_by_list != nullptr) {
            for (auto &order_by : *order_by_list) {
                delete order_by;
                order_by = nullptr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });
    DeferFn free_group_by_list([&]() {
        if (group_by_list != nullptr) {
            for (auto &group_by : *group_by_list) {
                delete group_by;
                group_by = nullptr;
            }
            delete group_by_list;
            group_by_list = nullptr;
        }
    });

    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<ExplainStatement> explain_statement = std::make_unique<ExplainStatement>();
    explain_statement->type_ = explain_type;

    SelectStatement *select_statement = new SelectStatement();

    auto *table_ref = new TableReference();

    table_ref->db_name_ = db_name;
    ToLower(table_ref->db_name_);

    table_ref->table_name_ = table_name;
    ToLower(table_ref->table_name_);

    select_statement->table_ref_ = table_ref;

    // TODO: to lower expression identifier string
    select_statement->select_list_ = output_columns;
    select_statement->highlight_list_ = highlight_columns;
    select_statement->where_expr_ = filter;
    select_statement->search_expr_ = search_expr;
    select_statement->limit_expr_ = limit;
    select_statement->offset_expr_ = offset;
    select_statement->order_by_list_ = order_by_list;
    select_statement->group_by_list_ = group_by_list;
    select_statement->having_expr_ = having;

    explain_statement->statement_ = select_statement;

    output_columns = nullptr;
    highlight_columns = nullptr;
    order_by_list = nullptr;
    group_by_list = nullptr;
    search_expr = nullptr;

    QueryResult result = query_context_ptr->QueryStatement(explain_statement.get());

    return result;
}

QueryResult Infinity::Search(const std::string &db_name,
                             const std::string &table_name,
                             SearchExpr *&search_expr,
                             ParsedExpr *filter,
                             ParsedExpr *limit,
                             ParsedExpr *offset,
                             std::vector<ParsedExpr *> *&output_columns,
                             std::vector<ParsedExpr *> *highlight_columns,
                             std::vector<OrderByExpr *> *order_by_list,
                             std::vector<ParsedExpr *> *group_by_list,
                             ParsedExpr *having,
                             bool total_hits_count_flag) {
    if (total_hits_count_flag) {
        if (limit == nullptr) {
            QueryResult query_result;
            query_result.result_table_ = nullptr;
            query_result.status_ = Status::InvalidQueryOption("'total_hits_count' is only valid when limit keyword is set");
            return query_result;
        }
    }

    DeferFn free_output_columns([&]() {
        if (output_columns != nullptr) {
            for (auto &output_column : *output_columns) {
                delete output_column;
                output_column = nullptr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });
    DeferFn free_highlight_columns([&]() {
        if (highlight_columns != nullptr) {
            for (auto &highlight_column : *highlight_columns) {
                delete highlight_column;
                highlight_column = nullptr;
            }
            delete highlight_columns;
            highlight_columns = nullptr;
        }
    });
    DeferFn free_order_by_list([&]() {
        if (order_by_list != nullptr) {
            for (auto &order_by : *order_by_list) {
                delete order_by;
                order_by = nullptr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });
    DeferFn free_group_by_list([&]() {
        if (group_by_list != nullptr) {
            for (auto &group_by : *group_by_list) {
                delete group_by;
                group_by = nullptr;
            }
            delete group_by_list;
            group_by_list = nullptr;
        }
    });
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<SelectStatement> select_statement = std::make_unique<SelectStatement>();

    auto *table_ref = new TableReference();

    table_ref->db_name_ = db_name;
    ToLower(table_ref->db_name_);

    table_ref->table_name_ = table_name;
    ToLower(table_ref->table_name_);

    select_statement->table_ref_ = table_ref;

    // TODO: to lower expression identifier string
    select_statement->select_list_ = output_columns;
    select_statement->highlight_list_ = highlight_columns;
    select_statement->where_expr_ = filter;
    select_statement->search_expr_ = search_expr;
    select_statement->limit_expr_ = limit;
    select_statement->offset_expr_ = offset;
    select_statement->order_by_list_ = order_by_list;
    select_statement->group_by_list_ = group_by_list;
    select_statement->having_expr_ = having;
    select_statement->total_hits_count_flag_ = total_hits_count_flag;

    output_columns = nullptr;
    highlight_columns = nullptr;
    order_by_list = nullptr;
    group_by_list = nullptr;
    search_expr = nullptr;

    QueryResult result = query_context_ptr->QueryStatement(select_statement.get());

    return result;
}

QueryResult Infinity::Optimize(const std::string &db_name, const std::string &table_name, OptimizeOptions optimize_option) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);
    std::unique_ptr<OptimizeStatement> optimize_statement = std::make_unique<OptimizeStatement>();

    optimize_statement->schema_name_ = db_name;
    ToLower(optimize_statement->schema_name_);

    optimize_statement->table_name_ = table_name;
    ToLower(optimize_statement->table_name_);

    if (!optimize_option.index_name_.empty()) {
        optimize_statement->index_name_ = std::move(optimize_option.index_name_);
        ToLower(optimize_statement->index_name_);
        for (auto *param_ptr : optimize_option.opt_params_) {
            auto param = std::make_unique<InitParameter>(std::move(param_ptr->param_name_), std::move(param_ptr->param_value_));
            optimize_statement->opt_params_.push_back(std::move(param));
            delete param_ptr;
        }
    }

    QueryResult result = query_context_ptr->QueryStatement(optimize_statement.get());
    return result;
}

QueryResult Infinity::AddColumns(const std::string &db_name, const std::string &table_name, std::vector<std::shared_ptr<ColumnDef>> column_defs) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto add_columns_statement = std::make_unique<AddColumnsStatement>(db_name.c_str(), table_name.c_str());
    add_columns_statement->column_defs_ = std::move(column_defs);

    QueryResult result = query_context_ptr->QueryStatement(add_columns_statement.get());
    return result;
}

QueryResult Infinity::DropColumns(const std::string &db_name, const std::string &table_name, std::vector<std::string> column_names) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto drop_columns_statement = std::make_unique<DropColumnsStatement>(db_name.c_str(), table_name.c_str());
    drop_columns_statement->column_names_ = std::move(column_names);

    QueryResult result = query_context_ptr->QueryStatement(drop_columns_statement.get());
    return result;
}

QueryResult Infinity::Cleanup() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();

    command_statement->command_info_ = std::make_unique<CleanupCmd>();

    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::DumpIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();

    command_statement->command_info_ = std::make_unique<DumpIndexCmd>(db_name.c_str(), table_name.c_str(), index_name.c_str());

    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::ForceCheckpoint() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto flush_statement = std::make_unique<FlushStatement>();
    flush_statement->type_ = infinity::FlushType::kData;

    QueryResult result = query_context_ptr->QueryStatement(flush_statement.get());

    return result;
}

QueryResult Infinity::CompactTable(const std::string &db_name, const std::string &table_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto compact_statement = std::make_unique<ManualCompactStatement>(db_name, table_name);

    QueryResult result = query_context_ptr->QueryStatement(compact_statement.get());

    return result;
}

QueryResult Infinity::TestCommand(const std::string &command_content) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<TestCmd>(command_content);

    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());

    return result;
}

QueryResult Infinity::CreateTableSnapshot(const std::string &db_name, const std::string &table_name, const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kCreate, SnapshotScope::kTable, table_name);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::CreateDatabaseSnapshot(const std::string &db_name, const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kCreate, SnapshotScope::kDatabase, db_name);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::CreateSystemSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kCreate, SnapshotScope::kSystem);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::RestoreTableSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kRestore, SnapshotScope::kTable);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::RestoreDatabaseSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kRestore, SnapshotScope::kDatabase);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::RestoreSystemSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kRestore, SnapshotScope::kSystem);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::DropSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto command_statement = std::make_unique<CommandStatement>();
    command_statement->command_info_ = std::make_unique<SnapshotCmd>(snapshot_name, SnapshotOp::kDrop, SnapshotScope::kIgnore);
    
    QueryResult result = query_context_ptr->QueryStatement(command_statement.get());
    return result;
}

QueryResult Infinity::ShowSnapshot(const std::string &snapshot_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kShowSnapshot;
    show_statement->snapshot_name_ = snapshot_name;
    
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::ListSnapshots() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto show_statement = std::make_unique<ShowStatement>();
    show_statement->show_type_ = ShowStmtType::kListSnapshots;
    
    QueryResult result = query_context_ptr->QueryStatement(show_statement.get());
    return result;
}

QueryResult Infinity::AdminShowLogs() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kListLogFiles;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowLog(i64 index) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kShowLogFile;
    admin_statement->log_file_index_ = index;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowConfigs() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kListConfigs;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowVariables() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kListVariables;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowVariable(std::string var_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kShowVariable;
    ToLower(var_name);
    admin_statement->variable_name_ = var_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowNodes() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kListNodes;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowNode(std::string node_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true, true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kShowNode;
    ToLower(node_name);
    admin_statement->node_name_ = node_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminShowCurrentNode() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kShowCurrentNode;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminRemoveNode(std::string node_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    ToLower(node_name);
    admin_statement->admin_type_ = AdminStmtType::kRemoveNode;
    admin_statement->node_name_ = node_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminSetAdmin() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kSetRole;
    admin_statement->node_role_ = NodeRole::kAdmin;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminSetStandalone() {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kSetRole;
    admin_statement->node_role_ = NodeRole::kStandalone;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminSetLeader(std::string node_name) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kSetRole;
    admin_statement->node_role_ = NodeRole::kLeader;
    ToLower(node_name);
    admin_statement->node_name_ = node_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminSetFollower(std::string node_name, const std::string &leader_address) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kSetRole;
    admin_statement->node_role_ = NodeRole::kFollower;
    admin_statement->leader_address_ = leader_address;
    ToLower(node_name);
    admin_statement->node_name_ = node_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

QueryResult Infinity::AdminSetLearner(std::string node_name, const std::string &leader_address) {
    std::unique_ptr<QueryContext> query_context_ptr;
    GET_QUERY_CONTEXT(GetQueryContext(true), query_context_ptr);

    auto admin_statement = std::make_unique<AdminStatement>();
    admin_statement->admin_type_ = AdminStmtType::kSetRole;
    admin_statement->node_role_ = NodeRole::kLearner;
    admin_statement->leader_address_ = leader_address;
    ToLower(node_name);
    admin_statement->node_name_ = node_name;
    QueryResult result = query_context_ptr->QueryStatement(admin_statement.get());
    return result;
}

} // namespace infinity
