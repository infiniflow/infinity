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

export module infinity_core:file_worker_type;

import :stl;
import :infinity_exception;

namespace infinity {

export enum class FileWorkerType {
    kDataFile,
    kVarFile,
    kIVFIndexFile,
    kHNSWIndexFile,
    kRawFile,
    kSecondaryIndexFile,
    kVersionDataFile,
    kIndexFile,
    kEMVBIndexFile,
    kBMPIndexFile,
    kInvalid,
};

export String FileWorkerType2Str(FileWorkerType type) {
    switch (type) {
        case FileWorkerType::kDataFile: {
            return "data";
        }
        case FileWorkerType::kVarFile: {
            return "var";
        }
        case FileWorkerType::kIVFIndexFile: {
            return "IVF index";
        }
        case FileWorkerType::kHNSWIndexFile: {
            return "HNSW index";
        }
        case FileWorkerType::kRawFile: {
            return "raw";
        }
        case FileWorkerType::kSecondaryIndexFile: {
            return "secondary index";
        }
        case FileWorkerType::kVersionDataFile: {
            return "version data";
        }
        case FileWorkerType::kIndexFile: {
            return "index";
        }
        case FileWorkerType::kEMVBIndexFile: {
            return "EMVB index";
        }
        case FileWorkerType::kBMPIndexFile: {
            return "BMP index";
        }
        case FileWorkerType::kInvalid: {
            String error_message = "Invalid file worker type";
            UnrecoverableError(error_message);
        }
    }
    String error_message = "Unreachable";
    UnrecoverableError(error_message);
    return error_message;
}

} // namespace infinity
