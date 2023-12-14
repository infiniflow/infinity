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

#include <string>

import compilation_config;
import local_file_system;
import profiler;
import infinity;
import database;
import table;
import parser;

using namespace infinity;

int main() {
    std::string infinity_path = "/tmp/infinity";
    Infinity::LocalInit(infinity_path);

    std::string db_name = "default";
    std::string table_name = "fulltext_benchmark";
    std::string index_name = "fulltext_index";

    
}