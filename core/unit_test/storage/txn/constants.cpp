//
// Created by jinhai on 23-6-2.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import txn_state;
import infinity;

class TxnContants : public BaseTest {};

TEST_F(TxnContants, test1) {
    using namespace infinity;
    EXPECT_EQ(ToString(TxnState::kStarted), "Started");
}
