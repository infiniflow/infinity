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

export module infinity_core:status;

import :infinity_type;

import std;
import third_party;

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
    kInvalidMemIndex = 1009,

    // 2. Auth error
    kWrongPasswd = 2001,
    kInsufficientPrivilege = 2002,
    kUnsupportedVersionIndex = 2003,
    kClientVersionMismatch = 2004,
    kAdminOnlySupportInMaintenanceMode = 2005,
    kNotSupportInMaintenanceMode = 2006,
    kInfinityIsStarting = 2007,
    kInfinityIsIniting = 2008,

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
    kFileIsOpen = 3090,
    kUnknown = 3091,
    kInvalidQueryOption = 3092,
    kFailToStartTxn = 3093,
    kAlreadyLocked = 3094,
    kNotLocked = 3095,
    kTableIsUsing = 3096,
    kDuplicateColumnIndex = 3097,
    kInvalidParameter = 3098,
    kIndexOnColumn = 3099,
    kSnapshotAlreadyExists = 3100,
    kSnapshotAlreadyDeleted = 3101,
    kEmptyMemIndex = 3102,
    // 4. Txn fail
    kTxnRollback = 4001,
    kTxnConflict = 4002,
    kTxnWWConflict = 4003,
    kTxnRWConflict = 4004,
    kTxnConflictNoRetry = 4005,

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
    kFileCorrupted = 7005,
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
    kMinioInvalidAccessKey = 7022,
    kMinioBucketNotExists = 7023,
    kInvalidStorageType = 7024,
    kNotRegistered = 7025,
    kCantSwitchRole = 7026,
    kTooManyFollower = 7027,
    kTooManyLearner = 7028,
    kCheckpointing = 7029,

    // 8. meta error
    kInvalidEntry = 8001,
    kDuplicateEntry = 8002,
    kNotFoundEntry = 8003,
    kEmptyEntryList = 8004,
    kNoWALEntryFound = 8005,
    kWrongCheckpointType = 8006,
    kInvalidNodeRole = 8007,
    kInvalidNodeStatus = 8008,
    kNodeInfoUpdated = 8009,
    kNodeNameMismatch = 8010,

    // 9. internal error
    kCatalogError = 9001,
    kBufferManagerError = 9002,
    kRocksDBError = 9003,
};

export class Status {
public:
    // 0. Success
    static Status OK() { return {}; }
    static Status Ignore();

    // 1. Config error
    static Status InvalidTimeInfo(const std::string &time_info);
    static Status EmptyConfigParameter();
    static Status MismatchVersion(const std::string &current_version, const std::string &expected_version);
    static Status InvalidTimezone(const std::string &timezone);
    static Status InvalidByteSize(const std::string &byte_size);
    static Status InvalidIPAddr(const std::string &ip_addr);
    static Status InvalidLogLevel(const std::string &log_level);
    static Status InvalidConfig(const std::string &detailed_info);
    static Status InvalidMemIndex(const std::string &invalid_info);

    // 2. Auth error
    static Status WrongPasswd(const std::string &user_name);
    static Status InsufficientPrivilege(const std::string &user_name, const std::string &detailed_error);
    static Status UnsupportedVersionIndex(i64 given_index);
    static Status ClientVersionMismatch(const char *expected_version, const char *given_version);
    static Status AdminOnlySupportInMaintenanceMode();
    static Status NotSupportInMaintenanceMode();
    static Status InfinityIsStarting();
    static Status InfinityIsIniting();

