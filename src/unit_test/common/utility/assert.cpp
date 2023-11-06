//
// Created by jinhai on 23-10-15.
//

#include "unit_test/base_test.h"

import infinity_exception;

import stl;

class AssertTest : public BaseTest {};

TEST_F(AssertTest, all_assert_cases) {
    using namespace infinity;

    EXPECT_ANY_THROW(Assert<ClientException>(false, "Error"));
    EXPECT_ANY_THROW(Error<ClientException>("Error"));
    Assert<ClientException>(true, "Error");

    EXPECT_ANY_THROW(Assert<CatalogException>(false, "Error"));
    EXPECT_ANY_THROW(Error<CatalogException>("Error"));
    Assert<CatalogException>(true, "Error");

    EXPECT_ANY_THROW(Assert<NetworkException>(false, "Error"));
    EXPECT_ANY_THROW(Error<NetworkException>("Error"));
    Assert<NetworkException>(true, "Error");

    EXPECT_ANY_THROW(Assert<PlannerException>(false, "Error"));
    EXPECT_ANY_THROW(Error<CatalogException>("Error"));
    Assert<PlannerException>(true, "Error");

    EXPECT_ANY_THROW(Assert<OptimizerException>(false, "Error"));
    EXPECT_ANY_THROW(Error<OptimizerException>("Error"));
    Assert<OptimizerException>(true, "Error");

    EXPECT_ANY_THROW(Assert<ExecutorException>(false, "Error"));
    EXPECT_ANY_THROW(Error<ExecutorException>("Error"));
    Assert<ExecutorException>(true, "Error");

    EXPECT_ANY_THROW(Assert<SchedulerException>(false, "Error"));
    EXPECT_ANY_THROW(Error<SchedulerException>("Error"));
    Assert<SchedulerException>(true, "Error");

    EXPECT_ANY_THROW(Assert<StorageException>(false, "Error"));
    EXPECT_ANY_THROW(Error<StorageException>("Error"));
    Assert<StorageException>(true, "Error");

    EXPECT_ANY_THROW(Assert<TypeException>(false, "Error"));
    EXPECT_ANY_THROW(Error<TypeException>("Error"));
    Assert<TypeException>(true, "Error");

    EXPECT_ANY_THROW(Assert<FunctionException>(false, "Error"));
    EXPECT_ANY_THROW(Error<FunctionException>("Error"));
    Assert<FunctionException>(true, "Error");

    EXPECT_ANY_THROW(Assert<NotImplementException>(false, "Error"));
    EXPECT_ANY_THROW(Error<NotImplementException>("Error"));
    Assert<NotImplementException>(true, "Error");

    EXPECT_ANY_THROW(Assert<TransactionException>(false, "Error"));
    EXPECT_ANY_THROW(Error<TransactionException>("Error"));
    Assert<TransactionException>(true, "Error");
}