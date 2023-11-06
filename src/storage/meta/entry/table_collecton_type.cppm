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

import infinity_exception;

export module table_collection_type;

namespace infinity {

export enum TableCollectionType : i8 {
    kTableEntry,
    kCollectionEntry,
};

export inline String ToString(TableCollectionType type) {
    switch (type) {
        case TableCollectionType::kTableEntry: {
            return "Table";
        }
        case TableCollectionType::kCollectionEntry: {
            return "Collection";
        }
        default: {
            Error<ExecutorException>("Invalid base table type");
        }
    }
}

} // namespace infinity
