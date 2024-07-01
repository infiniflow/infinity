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

#pragma once

import stl;
import file_system;
import local_file_system;
import infinity_exception;
import file_system_type;

using namespace infinity;

namespace benchmark {

template <typename T>
Tuple<SizeT, i32, UniquePtr<T[]>> DecodeFVecsDataset(const Path &path) {
    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(path.string(), FileFlags::READ_FLAG, FileLockType::kReadLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    i32 dim = 0;
    file_handler->Read(&dim, sizeof(dim));
    SizeT file_size = fs.GetFileSize(*file_handler);
    SizeT vec_num = file_size / (dim * sizeof(T) + sizeof(dim));
    auto data = MakeUniqueForOverwrite<T[]>(vec_num * dim);
    for (SizeT i = 0; i < vec_num - 1; ++i) {
        file_handler->Read(data.get() + i * dim, dim * sizeof(T));
        i32 dim1 = 0;
        file_handler->Read(&dim1, sizeof(dim1));
        if (dim1 != dim) {
            UnrecoverableError("dim not match");
        }
    }
    file_handler->Read(data.get() + (vec_num - 1) * dim, dim * sizeof(T));
    return {vec_num, dim, std::move(data)};
}

} // namespace benchmark
