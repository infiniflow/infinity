// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module base_meta;

import stl;
import infinity_exception;
import meta_type;
//  import lru;
//  import status;
//  import meta_info;
//  import extra_ddl_info;
//  import default_values;
//  import internal_types;
//  import third_party;
//  import status;

namespace infinity {

export struct BaseMeta {
public:
    BaseMeta(MetaType meta_type) : type_(meta_type) {
        if (meta_type == MetaType::kInvalid) {
            UnrecoverableError("BaseMeta cannot be initialized with kInvalid type");
        }
    }
    virtual ~BaseMeta() = default;

    MetaType Type() const { return type_; }

private:
    MetaType type_{MetaType::kInvalid};
};
// using MetaCache = lru_cache<String, SharedPtr<BaseMeta>>;

} // namespace infinity