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

#include "gtest/gtest.h"

export module request_test;

import base_test;
import stl;
import infinity_context;
import session;
import session_manager;
import query_context;
import query_result;
import data_table;

using namespace infinity;

export class NewRequestTest : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();

        SessionManager *session_manager = InfinityContext::instance().session_manager();
        SharedPtr<RemoteSession> remote_session = session_manager->CreateRemoteSession();
        if (remote_session == nullptr) {
            throw std::runtime_error("remote session is null");
        }

        session_ = std::move(remote_session);

        SharedPtr<RemoteSession> remote_session2 = session_manager->CreateRemoteSession();
        if (remote_session2 == nullptr) {
            throw std::runtime_error("remote session is null");
        }

        session2_ = std::move(remote_session2);
    }

    void TearDown() override {
        SessionManager *session_manager = InfinityContext::instance().session_manager();
        session_manager->RemoveSessionByID(session_->session_id());
        session_manager->RemoveSessionByID(session2_->session_id());

        session_.reset();
        session2_.reset();

        BaseTestParamStr::TearDown();
    }

    UniquePtr<QueryContext> MakeQueryContext() {
        UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
        query_context_ptr->Init(InfinityContext::instance().config(),
                                InfinityContext::instance().task_scheduler(),
                                InfinityContext::instance().storage(),
                                InfinityContext::instance().resource_manager(),
                                InfinityContext::instance().session_manager(),
                                InfinityContext::instance().persistence_manager());
        return query_context_ptr;
    }

    UniquePtr<QueryContext> MakeQueryContext2() {
        UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session2_.get());
        query_context_ptr->Init(InfinityContext::instance().config(),
                                InfinityContext::instance().task_scheduler(),
                                InfinityContext::instance().storage(),
                                InfinityContext::instance().resource_manager(),
                                InfinityContext::instance().session_manager(),
                                InfinityContext::instance().persistence_manager());
        return query_context_ptr;
    }

    bool HandleQueryResult(const QueryResult &result, DataTable **result_table = nullptr) {
        if (result.result_table_.get() == nullptr) {
            // error
            return false;
        }
        // handle result value here
        if (result_table) {
            *result_table = result.result_table_.get();
        }
        return true;
    }

protected:
    SharedPtr<RemoteSession> session_{};
    SharedPtr<RemoteSession> session2_{};
};
