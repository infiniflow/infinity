//
// Created by jinhai on 23-6-2.
//

#include "base_test.h"
#include "main/infinity.h"

class TxnContants : public BaseTest {};

TEST_F(TxnContants, test1) {
    using namespace infinity;
    EXPECT_EQ(ToString(TxnState::kStarted), "Started");
}