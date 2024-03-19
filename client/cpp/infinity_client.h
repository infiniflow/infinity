// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#pragma once

#include "InfinityService.h"
#include "table_info.h"
#include <memory>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <vector>

namespace infinity::client {

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace infinity_thrift_rpc;

enum ClientErrorCode : int64_t {

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
    kInvalidFilterExpression = 3063,
    kMultipleFunctionMatched = 3064,
    kInsertWithoutValues = 3065,
    kInvalidConflictType = 3066,

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

struct ClientStatus {
    inline static ClientStatus OK() { return {ClientErrorCode::kOk, {}}; }
    inline bool IsOK() { return error_code_ == ClientErrorCode::kOk; }
    ClientErrorCode ErrorCode() const { return error_code_; }
    const std::string &ErrorMessage() const { return msg_; }

    ClientErrorCode error_code_{0};
    std::string msg_{};
};

enum CreateOption : int8_t { kErrorIfExists = 0, kIgnoreIfExists = 1, kReplaceIfExists = 2 };

enum class DropOption { kErrorIfNotExists, kIgnoreIfNotExists };

struct DatabaseInfo {
    std::string name_;
    std::string storage_path_;
};

struct TableInfo {
    std::string database_name_;
    std::string name_;
    std::string storage_path_;
    int64_t column_count_;
    int64_t segment_count_;
    int64_t row_count_;
};

class Client {
public:
    /// TODO: comment
    static Client Connect(const std::string &ip_address, uint16_t port);

    Client(std::shared_ptr<TTransport> socket,
           std::shared_ptr<TTransport> transport,
           std::shared_ptr<TProtocol> protocol,
           std::unique_ptr<InfinityServiceClient> client,
           int64_t session_id)
        : socket_(socket), transport_(transport), protocol_(protocol), client_(std::move(client)), session_id_(session_id) {}

    /// TODO: comment
    ClientStatus Disconnect();

    /// TODO: comment
    ClientStatus CreateDatabase(const std::string &db_name, CreateOption create_option = CreateOption::kErrorIfExists);

    /// TODO: comment
    ClientStatus DropDatabase(const std::string &db_name, DropOption drop_option = DropOption::kErrorIfNotExists);

    /// TODO: comment
    [[nodiscard]] std::vector<std::string> ListDatabases() const;

    /// Describe database
    [[nodiscard]] DatabaseInfo ShowDatabase(const std::string &db_name) const;

    /// Create table
    ClientStatus CreateTable(const std::string &table_name,
                             std::vector<ColumnDefinition> &columns,
                             const TableProperties &table_properties,
                             CreateOption create_option = CreateOption::kErrorIfExists);

    /// Drop table
    ClientStatus DropTable(const std::string &table_name, DropOption drop_option = DropOption::kErrorIfNotExists);

    /// List tables
    [[nodiscard]] std::vector<std::string> ListTables(const std::string &database_name) const;

    /// Show table
    TableInfo ShowTable(const std::string &db_name, const std::string &table_name) const;

    /// Create Index
    /// Drop Index
    /// Show index
    /// Insert
    /// Import
    /// Select
    /// Delete
    /// Update
    /// Explain
    /// ShowVariable
    /// Show Segments
    /// Show Segment
    /// Show blocks
    /// Show block

private:
    std::shared_ptr<TTransport> socket_;
    std::shared_ptr<TTransport> transport_;
    std::shared_ptr<TProtocol> protocol_;
    std::unique_ptr<InfinityServiceClient> client_;
    int64_t session_id_;
};

} // namespace infinity::client
