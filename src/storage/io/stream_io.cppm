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

#include <fstream>

export module stream_io;

import stl;
import status;

namespace infinity {

export class StreamIO {

public:
    StreamIO() = default;
    ~StreamIO();

    void Init(const String& file_name, u8 flags);
    bool ReadLine(String& line);
    void Close();

private:
    std::fstream file_;
    bool reader_{false};
    bool writer_{false};
};

} // namespace infinity