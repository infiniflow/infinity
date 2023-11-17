/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

# Thrift Tutorial
# Mark Slee (mcslee@facebook.com)
#
# This file aims to teach you how to use Thrift, in a .thrift file. Neato. The
# first thing to notice is that .thrift files support standard shell comments.
# This lets you make your thrift file executable and include your Thrift build
# step on the top line. And you can place comments like this anywhere you like.
#
# Before running this file, you will need to have installed the thrift compiler
# into /usr/local/bin.

/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  i8 (byte)   Signed 8-bit integer
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */

// Just in case you were wondering... yes. We support simple C comments too.

/**
 * Thrift files can reference other Thrift files to include common struct
 * and service definitions. These are found using the current path, or by
 * searching relative to any paths specified with the -I compiler flag.
 *
 * Included objects are accessed using the name of the .thrift file as a
 * prefix. i.e. shared.SharedObject
 */
include "shared.thrift"

/**
 * Thrift files can namespace, package, or prefix their output in various
 * target languages.
 */


namespace cpp tutorial
namespace d tutorial
namespace dart tutorial
namespace java tutorial
namespace php tutorial
namespace perl tutorial
namespace haxe tutorial
namespace netstd tutorial

/**
 * Thrift lets you do typedefs to get pretty names for your types. Standard
 * C style here.
 */
typedef i32 MyInteger

/**
 * Thrift also lets you define constants for use across languages. Complex
 * types and structs are specified using JSON notation.
 */
const i32 INT32CONSTANT = 9853
const map<string,string> MAPCONSTANT = {'hello':'world', 'goodnight':'moon'}

/**
 * You can define enums, which are just 32 bit integers. Values are optional
 * and start at 1 if not supplied, C style again.
 */
enum Operation {
  ADD = 1,
  SUBTRACT = 2,
  MULTIPLY = 3,
  DIVIDE = 4
}

/**
 * Structs are the basic complex data structures. They are comprised of fields
 * which each have an integer identifier, a type, a symbolic name, and an
 * optional default value.
 *
 * Fields can be declared "optional", which ensures they will not be included
 * in the serialized output if they aren't set.  Note that this requires some
 * manual management in some languages.
 */
struct Work {
  1: i32 num1 = 0,
  2: i32 num2,
  3: Operation op,
  4: optional string comment,
}

/**
 * Structs can also be exceptions, if they are nasty.
 */
exception InvalidOperation {
  1: i32 whatOp,
  2: string why
}

/**
 * Ahh, now onto the cool part, defining a service. Services just need a name
 * and can optionally inherit from another service using the extends keyword.
 */
service Calculator extends shared.SharedService {

  /**
   * A method definition looks like C code. It has a return type, arguments,
   * and optionally a list of exceptions that it may throw. Note that argument
   * lists and exception lists are specified using the exact same syntax as
   * field lists in struct or exception definitions.
   */

   void ping(),

   i32 add(1:i32 num1, 2:i32 num2),

   i32 calculate(1:i32 logid, 2:Work w) throws (1:InvalidOperation ouch),

   /**
    * This method has a oneway modifier. That means the client only makes
    * a request and does not listen for any response at all. Oneway methods
    * must be void.
    */
   oneway void zip(),

   CommonResponse Connect(),
   CommonResponse Disconnect(1:CommonRequest request),

   CommonResponse CreateDatabase(1:CreateDatabaseRequest request),
   CommonResponse DropDatabase(1:DropDatabaseRequest request),
//    CommonResponse CreateTable(1:CreateTableRequest request),
//    CommonResponse DropTable(1:DropTableRequest request),
//    CommonResponse Insert(1:InsertRequest request),
//    CommonResponse Import(1:ImportRequest request),
//    SelectResponse Select(1:SelectRequest request),

//    ListDatabaseResponse ListDatabase(1:ListDatabaseRequest request),
//    ListTableResponse ListTable(1:ListTableRequest request),

//    DescribeDatabaseResponse DescribeDatabase(1:DescribeDatabaseRequest request),
//    DescribeTableResponse DescribeTable(1:DescribeTableRequest request),

//    CommonResponse GetDatabase(1:GetDatabaseRequest request),
//    CommonResponse GetTable(1:GetTableRequest request),

//    CommonResponse CreateIndex(1:CreateIndexRequest request),
//    CommonResponse DropIndex(1:DropIndexRequest request),

}

/**
 * That just about covers the basics. Take a look in the test/ folder for more
 * detailed examples. After you run this file, your generated code shows up
 * in folders with names gen-<language>. The generated code isn't too scary
 * to look at. It even has pretty indentation.
 */

// enum LogicType {
//     Boolean,
//     TinyInt,
//     SmallInt,
//     Integer,
//     BigInt,
//     HugeInt,
//     Decimal,
//     Float,
//     Double,
//     Varchar,
//     Embedding,
//     Invalid
// }

struct Option {}

struct CreateDatabaseRequest {
    1:  string db_name,
    2:  i64 session_id,
    3:  Option option,
}

struct DropDatabaseRequest {
    1:  string db_name,
    2:  i64 session_id,
    3:  Option option,
}

// struct CreateTableRequest {
//     1:  string db_name,
//     2:  string table_name,
//     3:  list<ColumnDef> column_defs = [],
//     6:  i64 session_id,
//     7:  Option option,
// }

// enum Constraint {
//     PrimaryKey,
//     NotNull,
//     Null,
//     Unique,
// }


// struct ColumnDef {
//     1:  i32 id,
//     2:  string name,
//     3:  DataType data_type,
//     4:  Constraint constraint = [],
// }

// struct DataType {
//     1:  LogicType logic_type,
//     2:  PhysicalType physical_type,
// }

