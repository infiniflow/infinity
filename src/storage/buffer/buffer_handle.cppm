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
import third_party;

export module buffer_handle;

namespace infinity {

class BufferObj;
class FileWorker;

export class BufferHandle {

public:
    BufferHandle() = default;

    explicit BufferHandle(BufferObj *buffer_handle, void *data);

    BufferHandle(const BufferHandle &other);

    BufferHandle &operator=(const BufferHandle &other);

    BufferHandle(BufferHandle &&other);

    BufferHandle &operator=(BufferHandle &&other);

    ~BufferHandle();

public:
    [[nodiscard]] const void *GetData() const;

    [[nodiscard]] void *GetDataMut();

    const FileWorker *GetFileWorker() const;

    FileWorker *GetFileWorkerMut();

private:
    BufferObj *buffer_obj_{};

    void *data_{};
};

} // namespace infinity