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

export module table_entry_type;

import stl;
import logger;
import infinity_exception;

namespace infinity {

export enum TableEntryType : i8 {
    kTableEntry,
    kCollectionEntry,
};

export inline String ToString(TableEntryType type) {
    switch (type) {
        case TableEntryType::kTableEntry: {
            return "Table";
        }
        case TableEntryType::kCollectionEntry: {
            return "Collection";
        }
        default: {
            String error_message = "Invalid base table type";
            UnrecoverableError(error_message);
        }
    }
}

} // namespace infinity
