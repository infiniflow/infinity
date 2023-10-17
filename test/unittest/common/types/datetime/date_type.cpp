//
// Created by jinhai on 23-3-10.
//

#include "base_test.h"
#include "main/infinity.h"

class DateTypeTest : public BaseTest {};

TEST_F(DateTypeTest, test1) {
    using namespace infinity;

    DateType date1;

    EXPECT_EQ(date1.ToString(), "1970-01-01");

    date1.FromString("2000-01-01");
    EXPECT_EQ(date1.ToString(), "2000-01-01");

    date1.FromString("2000/10/31");
    EXPECT_EQ(date1.ToString(), "2000-10-31");

    date1.FromString("1-1-1");
    EXPECT_EQ(date1.ToString(), "1-01-01");

    date1.FromString("9999-12-31");
    EXPECT_EQ(date1.ToString(), "9999-12-31");

    EXPECT_THROW(date1.FromString("0-0-0"), ExecutorException);
    EXPECT_THROW(date1.FromString("0/0/0"), ExecutorException);
}