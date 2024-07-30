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

export module bg_query_state;

import stl;
import logical_node;
import physical_operator;
import plan_fragment;
import fragment_context;
import query_context;
import infinity_context;
import session_manager;
import session;
import txn;

namespace infinity {

export struct BGQueryState {
    Vector<SharedPtr<LogicalNode>> logical_plans{};
    Vector<UniquePtr<PhysicalOperator>> physical_plans{};
    SharedPtr<PlanFragment> plan_fragment{};
    UniquePtr<Notifier> notifier{};
};

export struct BGQueryContextWrapper {
    UniquePtr<QueryContext> query_context_;

    SessionManager *session_mgr_;
    SharedPtr<BaseSession> session_;

    BGQueryContextWrapper(BGQueryContextWrapper &&other)
        : query_context_(std::move(other.query_context_)), session_mgr_(other.session_mgr_), session_(std::move(other.session_)) {
        other.session_mgr_ = nullptr;
    }

    BGQueryContextWrapper(Txn *txn) : session_mgr_(InfinityContext::instance().session_manager()) {
        session_ = session_mgr_->CreateLocalSession(false);
        query_context_ = MakeUnique<QueryContext>(session_.get());
        query_context_->Init(InfinityContext::instance().config(),
                             InfinityContext::instance().task_scheduler(),
                             InfinityContext::instance().storage(),
                             InfinityContext::instance().resource_manager(),
                             InfinityContext::instance().session_manager());
        query_context_->SetTxn(txn);
    }

    ~BGQueryContextWrapper() {
        if (session_mgr_ != nullptr) {
            auto *session = query_context_->current_session();
            session_mgr_->RemoveSessionByID(session->session_id());
        }
    }
};

} // namespace infinity
