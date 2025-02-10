// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
#include <chrono>
export module DatetimeManager;

import config;
import stl;
import fragment_task;
import blocking_queue;
import base_statement;

namespace infinity {

export class DatetimeManager {
public:
    DatetimeManager() : SystemTimeZone(nullptr), SystemDate(nullptr), UserTimeZone(nullptr), UserDate(nullptr) {}

    ~DatetimeManager() {
        delete[] UserTimeZone;
        delete[] UserDate;
    }

    DatetimeManager(const DatetimeManager& other) :
        SystemTimeZone(other.SystemTimeZone),
        SystemDate(other.SystemDate),
        UserTimeZone(nullptr),
        UserDate(nullptr) {
        if (other.UserTimeZone) {
            UserTimeZone = new char[strlen(other.UserTimeZone) + 1];
            strcpy(UserTimeZone, other.UserTimeZone);
        }
        if (other.UserDate) {
            UserDate = new char[strlen(other.UserDate) + 1];
            strcpy(UserDate, other.UserDate);
        }
    }

    DatetimeManager& operator=(const DatetimeManager& other) {
        if (this != &other) {
            SystemTimeZone = other.SystemTimeZone;
            SystemDate = other.SystemDate;
            delete[] UserTimeZone;
            UserTimeZone = nullptr;
            delete[] UserDate;
            UserDate = nullptr;
            if (other.UserTimeZone) {
                UserTimeZone = new char[strlen(other.UserTimeZone) + 1];
                strcpy(UserTimeZone, other.UserTimeZone);
            }
            if (other.UserDate) {
                UserDate = new char[strlen(other.UserDate) + 1];
                strcpy(UserDate, other.UserDate);
            }
        }
        return *this;
    }

    void update() {}

    const char* SystemTimeZone;
    const char* SystemDate;
    char* UserTimeZone;
    char* UserDate;

};


} // namespace infinity