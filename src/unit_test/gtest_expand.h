#ifndef GTEST_EXPAND_H
#define GTEST_EXPAND_H

#include "gtest/gtest.h"

#define EXPECT_THROW_WITHOUT_STACKTRACE(statement, expected_exception)                                                                               \
    SetPrintStacktrace(false);                                                                                                                       \
    EXPECT_THROW(statement, expected_exception);                                                                                                     \
    SetPrintStacktrace(true);

#endif // GTEST_EXPAND_H
