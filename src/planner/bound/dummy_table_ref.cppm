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

export module infinity_core:dummy_table_ref;

import :stl;

import :table_ref;
import table_reference;

namespace infinity {

class DummyTableRef : public TableRef {
public:
    explicit DummyTableRef() : TableRef(TableRefType::kDummy, "Dummy") {}
};

} // namespace infinity
