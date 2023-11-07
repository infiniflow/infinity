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

import stl;
import parser;
import data_block;

export module task_result;

namespace infinity {

enum class TaskResultType { kInvalid };

class TaskResult {
public:
    explicit TaskResult(TaskResultType task_result_type) : task_result_type_(task_result_type) {}

    void Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity);

    void Seal();

private:
    UniquePtr<DataBlock> data_{};
    TaskResultType task_result_type_{kInvalid};
};

} // namespace infinity
