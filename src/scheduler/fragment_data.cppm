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
import data_block;

export module fragment_data;

namespace infinity {

export enum class FragmentDataType {
    kData,
    kNone,
    kError,
    kInvalid,
};

export struct FragmentDataBase {
    FragmentDataType type_{FragmentDataType::kInvalid};
    u64 fragment_id_{u64_max};

    FragmentDataBase(FragmentDataType type, u64 fragment_id) : type_(type), fragment_id_(fragment_id) {}
};

export struct FragmentError : public FragmentDataBase {
    UniquePtr<String> error_message_{};

    FragmentError(u64 fragment_id, UniquePtr<String> error_message)
        : FragmentDataBase(FragmentDataType::kError, fragment_id), error_message_(Move(error_message)) {}
};

export struct FragmentData : public FragmentDataBase {
    UniquePtr<DataBlock> data_block_{};
    i64 task_id_{-1};
    Optional<SizeT> data_idx_{};
    SizeT data_count_{u64_max};

    FragmentData(u64 fragment_id, UniquePtr<DataBlock> data_block, i64 task_id, SizeT data_idx, SizeT data_count)
        : FragmentDataBase(FragmentDataType::kData, fragment_id), data_block_(Move(data_block)), task_id_(task_id), data_idx_(data_idx),
          data_count_(data_count) {}
};

export struct FragmentNone : public FragmentDataBase {
    FragmentNone(u64 fragment_id) : FragmentDataBase(FragmentDataType::kNone, fragment_id) {}
};

} // namespace infinity
