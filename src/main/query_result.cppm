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
import status;
import logical_node_type;

namespace infinity {

export struct BaseResult {
public:

    [[nodiscard]] inline bool IsOk() const { return status_.ok(); }
    [[nodiscard]] inline ErrorCode ErrorCode() const { return status_.code(); }
    [[nodiscard]] inline DataTable* ResultTable() const { return result_table_.get(); }

//    template<typename ObjectType>
//    inline ObjectType* AsInfinity() { return static_cast<ObjectType*>(database_object_.get()); }

public:
    Status status_{};
    SharedPtr<DataTable> result_table_{};
//    SharedPtr<DatabaseObject> database_object_{};
};

export struct QueryResult : public BaseResult {
    LogicalNodeType root_operator_type_{LogicalNodeType::kInvalid};
    String ToString() const;
};

}
