//
// Created by jinhai on 23-10-15.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;

class AssertTest : public BaseTest {};

TEST_F(AssertTest, all_assert_cases) {
    using namespace infinity;

    EXPECT_ANY_THROW(Assert<ClientException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<ClientException>("Error", __FILE__, __LINE__));
    Assert<ClientException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<CatalogException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<CatalogException>("Error", __FILE__, __LINE__));
    Assert<CatalogException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<NetworkException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<NetworkException>("Error", __FILE__, __LINE__));
    Assert<NetworkException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<ParserException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<ParserException>("Error", __FILE__, __LINE__));
    Assert<ParserException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<PlannerException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<CatalogException>("Error", __FILE__, __LINE__));
    Assert<PlannerException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<OptimizerException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<OptimizerException>("Error", __FILE__, __LINE__));
    Assert<OptimizerException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<ExecutorException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<ExecutorException>("Error", __FILE__, __LINE__));
    Assert<ExecutorException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<SchedulerException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<SchedulerException>("Error", __FILE__, __LINE__));
    Assert<SchedulerException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<StorageException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<StorageException>("Error", __FILE__, __LINE__));
    Assert<StorageException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<TypeException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<TypeException>("Error", __FILE__, __LINE__));
    Assert<TypeException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<FunctionException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<FunctionException>("Error", __FILE__, __LINE__));
    Assert<FunctionException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<NotImplementException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<NotImplementException>("Error", __FILE__, __LINE__));
    Assert<NotImplementException>(true, "Error", __FILE__, __LINE__);

    EXPECT_ANY_THROW(Assert<TransactionException>(false, "Error", __FILE__, __LINE__));
    EXPECT_ANY_THROW(Error<TransactionException>("Error", __FILE__, __LINE__));
    Assert<TransactionException>(true, "Error", __FILE__, __LINE__);
}