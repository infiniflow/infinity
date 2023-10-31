//
// Created by jinhai on 23-10-30.
//

export module query_result;

import stl;
import data_table;
import database_object;

namespace infinity {

export struct QueryResult {
public:

    [[nodiscard]] inline bool IsOk() const { return error_code_ == 0; }
    [[nodiscard]] inline i64 ErrorCode() const { return error_code_; }
    [[nodiscard]] inline DataTable* ResultTable() const { return result_table_.get(); }

    template<typename ObjectType>
    inline ObjectType* AsInfinity() { return static_cast<ObjectType*>(database_object_.get()); }

public:
    i64 error_code_{};
    SharedPtr<String> error_message_{};
    SharedPtr<DataTable> result_table_{};
    SharedPtr<DatabaseObject> database_object_{};
};

}
