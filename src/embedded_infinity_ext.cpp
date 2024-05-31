#include <nanobind/nanobind.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/vector.h>

#include "parser/definition/column_def.h"
#include "parser/expr/parsed_expr.h"
#include "parser/statement/command_statement.h"
#include "parser/statement/extra/create_index_info.h"
#include "parser/statement/extra/extra_ddl_info.h"
#include "parser/statement/statement_common.h"

import infinity;
import stl;
import query_options;
import query_result;
import logical_node_type;
import status;
import base_table;
import data_table;
import table_entry_type;
import table_def;
import wrap_infinity;
import logical_type;
import constant_expr;

class SayHello {
public:
    void hello() {
        std::cout << "say hello success" << std::endl;
        infinity::Infinity::Hello();
    }
};

int add(int a, int b) {
    std::cout << "begin add" << std::endl;
    infinity::UniquePtr<char[]> buf = infinity::MakeUnique<char[]>(10);
    std::cout << "success make unique" << std::endl;
    auto hello = SayHello();
    hello.hello();

    buf[0] = 'a';
    return a + b;
}

int mul(int a, int b) { return a * b; }

void print_str(infinity::String str) { std::cout << str << std::endl; }

const char *get_hello() {
    const static char hello[] = "hello";
    std::cout << "get hello func: " << hello << std::endl;
    return hello;
}

void test_shared(infinity::SharedPtr<infinity::String> ptr) { std::cout << ptr.get() << std::endl; }

void test_shared_wrap(infinity::String str) {
    auto str_ptr = infinity::MakeShared<infinity::String>(str);
    test_shared(str_ptr);
}

// using infinity::Infinity;
using namespace infinity;
namespace nb = nanobind;

namespace infinity {
//
// struct WrapQueryResult {
//    ErrorCode error_code;
//    String error_msg;
//};
//
// WrapQueryResult WrapCreateDatabase(Infinity& instance,
//                                   const String& db_name,
//                                   const CreateDatabaseOptions& options) {
//    WrapQueryResult result;
//    auto query_result = instance.CreateDatabase(db_name, options);
//    result.error_code = query_result.ErrorCode();
//    result.error_msg = query_result.ErrorMsg();
//    return result;
//}

// void WrapInsert(Infinity& instance,
//                 const String& db_name,
//                 const String& table_name,
//                 Vector<String> columns,
//                 Vector<Vector<ParsedExpr*>> values) {
//     Vector<Vector<ParsedExpr*>*> value_ptr;
//     for (auto& value : values) {
//         value_ptr.push_back(&value);
//     }
//     instance.Insert(db_name, table_name, &columns, &value_ptr);
// }
} // namespace infinity

