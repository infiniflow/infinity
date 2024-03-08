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

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "query_driver.h"

int main(const int argc, const char **argv) {
    /** check for the right # of arguments **/
    int rc = 0;
    if (argc == 2) {
        infinity::QueryDriver driver;
        /** example for piping input from terminal, i.e., using cat **/
        if (std::strncmp(argv[1], "-o", 2) == 0) {
            rc = driver.ParseStream(std::cin);
        }
        /** simple help menu **/
        else if (std::strncmp(argv[1], "-h", 2) == 0) {
            std::cout << "use -o for pipe to std::cin\n";
            std::cout << "just give a filename to read from\n";
            std::cout << "use -h to get this menu\n";
            return (EXIT_SUCCESS);
        }
        /** example reading input from a file **/
        else {
            /** assume file, read and parse line by line **/
            std::ifstream in_file(argv[1]);
            rc = driver.ParseStream(in_file);
        }
    } else {
        /** exit with failure condition **/
        return (EXIT_FAILURE);
    }
    return rc;
}
