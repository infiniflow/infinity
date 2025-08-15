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

export module infinity_core:cross_product_table_ref;

import :table_ref;

import std;

import table_reference;

namespace infinity {

export class CrossProductTableRef : public TableRef {
public:
    explicit CrossProductTableRef(std::string alias) : TableRef(TableRefType::kCrossProduct, alias) {}

    //    std::shared_ptr<BindContext> left_bind_context_{};
    //    std::shared_ptr<BindContext> right_bind_context_{};

    std::shared_ptr<TableRef> left_table_ref_{};
    std::shared_ptr<TableRef> right_table_ref_{};
};

} // namespace infinity