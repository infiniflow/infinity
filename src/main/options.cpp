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

namespace infinity {

void GlobalOptions::AddOption(UniquePtr<BaseOption> option, GlobalOptionIndex global_option_index) {
    SizeT option_index = static_cast<SizeT>(global_option_index);
    if (option_index != options_.size()) {
        UnrecoverableError(fmt::format("Attempt to insert option: {} with index {} at index: {}", option->name_, option_index, options_.size()));
    }
    name2index_.emplace(option->name_, global_option_index);
    options_.emplace_back(std::move(option));
}

GlobalOptionIndex GlobalOptions::GetOptionIndex(const String &option_name) { return name2index_[option_name]; }

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
        UnrecoverableError("Attempt to fetch string value from non-string data type option");
    }
    StringOption* string_option = static_cast<StringOption*>(base_option);
    return string_option->value_;
}

i64 GlobalOptions::GetIntegerValue(GlobalOptionIndex option_index) {
    BaseOption* base_option = GetOptionByIndex(option_index);
    if(base_option->data_type_ != BaseOptionDataType::kInteger) {
        UnrecoverableError("Attempt to fetch integer value from non-integer data type option");
    }
    IntegerOption* integer_option = static_cast<IntegerOption*>(base_option);
    return integer_option->value_;
}

bool GlobalOptions::GetBoolValue(GlobalOptionIndex option_index) {
    BaseOption* base_option = GetOptionByIndex(option_index);
    if(base_option->data_type_ != BaseOptionDataType::kBoolean) {
        UnrecoverableError("Attempt to fetch bool value from non-bool data type option");
    }
    BooleanOption* boolean_option = static_cast<BooleanOption*>(base_option);
    return boolean_option->value_;
}

}
