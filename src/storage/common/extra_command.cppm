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

export module extra_command;

import stl;

namespace infinity {

export enum class ExtraCommandType {
    kLockTable,
    kUnlockTable,
};

export struct BaseExtraCommand {
public:
    BaseExtraCommand(ExtraCommandType type) : type_(type) {}

    ExtraCommandType GetType() const;

    virtual String ToString() = 0;

private:
    ExtraCommandType type_;
};

export struct LockTableCommand final : public BaseExtraCommand {
public:
    LockTableCommand(const String &table_key) : BaseExtraCommand(ExtraCommandType::kLockTable), table_key_(table_key) {}

    String ToString() final;
    const String &table_key() const { return table_key_; }

private:
    String table_key_;
};

export struct UnlockTableCommand final : public BaseExtraCommand {
public:
    UnlockTableCommand(const String &table_key) : BaseExtraCommand(ExtraCommandType::kUnlockTable), table_key_(table_key) {}

    String ToString() final;
    const String &table_key() const { return table_key_; }

private:
    String table_key_;
};

} // namespace infinity
