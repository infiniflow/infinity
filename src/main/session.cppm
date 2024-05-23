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

export module session;

import stl;
import txn;
import config;
import options;
import profiler;
import catalog;

namespace infinity {

export class SessionVar {
public:
    i64 query_count_{};
    i64 total_commit_count_{};
    bool enable_profile_{false};
    i64 connected_time_{};
};

export enum class SessionType {
    kLocal,
    kRemote,
};

export class BaseSession {

public:
    BaseSession(u64 session_id, SessionType session_type)
        : connected_time_(std::time(nullptr)), current_database_("default_db"), session_type_(session_type),
        session_id_(session_id) {}

    inline void set_current_schema(const String &current_database) { current_database_ = current_database; }
    [[nodiscard]] inline String &current_database() { return current_database_; }
    SessionVar *SessionVariables() { return &session_variables_; }
    [[nodiscard]] inline u64 session_id() const { return session_id_; }

    [[nodiscard]] inline Txn *GetTxn() const { return txn_; }
    inline void SetTxn(Txn *txn) { txn_ = txn; }

    const QueryProfiler *GetProfileRecord(SizeT index) { return txn_->GetCatalog()->GetProfileRecord(index); }

    void IncreaseQueryCount() { ++query_count_; }

    [[nodiscard]] u64 query_count() const { return query_count_; }

    void IncreaseCommittedTxnCount() { ++committed_txn_count_; }

    u64 committed_txn_count() const { return committed_txn_count_; }

    void IncreaseRollbackedTxnCount() { ++committed_txn_count_; }

    u64 rollbacked_txn_count() const { return rollbacked_txn_count_; }

    String ConnectedTimeToStr() const { return std::asctime(std::localtime(&connected_time_)); }

protected:
    std::time_t connected_time_;

    // Current schema
    String current_database_{};

    // Txn is session level.
    Txn *txn_{};

    SessionType session_type_{SessionType::kRemote};

    SessionVar session_variables_;

    u64 session_id_{0};

    u64 query_count_{0};

    u64 committed_txn_count_{0};
    u64 rollbacked_txn_count_{0};
};

export class LocalSession : public BaseSession {

public:
    explicit LocalSession(u64 session_id) : BaseSession(session_id, SessionType::kLocal) {}
};

export class RemoteSession : public BaseSession {

public:
    explicit RemoteSession(u64 session_id) : BaseSession(session_id, SessionType::kRemote) {}

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

// export class Session {
// public:
//     explicit Session() : current_database_("default_db") {}
//
//     [[nodiscard]] inline String &current_database() { return current_database_; }
//
//     [[nodiscard]] inline const String &user_name() const { return user_name_; }
//
//     [[nodiscard]] inline u64 user_id() const { return user_id_; }
//
//     inline void SetClientInfo(const String &ip_address, u16 port) {
//         client_address_ = ip_address;
//         client_port_ = port;
//     }
//
// public:
//     // Txn is session level.
//     Txn *txn_{};
//
// private:
//     // Current schema
//     String current_database_;
//
//     // User / Tenant information
//     String tenant_name_;
//     String user_name_;
//
//     u64 user_id_{0};
//
//     String client_address_{};
//     u16 client_port_{};
// };

} // namespace infinity
