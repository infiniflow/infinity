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

module stream_reader;

import stl;
import logger;
import status;
import infinity_exception;
import third_party;

namespace infinity {

StreamReader::~StreamReader() {
    Close();
}

Status StreamReader::Init(const String& file_name) {
    file_.open(file_name);
    if (!file_.is_open()) {
        return Status::IOError(fmt::format("{} can't open", file_name));
    }
    return Status::OK();
}

bool StreamReader::ReadLine(String& line) {
    if(getline(file_, line)) {
        return true;
    } else {
        return false;
    }
}

void StreamReader::Close() {
    file_.close();
}

} // namespace infinity