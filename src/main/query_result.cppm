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
