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
import table_collection_type;

export module table_collection_detail;

namespace infinity {

export struct TableCollectionDetail {
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_collection_name_{};
    TableCollectionType table_collection_type_;
    i64 column_count_{};
    i64 row_count_{};
    i64 segment_count_{};
    i64 block_count_{};
    i64 segment_capacity_{};
};

} // namespace infinity
