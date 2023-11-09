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

import stl;
import txn;
import config;
import options;

export module session;

namespace infinity {

enum class SessionType {
    kEmbedded,
    kRemote,
};

export class SessionBase {

public:
    explicit SessionBase(SessionType session_type) : current_database_("default"), session_type_(session_type) {}

    inline void set_current_schema(const String &current_database) { current_database_ = current_database; }
    [[nodiscard]] inline String &current_database() { return current_database_; }
    SessionOptions* options() { return &session_options; }

    inline Txn*& txn() { return txn_; }
protected:
    // Current schema
    String current_database_{};

    // Txn is session level.
    Txn *txn_{};

    SessionType session_type_{SessionType::kRemote};

    SessionOptions session_options;
};

export class EmbeddedSession : public SessionBase {

public:
    EmbeddedSession() : SessionBase(SessionType::kEmbedded) {}
};

export class RemoteSession : public SessionBase {

public:
    RemoteSession() : SessionBase(SessionType::kRemote) {}

    [[nodiscard]] inline const String &user_name() const { return user_name_; }

    [[nodiscard]] inline u64 user_id() const { return user_id_; }

    inline void SetClientInfo(const String &ip_address, u16 port) {
        client_address_ = ip_address;
        client_port_ = port;
    }

private:
    // User / Tenant information
    String tenant_name_{};
    String user_name_{};

    u64 user_id_{0};

    String client_address_{};
    u16 client_port_{};
};

//export class Session {
//public:
//    explicit Session() : current_database_("default") {}
//
//    [[nodiscard]] inline String &current_database() { return current_database_; }
//
//    [[nodiscard]] inline const String &user_name() const { return user_name_; }
//
//    [[nodiscard]] inline u64 user_id() const { return user_id_; }
//
//    inline void SetClientInfo(const String &ip_address, u16 port) {
//        client_address_ = ip_address;
//        client_port_ = port;
//    }
//
//public:
//    // Txn is session level.
//    Txn *txn_{};
//
//private:
//    // Current schema
//    String current_database_;
//
//    // User / Tenant information
//    String tenant_name_;
//    String user_name_;
//
//    u64 user_id_{0};
//
//    String client_address_{};
//    u16 client_port_{};
//};

} // namespace infinity
