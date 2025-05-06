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

#include "gtest/gtest.h"

export module replay_test;

import base_test;
import stl;
import new_txn_manager;
import infinity_context;
import wal_manager;

using namespace infinity;

export class NewReplayTest : public BaseTestParamStr {
protected:
    void RestartTxnMgr() {
        new_txn_mgr->PrintAllKeyValue();
        infinity::InfinityContext::instance().UnInit();

        InfinityContext::instance().InitPhase1(this->config_path);
        InfinityContext::instance().InitPhase2();
        new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();

        new_txn_mgr->PrintAllKeyValue();
    }

    void UninitTxnMgr() {
        new_txn_mgr->PrintAllKeyValue();
        infinity::InfinityContext::instance().UnInit();
        new_txn_mgr = nullptr;
    }

    void InitTxnMgr() {
        InfinityContext::instance().InitPhase1(this->config_path);
        InfinityContext::instance().InitPhase2();
        new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    void SetUp() override {
        BaseTestParamStr::SetUp();
        new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        wal_manager_ = infinity::InfinityContext::instance().storage()->wal_manager();
    }

    WalManager *wal_manager_{};
    NewTxnManager *new_txn_mgr{};
};
