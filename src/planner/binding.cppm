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

export module infinity_core:binding;

import :meta_info;

import internal_types;
import data_type;

namespace infinity {

struct BlockIndex;

export enum class BindingType { kInvalid, kTable, kSubquery, kCTE, kView };

export class Binding {
public:
    Binding() = default;

    virtual ~Binding();

    static std::shared_ptr<Binding> MakeBinding(BindingType binding_type,
                                                const std::string &name,
                                                u64 table_index,
                                                std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                                std::shared_ptr<std::vector<std::string>> column_names);

    static std::shared_ptr<Binding> MakeBinding(BindingType binding_type,
                                                const std::string &name,
                                                u64 table_index,
                                                std::shared_ptr<TableInfo> table_info,
                                                std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                                std::shared_ptr<std::vector<std::string>> column_names,
                                                std::shared_ptr<BlockIndex> block_index);

    // Binding type
    BindingType binding_type_{BindingType::kInvalid};

    // Binding table alias or name
    std::string table_name_{};

    u64 table_index_{};

    // Node id of the corresponding logical node
    //    i64 logical_node_id_{-1};

    // ptr to the corresponding logical node.
    //    std::shared_ptr<LogicalNode> logical_node_ptr_{nullptr};

    // if the binding is table, this is the table_ptr
    std::shared_ptr<TableInfo> table_info_{nullptr};

    std::shared_ptr<BlockIndex> block_index_{};

    // Column types of the binding
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types_{};

    // Column names of the binding
    std::shared_ptr<std::vector<std::string>> column_names_{};

    // Column name -> column index mapping
    std::unordered_map<std::string, i64> name2index_{};
};

// class TableBinding: public GenericBinding {
// public:
//
// };

} // namespace infinity