NB_MODULE(embedded_infinity_ext, m) {
    m.def("add", &add);
    m.def("mul", &mul);
    m.def("hello", &infinity::Infinity::Hello);
    m.def("print_str", &print_str);
    m.def("get_hello", &get_hello);
    m.def("test_shared", &test_shared_wrap);

    nb::class_<WrapQueryResult>(m, "WrapQueryResult")
        .def(nb::init<>())
        .def_rw("error_code", &WrapQueryResult::error_code)
        .def_rw("error_msg", &WrapQueryResult::error_msg)
        .def_rw("names", &WrapQueryResult::names);

    nb::class_<WrapDataType>(m, "WrapDataType")
        .def(nb::init<>())
        .def_rw("logical_type", &WrapDataType::logical_type);

    nb::class_<WrapConstantExpr>(m, "WrapConstantExpr")
        .def(nb::init<>())
        .def_rw("literal_type", &WrapConstantExpr::literal_type)
        .def_rw("bool_value", &WrapConstantExpr::bool_value)
        .def_rw("i64_value", &WrapConstantExpr::i64_value)
        .def_rw("f64_value", &WrapConstantExpr::f64_value)
        .def_rw("str_value", &WrapConstantExpr::str_value)
        .def_rw("i64_array_value", &WrapConstantExpr::i64_array_value)
        .def_rw("f64_array_value", &WrapConstantExpr::f64_array_value);

    nb::class_<WrapColumnDef>(m, "WrapColumnDef")
        .def(nb::init<>())
        .def_rw("id", &WrapColumnDef::id)
        .def_rw("column_type", &WrapColumnDef::column_type)
        .def_rw("column_name", &WrapColumnDef::column_name)
        .def_rw("constraints", &WrapColumnDef::constraints)
        .def_rw("constant_expr", &WrapColumnDef::constant_expr);

    // infinity
    nb::class_<Infinity>(m, "Infinity")
        .def(nb::init<>()) // bind constructor

        .def("GetSessionId", &Infinity::GetSessionId)

        .def_static("LocalInit", &Infinity::LocalInit)
        .def_static("LocalUnInit", &Infinity::LocalUnInit)
        .def_static("Hello", &Infinity::Hello)

        .def_static("RemoteConnect", &Infinity::RemoteConnect)
        .def("RemoteDisconnect", &Infinity::RemoteDisconnect)
        .def_static("LocalConnect", &Infinity::LocalConnect)
        .def("LocalDisconnect", &Infinity::LocalDisconnect)

        .def("CreateDatabase", &WrapCreateDatabase)
        .def("DropDatabase", &WrapDropDatabase)
        .def("ListDatabases", &WrapListDatabases)
        .def("GetDatabase", &WrapGetDatabase)
        .def("ShowDatabase", &WrapShowDatabase)
        .def("Flush", &WrapFlush)

        .def("SetVariableOrConfig", nb::overload_cast<Infinity &, const String &, bool, SetScope>(&WrapSetVariableOrConfig))
        .def("SetVariableOrConfig", nb::overload_cast<Infinity &, const String &, i64, SetScope>(&WrapSetVariableOrConfig))
        .def("SetVariableOrConfig", nb::overload_cast<Infinity &, const String &, double, SetScope>(&WrapSetVariableOrConfig))
        .def("SetVariableOrConfig", nb::overload_cast<Infinity &, const String &, String, SetScope>(&WrapSetVariableOrConfig))

        .def("ShowVariable", &WrapShowVariable)
        .def("ShowVariables", &WrapShowVariables)
        .def("ShowConfig", &WrapShowConfig)
        .def("ShowConfigs", &WrapShowConfigs)

        .def("Query", &WrapQuery)

        .def("CreateTable", &WrapCreateTable)
        .def("DropTable", &WrapDropTable)
        .def("ListTables", &WrapListTables)
        .def("ShowTable", &WrapShowTable)
        .def("ShowColumns", &WrapShowColumns)
        .def("ListTableIndexes", &WrapListTableIndexes)
        .def("ShowTables", &WrapShowTables)
        .def("GetTable", &WrapGetTable)

        .def("CreateIndex", &WrapCreateIndex)
        .def("DropIndex", &WrapDropIndex)
        .def("ShowIndex", &WrapShowIndex)
        .def("ShowSegment", &WrapShowSegment)
        .def("ShowSegments", &WrapShowSegments)
        .def("ShowBlock", &WrapShowBlock)
        .def("ShowBlocks", &WrapShowBlocks)
        .def("ShowBlockColumn", &WrapShowBlockColumn)

        .def("Insert", &WrapInsert)
        .def("Import", &WrapImport)
        .def("Delete", &WrapDelete)
        .def("Update", &WrapUpdate)
        .def("Explain", &WrapExplain)
        .def("Search", &WrapSearch)
        .def("Optimize", &WrapOptimize);

    // extra_ddl_info
    nb::enum_<ConflictType>(m, "ConflictType")
        .value("kInvalid", ConflictType::kInvalid)
        .value("kIgnore", ConflictType::kIgnore)
        .value("kError", ConflictType::kError)
        .value("kReplace", ConflictType::kReplace);

    // statement_common
    nb::enum_<CopyFileType>(m, "CopyFileType")
        .value("kCSV", CopyFileType::kCSV)
        .value("kJSON", CopyFileType::kJSON)
        .value("kJSONL", CopyFileType::kJSONL)
        .value("kFVECS", CopyFileType::kFVECS)
        .value("kInvalid", CopyFileType::kInvalid);

    // command_statement
    nb::enum_<SetScope>(m, "SetScope")
        .value("kSession", SetScope::kSession)
        .value("kGlobal", SetScope::kGlobal)
        .value("kConfig", SetScope::kConfig)
        .value("kInvalid", SetScope::kInvalid);

    // query_options
    nb::class_<CreateDatabaseOptions>(m, "CreateDatabaseOptions").def(nb::init<>()).def_rw("conflict_type", &CreateDatabaseOptions::conflict_type_);

    nb::class_<DropDatabaseOptions>(m, "DropDatabaseOptions").def(nb::init<>()).def_rw("conflict_type", &DropDatabaseOptions::conflict_type_);

    nb::class_<CreateTableOptions>(m, "CreateTableOptions")
        .def(nb::init<>())
        .def_rw("conflict_type", &CreateTableOptions::conflict_type_)
        .def_rw("properties", &CreateTableOptions::properties_);

    nb::class_<DropTableOptions>(m, "DropTableOptions").def(nb::init<>()).def_rw("conflict_type", &DropTableOptions::conflict_type_);

    nb::class_<CreateIndexOptions>(m, "CreateIndexOptions").def(nb::init<>()).def_rw("conflict_type", &CreateIndexOptions::conflict_type_);

    nb::class_<DropIndexOptions>(m, "DropIndexOptions").def(nb::init<>()).def_rw("conflict_type", &DropIndexOptions::conflict_type_);

    nb::class_<ImportOptions>(m, "ImportOptions")
        .def(nb::init<>())
        .def_rw("delimiter", &ImportOptions::delimiter_)
        .def_rw("header", &ImportOptions::header_)
        .def_rw("copy_file_type", &ImportOptions::copy_file_type_);

    // parsed_expr
    nb::enum_<ParsedExprType>(m, "ParsedExprType")
        .value("kConstant", ParsedExprType::kConstant)
        .value("kParameter", ParsedExprType::kParameter)
        .value("kColumn", ParsedExprType::kColumn)
        .value("kFunction", ParsedExprType::kFunction)
        .value("kBetween", ParsedExprType::kBetween)
        .value("kOperator", ParsedExprType::kOperator)
        .value("kSubquery", ParsedExprType::kSubquery)
        .value("kCase", ParsedExprType::kCase)
        .value("kCast", ParsedExprType::kCast)
        .value("kIn", ParsedExprType::kIn)
        .value("kKnn", ParsedExprType::kKnn)
        .value("kMatch", ParsedExprType::kMatch)
        .value("kMatchTensor", ParsedExprType::kMatchTensor)
        .value("kFusion", ParsedExprType::kFusion)
        .value("kSearch", ParsedExprType::kSearch)
        .export_values();

    //    abstract class how to bind?
    //    nb::class_<ParsedExpr>(m, "ParsedExpr")
    //        .def(nb::init<ParsedExprType>())
    //        .def("GetName", &ParsedExpr::GetName)
    //        .def("GetAlias", &ParsedExpr::GetAlias)
    //        .def("HasAlias", &ParsedExpr::HasAlias)
    //        .def("ToString", &ParsedExpr::ToString)
    //        .def_rw("type", &ParsedExpr::type_)
    //        .def_rw("alias", &ParsedExpr::alias_);

    // logical_node_type
    nb::enum_<LogicalNodeType>(m, "LogicalNodeType")
        .value("kInvalid", LogicalNodeType::kInvalid)
        .value("kAggregate", LogicalNodeType::kAggregate)
        .value("kExcept", LogicalNodeType::kExcept)
        .value("kUnion", LogicalNodeType::kUnion)
        .value("kIntersect", LogicalNodeType::kIntersect)
        .value("kJoin", LogicalNodeType::kJoin)
        .value("kCrossProduct", LogicalNodeType::kCrossProduct)
        .value("kLimit", LogicalNodeType::kLimit)
        .value("kFilter", LogicalNodeType::kFilter)
        .value("kProjection", LogicalNodeType::kProjection)
        .value("kSort", LogicalNodeType::kSort)
        .value("kTop", LogicalNodeType::kTop)
        .value("kDelete", LogicalNodeType::kDelete)
        .value("kUpdate", LogicalNodeType::kUpdate)
        .value("kInsert", LogicalNodeType::kInsert)
        .value("kImport", LogicalNodeType::kImport)
        .value("kExport", LogicalNodeType::kExport)
        .value("kAlter", LogicalNodeType::kAlter)
        .value("kCreateTable", LogicalNodeType::kCreateTable)
        .value("kCreateIndex", LogicalNodeType::kCreateIndex)
        .value("kCreateCollection", LogicalNodeType::kCreateCollection)
        .value("kCreateSchema", LogicalNodeType::kCreateSchema)
        .value("kCreateView", LogicalNodeType::kCreateView)
        .value("kDropTable", LogicalNodeType::kDropTable)
        .value("kDropIndex", LogicalNodeType::kDropIndex)
        .value("kDropCollection", LogicalNodeType::kDropCollection)
        .value("kDropSchema", LogicalNodeType::kDropSchema)
        .value("kDropView", LogicalNodeType::kDropView)
        .value("kFusion", LogicalNodeType::kFusion)
        .value("kMatch", LogicalNodeType::kMatch)
        .value("kTableScan", LogicalNodeType::kTableScan)
        .value("kIndexScan", LogicalNodeType::kIndexScan)
        .value("kViewScan", LogicalNodeType::kViewScan)
        .value("kDummyScan", LogicalNodeType::kDummyScan)
        .value("kKnnScan", LogicalNodeType::kKnnScan)
        .value("kMatchTensorScan", LogicalNodeType::kMatchTensorScan)
        .value("kShow", LogicalNodeType::kShow)
        .value("kExplain", LogicalNodeType::kExplain)
        .value("kCommand", LogicalNodeType::kCommand)
        .value("kPrepare", LogicalNodeType::kPrepare)
        .value("kFlush", LogicalNodeType::kFlush)
        .value("kOptimize", LogicalNodeType::kOptimize)
        .value("kCompact", LogicalNodeType::kCompact)
        .value("kCompactIndex", LogicalNodeType::kCompactIndex)
        .value("kCompactFinish", LogicalNodeType::kCompactFinish)
        .export_values();

    // column_def
    nb::enum_<TableElementType>(m, "TableElementType")
        .value("kConstraint", TableElementType::kConstraint)
        .value("kColumn", TableElementType::kColumn)
        .export_values();

    nb::enum_<ConstraintType>(m, "ConstraintType")
        .value("kNull", ConstraintType::kNull)
        .value("kNotNull", ConstraintType::kNotNull)
        .value("kPrimaryKey", ConstraintType::kPrimaryKey)
        .value("kUnique", ConstraintType::kUnique)
        .value("kInvalid", ConstraintType::kInvalid)
        .export_values();

    nb::class_<TableElement>(m, "TableElement").def(nb::init<TableElementType>()).def_rw("type", &TableElement::type_);

    nb::class_<TableConstraint, TableElement>(m, "TableConstraint")
        .def(nb::init<>())
        .def("ToString", &TableConstraint::ToString)
        .def_rw("names_ptr", &TableConstraint::names_ptr_)
        .def_rw("constraint", &TableConstraint::constraint_);

    //    nb::class_<ColumnDef, TableElement>(m, "ColumnDef")
    //        .def(nb::init<int64_t, std::shared_ptr<DataType>, std::string, std::set<ConstraintType>, std::shared_ptr<ParsedExpr>>())
    //        .def(nb::init<LogicalType, const std::shared_ptr<TypeInfo>&, std::shared_ptr<ParsedExpr>>())
    //        .def("__eq__", &ColumnDef::operator==)
    //        .def("ToString", &ColumnDef::ToString)
    //        .def("name", &ColumnDef::name)
    //        .def("id", &ColumnDef::id)
    //        .def("type", &ColumnDef::type)
    //        .def("has_default_value", &ColumnDef::has_default_value);

    // create_index_info
    nb::enum_<IndexType>(m, "IndexType")
        .value("kIVFFlat", IndexType::kIVFFlat)
        .value("kHnsw", IndexType::kHnsw)
        .value("kFullText", IndexType::kFullText)
        .value("kSecondary", IndexType::kSecondary)
        .value("kInvalid", IndexType::kInvalid)
        .export_values();

    //    nb::class_<IndexInfo>(m, "IndexInfo")
    //        .def(nb::init<>())
    //        .def_rw("index_type", &IndexInfo::index_type_)
    //        .def_rw("column_name", &IndexInfo::column_name_)
    //        .def_rw("index_param_list", &IndexInfo::index_param_list_)
    //        .def_static("IndexTypeToString", &IndexInfo::IndexTypeToString)
    //        .def_static("StringToIndexType", &IndexInfo::StringToIndexType);
    //
    // status
    nb::enum_<ErrorCode>(m, "ErrorCode")
        .value("kOk", ErrorCode::kOk)
        .value("kIgnore", ErrorCode::kIgnore)

        .value("kInvalidTimeInfo", ErrorCode::kInvalidTimeInfo)
        .value("kEmptyConfigParameter", ErrorCode::kEmptyConfigParameter)
        .value("kMismatchVersion", ErrorCode::kMismatchVersion)
        .value("kInvalidTimezone", ErrorCode::kInvalidTimezone)
        .value("kInvalidByteSize", ErrorCode::kInvalidByteSize)
        .value("kInvalidIPAddr", ErrorCode::kInvalidIPAddr)
        .value("kInvalidLogLevel", ErrorCode::kInvalidLogLevel)
        .value("kInvalidConfig", ErrorCode::kInvalidConfig)

        .value("kWrongPasswd", ErrorCode::kWrongPasswd)
        .value("kInsufficientPrivilege", ErrorCode::kInsufficientPrivilege)

        .value("kInvalidUsername", ErrorCode::kInvalidUsername)
        .value("kInvalidPasswd", ErrorCode::kInvalidPasswd)
        .value("kInvalidDbName", ErrorCode::kInvalidDbName)
        .value("kInvalidTableName", ErrorCode::kInvalidTableName)
        .value("kInvalidColumnName", ErrorCode::kInvalidColumnName)
        .value("kInvalidIndexName", ErrorCode::kInvalidIndexName)
        .value("kInvalidColumnDefinition", ErrorCode::kInvalidColumnDefinition)
        .value("kInvalidTableDefinition", ErrorCode::kInvalidTableDefinition)
        .value("kInvalidIndexDefinition", ErrorCode::kInvalidIndexDefinition)
        .value("kDataTypeMismatch", ErrorCode::kDataTypeMismatch)
        .value("kNameTooLong", ErrorCode::kNameTooLong)
        .value("kReservedName", ErrorCode::kReservedName)
        .value("kSyntaxError", ErrorCode::kSyntaxError)
        .value("kInvalidParameterValue", ErrorCode::kInvalidParameterValue)
        .value("kDuplicateUser", ErrorCode::kDuplicateUser)
        .value("kDuplicateDatabaseName", ErrorCode::kDuplicateDatabaseName)
        .value("kDuplicateTableName", ErrorCode::kDuplicateTableName)
        .value("kDuplicateIndexName", ErrorCode::kDuplicateIndexName)
        .value("kDuplicateIndexOnColumn", ErrorCode::kDuplicateIndexOnColumn)
        .value("kNoSuchUser", ErrorCode::kNoSuchUser)
        .value("kDBNotExist", ErrorCode::kDBNotExist)
        .value("kTableNotExist", ErrorCode::kTableNotExist)
        .value("kIndexNotExist", ErrorCode::kIndexNotExist)
        .value("kColumnNotExist", ErrorCode::kColumnNotExist)
        .value("kAggNotAllowInWhereClause", ErrorCode::kAggNotAllowInWhereClause)
        .value("kColumnInSelectNotInGroupBy", ErrorCode::kColumnInSelectNotInGroupBy)
        .value("kNoSuchSystemVar", ErrorCode::kNoSuchSystemVar)
        .value("kInvalidSystemVarValue", ErrorCode::kInvalidSystemVarValue)
        .value("kSystemVarReadOnly", ErrorCode::kSystemVarReadOnly)
        .value("kFunctionNotFound", ErrorCode::kFunctionNotFound)
        .value("kSpecialFunctionNotFound", ErrorCode::kSpecialFunctionNotFound)
        .value("kNotSupported", ErrorCode::kNotSupported)
        .value("kDroppingUsingDb", ErrorCode::kDroppingUsingDb)
        .value("kSessionNotFound", ErrorCode::kSessionNotFound)
        .value("kRecursiveAgg", ErrorCode::kRecursiveAgg)
        .value("kFunctionArgsError", ErrorCode::kFunctionArgsError)
        .value("kImportFileFormatError", ErrorCode::kImportFileFormatError)
        .value("kDataNotExist", ErrorCode::kDataNotExist)
        .value("kColumnCountMismatch", ErrorCode::kColumnCountMismatch)
        .value("kEmptyDbName", ErrorCode::kEmptyDbName)
        .value("kEmptyTableName", ErrorCode::kEmptyTableName)
        .value("kEmptyColumnName", ErrorCode::kEmptyColumnName)
        .value("kEmptyIndexName", ErrorCode::kEmptyIndexName)
        .value("kExceedDBNameLength", ErrorCode::kExceedDBNameLength)
        .value("kExceedTableNameLength", ErrorCode::kExceedTableNameLength)
        .value("kExceedColumnNameLength", ErrorCode::kExceedColumnNameLength)
        .value("kExceedIndexNameLength", ErrorCode::kExceedIndexNameLength)
        .value("kNoColumnDefined", ErrorCode::kNoColumnDefined)
        .value("kNotSupportedTypeConversion", ErrorCode::kNotSupportedTypeConversion)
        .value("kEmptySelectFields", ErrorCode::kEmptySelectFields)
        .value("kInvalidDataType", ErrorCode::kInvalidDataType)
        .value("kParseMatchExprFailed", ErrorCode::kParseMatchExprFailed)
        .value("kFTSIndexNotExist", ErrorCode::kFTSIndexNotExist)
        .value("kUnknownFTSFault", ErrorCode::kUnknownFTSFault)
        .value("kInvalidConstraintType", ErrorCode::kInvalidConstraintType)
        .value("kInvalidKnnDistanceType", ErrorCode::kInvalidKnnDistanceType)
        .value("kInvalidEmbeddingDataType", ErrorCode::kInvalidEmbeddingDataType)
        .value("kInvalidConstantType", ErrorCode::kInvalidConstantType)
        .value("kInvalidParsedExprType", ErrorCode::kInvalidParsedExprType)
        .value("kInvalidIndexType", ErrorCode::kInvalidIndexType)
        .value("kInvalidIndexParam", ErrorCode::kInvalidIndexParam)
        .value("kLackIndexParam", ErrorCode::kLackIndexParam)
        .value("kInvalidFilterExpression", ErrorCode::kInvalidFilterExpression)
        .value("kMultipleFunctionMatched", ErrorCode::kMultipleFunctionMatched)
        .value("kInsertWithoutValues", ErrorCode::kInsertWithoutValues)
        .value("kInvalidConflictType", ErrorCode::kInvalidConflictType)
        .value("kInvalidJsonFormat", ErrorCode::kInvalidJsonFormat)
        .value("kDuplicateColumnName", ErrorCode::kDuplicateColumnName)
        .value("kInvalidExpression", ErrorCode::kInvalidExpression)
        .value("kSegmentNotExist", ErrorCode::kSegmentNotExist)
        .value("kAggregateFunctionWithEmptyArgs", ErrorCode::kAggregateFunctionWithEmptyArgs)
        .value("kBlockNotExist", ErrorCode::kBlockNotExist)
        .value("kInvalidTopKType", ErrorCode::kInvalidTopKType)
        .value("kInvalidCreateOption", ErrorCode::kInvalidCreateOption)
        .value("kInvalidDropOption", ErrorCode::kInvalidDropOption)
        .value("kInvalidCommand", ErrorCode::kInvalidCommand)
        .value("kAnalyzerNotFound", ErrorCode::kAnalyzerNotFound)
        .value("kNotSupportedAnalyzer", ErrorCode::kNotSupportedAnalyzer)
        .value("kInvalidAnalyzerName", ErrorCode::kInvalidAnalyzerName)
        .value("kInvalidAnalyzerFile", ErrorCode::kInvalidAnalyzerFile)

        .value("kTxnRollback", ErrorCode::kTxnRollback)
        .value("kTxnConflict", ErrorCode::kTxnConflict)

        .value("kDiskFull", ErrorCode::kDiskFull)
        .value("kOutOfMemory", ErrorCode::kOutOfMemory)
        .value("kTooManyConnections", ErrorCode::kTooManyConnections)
        .value("kConfigurationLimitExceed", ErrorCode::kConfigurationLimitExceed)
        .value("kQueryIsTooComplex", ErrorCode::kQueryIsTooComplex)

        .value("kQueryCancelled", ErrorCode::kQueryCancelled)
        .value("kQueryNotSupported", ErrorCode::kQueryNotSupported)
        .value("kClientClose", ErrorCode::kClientClose)

        .value("kIOError", ErrorCode::kIOError)
        .value("kDuplicatedFile", ErrorCode::kDuplicatedFile)
        .value("kConfigFileError", ErrorCode::kConfigFileError)
        .value("kLockFileExists", ErrorCode::kLockFileExists)
        .value("kCatalogCorrupted", ErrorCode::kCatalogCorrupted)
        .value("kDataCorrupted", ErrorCode::kDataCorrupted)
        .value("kIndexCorrupted", ErrorCode::kIndexCorrupted)
        .value("kFileNotFound", ErrorCode::kFileNotFound)
        .value("kDirNotFound", ErrorCode::kDirNotFound)
        .value("kDataIOError", ErrorCode::kDataIOError)
        .value("kUnexpectedError", ErrorCode::kUnexpectedError)
        .value("kParserError", ErrorCode::kParserError)
        .value("kMmapFileError", ErrorCode::kMmapFileError)
        .value("kMunmapFileError", ErrorCode::kMunmapFileError)

        .value("kInvalidEntry", ErrorCode::kInvalidEntry)
        .value("kDuplicateEntry", ErrorCode::kDuplicateEntry)
        .value("kNotFoundEntry", ErrorCode::kNotFoundEntry)
        .value("kEmptyEntryList", ErrorCode::kEmptyEntryList)
        .export_values();

    // table_entry_type
    nb::enum_<TableEntryType>(m, "TableEntryType")
        .value("kTableEntry", TableEntryType::kTableEntry)
        .value("kCollectionEntry", TableEntryType::kCollectionEntry)
        .export_values();

    // logical_type
    nb::enum_<LogicalType>(m, "LogicalType")
        .value("kBoolean", LogicalType::kBoolean)
        .value("kTinyInt", LogicalType::kTinyInt)
        .value("kSmallInt", LogicalType::kSmallInt)
        .value("kInteger", LogicalType::kInteger)
        .value("kBigInt", LogicalType::kBigInt)
        .value("kHugeInt", LogicalType::kHugeInt)
        .value("kDecimal", LogicalType::kDecimal)
        .value("kFloat", LogicalType::kFloat)
        .value("kDouble", LogicalType::kDouble)
        .value("kVarchar", LogicalType::kVarchar)
        .value("kDate", LogicalType::kDate)
        .value("kTime", LogicalType::kTime)
        .value("kDateTime", LogicalType::kDateTime)
        .value("kTimestamp", LogicalType::kTimestamp)
        .value("kInterval", LogicalType::kInterval)
        .value("kArray", LogicalType::kArray)
        .value("kTuple", LogicalType::kTuple)
        .value("kPoint", LogicalType::kPoint)
        .value("kLine", LogicalType::kLine)
        .value("kLineSeg", LogicalType::kLineSeg)
        .value("kBox", LogicalType::kBox)
        .value("kCircle", LogicalType::kCircle)
        .value("kUuid", LogicalType::kUuid)
        .value("kEmbedding", LogicalType::kEmbedding)
        .value("kRowID", LogicalType::kRowID)
        .value("kMixed", LogicalType::kMixed)
        .value("kNull", LogicalType::kNull)
        .value("kMissing", LogicalType::kMissing)
        .value("kTensor", LogicalType::kTensor)
        .value("kTensorArray", LogicalType::kTensorArray)
        .value("kInvalid", LogicalType::kInvalid)
        .export_values();

    // constant_expr
    nb::enum_<LiteralType>(m, "LiteralType")
        .value("kBoolean", LiteralType::kBoolean)
        .value("kDouble", LiteralType::kDouble)
        .value("kString", LiteralType::kString)
        .value("kInteger", LiteralType::kInteger)
        .value("kNull", LiteralType::kNull)
        .value("kDate", LiteralType::kDate)
        .value("kTime", LiteralType::kTime)
        .value("kDateTime", LiteralType::kDateTime)
        .value("kTimestamp", LiteralType::kTimestamp)
        .value("kIntegerArray", LiteralType::kIntegerArray)
        .value("kDoubleArray", LiteralType::kDoubleArray)
        .value("kSubArrayArray", LiteralType::kSubArrayArray)
        .value("kInterval", LiteralType::kInterval);

}
