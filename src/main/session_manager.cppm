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

export module infinity_core:session_manager;

import :profiler;
import :session;

namespace infinity {

export struct QueryInfo {
    QueryInfo(u64 query_id, std::string query_kind, std::string query_text, BaseProfiler profiler)
        : query_id_(query_id), query_kind_(std::move(query_kind)), query_text_(std::move(query_text)), profiler_(std::move(profiler)) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("QueryInfo");
#endif
    }

    ~QueryInfo() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("QueryInfo");
#endif
    }

    u64 query_id_;
    std::string query_kind_;
    std::string query_text_;
    BaseProfiler profiler_;
};

export class SessionManager {

public:
    SessionManager() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("SessionManager");
#endif
    }

    ~SessionManager() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("SessionManager");
#endif
    }

    std::shared_ptr<RemoteSession> CreateRemoteSession() {
        u64 session_id = ++session_id_generator_;
        auto remote_session = std::make_shared<RemoteSession>(session_id);
        {
            std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
            sessions_.emplace(session_id, remote_session.get());
        }
        return remote_session;
    }

    std::shared_ptr<LocalSession> CreateLocalSession() {
        u64 session_id = ++session_id_generator_;
        auto local_session = std::make_shared<LocalSession>(session_id);
        {
            std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
            sessions_.emplace(session_id, local_session.get());
        }
        return local_session;
    }

    BaseSession *GetSessionByID(u64 session_id) {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        auto iter = sessions_.find(session_id);
        if (iter == sessions_.end()) {
            return nullptr;
        } else {
            return iter->second;
        }
    }

    void RemoveSessionByID(u64 session_id) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        sessions_.erase(session_id);
    }

    size_t GetSessionCount() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return sessions_.size();
    }

    void IncreaseQueryCount() { ++total_query_count_; }

    u64 total_query_count() const { return total_query_count_; }

    void AddQueryRecord(u64 session_id, u64 query_id, const std::string &query_kind, const std::string &query_text) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        auto query_info = std::make_shared<QueryInfo>(query_id, query_kind, query_text, BaseProfiler());
        query_info->profiler_.Begin();
        query_record_container_.emplace(session_id, std::move(query_info));
    }

    void RemoveQueryRecord(u64 session_id) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        query_record_container_.erase(session_id);
    }

    std::map<u64, std::shared_ptr<QueryInfo>> QueryRecords() {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        return query_record_container_;
    }

    QueryInfo *GetQueryRecord(u64 session_id) {
        std::unique_lock<std::mutex> lock(query_record_locker_);
        auto iter = query_record_container_.find(session_id);
        if (iter == query_record_container_.end()) {
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
    std::unordered_map<u64, BaseSession *> sessions_;

    // First session is ONE;
    std::atomic_uint64_t session_id_generator_{};

    std::atomic_uint64_t total_query_count_{0};

    // session id -> query info
    std::mutex query_record_locker_{};
    std::map<u64, std::shared_ptr<QueryInfo>> query_record_container_;
};

} // namespace infinity