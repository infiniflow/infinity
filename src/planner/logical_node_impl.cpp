//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

#include <vector>
module infinity_core:logical_node.impl;

import :logical_node;

import :column_binding;
import :stl;
import :logical_node;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalCommonFunctionUsingLoadMeta::GetColumnBindings(const LogicalNode &op) {
    auto bindings = op.left_node()->GetColumnBindings();
    if (auto ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            bindings.push_back(load_meta.binding_);
        }
    }
    return bindings;
}

SharedPtr<Vector<String>> LogicalCommonFunctionUsingLoadMeta::GetOutputNames(const LogicalNode &op) {
    auto output_names = op.left_node()->GetOutputNames();
    if (auto ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_names->insert(output_names->begin() + load_meta.index_, load_meta.column_name_);
        }
    }
    return output_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCommonFunctionUsingLoadMeta::GetOutputTypes(const LogicalNode &op) {
    auto output_types = op.left_node()->GetOutputTypes();
    if (auto ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_types->insert(output_types->begin() + load_meta.index_, load_meta.type_);
        }
    }
    return output_types;
}

} // namespace infinity