// union PhysicalType {
//     1:  NumberType number_type,
//     2:  VarcharType varchar_type,
//     3:  EmbeddingType embedding_type,
// }

// struct NumberType {}

// struct VarcharType {}

// struct EmbeddingType {
//     1:  i32 dimension,
//     2:  ElementType element_type,
// }

// enum ElementType {
//     ElementBit,
//     ElementInt8,
//     ElementInt16,
//     ElementInt32,
//     ElementInt64,
//     ElementFloat32,
//     ElementFloat64,
// }

// struct DropTableRequest {
//     1:  string db_name,
//     2:  string table_name,
//     3:  i64 session_id,
//     4:  Option option,
// }

// struct InsertRequest {
//     1:  string db_name,
//     2:  string table_name,
//     3:  list<string> column_names = [],
//     4:  list<Field> fields = [],
//     5:  i64 session_id,
// }

// struct Field {
//     1: list<ParsedExpr> parse_exprs = [],
// }

// struct ImportRequest{
//     1:  string db_name,
//     2:  string table_name,
//     3:  string file_path,
//     4:  ImportOption import_option,
//     5:  i64 session_id,
// }

// struct ImportOption {
//     1:  string delimiter,
//     2:  string file_type,
//     3:  string table_name,
//     4:  string schema_name,
//     5:  bool copy_from,
//     6:  bool has_header,
//     7:  CopyFileType copy_file_type,
// }

// enum CopyFileType {
//     CSV,
//     JSON,
//     FVECS,
// }

struct CommonResponse {
    1:  bool success,
    2:  string error_msg,
    3:  i64 session_id,
}

struct CommonRequest {
    1:  i64 session_id,
}

// union ParsedExprType {
//     1: ConstantExpr constant_expr,
//     2: ColumnExpr column_expr,
//     3: FunctionExpr function_expr,
//     4: KnnExpr knn_expr,
// }

// struct ParsedExpr {
//     1: ParsedExprType type ,
//     2: string alias_name,
// }

// enum LiteralType {
//     Boolean,
//     Double,
//     String,
//     Int64,
//     Null,
//     IntegerArray,
//     DoubleArray,
// }

// struct ConstantExpr {
//     1: LiteralType literal_type,
//     2: bool bool_value,
//     3: i64 i64_value,
//     4: double f64_value,
//     5: string str_value,
// }

// struct ColumnExpr {
//     1: list<string> column_name = [],
//     2: bool star,
// }

// struct FunctionExpr {
//     1: string function_name,
//     2: list<ParsedExpr> arguments =[],
// }

// struct BetweenExpr {
//     1: ParsedExpr value,
//     2: ParsedExpr upper_bound,
//     3: ParsedExpr lower_bound,
// }

// struct KnnExpr {
//     1: ParsedExpr column_expr,
//     2: list<string> embedding_data,
//     3: i64 dimension,
//     4: KnnDistanceType distance_type,
//     5: EmbeddingType embedding_type,
// }

// enum KnnDistanceType{
//     L2,
//     Cosine,
//     InnerProduct,
//     Hamming,
// }

// struct SelectRequest {
//     1:  string db_name,
//     2:  string table_name,
//     3:  list<ParsedExpr> select_list = [],
//     4:  ParsedExpr where_expr,
//     5:  list<ParsedExpr> group_by_list = [],
//     6:  ParsedExpr having_expr,
//     7:  ParsedExpr limit_expr,
//     8:  ParsedExpr offset_expr,
//     9:  i64 session_id,
// }

// struct SelectResponse {
//     1: bool success,
//     2: string error_msg,
//     3: list<ColumnDef> column_defs = [],

// }

// struct ColumnField {
//     1: ColumnType column_type,
//     2: binary column_vector,
// }

// enum ColumnType {
//     ColumnBool,
//     ColumnInt8,
//     ColumnInt16,
//     ColumnInt32,
//     ColumnInt64,
//     ColumnFloat32,
//     ColumnFloat64,
//     ColumnVerchar,
//     ColumnEmbedding,
//     ColumnInvalid,
// }

// struct ListDatabaseRequest {
//     1: i64 session_id,
// }

// struct ListDatabaseResponse {
//     1: bool success,
//     2: string error_msg,
//     3: list<string> db_names = [],
// }

// struct ListTableRequest {
//     1: string db_name,
//     2: i64 session_id,
// }

// struct ListTableResponse {
//     1: bool success,
//     2: string error_msg,
//     3: list<string> table_names = [],
// }

// struct DescribeDatabaseRequest {
//     1: string db_name,
//     2: i64 session_id,
// }

// struct DescribeDatabaseResponse {
//     1: bool success,
//     2: string error_msg,
//     3: i64 num_segments,
//     4: i64 num_rows,
//     5: i64 num_blocks,
// }

// struct DescribeTableRequest {
//     1: string db_name,
//     2: string table_name,
//     3: i64 session_id,
// }

// struct DescribeTableResponse {
//     1: bool success,
//     2: string error_msg,
//     3: i64 num_segments,
//     4: i64 num_rows,
//     5: i64 num_blocks,
// }

// struct GetTableRequest {
//     1: string db_name,
//     2: string table_name,
//     3: i64 session_id,
// }

// struct CreateIndexRequest {
//     1: string db_name,
//     2: string table_name,
//     3: string index_name,
//     4: list<string> column_names = [],
//     5: string method_type,
//     6: list<InitParameter> index_para_list = [],
//     7: i64 session_id,
//     8: Option option,
// }

// struct InitParameter {
//     1: string para_name,
//     2: string para_value,
// }

// struct DropIndexRequest {
//     1: string db_name,
//     2: string table_name,
//     3: string index_name,
//     4: i64 session_id,
// }

// struct GetDatabaseRequest {
//     1: string db_name,
//     2: i64 session_id,
// }








