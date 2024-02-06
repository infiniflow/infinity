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

module;

export module bound_statement;

import stl;
import logical_node;
import query_context;

import internal_types;
import data_type;

namespace infinity {

export struct BoundStatement {
public:
    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<SharedPtr<DataType>>> types_ptr_{};

    virtual SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) = 0;
    virtual ~BoundStatement() = default;
};

} // namespace infinity
