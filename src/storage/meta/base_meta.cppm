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

export module catalog:base_meta;

import stl;
import parser;
import status;
import infinity_exception;

namespace infinity {

class TxnManager;

export class BaseMeta {
public:
    static Status CheckDDLResult(Status result, ConflictType conflict_type) {
        switch (conflict_type) {
            case ConflictType::kError: {
                return result;
            }
            case ConflictType::kIgnore: {
                if (result.code() == ErrorCode::kDuplicate or result.code() == ErrorCode::kNotFound) {
                    return Status::OK();
                } else {
                    return result;
                }
            }
            default: {
                Error<StorageException>("Invalid conflict type.");
                return result;
            }
        }
    }
};

} // namespace infinity