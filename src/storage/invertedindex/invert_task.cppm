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

export module invert_task;

import stl;
import task_executor;
import column_inverter;
import column_vector;

import internal_types;

namespace infinity {

export class InvertTask : public TaskExecutor::Task {
public:
    explicit InvertTask(ColumnInverter *inverter, const String &value, u32 row_id);

    ~InvertTask() = default;

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};

    const String &value_;

    u32 row_id_;
};

export class BatchInvertTask : public TaskExecutor::Task {
public:
    BatchInvertTask(ColumnInverter *inverter, const ColumnVector &column_vector, RowID start_row_id);

    ~BatchInvertTask() = default;

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};

    ColumnVector column_vector_;

    RowID start_row_id_;
};
} // namespace infinity
