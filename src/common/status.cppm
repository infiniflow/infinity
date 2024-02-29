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

export module status;

import stl;

// If new error codes are added, it also needs to be added to python/infinity/errors.py.
namespace infinity {

export enum class ErrorCode : long {

    kOk = 0, // success

    // 1. config error,
    kInvalidTimeInfo = 1001,
    kEmptyConfigParameter = 1002,
    kMismatchVersion = 1003,
    kInvalidTimezone = 1004,
    kInvalidByteSize = 1005,
    kInvalidIPAddr = 1006,
    kInvalidLogLevel = 1007,

    // 2. Auth error
    kWrongPasswd = 2001,
    kInsufficientPrivilege = 2002,

    // 3. syntax error or access rule violation
    kInvalidUsername = 3001,
    kInvalidPasswd = 3002,
    kInvalidDbName = 3003,
    kInvalidTableName = 3004,
    kInvalidColumnName = 3005,
    kInvalidIndexName = 3006,
    kInvalidColumnDefinition = 3007,
    kInvalidTableDefinition = 3008,
    kInvalidIndexDefinition = 3009,
    kDataTypeMismatch = 3010,
    kNameTooLong = 3011,
    kReservedName = 3012,
    kSyntaxError = 3013,
    kInvalidParameterValue = 3014,
    kDuplicateUser = 3015,
    kDuplicateDatabaseName = 3016,
    kDuplicateTableName = 3017,
    kDuplicateIndexName = 3018,
    kDuplicateIndexOnColumn = 3019,
    kNoSuchUser = 3020,
    kDBNotExist = 3021,
    kTableNotExist = 3022,
    kIndexNotExist = 3023,
    kColumnNotExist = 3024,
    kAggNotAllowInWhereClause = 3025,
    kColumnInSelectNotInGroupBy = 3026,
    kNoSuchSystemVar = 3027,
    kInvalidSystemVarValue = 3028,
    kSystemVarReadOnly = 3029,
    kFunctionNotFound = 3030,
    kSpecialFunctionNotFound = 3031,
    kNotSupported = 3032,
    kDroppingUsingDb = 3033,
    kSessionNotFound = 3034,
    kRecursiveAgg = 3035,
    kFunctionArgsError = 3036,
    kImportFileFormatError = 3037,
    kDataNotExist = 3038,
    kColumnCountMismatch = 3039,
    kEmptyDbName = 3040,
    kEmptyTableName = 3041,
    kEmptyColumnName = 3042,
    kEmptyIndexName = 3043,
    kExceedDBNameLength = 3044,
    kExceedTableNameLength = 3045,
    kExceedColumnNameLength = 3046,
    kExceedIndexNameLength = 3047,
    kNoColumnDefined = 3048,
    kNotSupportedTypeConversion = 3049,
    kEmptySelectFields = 3050,
    kInvalidDataType = 3051,
    kParseMatchExprFailed = 3052,
    kFTSIndexNotExist = 3053,
    kUnknownFTSFault = 3054,
    kInvalidConstraintType = 3055,
    kInvalidKnnDistanceType = 3056,
    kInvalidEmbeddingDataType = 3057,
    kInvalidConstantType = 3058,
    kInvalidParsedExprType = 3059,
    kInvalidIndexType = 3060,
    kInvalidIndexParam = 3061,
    kLackIndexParam = 3062,

    // 4. Txn fail
    kTxnRollback = 4001,
    kTxnConflict = 4002,

    // 5. Insufficient resource or exceed limits
    kDiskFull = 5001,
    kOutOfMemory = 5002,
    kTooManyConnections = 5003,
    kConfigurationLimitExceed = 5004,
    kQueryIsTooComplex = 5005,

    // 6. Query intervention
    kQueryCancelled = 6001,
    kQueryNotSupported = 6002,
    kClientClose = 6003,

    // 7. System error
    kIOError = 7001,
    kDuplicatedFile = 7002,
    kConfigFileError = 7003,
    kLockFileExists = 7004,
    kCatalogCorrupted = 7005,
    kDataCorrupted = 7006,
    kIndexCorrupted = 7007,
    kFileNotFound = 7008,
    kDirNotFound = 7009,
    kDataIOError = 7010,
    kUnexpectedError = 7011,

    // 8. meta error
    kInvalidEntry = 8001,
    kNotFoundEntry = 8002,
    kEmptyEntryList = 8003,
};

export class Status {
public:
    // 0. Success
    static Status OK() { return {}; }

    // 1. Config error
    static Status InvalidTimeInfo(const String &time_info);
    static Status EmptyConfigParameter();

    // 2. Auth error
    static Status WrongPasswd(const String &user_name);
    static Status InsufficientPrivilege(const String &user_name, const String &detailed_error);
    static Status MismatchVersion(const String &current_version, const String &expected_version);
    static Status InvalidTimezone(const String &timezone);
    static Status InvalidByteSize(const String &byte_size);
    static Status InvalidIPAddr(const String &ip_addr);
    static Status InvalidLogLevel(const String &log_level);

