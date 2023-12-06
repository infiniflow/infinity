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
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;

export module physical_create_collection;

namespace infinity {

export class PhysicalCreateCollection final : public PhysicalOperator {
public:
    explicit PhysicalCreateCollection(SharedPtr<String> schema_name,
                                      SharedPtr<String> collection_name,
                                      ConflictType conflict_type,
                                      SharedPtr<Vector<String>> output_names,
                                      SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                      u64 table_index,
                                      u64 id,
                                      SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalCreateCollection() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 table_index() const { return table_index_; }

    inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    inline const SharedPtr<String> &collection_name() const { return collection_name_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
