
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







