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

module infinity_core:status.impl;

import :default_values;
import :status;

namespace infinity {

Status::Status(ErrorCode code, const char *msg) : code_(code) { msg_ = std::make_unique<std::string>(msg); }

Status::Status(Status &s) { MoveStatus(s); }

Status::Status(Status &&s) noexcept { MoveStatus(s); }

Status &Status::operator=(Status &s) noexcept {
    MoveStatus(s);
    return *this;
}

Status &Status::operator=(Status &&s) noexcept {
    MoveStatus(s);
    return *this;
}

void Status::MoveStatus(Status &s) {
    code_ = s.code_;
    msg_ = std::move(s.msg_);
}

void Status::MoveStatus(Status &&s) {
    code_ = s.code_;
    msg_ = std::move(s.msg_);
}

void Status::Init(ErrorCode code, const char *msg) {
    if (msg_.get() != nullptr) {
        msg_.reset();
    }
    msg_ = std::make_unique<std::string>(msg);
    code_ = code;
}

void Status::AppendMessage(const std::string &msg) {
    if (msg_.get() != nullptr) {
        msg_->append(msg);
    } else {
        msg_ = std::make_unique<std::string>(msg);
    }
}

const char *Status::message() const {
    if (msg_.get() != nullptr) {
        return msg_->c_str();
    } else {
        return nullptr;
    }
}

// Error functions

Status Status::Ignore() { return Status(ErrorCode::kIgnore); }

// 1. Config error
Status Status::InvalidTimeInfo(const std::string &time_info) {
    return Status(ErrorCode::kInvalidTimeInfo, std::make_unique<std::string>(fmt::format("Invalid time info format: {}", time_info)));
}

Status Status::EmptyConfigParameter() { return Status(ErrorCode::kEmptyConfigParameter, std::make_unique<std::string>("Empty configure parameter.")); }

Status Status::MismatchVersion(const std::string &current_version, const std::string &expected_version) {
    return Status(ErrorCode::kMismatchVersion,
                  std::make_unique<std::string>(fmt::format("Current infinity version: {}, expected version: {}", current_version, expected_version)));
}

Status Status::InvalidTimezone(const std::string &timezone) {
    return Status(ErrorCode::kInvalidTimezone, std::make_unique<std::string>(fmt::format("Invalid time zone: {}.", timezone)));
}

Status Status::InvalidByteSize(const std::string &byte_size) {
    return Status(ErrorCode::kInvalidByteSize, std::make_unique<std::string>(fmt::format("Invalid byte size: {}.", byte_size)));
}

Status Status::InvalidIPAddr(const std::string &ip_addr) {
    return Status(ErrorCode::kInvalidIPAddr, std::make_unique<std::string>(fmt::format("Invalid ip address: {}.", ip_addr)));
}

Status Status::InvalidLogLevel(const std::string &log_level) {
    return Status(ErrorCode::kInvalidLogLevel, std::make_unique<std::string>(fmt::format("Invalid log level: {}.", log_level)));
}

Status Status::InvalidConfig(const std::string &detailed_info) { return Status(ErrorCode::kInvalidConfig, std::make_unique<std::string>(detailed_info)); }

Status Status::InvalidMemIndex(const std::string &invalid_info) { return Status(ErrorCode::kInvalidMemIndex, std::make_unique<std::string>(invalid_info)); }

// 2. Auth error
Status Status::WrongPasswd(const std::string &user_name) {
    return Status(ErrorCode::kWrongPasswd, std::make_unique<std::string>(fmt::format("Invalid password to login user: {}", user_name)));
}

Status Status::InsufficientPrivilege(const std::string &user_name, const std::string &detailed_error) {
    return Status(ErrorCode::kInsufficientPrivilege, std::make_unique<std::string>(fmt::format("{} do not have permission to {}", user_name, detailed_error)));
}

Status Status::UnsupportedVersionIndex(i64 given_index) {
    return Status(
        ErrorCode::kUnsupportedVersionIndex,
        std::make_unique<std::string>(fmt::format(
            "Index: {} isn't supported, you are using a deprecated version of Python SDK. Please install the corresponding version Python SDK.",
            given_index)));
}

Status Status::ClientVersionMismatch(const char *expected_version, const char *given_version) {
    return Status(ErrorCode::kClientVersionMismatch,
                  std::make_unique<std::string>(fmt::format("Expected version index: {}, connecting version: {}", expected_version, given_version)));
}

Status Status::AdminOnlySupportInMaintenanceMode() {
    return Status(ErrorCode::kAdminOnlySupportInMaintenanceMode, std::make_unique<std::string>("Only maintenance mode supports ADMIN command"));
}

Status Status::NotSupportInMaintenanceMode() {
    return Status(ErrorCode::kAdminOnlySupportInMaintenanceMode, std::make_unique<std::string>("Not support in maintenance mode"));
}

Status Status::InfinityIsStarting() { return Status(ErrorCode::kInfinityIsStarting, std::make_unique<std::string>("Infinity is starting")); }

Status Status::InfinityIsIniting() { return Status(ErrorCode::kInfinityIsIniting, std::make_unique<std::string>("Infinity is initing")); }

// 3. Syntax error or access rule violation
Status Status::InvalidUserName(const std::string &user_name) {
    return Status(ErrorCode::kInvalidUsername, std::make_unique<std::string>(fmt::format("{} is a invalid user name", user_name)));
}

Status Status::InvalidPasswd() { return Status(ErrorCode::kInvalidPasswd, std::make_unique<std::string>(fmt::format("Invalid password"))); }

Status Status::InvalidIdentifierName(const std::string &db_name) {
    return Status(ErrorCode::kInvalidIdentifierName, std::make_unique<std::string>(fmt::format("{} is a invalid identifier name", db_name)));
}

Status Status::InvalidTableName(const std::string &table_name) {
    return Status(ErrorCode::kInvalidTableName, std::make_unique<std::string>(fmt::format("{} is a invalid table name", table_name)));
}

Status Status::InvalidColumnName(const std::string &column_name) {
    return Status(ErrorCode::kInvalidColumnName, std::make_unique<std::string>(fmt::format("{} is a invalid column name", column_name)));
}

Status Status::InvalidIndexName(const std::string &index_name) {
    return Status(ErrorCode::kInvalidIndexName, std::make_unique<std::string>(fmt::format("{} is a invalid index name", index_name)));
}

Status Status::InvalidColumnDefinition(const std::string &detailed_info) {
    return Status(ErrorCode::kInvalidColumnDefinition, std::make_unique<std::string>(detailed_info));
}

Status Status::InvalidTableDefinition(const std::string &detailed_info) {
    return Status(ErrorCode::kInvalidTableDefinition, std::make_unique<std::string>(detailed_info));
}

Status Status::InvalidIndexDefinition(const std::string &detailed_info) {
    return Status(ErrorCode::kInvalidIndexDefinition, std::make_unique<std::string>(detailed_info));
}

Status Status::DataTypeMismatch(const std::string &type1, const std::string &type2) {
    return Status(ErrorCode::kDataTypeMismatch, std::make_unique<std::string>(fmt::format("Expected: {}, but {} is given.", type1, type2)));
}
Status Status::NameTooLong(const std::string &name, const std::string &object_type) {
    return Status(ErrorCode::kNameTooLong, std::make_unique<std::string>(fmt::format("{} is too long for a {} name", name, object_type)));
}

Status Status::ReservedName(const std::string &name) {
    return Status(ErrorCode::kReservedName, std::make_unique<std::string>(fmt::format("{} is a reserved name", name)));
}

Status Status::SyntaxError(const std::string &detailed_info) {
    return Status(ErrorCode::kSyntaxError, std::make_unique<std::string>(fmt::format("{}", detailed_info)));
}

Status Status::InvalidParameterValue(const std::string &parameter_name, const std::string &parameter_value, const std::string &recommend_value) {
    return Status(ErrorCode::kInvalidParameterValue,
                  std::make_unique<std::string>(fmt::format("{}: {} is invalid, Recommended value: {}", parameter_name, parameter_value, recommend_value)));
}

Status Status::DuplicateUserName(const std::string &user_name) {
    return Status(ErrorCode::kDuplicateUser, std::make_unique<std::string>(fmt::format("User: {} is already existed", user_name)));
}

Status Status::DuplicateDatabase(const std::string &db_name) {
    return Status(ErrorCode::kDuplicateDatabaseName, std::make_unique<std::string>(fmt::format("Database: {} is already existed", db_name)));
}

Status Status::DuplicateTable(const std::string &table_name) {
    return Status(ErrorCode::kDuplicateTableName, std::make_unique<std::string>(fmt::format("Table: {} is already existed", table_name)));
}

Status Status::DuplicateIndex(const std::string &index_name) {
    return Status(ErrorCode::kDuplicateIndexName, std::make_unique<std::string>(fmt::format("Index: {} is already existed", index_name)));
}

Status Status::DuplicateIndexOnColumn(const std::string &table_name, const std::string &column_name) {
    return Status(ErrorCode::kDuplicateIndexOnColumn,
                  std::make_unique<std::string>(fmt::format("Index: {} is created on column : {} with same parameters", table_name, column_name)));
}

Status Status::NoUser(const std::string &user_name) {
    return Status(ErrorCode::kNoSuchUser, std::make_unique<std::string>(fmt::format("No such user: {}", user_name)));
}

Status Status::DBNotExist(const std::string &db_name) {
    return Status(ErrorCode::kDBNotExist, std::make_unique<std::string>(fmt::format("Database: {} doesn't exist.", db_name)));
}

Status Status::TableNotExist(const std::string &table_name) {
    return Status(ErrorCode::kTableNotExist, std::make_unique<std::string>(fmt::format("Table: {} doesn't exist.", table_name)));
}

Status Status::IndexNotExist(const std::string &index_name) {
    return Status(ErrorCode::kIndexNotExist, std::make_unique<std::string>(fmt::format("Index: {} doesn't exist.", index_name)));
}

Status Status::ColumnNotExist(const std::string &column_name) {
    return Status(ErrorCode::kColumnNotExist, std::make_unique<std::string>(fmt::format("Column: {} doesn't exist", column_name)));
}

Status Status::ColumnNotExist(const ColumnID &column_idx) {
    return Status(ErrorCode::kColumnNotExist, std::make_unique<std::string>(fmt::format("Column: {} column doesn't exist", column_idx)));
}

Status Status::AggNotAllowInWhere(const std::string &func_name) {
    return Status(ErrorCode::kAggNotAllowInWhereClause,
                  std::make_unique<std::string>(fmt::format("Aggregate function: {} isn't allowed in where clause", func_name)));
}

Status Status::ColumnInSelectNotInGroupBy(const std::string &column_name) {
    return Status(ErrorCode::kColumnInSelectNotInGroupBy,
                  std::make_unique<std::string>(fmt::format("Column: {} must appear in the GROUP BY clause or be used in an aggregated function", column_name)));
}

Status Status::NoSysVar(const std::string &variable_name) {
    return Status(ErrorCode::kNoSuchSystemVar, std::make_unique<std::string>(fmt::format("No such system variable: {}", variable_name)));
}

Status Status::SetInvalidVarValue(const std::string &variable_name, const std::string &valid_value_range) {
    return Status(ErrorCode::kInvalidSystemVarValue, std::make_unique<std::string>(fmt::format("{} value range is {}", variable_name, valid_value_range)));
}

Status Status::ReadOnlySysVar(const std::string &sys_var) {
    return Status(ErrorCode::kSystemVarReadOnly, std::make_unique<std::string>(fmt::format("{} is read-only", sys_var)));
}

Status Status::FunctionNotFound(const std::string &function_name) {
    return Status(ErrorCode::kFunctionNotFound, std::make_unique<std::string>(fmt::format("{} not found", function_name)));
}

Status Status::SpecialFunctionNotFound() { return Status(ErrorCode::kSpecialFunctionNotFound); }

Status Status::NotSupport(const std::string &detailed_info) { return Status(ErrorCode::kNotSupported, std::make_unique<std::string>(detailed_info)); }

Status Status::DroppingUsingDb(const std::string &db_name) {
    return Status(ErrorCode::kDroppingUsingDb, std::make_unique<std::string>(fmt::format("Can't drop using database: {}", db_name)));
}

Status Status::SessionNotFound(i64 session_id) {
    return Status(ErrorCode::kSessionNotFound, std::make_unique<std::string>(fmt::format("Session id: {} isn't found", session_id)));
}

Status Status::RecursiveAggregate(const std::string &expr_name) {
    return Status(ErrorCode::kRecursiveAgg, std::make_unique<std::string>(fmt::format("{} is in another aggregate expression", expr_name)));
}

Status Status::FunctionArgsError(const std::string &func_name) {
    return Status(ErrorCode::kFunctionArgsError, std::make_unique<std::string>(fmt::format("{} arguments have errors", func_name)));
}

Status Status::ImportFileFormatError(const std::string &detailed_info) {
    return Status(ErrorCode::kImportFileFormatError, std::make_unique<std::string>(fmt::format("Import file format error: {}", detailed_info)));
}

Status Status::DataNotExist(const std::string &detailed_info) {
    return Status(ErrorCode::kImportFileFormatError, std::make_unique<std::string>(fmt::format("Data not exist: {}", detailed_info)));
}

Status Status::EmptyDBName() { return Status(ErrorCode::kEmptyDbName, std::make_unique<std::string>("Empty database name.")); }

Status Status::EmptyTableName() { return Status(ErrorCode::kEmptyTableName, std::make_unique<std::string>("Empty table name.")); }

Status Status::EmptyColumnName() { return Status(ErrorCode::kEmptyColumnName, std::make_unique<std::string>("Empty column name.")); }

Status Status::EmptyIndexName() { return Status(ErrorCode::kEmptyIndexName, std::make_unique<std::string>("Empty index name.")); }

Status Status::ExceedDBNameLength(u64 db_name_length) {
    return Status(ErrorCode::kExceedDBNameLength,
                  std::make_unique<std::string>(fmt::format("Given database name length exceeds {}", MAX_IDENTIFIER_NAME_LENGTH)));
}

Status Status::ExceedTableNameLength(u64 table_name_length) {
    return Status(ErrorCode::kExceedTableNameLength,
                  std::make_unique<std::string>(fmt::format("Given table name length exceeds {}", MAX_IDENTIFIER_NAME_LENGTH)));
}

Status Status::ExceedColumnNameLength(u64 column_name_length) {
    return Status(ErrorCode::kExceedColumnNameLength,
                  std::make_unique<std::string>(fmt::format("Given column name length exceeds {}", MAX_IDENTIFIER_NAME_LENGTH)));
}

Status Status::ExceedIndexNameLength(u64 index_name_length) {
    return Status(ErrorCode::kExceedIndexNameLength,
                  std::make_unique<std::string>(fmt::format("Given index name length exceeds {}", MAX_IDENTIFIER_NAME_LENGTH)));
}

Status Status::NoColumnDefined(const std::string &table_name) {
    return Status(ErrorCode::kNoColumnDefined, std::make_unique<std::string>(fmt::format("Try to define Table {} without any column.", table_name)));
}

Status Status::NotSupportedTypeConversion(const std::string &from_type, const std::string &to_type) {
    return Status(ErrorCode::kNotSupportedTypeConversion, std::make_unique<std::string>(fmt::format("Not support to convert {} to {}", from_type, to_type)));
}

Status Status::EmptySelectFields() { return Status(ErrorCode::kEmptySelectFields, std::make_unique<std::string>("Select fields are empty")); }

Status Status::InvalidDataType() { return Status(ErrorCode::kInvalidDataType, std::make_unique<std::string>("Invalid data type")); }

Status Status::ParseMatchExprFailed(const std::string &fields, const std::string &matching_text) {
    return Status(ErrorCode::kParseMatchExprFailed,
                  std::make_unique<std::string>(fmt::format("Trying to match: {} on fields: {} failed.", matching_text, fields)));
}

Status Status::FTSIndexNotExist(const std::string &table_name) {
    return Status(ErrorCode::kFTSIndexNotExist, std::make_unique<std::string>(fmt::format("Full text index of table: {} not exists.", table_name)));
}

Status Status::UnknownFTSFault() { return Status(ErrorCode::kUnknownFTSFault, std::make_unique<std::string>(fmt::format("Unknown full text index fault."))); }

Status Status::InvalidConstraintType() { return Status(ErrorCode::kInvalidConstraintType, std::make_unique<std::string>("Invalid constraint type.")); }

Status Status::InvalidKnnDistanceType() { return Status(ErrorCode::kInvalidKnnDistanceType, std::make_unique<std::string>("Invalid knn distance type.")); }

Status Status::InvalidEmbeddingDataType(const std::string &type_str) {
    return Status(ErrorCode::kInvalidEmbeddingDataType, std::make_unique<std::string>(fmt::format("Invalid embedding data type: {}.", type_str)));
}

Status Status::InvalidConstantType() { return Status(ErrorCode::kInvalidConstantType, std::make_unique<std::string>("Invalid constant type.")); }

Status Status::InvalidParsedExprType() { return Status(ErrorCode::kInvalidParsedExprType, std::make_unique<std::string>("Invalid parsed expression type.")); }

Status Status::InvalidIndexType(const std::string &message) {
    if (message.empty()) {
        return Status(ErrorCode::kInvalidIndexType, std::make_unique<std::string>("No index type is given"));
    } else {
        return Status(ErrorCode::kInvalidIndexType, std::make_unique<std::string>(fmt::format("Invalid index type: ", message)));
    }
}

Status Status::InvalidIndexParam(const std::string &param_name) {
    return Status(ErrorCode::kInvalidIndexParam, std::make_unique<std::string>(fmt::format("Invalid index parameter type: {}", param_name)));
}

Status Status::LackIndexParam() { return Status(ErrorCode::kLackIndexParam, std::make_unique<std::string>("Lack index parameter")); }

Status Status::InvalidFilterExpression(const std::string &expr_str) {
    return Status(ErrorCode::kInvalidFilterExpression,
                  std::make_unique<std::string>(fmt::format("Invalid expression in where clause: {} expression", expr_str)));
}

Status Status::MultipleFunctionMatched(const std::string &function, const std::string &functions) {
    return Status(ErrorCode::kMultipleFunctionMatched, std::make_unique<std::string>(fmt::format("{}: matched multiple functions: {}", function, functions)));
}

Status Status::InsertWithoutValues() { return Status(ErrorCode::kInsertWithoutValues, std::make_unique<std::string>("Insert into table without any values")); }

Status Status::InvalidConflictType() { return Status(ErrorCode::kInvalidConflictType, std::make_unique<std::string>("invalid conflict type")); }

Status Status::InvalidJsonFormat(const std::string &error_message) {
    return Status(ErrorCode::kInvalidJsonFormat, std::make_unique<std::string>(fmt::format("Invalid format json: {}", error_message)));
}

Status Status::DuplicateColumnName(const std::string &column_name) {
    return Status(ErrorCode::kDuplicateColumnName, std::make_unique<std::string>(fmt::format("Duplicated column name: {}", column_name)));
}

Status Status::InvalidExpression(const std::string &expr_str) { return Status(ErrorCode::kInvalidExpression, std::make_unique<std::string>(expr_str)); }

Status Status::SegmentNotExist(SegmentID segment_id) {
    return Status(ErrorCode::kSegmentNotExist, std::make_unique<std::string>(fmt::format("Segment: {} doesn't exist", segment_id)));
}

Status Status::BlockNotExist(BlockID block_id) {
    return Status(ErrorCode::kBlockNotExist, std::make_unique<std::string>(fmt::format("Block: {} doesn't exist", block_id)));
}

Status Status::AggregateFunctionWithEmptyArgs() {
    return Status(ErrorCode::kAggregateFunctionWithEmptyArgs, std::make_unique<std::string>("Aggregate function with empty arguments"));
}

Status Status::InvalidCommand(const std::string &detailed_error) {
    return Status(ErrorCode::kInvalidCommand, std::make_unique<std::string>(fmt::format("Invalid command: {}", detailed_error)));
}

Status Status::AnalyzerNotFound(const std::string &name) {
    return Status(ErrorCode::kAnalyzerNotFound, std::make_unique<std::string>(fmt::format("Analyzer {} isn't found", name)));
}

Status Status::NotSupportedAnalyzer(const std::string &name) {
    return Status(ErrorCode::kNotSupportedAnalyzer, std::make_unique<std::string>(fmt::format("Analyzer {} isn't supported", name)));
}

Status Status::InvalidAnalyzerName(const std::string &name) { return Status(ErrorCode::kInvalidAnalyzerName, std::make_unique<std::string>(name)); }

Status Status::InvalidAnalyzerFile(const std::string &detailed_info) {
    return Status(ErrorCode::kInvalidAnalyzerName, std::make_unique<std::string>(fmt::format("Invalid analyzer file path: {}", detailed_info)));
}

Status Status::ChunkNotExist(ChunkID chunk_id) {
    return Status(ErrorCode::kChunkNotExist, std::make_unique<std::string>(fmt::format("Index chunk: {} doesn't exist", chunk_id)));
}

Status Status::NameMismatched(const std::string &name_left, const std::string &name_right) {
    return Status(ErrorCode::kNameMismatched, std::make_unique<std::string>(fmt::format("It is {}, expects {}", name_left, name_right)));
}

Status Status::TransactionNotFound(TransactionID txn_id) {
    return Status(ErrorCode::kTransactionNotFound, std::make_unique<std::string>(fmt::format("Transaction {} isn't found", txn_id)));
}

Status Status::InvalidDatabaseIndex(u64 database_index, u64 capacity) {
    return Status(ErrorCode::kInvalidDatabaseIndex,
                  std::make_unique<std::string>(fmt::format("Invalid database index: {} (0-{})", database_index, capacity - 1)));
}

Status Status::InvalidTableIndex(u64 table_index, u64 capacity) {
    return Status(ErrorCode::kInvalidTableIndex, std::make_unique<std::string>(fmt::format("Invalid table index: {} (0-{})", table_index, capacity - 1)));
}

Status Status::FunctionIsDisable(const std::string &function_name) {
    return Status(ErrorCode::kFunctionIsDisable, std::make_unique<std::string>(fmt::format("Function: {} is disable", function_name)));
}

Status Status::NotFound(const std::string &detailed_info) { return Status(ErrorCode::kNotFound, std::make_unique<std::string>(detailed_info)); }

Status Status::ErrorInit(const std::string &detailed_info) { return Status(ErrorCode::kErrorInit, std::make_unique<std::string>(detailed_info)); }

Status Status::FileIsOpen(const std::string &filename) { return Status(ErrorCode::kFileIsOpen, std::make_unique<std::string>(filename)); }

Status Status::Unknown(const std::string &name) { return Status(ErrorCode::kUnknown, std::make_unique<std::string>(fmt::format("Unknown {}", name))); }

Status Status::InvalidQueryOption(const std::string &detail) {
    return Status(ErrorCode::kUnknown, std::make_unique<std::string>(fmt::format("Invalid query option: {}", detail)));
}

Status Status::FailToStartTxn(const std::string &detail) { return Status(ErrorCode::kFailToStartTxn, std::make_unique<std::string>(detail)); }

Status Status::AlreadyLocked(const std::string &detail) { return Status(ErrorCode::kAlreadyLocked, std::make_unique<std::string>(detail)); }

Status Status::NotLocked(const std::string &detail) { return Status(ErrorCode::kNotLocked, std::make_unique<std::string>(detail)); }

Status Status::TableIsUsing(const std::string &detail) { return Status(ErrorCode::kTableIsUsing, std::make_unique<std::string>(detail)); }

Status Status::DuplicateColumnIndex(const std::string &detail) { return Status(ErrorCode::kDuplicateColumnIndex, std::make_unique<std::string>(detail)); }

Status Status::InvalidParameter(const std::string &detail) { return Status(ErrorCode::kInvalidParameter, std::make_unique<std::string>(detail)); }

Status Status::IndexOnColumn(const std::string &column_name) {
    return Status(ErrorCode::kIndexOnColumn, std::make_unique<std::string>(fmt::format("Index is created on column: {}", column_name)));
}

// 4. TXN fail
Status Status::TxnRollback(u64 txn_id, const std::string &rollback_reason) {
    return Status(ErrorCode::kTxnRollback, std::make_unique<std::string>(fmt::format("Transaction: {} is rollback. {}", txn_id, rollback_reason)));
}

Status Status::TxnConflict(u64 txn_id, const std::string &conflict_reason) {
    return Status(ErrorCode::kTxnConflict,
                  std::make_unique<std::string>(fmt::format("Transaction: {} is conflicted, detailed info: {}", txn_id, conflict_reason)));
}

Status Status::TxnConflictNoRetry(u64 txn_id, const std::string &conflict_reason) {
    return Status(ErrorCode::kTxnConflictNoRetry,
                  std::make_unique<std::string>(fmt::format("Transaction: {} is conflicted, detailed info: {}", txn_id, conflict_reason)));
}

Status Status::TxnWWConflict(const std::string &detailed_message) { return Status(ErrorCode::kTxnWWConflict, std::make_unique<std::string>(detailed_message)); }

Status Status::TxnRWConflict(const std::string &detailed_message) { return Status(ErrorCode::kTxnRWConflict, std::make_unique<std::string>(detailed_message)); }

// 5. Insufficient resource or exceed limits
Status Status::DiskFull(const std::string &detailed_info) {
    return Status(ErrorCode::kDiskFull, std::make_unique<std::string>(fmt::format("Disk full: {}", detailed_info)));
}

Status Status::OutOfMemory(const std::string &detailed_info) {
    return Status(ErrorCode::kOutOfMemory, std::make_unique<std::string>(fmt::format("Out of memory: {}", detailed_info)));
}

Status Status::TooManyConnections(const std::string &detailed_info) {
    return Status(ErrorCode::kTooManyConnections, std::make_unique<std::string>(fmt::format("Too many connections, {}", detailed_info)));
}

Status Status::ConfigurationLimitExceed(const std::string &config_name, const std::string &config_value, const std::string &valid_value_range) {
    return Status(ErrorCode::kConfigurationLimitExceed,
                  std::make_unique<std::string>(
                      fmt::format("Set {} value: {} failed, {} value valid range : {}", config_name, config_value, config_name, valid_value_range)));
}

Status Status::QueryTooBig(const std::string &query_text, u64 ast_node) {
    return Status(ErrorCode::kQueryIsTooComplex, std::make_unique<std::string>(fmt::format("Query: {} is too complex with {} AST nodes", query_text, ast_node)));
}

Status Status::FailToGetSysInfo(const std::string &detailed_info) { return Status(ErrorCode::kFailToGetSysInfo, std::make_unique<std::string>(detailed_info)); }

// 6. Operation intervention
Status Status::QueryCancelled(const std::string &query_text) {
    return Status(ErrorCode::kQueryCancelled, std::make_unique<std::string>(fmt::format("Query: {} is cancelled", query_text)));
}

Status Status::QueryNotSupported(const std::string &query_text, const std::string &detailed_reason) {
    return Status(ErrorCode::kQueryNotSupported, std::make_unique<std::string>(fmt::format("Query: {} isn't supported, {}", query_text, detailed_reason)));
}

Status Status::ClientClose() { return Status(ErrorCode::kClientClose); }

// 7. System error
Status Status::IOError(const std::string &detailed_info) {
    return Status(ErrorCode::kIOError, std::make_unique<std::string>(fmt::format("IO error: {}", detailed_info)));
}

Status Status::DuplicatedFile(const std::string &filename) {
    return Status(ErrorCode::kDuplicatedFile, std::make_unique<std::string>(fmt::format("File already existed: {}", filename)));
}

Status Status::ConfigFileError(const std::string &path, const std::string &detailed_info) {
    return Status(ErrorCode::kConfigFileError, std::make_unique<std::string>(fmt::format("Config file: {}, {}", path, detailed_info)));
}

Status Status::LockFileError(const std::string &path, const std::string &error_msg) {
    return Status(ErrorCode::kLockFileError, std::make_unique<std::string>(fmt::format("Lock file error: {}, {}", path, error_msg)));
}

Status Status::FileCorrupted(const std::string &path) {
    return Status(ErrorCode::kFileCorrupted, std::make_unique<std::string>(fmt::format("Catalog: {} is corrupted", path)));
}

Status Status::DataCorrupted(const std::string &path) {
    return Status(ErrorCode::kDataCorrupted, std::make_unique<std::string>(fmt::format("Data: {} is corrupted", path)));
}

Status Status::IndexCorrupted(const std::string &path) {
    return Status(ErrorCode::kIndexCorrupted, std::make_unique<std::string>(fmt::format("Index: {} is corrupted", path)));
}

Status Status::FileNotFound(const std::string &path) {
    return Status(ErrorCode::kFileNotFound, std::make_unique<std::string>(fmt::format("File: {} isn't found", path)));
}

Status Status::DirNotFound(const std::string &path) {
    return Status(ErrorCode::kDirNotFound, std::make_unique<std::string>(fmt::format("Directory: {} isn't found", path)));
}

Status Status::DataIOError(const std::string &detailed_info) {
    return Status(ErrorCode::kDataIOError, std::make_unique<std::string>(fmt::format("Data read error: {}", detailed_info)));
}

Status Status::UnexpectedError(const std::string &detailed_info) {
    return Status(ErrorCode::kUnexpectedError, std::make_unique<std::string>(fmt::format("Unexpected error: {}", detailed_info)));
}

Status Status::ParserError(const std::string &detailed_info) {
    return Status(ErrorCode::kParserError, std::make_unique<std::string>(fmt::format("Parser error: {}", detailed_info)));
}

Status Status::MmapFileError(const std::string &detailed_info) {
    return Status(ErrorCode::kMmapFileError, std::make_unique<std::string>(fmt::format("mmap error: {}", detailed_info)));
}

Status Status::MunmapFileError(const std::string &detailed_info) {
    return Status(ErrorCode::kMunmapFileError, std::make_unique<std::string>(fmt::format("munmap error: {}", detailed_info)));
}

Status Status::InvalidFileFlag(u8 flag) {
    return Status(ErrorCode::kInvalidFileFlag, std::make_unique<std::string>(fmt::format("Invalid open file flag: {}", flag)));
}

Status Status::FailToRunPython(const std::string &reason) { return Status(ErrorCode::kFailToRunPython, std::make_unique<std::string>(reason)); }

Status Status::InvalidServerAddress(const std::string &error_address) {
    return Status(ErrorCode::kInvalidServerAddress, std::make_unique<std::string>(fmt::format("Invalid server address: {}", error_address)));
}

Status Status::ColumnCountMismatch(const std::string &detailed_info) {
    return Status(ErrorCode::kColumnCountMismatch, std::make_unique<std::string>(fmt::format("Column count mismatch: {}", detailed_info)));
}

Status Status::CantConnectServer(const std::string &ip, i64 port, const std::string &reason) {
    return Status(ErrorCode::kCantConnectServer, std::make_unique<std::string>(fmt::format("Can't connect server: {}:{}, {}", ip, port, reason)));
}

Status Status::NotExistNode(const std::string &node_info) {
    return Status(ErrorCode::kNotExistNode, std::make_unique<std::string>(fmt::format("Node doesn't exist: {}", node_info)));
}
Status Status::DuplicateNode(const std::string &node_info) {
    return Status(ErrorCode::kDuplicateNode, std::make_unique<std::string>(fmt::format("Duplicate node: {}", node_info)));
}

Status Status::CantConnectLeader(const std::string &leader_info) {
    return Status(ErrorCode::kCantConnectLeader, std::make_unique<std::string>(fmt::format("Can't connect leader: {}", leader_info)));
}

Status Status::MinioInvalidAccessKey(const std::string &detailed) {
    return Status(ErrorCode::kMinioInvalidAccessKey, std::make_unique<std::string>(fmt::format("Minio error: {}", detailed)));
}

Status Status::MinioBucketNotExists(const std::string &bucket_name) {
    return Status(ErrorCode::kMinioBucketNotExists, std::make_unique<std::string>(fmt::format("Bucket {} not found", bucket_name)));
}

Status Status::InvalidStorageType(const std::string &expected, const std::string &actual) {
    return Status(ErrorCode::kInvalidStorageType,
                  std::make_unique<std::string>(fmt::format("Expect storage type: {}, actual storage type: {}", expected, actual)));
}

Status Status::NotRegistered(const std::string &node_info) {
    return Status(ErrorCode::kNotRegistered, std::make_unique<std::string>(fmt::format("Not registered node: {}", node_info)));
}

Status Status::CantSwitchRole(const std::string &detailed_info) { return Status(ErrorCode::kCantSwitchRole, std::make_unique<std::string>(detailed_info)); }

Status Status::TooManyFollower(u8 follower_limit) {
    return Status(ErrorCode::kTooManyFollower, std::make_unique<std::string>(fmt::format("Too many followers, limit: {}", follower_limit)));
}

Status Status::TooManyLearner() { return Status(ErrorCode::kTooManyLearner, std::make_unique<std::string>("Too many learner, limit: 255")); }

Status Status::Checkpointing() { return Status(ErrorCode::kCheckpointing, std::make_unique<std::string>("Checkpointing")); }

// meta
Status Status::InvalidEntry() { return Status(ErrorCode::kInvalidEntry, std::make_unique<std::string>("Invalid entry")); }

Status Status::NotFoundEntry() { return Status(ErrorCode::kNotFoundEntry, std::make_unique<std::string>("Not found entry")); }

Status Status::DuplicateEntry(const std::string &detail_info) { return Status(ErrorCode::kDuplicateEntry, std::make_unique<std::string>(detail_info)); }

Status Status::EmptyEntryList() { return Status(ErrorCode::kEmptyEntryList, std::make_unique<std::string>("Empty entry list")); }

Status Status::NoWALEntryFound(const std::string &file_name, i64 index) {
    return Status(ErrorCode::kEmptyEntryList, std::make_unique<std::string>(fmt::format("No WAL entry: {} found in WAL file: {}", index, file_name)));
}

Status Status::WrongCheckpointType(const std::string &expect_type, const std::string &actual_type) {
    return Status(ErrorCode::kWrongCheckpointType,
                  std::make_unique<std::string>(fmt::format("Expect checkpoint type: {}, actual checkpoint type: {}", expect_type, actual_type)));
}

Status Status::InvalidNodeRole(const std::string &message) { return Status(ErrorCode::kInvalidNodeRole, std::make_unique<std::string>(message)); }

Status Status::InvalidNodeStatus(const std::string &message) { return Status(ErrorCode::kInvalidNodeStatus, std::make_unique<std::string>(message)); }

Status Status::NodeInfoUpdated(const std::string &message) { return Status(ErrorCode::kNodeInfoUpdated, std::make_unique<std::string>(message)); }

Status Status::NodeNameMismatch(const std::string &actual_node_name, const std::string &expected_node_name) {
    return Status(ErrorCode::kNodeNameMismatch,
                  std::make_unique<std::string>(fmt::format("Expect node name: {}, actual node name: {}", expected_node_name, actual_node_name)));
}

Status Status::CatalogError(const std::string &detailed_info) {
    return Status(ErrorCode::kCatalogError, std::make_unique<std::string>(fmt::format("Catalog error: {}", detailed_info)));
}

Status Status::BufferManagerError(const std::string &detailed_info) {
    return Status(ErrorCode::kBufferManagerError, std::make_unique<std::string>(fmt::format("Buffer manager error: {}", detailed_info)));
}

Status Status::RocksDBError(rocksdb::Status rocksdb_s, const std::string &msg) {
    return Status(ErrorCode::kRocksDBError, std::make_unique<std::string>(fmt::format("{}, {}", msg, rocksdb_s.ToString())));
}

Status Status::RocksDBError(rocksdb::IOStatus rocksdb_s, const std::string &msg) {
    return Status(ErrorCode::kRocksDBError, std::make_unique<std::string>(fmt::format("{}, {}", msg, rocksdb_s.ToString())));
}

Status Status::SnapshotAlreadyExists(const std::string &snapshot_name) {
    return Status(ErrorCode::kSnapshotAlreadyExists, std::make_unique<std::string>(fmt::format("Snapshot: {} already exists", snapshot_name)));
}

} // namespace infinity
