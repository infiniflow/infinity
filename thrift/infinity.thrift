
namespace py3 infinity_thrift


struct CreateDatabaseRequest {
     1:  string db_name,
     2:  i64 session_id,
}

struct CommonResponse {
    1:  bool success,
    2:  string error_msg,
    3:  i64 session_id,
}

struct CommonRequest {
    1:  i64 session_id,
}

service InfinityService {
    CommonResponse Connect(1:CommonRequest req),
    CommonResponse CreateDatabase(1:CreateDatabaseRequest req)
}



union ParsedExprType {
   1: ConstantExpr constant_expr,
   2: ColumnExpr column_expr,
   3: FunctionExpr function_expr,
}

struct ParsedExpr {
   1: ParsedExprType type ,
   2: string alias,
}

enum LiteralType {
    kBoolean,
    kDouble,
    kString,
    kInt64,
    kNull,
    kIntegerArray,
    kDoubleArray,
}

struct ConstantExpr {
    1: LiteralType literal_type,
    2: bool bool_value,
    3: int64 i64_value,
    4: double f64_value,
    5: string str_value,
}

struct ColumnExpr {
    1: list<string> column_name = [],
    2: bool star,
}

struct FunctionExpr {
    1: string function_name,
    2: list<ParsedExpr> arguments =[],
}



