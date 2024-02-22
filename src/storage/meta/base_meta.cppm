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

#include <concepts>

export module base_meta;

import stl;
import base_entry;
import infinity_exception;

namespace infinity {

export class MetaInterface {
public:
    virtual void Cleanup() = 0;

    virtual void CleanupDelete(TxnTimeStamp oldest_txn_ts) = 0;

    virtual void CleanupMeta() = 0;
};

export template <typename Meta>
concept MetaConcept = std::derived_from<Meta, MetaInterface> && std::derived_from<typename Meta::EntryT, EntryInterface>;

} // namespace infinity