    // 3. Syntax error or access rule violation`
    static Status InvalidUserName(const std::string &user_name);
    static Status InvalidPasswd();
    static Status InvalidIdentifierName(const std::string &db_name);
    static Status InvalidTableName(const std::string &table_name);
    static Status InvalidColumnName(const std::string &column_name);
    static Status InvalidIndexName(const std::string &index_name);
    static Status InvalidColumnDefinition(const std::string &detailed_info);
    static Status InvalidTableDefinition(const std::string &detailed_info);
    static Status InvalidIndexDefinition(const std::string &detailed_info);
    static Status DataTypeMismatch(const std::string &type1, const std::string &type2);
    static Status NameTooLong(const std::string &name, const std::string &object_type);
    static Status ReservedName(const std::string &name);
    static Status SyntaxError(const std::string &detailed);
    static Status InvalidParameterValue(const std::string &parameter_name, const std::string &parameter_value, const std::string &recommend_value);
    static Status DuplicateUserName(const std::string &user_name);
    static Status DuplicateDatabase(const std::string &db_name);
    static Status DuplicateTable(const std::string &table_name);
    static Status DuplicateIndex(const std::string &index_name);
    static Status DuplicateIndexOnColumn(const std::string &table_name, const std::string &column_name);
    static Status NoUser(const std::string &user_name);
    static Status DBNotExist(const std::string &db_name);
    static Status TableNotExist(const std::string &table_name);
    static Status IndexNotExist(const std::string &index_name);
    static Status ColumnNotExist(const std::string &column_name);
    static Status ColumnNotExist(const ColumnID &column_id);
    static Status AggNotAllowInWhere(const std::string &func_name);
    static Status ColumnInSelectNotInGroupBy(const std::string &column_name);
    static Status NoSysVar(const std::string &variable_name);
    static Status SetInvalidVarValue(const std::string &variable_name, const std::string &valid_value_range);
    static Status ReadOnlySysVar(const std::string &sys_var);
    static Status FunctionNotFound(const std::string &function_name);
    static Status SpecialFunctionNotFound();
    static Status NotSupport(const std::string &detailed);
    static Status DroppingUsingDb(const std::string &db_name);
    static Status SessionNotFound(i64 session_id);
    static Status RecursiveAggregate(const std::string &expr_name);
    static Status FunctionArgsError(const std::string &func_name);
    static Status ImportFileFormatError(const std::string &detailed_info);
    static Status DataNotExist(const std::string &detailed_info);
    static Status ColumnCountMismatch(const std::string &detailed_info);
    static Status EmptyDBName();
    static Status EmptyTableName();
    static Status EmptyColumnName();
    static Status EmptyIndexName();
    static Status ExceedDBNameLength(u64 db_name_length);
    static Status ExceedTableNameLength(u64 table_name_length);
    static Status ExceedColumnNameLength(u64 column_name_length);
    static Status ExceedIndexNameLength(u64 index_name_length);
    static Status NoColumnDefined(const std::string &table_name);
    static Status NotSupportedTypeConversion(const std::string &from_type, const std::string &to_type);
    static Status EmptySelectFields();
    static Status InvalidDataType();
    static Status ParseMatchExprFailed(const std::string &fields, const std::string &matching_text);
    static Status FTSIndexNotExist(const std::string &table_name);
    static Status UnknownFTSFault();
    static Status InvalidConstraintType();
    static Status InvalidKnnDistanceType();
    static Status InvalidEmbeddingDataType(const std::string &type_str);
    static Status InvalidConstantType();
    static Status InvalidParsedExprType();
    static Status InvalidIndexType(const std::string &message);
    static Status InvalidIndexParam(const std::string &param_name);
    static Status LackIndexParam();
    static Status InvalidFilterExpression(const std::string &expr);
    static Status MultipleFunctionMatched(const std::string &function, const std::string &matched_functions);
    static Status InsertWithoutValues();
    static Status InvalidConflictType();
    static Status InvalidJsonFormat(const std::string &invalid_json);
    static Status DuplicateColumnName(const std::string &column_name);
    static Status InvalidExpression(const std::string &expr_str);
    static Status SegmentNotExist(SegmentID segment_id);
    static Status BlockNotExist(BlockID block_id);
    static Status AggregateFunctionWithEmptyArgs();
    static Status InvalidCommand(const std::string &detailed_error);
    static Status AnalyzerNotFound(const std::string &name);
    static Status NotSupportedAnalyzer(const std::string &name);
    static Status InvalidAnalyzerName(const std::string &name);
    static Status InvalidAnalyzerFile(const std::string &detailed_info);
    static Status ChunkNotExist(ChunkID chunk_id);
    static Status NameMismatched(const std::string &name_left, const std::string &name_right);
    static Status TransactionNotFound(TransactionID txn_id);
    static Status InvalidDatabaseIndex(u64 database_index, u64 capacity);
    static Status InvalidTableIndex(u64 table_index, u64 capacity);
    static Status FunctionIsDisable(const std::string &function_name);
    static Status NotFound(const std::string &detailed_info);
    static Status ErrorInit(const std::string &detailed_info);
    static Status FileIsOpen(const std::string &filename);
    static Status Unknown(const std::string &name);
    static Status InvalidQueryOption(const std::string &detail);
    static Status FailToStartTxn(const std::string &detail);
    static Status AlreadyLocked(const std::string &detail);
    static Status NotLocked(const std::string &detail);
    static Status TableIsUsing(const std::string &detail);
    static Status DuplicateColumnIndex(const std::string &detail);
    static Status InvalidParameter(const std::string &detail);
    static Status IndexOnColumn(const std::string &column_name);
    static Status SnapshotAlreadyExists(const std::string &snapshot_name);
    static Status SnapshotAlreadyDeleted(const std::string &snapshot_name);
    static Status EmptyMemIndex();
    // 4. TXN fail
    static Status TxnRollback(u64 txn_id, const std::string &rollback_reason = "no reason");
    static Status TxnConflict(u64 txn_id, const std::string &conflict_reason);
    static Status TxnWWConflict(const std::string &detailed_message);
    static Status TxnRWConflict(const std::string &detailed_message);
    static Status TxnConflictNoRetry(u64 txn_id, const std::string &conflict_reason);

