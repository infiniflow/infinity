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

module extra_command;

import stl;
import third_party;

namespace infinity {

ExtraCommandType BaseExtraCommand::GetType() const { return type_; }

String LockTableCommand::ToString() { return fmt::format("LockTableCommand: table_key: {}", table_key_); }
String UnlockTableCommand::ToString() { return fmt::format("UnlockTableCommand: table_key: {}", table_key_); }
String DropTableCommand::ToString() { return fmt::format("DropTableCommand: db_id: {}, table_id: {}, table_key: {}", db_id_, table_id_, table_key_); }

} // namespace infinity