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
import status;
import logical_node_type;
import global_resource_usage;

namespace infinity {

export struct BaseResult {
public:
    BaseResult() {
        GlobalResourceUsage::IncrObjectCount("BaseResult");
    }

    BaseResult(BaseResult& other): status_(other.status_), result_table_(other.result_table_) {
        GlobalResourceUsage::IncrObjectCount("BaseResult");
    }

    ~BaseResult() {
        GlobalResourceUsage::DecrObjectCount("BaseResult");
    }

    BaseResult& operator=(BaseResult&& other)  noexcept {
        status_ = std::move(other.status_);
        result_table_ = std::move(other.result_table_);
        return *this;
    }

    [[nodiscard]] inline bool IsOk() const { return status_.ok(); }
    [[nodiscard]] inline ErrorCode ErrorCode() const { return status_.code(); }
    [[nodiscard]] inline DataTable* ResultTable() const { return result_table_.get(); }
    [[nodiscard]] inline const char *ErrorMsg() const { return status_.message(); }
    [[nodiscard]] inline String& ErrorStr() const { return *status_.msg_; }

public:
    Status status_{};
    SharedPtr<DataTable> result_table_{};
};

export struct QueryResult : public BaseResult {
    LogicalNodeType root_operator_type_{LogicalNodeType::kInvalid};
    String ToString() const;

    static QueryResult UnusedResult() {
        return {};
    }
};

}
