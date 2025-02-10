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
#include <string>
export module DatetimeManager;

import config;
import stl;
import fragment_task;
import blocking_queue;
import base_statement;

namespace infinity {

export class DatetimeManager {
public:
    DatetimeManager() = default;

    DatetimeManager(const DatetimeManager& other) = default;

    DatetimeManager& operator=(const DatetimeManager& other) = default;

    ~DatetimeManager() = default;

    void updateTZ(const std::string& user_timezone);
    void updateDate(const std::string& user_date);

    const std::string& getSystemTimeZone() const { return SystemTimeZone; }
    const std::string& getSystemDate() const { return SystemDate; }
    const std::string& getUserTimeZone() const { return UserTimeZone; }
    const std::string& getUserDate() const { return UserDate; }

    void setSystemTimeZone(const std::string& timezone) { SystemTimeZone = timezone; }
    void setSystemDate(const std::string& date) { SystemDate = date; }
    void setUserTimeZone(const std::string& timezone) { UserTimeZone = timezone; }
    void setUserDate(const std::string& date) { UserDate = date; }

private:
    std::string SystemTimeZone;
    std::string SystemDate;
    std::string UserTimeZone;
    std::string UserDate;
};


} // namespace infinity