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
    kIgnore = 1,

    // 1. config error,
    kInvalidTimeInfo = 1001,
    kEmptyConfigParameter = 1002,
    kMismatchVersion = 1003,
    kInvalidTimezone = 1004,
    kInvalidByteSize = 1005,
    kInvalidIPAddr = 1006,
    kInvalidLogLevel = 1007,
    kInvalidConfig = 1008,

    // 2. Auth error
    kWrongPasswd = 2001,
    kInsufficientPrivilege = 2002,
    kUnsupportedVersionIndex = 2003,
    kClientVersionMismatch = 2004,
    kAdminOnlySupportInMaintenanceMode = 2005,
    kNotSupportInMaintenanceMode = 2006,

    // 3. syntax error or access rule violation
    kInvalidUsername = 3001,
    kInvalidPasswd = 3002,
    kInvalidIdentifierName = 3003,
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
    kInvalidFilterExpression = 3063,
    kMultipleFunctionMatched = 3064,
    kInsertWithoutValues = 3065,
    kInvalidConflictType = 3066,
    kInvalidJsonFormat = 3067,
    kDuplicateColumnName = 3068,
    kInvalidExpression = 3069,
    kSegmentNotExist = 3070,
    kAggregateFunctionWithEmptyArgs = 3071,
    kBlockNotExist = 3072,
    kInvalidTopKType = 3073,
    kInvalidCreateOption = 3074,
    kInvalidDropOption = 3075,
    kInvalidCommand = 3076,
    kAnalyzerNotFound = 3077,
    kNotSupportedAnalyzer = 3078,
    kInvalidAnalyzerName = 3079,
    kInvalidAnalyzerFile = 3080,
    kInvalidExplainType = 3081,
    kChunkNotExist = 3082,
    kNameMismatched = 3083,
    kTransactionNotFound = 3084,
    kInvalidDatabaseIndex = 3085,
    kInvalidTableIndex = 3086,
    kFunctionIsDisable = 3087,
    kNotFound = 3088,
    kErrorInit = 3089,

    // 4. Txn fail
    kTxnRollback = 4001,
    kTxnConflict = 4002,

    // 5. Insufficient resource or exceed limits
    kDiskFull = 5001,
    kOutOfMemory = 5002,
    kTooManyConnections = 5003,
    kConfigurationLimitExceed = 5004,
    kQueryIsTooComplex = 5005,
    kFailToGetSysInfo = 5006,

    // 6. Query intervention
    kQueryCancelled = 6001,
    kQueryNotSupported = 6002,
    kClientClose = 6003,

    // 7. System error
    kIOError = 7001,
    kDuplicatedFile = 7002,
    kConfigFileError = 7003,
    kLockFileError = 7004,
    kCatalogCorrupted = 7005,
    kDataCorrupted = 7006,
    kIndexCorrupted = 7007,
    kFileNotFound = 7008,
    kDirNotFound = 7009,
    kDataIOError = 7010,
    kUnexpectedError = 7011,
    kParserError = 7012,
    kMmapFileError = 7013,
    kMunmapFileError = 7014,
    kInvalidFileFlag = 7015,
    kInvalidServerAddress = 7016,
    kFailToRunPython = 7017,
    kCantConnectServer = 7018,
    kNotExistNode = 7019,
    kDuplicateNode = 7020,
    kCantConnectLeader = 7021,

    // 8. meta error
    kInvalidEntry = 8001,
    kDuplicateEntry = 8002,
    kNotFoundEntry = 8003,
    kEmptyEntryList = 8004,
    kNoWALEntryFound = 8005,
    kWrongCheckpointType = 8006,
    kInvalidNodeRole = 8007,
    kInvalidNodeStatus = 8008,
    kNodeInfoUpdated = 8009
};

export class Status {
public:
    // 0. Success
    static Status OK() { return {}; }
    static Status Ignore();

    // 1. Config error
    static Status InvalidTimeInfo(const String &time_info);
    static Status EmptyConfigParameter();
    static Status MismatchVersion(const String &current_version, const String &expected_version);
    static Status InvalidTimezone(const String &timezone);
    static Status InvalidByteSize(const String &byte_size);
    static Status InvalidIPAddr(const String &ip_addr);
    static Status InvalidLogLevel(const String &log_level);
    static Status InvalidConfig(const String &detailed_info);

    // 2. Auth error
    static Status WrongPasswd(const String &user_name);
    static Status InsufficientPrivilege(const String &user_name, const String &detailed_error);
    static Status UnsupportedVersionIndex(i64 given_index);
    static Status ClientVersionMismatch(const char *expected_version, const char *given_version);
    static Status AdminOnlySupportInMaintenanceMode();
    static Status NotSupportInMaintenanceMode();