    // 3. Syntax error or access rule violation
    static Status InvalidUserName(const String &user_name);
    static Status InvalidPasswd();
    static Status InvalidDBName(const String &db_name);
    static Status InvalidTableName(const String &table_name);
    static Status InvalidColumnName(const String &column_name);
    static Status InvalidIndexName(const String &index_name);
    static Status InvalidColumnDefinition(const String &detailed_info);
    static Status InvalidTableDefinition(const String &detailed_info);
    static Status InvalidIndexDefinition(const String &detailed_info);
    static Status DataTypeMismatch(const String &type1, const String &type2);
    static Status NameTooLong(const String &name, const String &object_type);
    static Status ReservedName(const String &name);
    static Status SyntaxError(const String &detailed);
    static Status InvalidParameterValue(const String &parameter_name, const String &parameter_value, const String &recommend_value);
    static Status DuplicateUserName(const String &user_name);
    static Status DuplicateDatabase(const String &db_name);
    static Status DuplicateTable(const String &table_name);
    static Status DuplicateIndex(const String &index_name);
    static Status DuplicateIndexOnColumn(const String &table_name, const String &column_name);
    static Status NoUser(const String &user_name);
    static Status DBNotExist(const String &db_name);
    static Status TableNotExist(const String &table_name);
    static Status IndexNotExist(const String &index_name);
    static Status ColumnNotExist(const String &column_name);
    static Status AggNotAllowInWhere(const String &func_name);
    static Status ColumnInSelectNotInGroupBy(const String &column_name);
    static Status NoSysVar(const String &variable_name);
    static Status SetInvalidVarValue(const String &variable_name, const String &valid_value_range);
    static Status ReadOnlySysVar(const String &sys_var);
    static Status FunctionNotFound(const String &function_name);
    static Status SpecialFunctionNotFound();
    static Status NotSupport(const String &detailed);
    static Status DroppingUsingDb(const String &db_name);
    static Status SessionNotFound(i64 session_id);
    static Status RecursiveAggregate(const String &expr_name);
    static Status FunctionArgsError(const String &func_name);
    static Status ImportFileFormatError(const String &detailed_info);
    static Status DataNotExist(const String &detailed_info);
    static Status ColumnCountMismatch(const String &detailed_info);
    static Status EmptyDBName();
    static Status EmptyTableName();
    static Status EmptyColumnName();
    static Status EmptyIndexName();
    static Status ExceedDBNameLength(u64 db_name_length);
    static Status ExceedTableNameLength(u64 table_name_length);
    static Status ExceedColumnNameLength(u64 column_name_length);
    static Status ExceedIndexNameLength(u64 index_name_length);
    static Status NoColumnDefined(const String &table_name);
    static Status NotSupportedTypeConversion(const String &from_type, const String &to_type);
    static Status EmptySelectFields();
    static Status InvalidDataType();
    static Status ParseMatchExprFailed(const String &fields, const String &matching_text);
    static Status FTSIndexNotExist(const String &table_name);
    static Status UnknownFTSFault();
    static Status InvalidConstraintType();
    static Status InvalidKnnDistanceType();
    static Status InvalidEmbeddingDataType();
    static Status InvalidConstantType();
    static Status InvalidParsedExprType();
    static Status InvalidIndexType();
    static Status InvalidIndexParam(const String &param_name);
    static Status LackIndexParam();

    // 4. TXN fail
    static Status TxnRollback(u64 txn_id);
    static Status TxnConflict(u64 txn_id, const String &conflict_reason);

    // 5. Insufficient resource or exceed limits
    static Status DiskFull(const String &detailed_info);
    static Status OutOfMemory(const String &detailed_info);
    static Status TooManyConnections(const String &detailed_info);
    static Status ConfigurationLimitExceed(const String &config_name, const String &config_value, const String &valid_value_range);
    static Status QueryTooBig(const String &query_text, u64 ast_node);

    // 6. Operation intervention
    static Status QueryCancelled(const String &query_text);
    static Status QueryNotSupported(const String &query_text, const String &detailed_reason);
    static Status ClientClose();

    // 7. System error
    static Status IOError(const String &detailed_info);
    static Status DuplicatedFile(const String &filename);
    static Status ConfigFileError(const String &path, const String &detailed_info);
    static Status LockFileExists(const String &path);
    static Status CatalogCorrupted(const String &path);
    static Status DataCorrupted(const String &path);
    static Status IndexCorrupted(const String &path);
    static Status FileNotFound(const String &path);
    static Status DirNotFound(const String &path);
    static Status DataIOError(const String &detailed_info);
    static Status UnexpectedError(const String &detailed_info);

    // meta
    static Status InvalidEntry();
    static Status NotFoundEntry();
    static Status EmptyEntryList();

public:
    Status() = default;

    inline explicit Status(ErrorCode code) : code_(code) {}

    inline Status(ErrorCode code, UniquePtr<String> message) : code_(code), msg_(std::move(message)) {}

    Status(ErrorCode code, const char *msg);

    Status(Status &s);

    Status(Status &&s) noexcept;

    Status &operator=(Status &s) noexcept;

    Status &operator=(Status &&s) noexcept;

    const Status &operator=(const Status &s) noexcept = delete;

    const Status &operator=(const Status &&s) noexcept = delete;

    [[nodiscard]] bool ok() const { return code_ == ErrorCode::kOk && msg_.get() == nullptr; }

    [[nodiscard]] ErrorCode code() const { return code_; }

    void Init(ErrorCode code, const char *msg);

    [[nodiscard]] const char *message() const {
        if (msg_.get() != nullptr) {
            return msg_->c_str();
        } else {
            return nullptr;
        }
    }

    void MoveStatus(Status &s);
    void MoveStatus(Status &&s);

    void AppendMessage(const String &msg);

    inline Status clone() { return Status{code_, MakeUnique<String>(*msg_)}; }

    ErrorCode code_{ErrorCode::kOk};
    UniquePtr<String> msg_{};
};

} // namespace infinity
