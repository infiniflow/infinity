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

module stream_io;

import stl;
import logger;
import status;
import file_system_type;
import infinity_exception;
import third_party;

namespace infinity {

StreamIO::~StreamIO() = default;

void StreamIO::Init(const String& file_name, u8 flags) {
    bool reader_ = flags & FileFlags::READ_FLAG;
    bool writer_ = flags & FileFlags::WRITE_FLAG;
    if (reader_ && writer_) {
        file_.open(file_name, std::ios::in | std::ios::out);
    } else if (reader_) {
        file_.open(file_name, std::ios::in);
    } else if (writer_) {
        file_.open(file_name, std::ios::out);
    } else {
        Status status = Status::InvalidCommand("Not reachable");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }

    if (!file_.is_open()) {
        Status status = Status::IOError(fmt::format("{} can't open", file_name));
        LOG_ERROR(file_name);
        RecoverableError(status);
    }
}

bool StreamIO::ReadLine(String& line) {
    if(getline(file_, line)) {
        return true;
    } else {
        return false;
    }
}

void StreamIO::Close() {
    file_.close();
}

} // namespace infinity