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

module options;

import infinity_exception;
import third_party;
import status;
import default_values;
import logger;

namespace infinity {

GlobalOptions::GlobalOptions() {
    i32 invalid_option_index = static_cast<i32>(GlobalOptionIndex::kInvalid);
    options_.resize(invalid_option_index);

    name2index_[String(VERSION_OPTION_NAME)] = GlobalOptionIndex::kVersion;
    name2index_[String(SERVER_MODE_OPTION_NAME)] = GlobalOptionIndex::kServerMode;
    name2index_[String(TIME_ZONE_OPTION_NAME)] = GlobalOptionIndex::kTimeZone;
    name2index_[String(TIME_ZONE_BIAS_OPTION_NAME)] = GlobalOptionIndex::kTimeZoneBias;
    name2index_[String(CPU_LIMIT_OPTION_NAME)] = GlobalOptionIndex::kWorkerCPULimit;
    name2index_[String(SERVER_ADDRESS_OPTION_NAME)] = GlobalOptionIndex::kServerAddress;
    name2index_[String(PEER_SERVER_IP_OPTION_NAME)] = GlobalOptionIndex::kPeerServerIP;
    name2index_[String(PEER_SERVER_PORT_OPTION_NAME)] = GlobalOptionIndex::kPeerServerPort;

    name2index_[String(POSTGRES_PORT_OPTION_NAME)] = GlobalOptionIndex::kPostgresPort;
    name2index_[String(HTTP_PORT_OPTION_NAME)] = GlobalOptionIndex::kHTTPPort;
    name2index_[String(CLIENT_PORT_OPTION_NAME)] = GlobalOptionIndex::kClientPort;
    name2index_[String(CONNECTION_POOL_SIZE_OPTION_NAME)] = GlobalOptionIndex::kConnectionPoolSize;
    name2index_[String(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME)] = GlobalOptionIndex::kPeerServerConnectionPoolSize;
    name2index_[String(LOG_FILENAME_OPTION_NAME)] = GlobalOptionIndex::kLogFileName;

    name2index_[String(LOG_DIR_OPTION_NAME)] = GlobalOptionIndex::kLogDir;
    name2index_[String(LOG_TO_STDOUT_OPTION_NAME)] = GlobalOptionIndex::kLogToStdout;
    name2index_[String(LOG_FILE_MAX_SIZE_OPTION_NAME)] = GlobalOptionIndex::kLogFileMaxSize;
    name2index_[String(LOG_FILE_ROTATE_COUNT_OPTION_NAME)] = GlobalOptionIndex::kLogFileRotateCount;
    name2index_[String(LOG_LEVEL_OPTION_NAME)] = GlobalOptionIndex::kLogLevel;

    name2index_[String(DATA_DIR_OPTION_NAME)] = GlobalOptionIndex::kDataDir;
    name2index_[String(CLEANUP_INTERVAL_OPTION_NAME)] = GlobalOptionIndex::kCleanupInterval;
    name2index_[String(COMPACT_INTERVAL_OPTION_NAME)] = GlobalOptionIndex::kCompactInterval;
    name2index_[String(OPTIMIZE_INTERVAL_OPTION_NAME)] = GlobalOptionIndex::kOptimizeIndexInterval;
    name2index_[String(MEM_INDEX_CAPACITY_OPTION_NAME)] = GlobalOptionIndex::kMemIndexCapacity;

    name2index_[String(PERSISTENCE_DIR_OPTION_NAME)] = GlobalOptionIndex::kPersistenceDir;
    name2index_[String(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME)] = GlobalOptionIndex::kPersistenceObjectSizeLimit;

    name2index_[String(STORAGE_TYPE_NAME)] = GlobalOptionIndex::kStorageType;

    name2index_[String(BUFFER_MANAGER_SIZE_OPTION_NAME)] = GlobalOptionIndex::kBufferManagerSize;
    name2index_[String(LRU_NUM_OPTION_NAME)] = GlobalOptionIndex::kLRUNum;
    name2index_[String(TEMP_DIR_OPTION_NAME)] = GlobalOptionIndex::kTempDir;
    name2index_[String(MEMINDEX_MEMORY_QUOTA_OPTION_NAME)] = GlobalOptionIndex::kMemIndexMemoryQuota;

    name2index_[String(WAL_DIR_OPTION_NAME)] = GlobalOptionIndex::kWALDir;
    name2index_[String(WAL_COMPACT_THRESHOLD_OPTION_NAME)] = GlobalOptionIndex::kWALCompactThreshold;
    name2index_[String(FULL_CHECKPOINT_INTERVAL_OPTION_NAME)] = GlobalOptionIndex::kFullCheckpointInterval;

    name2index_[String(DELTA_CHECKPOINT_INTERVAL_OPTION_NAME)] = GlobalOptionIndex::kDeltaCheckpointInterval;
    name2index_[String(DELTA_CHECKPOINT_THRESHOLD_OPTION_NAME)] = GlobalOptionIndex::kDeltaCheckpointThreshold;
    name2index_[String(WAL_FLUSH_OPTION_NAME)] = GlobalOptionIndex::kFlushMethodAtCommit;
    name2index_[String(RESOURCE_DIR_OPTION_NAME)] = GlobalOptionIndex::kResourcePath;

    name2index_[String(RECORD_RUNNING_QUERY_OPTION_NAME)] = GlobalOptionIndex::kRecordRunningQuery;
}

Status GlobalOptions::AddOption(UniquePtr<BaseOption> option) {
    GlobalOptionIndex global_option_index = GetOptionIndex(option->name_);
    if(global_option_index == GlobalOptionIndex::kInvalid) {
        return Status::InvalidConfig(fmt::format("Attempt to insert option: {}, which isn't supported", option->name_));
    }

    SizeT option_index = static_cast<SizeT>(global_option_index);
    if (option_index >= options_.size()) {
        return Status::InvalidConfig(fmt::format("Attempt to insert option: {} with index {} at index: {}", option->name_, option_index, options_.size()));
    }

    if(options_[option_index].get() != nullptr) {
        return Status::InvalidConfig(fmt::format("Attempt to add an existed option", option->name_));
    }

    options_[option_index] = std::move(option);

    return Status::OK();
}

GlobalOptionIndex GlobalOptions::GetOptionIndex(const String &option_name) const {
    auto iter = name2index_.find(option_name);
    if(iter == name2index_.end()) {
        return GlobalOptionIndex::kInvalid;
    }
    return iter->second;
}

Tuple<BaseOption *, Status> GlobalOptions::GetOptionByName(const String &option_name) {
    auto iter = name2index_.find(option_name);
    if (iter == name2index_.end()) {
        return {nullptr, Status::InvalidConfig(fmt::format("Attempt to get option: {} which doesn't exist.", option_name))};
    }

    SizeT option_index = static_cast<SizeT>(iter->second);
    return {options_[option_index].get(), Status::OK()};
}

BaseOption *GlobalOptions::GetOptionByIndex(GlobalOptionIndex global_option_index) {
    SizeT option_index = static_cast<SizeT>(global_option_index);
    return options_[option_index].get();
}

String GlobalOptions::GetStringValue(GlobalOptionIndex option_index) {
    BaseOption* base_option = GetOptionByIndex(option_index);
    if(base_option->data_type_ != BaseOptionDataType::kString) {
        String error_message = "Attempt to fetch string value from non-string data type option";
        UnrecoverableError(error_message);
    }
    StringOption* string_option = static_cast<StringOption*>(base_option);
    return string_option->value_;
}

i64 GlobalOptions::GetIntegerValue(GlobalOptionIndex option_index) {
    BaseOption* base_option = GetOptionByIndex(option_index);
    if(base_option->data_type_ != BaseOptionDataType::kInteger) {
        String error_message = "Attempt to fetch integer value from non-integer data type option";
        UnrecoverableError(error_message);
    }
    IntegerOption* integer_option = static_cast<IntegerOption*>(base_option);
    return integer_option->value_;
}

bool GlobalOptions::GetBoolValue(GlobalOptionIndex option_index) {
    BaseOption* base_option = GetOptionByIndex(option_index);
    if(base_option->data_type_ != BaseOptionDataType::kBoolean) {
        String error_message = "Attempt to fetch bool value from non-bool data type option";
        UnrecoverableError(error_message);
    }
    BooleanOption* boolean_option = static_cast<BooleanOption*>(base_option);
    return boolean_option->value_;
}

}
