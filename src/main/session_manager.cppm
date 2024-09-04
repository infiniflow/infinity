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

export module session_manager;

import stl;
import session;
import profiler;
import status;
import global_resource_usage;

namespace infinity {

export struct QueryInfo {
    u64 query_id_;
    String query_kind_;
    String query_text_;
    BaseProfiler profiler_;
};

export class SessionManager {

public:
    SessionManager() = default;

    SharedPtr<RemoteSession> CreateRemoteSession(bool admin_mode) {
        u64 session_id = ++ session_id_generator_;
        SharedPtr<RemoteSession> remote_session = MakeShared<RemoteSession>(session_id, admin_mode);
        {
            std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
            if(admin_mode && sessions_.size() > 0) {
                return nullptr;
            }
            sessions_.emplace(session_id, remote_session.get());
        }
        return remote_session;
    }

    SharedPtr<LocalSession> CreateLocalSession(bool admin_mode) {
        u64 session_id = ++ session_id_generator_;
        SharedPtr<LocalSession> local_session = MakeShared<LocalSession>(session_id, admin_mode);
        {
            std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
            if(admin_mode && sessions_.size() > 0) {
                return nullptr;
            }
            sessions_.emplace(session_id, local_session.get());
        }
        return local_session;
    }

    BaseSession* GetSessionByID(u64 session_id) {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        auto iter = sessions_.find(session_id);
        if(iter == sessions_.end()) {
            return nullptr;
        } else {
            return iter->second;
        }
    }

    void RemoveSessionByID(u64 session_id) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        sessions_.erase(session_id);
    }

    SizeT GetSessionCount() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return sessions_.size();
    }

    void IncreaseQueryCount() { ++total_query_count_; }

    u64 total_query_count() const { return total_query_count_; }

    void AddQueryRecord(u64 session_id, u64 query_id, const String& query_kind, const String& query_text) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        SharedPtr<QueryInfo> query_info = MakeShared<QueryInfo>(query_id, query_kind, query_text, BaseProfiler());
        query_info->profiler_.Begin();
        query_record_container_.emplace(session_id, std::move(query_info));
    }

    void RemoveQueryRecord(u64 session_id) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        query_record_container_.erase(session_id);
    }

    Map<u64, SharedPtr<QueryInfo>> QueryRecords() {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        return query_record_container_;
    }

    QueryInfo* GetQueryRecord(u64 session_id) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        auto iter = query_record_container_.find(session_id);
        if(iter == query_record_container_.end()) {
            return nullptr;
        }
        return iter->second.get();
    }

    void ClearQueryRecord() {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        query_record_container_.clear();
    }

private:
    std::shared_mutex rw_locker_{};
    HashMap<u64, BaseSession*> sessions_;

    // First session is ONE;
    atomic_u64 session_id_generator_{};

    Atomic<u64> total_query_count_{0};

    // session id -> query info
    std::mutex query_record_locker_{};
    Map<u64, SharedPtr<QueryInfo>> query_record_container_;
};

}