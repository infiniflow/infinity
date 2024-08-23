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

export module options;

import stl;
import third_party;
import status;
import global_resource_usage;

namespace infinity {

export enum class BaseOptionDataType {
    kInteger,
    kFloat,
    kString,
    kBoolean,
    kLogLevel,
    kFlush,
    kInvalid,
};

export struct BaseOption {
    explicit BaseOption(std::string_view name, BaseOptionDataType data_type) : name_(std::move(name)), data_type_(data_type) {}
    virtual ~BaseOption() = default;

    String name_{};
    BaseOptionDataType data_type_{BaseOptionDataType::kInvalid};
};

export struct IntegerOption : public BaseOption {
    explicit IntegerOption(std::string_view name, i64 default_value, i64 upper_bound, i64 lower_bound)
        : BaseOption(std::move(name), BaseOptionDataType::kInteger), value_(default_value), upper_bound_(upper_bound), lower_bound_(lower_bound) {}

    [[nodiscard]] inline bool Validate() const { return value_ >= lower_bound_ && value_ <= upper_bound_; }

    i64 value_{};
    i64 upper_bound_{};
    i64 lower_bound_{};
};

export struct FloatOption : public BaseOption {
    explicit FloatOption(std::string_view name, f64 default_value, f64 upper_bound, f64 lower_bound)
        : BaseOption(std::move(name), BaseOptionDataType::kFloat), value_(default_value), upper_bound_(upper_bound), lower_bound_(lower_bound) {}

    f64 value_{};
    f64 upper_bound_{};
    f64 lower_bound_{};
};

export struct StringOption : public BaseOption {
    explicit StringOption(std::string_view name, std::string_view default_value)
        : BaseOption(std::move(name), BaseOptionDataType::kString), value_(std::move(default_value)) {}

    String value_{};
};

export struct BooleanOption : public BaseOption {
    explicit BooleanOption(std::string_view name, bool default_value) : BaseOption(std::move(name), BaseOptionDataType::kBoolean), value_(default_value) {}

    bool value_{};
};

export struct LogLevelOption : public BaseOption {
    explicit LogLevelOption(std::string_view name, LogLevel log_level) : BaseOption(std::move(name), BaseOptionDataType::kLogLevel), value_(log_level) {}

    LogLevel value_{};
};

export enum class FlushOptionType {
    kFlushAtOnce,
    kOnlyWrite,
    kFlushPerSecond,
};

export String FlushOptionTypeToString(FlushOptionType flush_option_type) {
    String flush_str;
    switch (flush_option_type) {
        case FlushOptionType::kFlushAtOnce: {
            flush_str = "FlushAtOnce";
            break;
        }
        case FlushOptionType::kOnlyWrite: {
            flush_str = "OnlyWrite";
            break;
        }
        case FlushOptionType::kFlushPerSecond: {
            flush_str = "FlushPerSecond";
            break;
        }
    }
    return flush_str;
}

export struct FlushOption : public BaseOption {
    explicit FlushOption(std::string_view name, FlushOptionType flush_type) : BaseOption(std::move(name), BaseOptionDataType::kFlush), value_(flush_type) {}

    FlushOptionType value_{};
};

export enum class GlobalOptionIndex {
    // System config
    kVersion = 0,
    kTimeZone = 1,
    kTimeZoneBias = 2,
    kWorkerCPULimit = 3,
    kServerAddress = 4,
    kPostgresPort = 5,
    kHTTPPort = 6,
    kClientPort = 7,
    kConnectionPoolSize = 8,
    kLogFileName = 9,
    kLogDir = 10,
    kLogToStdout = 11,
    kLogFileMaxSize = 12,
    kLogFileRotateCount = 13,
    kLogLevel = 14,
    kDataDir = 15,
    kCleanupInterval = 16,
    kCompactInterval = 17,
    kOptimizeIndexInterval = 18,
    kMemIndexCapacity = 19,
    kBufferManagerSize = 20,
    kLRUNum = 21,
    kTempDir = 22,
    kWALDir = 23,
    kWALCompactThreshold = 24,
    kFullCheckpointInterval = 25,
    kDeltaCheckpointInterval = 26,
    kDeltaCheckpointThreshold = 27,
    kFlushMethodAtCommit = 28,
    kResourcePath = 29,
    kRecordRunningQuery = 30,
    kPersistenceDir = 31,
    kPersistenceObjectSizeLimit = 32,
    kMemIndexMemoryQuota = 33,
    kInvalid = 34,
};

export struct GlobalOptions {
    GlobalOptions();
    Status AddOption(UniquePtr<BaseOption> option);
    GlobalOptionIndex GetOptionIndex(const String &option_name) const;
    Tuple<BaseOption *, Status> GetOptionByName(const String &option_name);
    BaseOption *GetOptionByIndex(GlobalOptionIndex option_index);

    String GetStringValue(GlobalOptionIndex option_index);
    i64 GetIntegerValue(GlobalOptionIndex option_index);
    bool GetBoolValue(GlobalOptionIndex option_index);

    Vector<UniquePtr<BaseOption>> options_;
    HashMap<String, GlobalOptionIndex> name2index_;
};

} // namespace infinity
