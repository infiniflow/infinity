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

export module physical_update;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_entry;
import base_expression;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import logger;

namespace infinity {

export class PhysicalUpdate : public PhysicalOperator {
public:
    explicit PhysicalUpdate(u64 id,
                            UniquePtr<PhysicalOperator> left,
                            TableEntry *table_entry_ptr,
                            const Vector<Pair<SizeT, SharedPtr<BaseExpression>>> &update_columns,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kUpdate, std::move(left), nullptr, id, load_metas), table_entry_ptr_(table_entry_ptr),
          update_columns_(update_columns) {}

    ~PhysicalUpdate() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    TableEntry *table_entry_ptr_;
    const Vector<Pair<SizeT, SharedPtr<BaseExpression>>> &update_columns_;

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