    // 3. Syntax error or access rule violation`
    static Status InvalidUserName(const String &user_name);
    static Status InvalidPasswd();
    static Status InvalidIdentifierName(const String &db_name);
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
    static Status InvalidEmbeddingDataType(const String &type_str);
    static Status InvalidConstantType();
    static Status InvalidParsedExprType();
    static Status InvalidIndexType(const String &message);
    static Status InvalidIndexParam(const String &param_name);
    static Status LackIndexParam();
    static Status InvalidFilterExpression(const String &expr);
    static Status MultipleFunctionMatched(const String &function, const String &matched_functions);
    static Status InsertWithoutValues();
    static Status InvalidConflictType();
    static Status InvalidJsonFormat(const String &invalid_json);
    static Status DuplicateColumnName(const String &column_name);
    static Status InvalidExpression(const String &expr_str);
    static Status SegmentNotExist(SegmentID segment_id);
    static Status BlockNotExist(BlockID block_id);
    static Status AggregateFunctionWithEmptyArgs();
    static Status InvalidCommand(const String &detailed_error);
    static Status AnalyzerNotFound(const String &name);
    static Status NotSupportedAnalyzer(const String &name);
    static Status InvalidAnalyzerName(const String &name);
    static Status InvalidAnalyzerFile(const String &detailed_info);
    static Status ChunkNotExist(ChunkID chunk_id);
    static Status NameMismatched(const String &name_left, const String &name_right);
    static Status TransactionNotFound(TransactionID txn_id);
    static Status InvalidDatabaseIndex(u64 database_index, u64 capacity);
    static Status InvalidTableIndex(u64 table_index, u64 capacity);
    static Status FunctionIsDisable(const String &function_name);
    static Status NotFound(const String &detailed_info);
    static Status ErrorInit(const String &detailed_info);

    // 4. TXN fail
    static Status TxnRollback(u64 txn_id, const String &rollback_reason = "no reanson gived");
    static Status TxnConflict(u64 txn_id, const String &conflict_reason);

    // 5. Insufficient resource or exceed limits
    static Status DiskFull(const String &detailed_info);
    static Status OutOfMemory(const String &detailed_info);
    static Status TooManyConnections(const String &detailed_info);
    static Status ConfigurationLimitExceed(const String &config_name, const String &config_value, const String &valid_value_range);
    static Status QueryTooBig(const String &query_text, u64 ast_node);
    static Status FailToGetSysInfo(const String &detailed_info);

    // 6. Operation intervention
    static Status QueryCancelled(const String &query_text);
    static Status QueryNotSupported(const String &query_text, const String &detailed_reason);
    static Status ClientClose();

    // 7. System error
    static Status IOError(const String &detailed_info);
    static Status DuplicatedFile(const String &filename);
    static Status ConfigFileError(const String &path, const String &detailed_info);
    static Status LockFileError(const String &path, const String &error_msg);
    static Status CatalogCorrupted(const String &path);
    static Status DataCorrupted(const String &path);
    static Status IndexCorrupted(const String &path);
    static Status FileNotFound(const String &path);
    static Status DirNotFound(const String &path);
    static Status DataIOError(const String &detailed_info);
    static Status UnexpectedError(const String &detailed_info);
    static Status ParserError(const String &detailed_info);
    static Status MmapFileError(const String &detailed_info);
    static Status MunmapFileError(const String &detailed_info);
    static Status InvalidFileFlag(u8 flag);
    static Status InvalidServerAddress(const String &error_address);
    static Status FailToRunPython(const String &reason);
    static Status CantConnectServer(const String& ip, i64 port, const String& reason);
    static Status NotExistNode(const String& node_info);
    static Status DuplicateNode(const String& node_info);
    static Status CantConnectLeader(const String& leader_info);

    // meta
    static Status InvalidEntry();
    static Status NotFoundEntry();
    static Status DuplicateEntry();
    static Status EmptyEntryList();
    static Status NoWALEntryFound(const String &file_name, i64 index);
    static Status WrongCheckpointType(const String &expect_type, const String &actual_type);
    static Status InvalidNodeRole(const String &message);
    static Status InvalidNodeStatus(const String &message);
    static Status NodeInfoUpdated(const String &message);

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

    inline Status clone() const { return Status{code_, MakeUnique<String>(*msg_)}; }

    ErrorCode code_{ErrorCode::kOk};
    UniquePtr<String> msg_{};
};

} // namespace infinity