    // 5. Insufficient resource or exceed limits
    static Status DiskFull(const std::string &detailed_info);
    static Status OutOfMemory(const std::string &detailed_info);
    static Status TooManyConnections(const std::string &detailed_info);
    static Status ConfigurationLimitExceed(const std::string &config_name, const std::string &config_value, const std::string &valid_value_range);
    static Status QueryTooBig(const std::string &query_text, u64 ast_node);
    static Status FailToGetSysInfo(const std::string &detailed_info);

    // 6. Operation intervention
    static Status QueryCancelled(const std::string &query_text);
    static Status QueryNotSupported(const std::string &query_text, const std::string &detailed_reason);
    static Status ClientClose();

    // 7. System error
    static Status IOError(const std::string &detailed_info);
    static Status DuplicatedFile(const std::string &filename);
    static Status ConfigFileError(const std::string &path, const std::string &detailed_info);
    static Status LockFileError(const std::string &path, const std::string &error_msg);
    static Status FileCorrupted(const std::string &path);
    static Status DataCorrupted(const std::string &path);
    static Status IndexCorrupted(const std::string &path);
    static Status FileNotFound(const std::string &path);
    static Status DirNotFound(const std::string &path);
    static Status DataIOError(const std::string &detailed_info);
    static Status UnexpectedError(const std::string &detailed_info);
    static Status ParserError(const std::string &detailed_info);
    static Status MmapFileError(const std::string &detailed_info);
    static Status MunmapFileError(const std::string &detailed_info);
    static Status InvalidFileFlag(u8 flag);
    static Status InvalidServerAddress(const std::string &error_address);
    static Status FailToRunPython(const std::string &reason);
    static Status CantConnectServer(const std::string &ip, i64 port, const std::string &reason);
    static Status NotExistNode(const std::string &node_info);
    static Status DuplicateNode(const std::string &node_info);
    static Status CantConnectLeader(const std::string &leader_info);
    static Status MinioInvalidAccessKey(const std::string &detailed);
    static Status MinioBucketNotExists(const std::string &bucket_name);
    static Status InvalidStorageType(const std::string &expected, const std::string &actual);
    static Status NotRegistered(const std::string &node_info);
    static Status CantSwitchRole(const std::string &detailed_info);
    static Status TooManyFollower(u8 follower_limit);
    static Status TooManyLearner();
    static Status Checkpointing();

    // meta
    static Status InvalidEntry();
    static Status NotFoundEntry();
    static Status DuplicateEntry(const std::string &detailed_info);
    static Status EmptyEntryList();
    static Status NoWALEntryFound(const std::string &file_name, i64 index);
    static Status WrongCheckpointType(const std::string &expect_type, const std::string &actual_type);
    static Status InvalidNodeRole(const std::string &message);
    static Status InvalidNodeStatus(const std::string &message);
    static Status NodeInfoUpdated(const std::string &message);
    static Status NodeNameMismatch(const std::string &actual_node_name, const std::string &expected_node_name);

    // catalog
    static Status CatalogError(const std::string &detailed_info);
    static Status BufferManagerError(const std::string &detailed_info);
    static Status RocksDBError(rocksdb::Status rocksdb_s, const std::string &msg);
    static Status RocksDBError(rocksdb::IOStatus rocksdb_s, const std::string &msg);

public:
    Status() = default;

    inline explicit Status(ErrorCode code) : code_(code) {}

    inline Status(ErrorCode code, std::unique_ptr<std::string> message) : code_(code), msg_(std::move(message)) {}

    Status(ErrorCode code, rocksdb::Status detail);

    Status(ErrorCode code, rocksdb::IOStatus detail);

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

    [[nodiscard]] const char *message() const;

    void MoveStatus(Status &s);
    void MoveStatus(Status &&s);

    void AppendMessage(const std::string &msg);

    inline Status clone() const { return Status{code_, std::make_unique<std::string>(*msg_)}; }

    ErrorCode code_{ErrorCode::kOk};
    mutable std::unique_ptr<std::string> msg_{};
};

} // namespace infinity
