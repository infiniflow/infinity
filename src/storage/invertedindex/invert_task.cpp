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

module invert_task;

import stl;
import column_inverter;
import column_vector;

import internal_types;

namespace infinity {

InvertTask::InvertTask(ColumnInverter *inverter, const String &value, u32 row_id) : inverter_(inverter), value_(value), row_id_(row_id) {}

void InvertTask::Run() { inverter_->InvertColumn(row_id_, value_); }

BatchInvertTask::BatchInvertTask(ColumnInverter *inverter, const ColumnVector &column_vector, RowID start_row_id)
    : inverter_(inverter), column_vector_(column_vector), start_row_id_(start_row_id) {}

void BatchInvertTask::Run() { inverter_->InvertColumn(column_vector_, start_row_id_); }

} // namespace infinity
