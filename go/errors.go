// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package infinity

// ErrorCode represents the error codes from Infinity
type ErrorCode int

const (
	ErrorCodeOK ErrorCode = 0

	ErrorCodeInvalidTimeInfo        ErrorCode = 1001
	ErrorCodeEmptyConfigParameter   ErrorCode = 1002
	ErrorCodeMismatchVersion        ErrorCode = 1003
	ErrorCodeInvalidTimezone        ErrorCode = 1004
	ErrorCodeInvalidByteSize        ErrorCode = 1005
	ErrorCodeInvalidIPAddr          ErrorCode = 1006
	ErrorCodeInvalidLogLevel        ErrorCode = 1007
	ErrorCodeInvalidConfig          ErrorCode = 1008

	ErrorCodeWrongPasswd             ErrorCode = 2001
	ErrorCodeInsufficientPrivilege   ErrorCode = 2002
	ErrorCodeUnsupportedVersionIndex ErrorCode = 2003
	ErrorCodeClientVersionMismatch   ErrorCode = 2004
	ErrorCodeInfinityIsStarting      ErrorCode = 2007
	ErrorCodeInfinityIsIniting       ErrorCode = 2008

	ErrorCodeInvalidUsername                 ErrorCode = 3001
	ErrorCodeInvalidPasswd                   ErrorCode = 3002
	ErrorCodeInvalidIdentifierName           ErrorCode = 3003
	ErrorCodeInvalidTableName                ErrorCode = 3004
	ErrorCodeInvalidColumnName               ErrorCode = 3005
	ErrorCodeInvalidIndexName                ErrorCode = 3006
	ErrorCodeInvalidColumnDefinition         ErrorCode = 3007
	ErrorCodeInvalidTableDefinition          ErrorCode = 3008
	ErrorCodeInvalidIndexDefinition          ErrorCode = 3009
	ErrorCodeDataTypeMismatch                ErrorCode = 3010
	ErrorCodeNameTooLong                     ErrorCode = 3011
	ErrorCodeReservedName                    ErrorCode = 3012
	ErrorCodeSyntaxError                     ErrorCode = 3013
	ErrorCodeInvalidParameterValue           ErrorCode = 3014
	ErrorCodeDuplicateUser                   ErrorCode = 3015
	ErrorCodeDuplicateDatabaseName           ErrorCode = 3016
	ErrorCodeDuplicateTableName              ErrorCode = 3017
	ErrorCodeDuplicateIndexName              ErrorCode = 3018
	ErrorCodeDuplicateIndexOnColumn          ErrorCode = 3019
	ErrorCodeNoSuchUser                      ErrorCode = 3020
	ErrorCodeDBNotExist                      ErrorCode = 3021
	ErrorCodeTableNotExist                   ErrorCode = 3022
	ErrorCodeIndexNotExist                   ErrorCode = 3023
	ErrorCodeColumnNotExist                  ErrorCode = 3024
	ErrorCodeAggNotAllowInWhereClause        ErrorCode = 3025
	ErrorCodeColumnInSelectNotInGroupBy      ErrorCode = 3026
	ErrorCodeNoSuchSystemVar                 ErrorCode = 3027
	ErrorCodeInvalidSystemVarValue           ErrorCode = 3028
	ErrorCodeSystemVarReadOnly               ErrorCode = 3029
	ErrorCodeFunctionNotFound                ErrorCode = 3030
	ErrorCodeSpecialFunctionNotFound         ErrorCode = 3031
	ErrorCodeNotSupported                    ErrorCode = 3032
	ErrorCodeDroppingUsingDB                 ErrorCode = 3033
	ErrorCodeSessionNotFound                 ErrorCode = 3034
	ErrorCodeRecursiveAgg                    ErrorCode = 3035
	ErrorCodeFunctionArgsError               ErrorCode = 3036
	ErrorCodeImportFileFormatError           ErrorCode = 3037
	ErrorCodeDataNotExist                    ErrorCode = 3038
	ErrorCodeColumnCountMismatch             ErrorCode = 3039
	ErrorCodeEmptyDBName                     ErrorCode = 3040
	ErrorCodeEmptyTableName                  ErrorCode = 3041
	ErrorCodeEmptyColumnName                 ErrorCode = 3042
	ErrorCodeEmptyIndexName                  ErrorCode = 3043
	ErrorCodeExceedDBNameLength              ErrorCode = 3044
	ErrorCodeExceedTableNameLength           ErrorCode = 3045
	ErrorCodeExceedColumnNameLength          ErrorCode = 3046
	ErrorCodeExceedIndexNameLength           ErrorCode = 3047
	ErrorCodeNoColumnDefined                 ErrorCode = 3048
	ErrorCodeNotSupportedTypeConversion      ErrorCode = 3049
	ErrorCodeEmptySelectFields               ErrorCode = 3050
	ErrorCodeInvalidDataType                 ErrorCode = 3051
	ErrorCodeParseMatchExprFailed            ErrorCode = 3052
	ErrorCodeFTSIndexNotExist                ErrorCode = 3053
	ErrorCodeUnknownFTSFault                 ErrorCode = 3054
	ErrorCodeInvalidConstraintType           ErrorCode = 3055
	ErrorCodeInvalidKNNDistanceType          ErrorCode = 3056
	ErrorCodeInvalidEmbeddingDataType        ErrorCode = 3057
	ErrorCodeInvalidConstantType             ErrorCode = 3058
	ErrorCodeInvalidParsedExprType           ErrorCode = 3059
	ErrorCodeInvalidIndexType                ErrorCode = 3060
	ErrorCodeInvalidIndexParam               ErrorCode = 3061
	ErrorCodeLackIndexParam                  ErrorCode = 3062
	ErrorCodeInvalidFilterExpression         ErrorCode = 3063
	ErrorCodeMultipleFunctionMatched         ErrorCode = 3064
	ErrorCodeInsertWithoutValues             ErrorCode = 3065
	ErrorCodeInvalidConflictType             ErrorCode = 3066
	ErrorCodeInvalidJSONFormat               ErrorCode = 3067
	ErrorCodeDuplicateColumnName             ErrorCode = 3068
	ErrorCodeInvalidExpression               ErrorCode = 3069
	ErrorCodeSegmentNotExist                 ErrorCode = 3070
	ErrorCodeAggregateFunctionWithEmptyArgs  ErrorCode = 3071
	ErrorCodeBlockNotExist                   ErrorCode = 3072
	ErrorCodeInvalidTopKType                 ErrorCode = 3073
	ErrorCodeInvalidCreateOption             ErrorCode = 3074
	ErrorCodeInvalidDropOption               ErrorCode = 3075
	ErrorCodeInvalidCommand                  ErrorCode = 3076
	ErrorCodeAnalyzerNotFound                ErrorCode = 3077
	ErrorCodeNotSupportedAnalyzer            ErrorCode = 3078
	ErrorCodeInvalidAnalyzerName             ErrorCode = 3079
	ErrorCodeInvalidAnalyzerFile             ErrorCode = 3080
	ErrorCodeInvalidExplainType              ErrorCode = 3081
	ErrorCodeChunkNotExist                   ErrorCode = 3082
	ErrorCodeNameMismatched                  ErrorCode = 3083
	ErrorCodeTransactionNotFound             ErrorCode = 3084
	ErrorCodeInvalidDatabaseIndex            ErrorCode = 3085
	ErrorCodeInvalidTableIndex               ErrorCode = 3086
	ErrorCodeFunctionIsDisable               ErrorCode = 3087
	ErrorCodeNotFound                        ErrorCode = 3088
	ErrorCodeErrorInit                       ErrorCode = 3089
	ErrorCodeFileIsOpen                      ErrorCode = 3090
	ErrorCodeUnknown                         ErrorCode = 3091
	ErrorCodeInvalidQueryOption              ErrorCode = 3092

	ErrorCodeTXNRollback ErrorCode = 4001
	ErrorCodeTXNConflict ErrorCode = 4002

	ErrorCodeDiskFull                ErrorCode = 5001
	ErrorCodeOutOfMemory             ErrorCode = 5002
	ErrorCodeTooManyConnections      ErrorCode = 5003
	ErrorCodeConfigurationLimitExceed ErrorCode = 5004
	ErrorCodeQueryIsTooComplex       ErrorCode = 5005
	ErrorCodeFailToGetSysInfo        ErrorCode = 5006

	ErrorCodeQueryCancelled   ErrorCode = 6001
	ErrorCodeQueryNotSupported ErrorCode = 6002
	ErrorCodeClientClose      ErrorCode = 6003

	ErrorCodeDiskIOError         ErrorCode = 7001
	ErrorCodeDuplicatedFile      ErrorCode = 7002
	ErrorCodeConfigFileError     ErrorCode = 7003
	ErrorCodeLockFileExists      ErrorCode = 7004
	ErrorCodeCatalogCorrupted    ErrorCode = 7005
	ErrorCodeDataCorrupted       ErrorCode = 7006
	ErrorCodeIndexCorrupted      ErrorCode = 7007
	ErrorCodeFileNotFound        ErrorCode = 7008
	ErrorCodeDirNotFound         ErrorCode = 7009
	ErrorCodeDataIOError         ErrorCode = 7010
	ErrorCodeUnexpectedError     ErrorCode = 7011
	ErrorCodeParserError         ErrorCode = 7012
	ErrorCodeMmapFileError       ErrorCode = 7013
	ErrorCodeMunmapFileError     ErrorCode = 7014
	ErrorCodeInvalidFileFlag     ErrorCode = 7015
	ErrorCodeInvalidServerAddress ErrorCode = 7016
	ErrorCodeFailToFunPython     ErrorCode = 7017
	ErrorCodeCantConnectServer   ErrorCode = 7018
	ErrorCodeNotExistNode        ErrorCode = 7019
	ErrorCodeDuplicateNode       ErrorCode = 7020
	ErrorCodeCantConnectLeader   ErrorCode = 7021
	ErrorCodeMinioInvalidAccessKey ErrorCode = 7022
	ErrorCodeMinioBucketNotExists  ErrorCode = 7023
	ErrorCodeInvalidStorageType    ErrorCode = 7024
	ErrorCodeNotRegistered         ErrorCode = 7025
	ErrorCodeCantSwitchRole        ErrorCode = 7026
	ErrorCodeTooManyFollower       ErrorCode = 7027
	ErrorCodeTooManyLearner        ErrorCode = 7028
	ErrorCodeCheckpointing         ErrorCode = 7029

	ErrorCodeInvalidEntry      ErrorCode = 8001
	ErrorCodeDuplicateEntry    ErrorCode = 8002
	ErrorCodeNotFoundEntry     ErrorCode = 8003
	ErrorCodeEmptyEntryList    ErrorCode = 8004
	ErrorCodeNoWALEntryFound   ErrorCode = 8005
	ErrorCodeWrongCheckpointType ErrorCode = 8006
	ErrorCodeInvalidNodeRole   ErrorCode = 8007
	ErrorCodeInvalidNodeStatus ErrorCode = 8008
	ErrorCodeNodeInfoUpdated   ErrorCode = 8009
	ErrorCodeNodeNameMismatch  ErrorCode = 8010
)
