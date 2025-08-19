#ifndef GTEST_EXPAND_H
#define GTEST_EXPAND_H

import std;
import std.compat;

#define _GLIBCXX_MEMORY 1
#define _GLIBCXX_OSTREAM 1
#define _GLIBCXX_SET 1
#define _GLIBCXX_SSTREAM 1
#define _GLIBCXX_STRING 1
#define _GLIBCXX_VECTOR 1
#define _GLIBCXX_IOSTREAM 1
#define _GLIBCXX_LOCALE 1
#define _GLIBCXX_TUPLE 1
#define _GLIBCXX_CONDITION_VARIABLE 1
#define _GLIBCXX_MUTEX 1
#define _GLIBCXX_OPTIONAL 1
#define _GLIBCXX_STRING_VIEW 1
#define _GLIBCXX_VARIANT 1
#define _GLIBCXX_ATOMIC 1
#define _GLIBCXX_FUNCTIONAL 1
#define _GLIBCXX_UTILITY 1
#define _GLIBCXX_MAP 1
#define _GLIBCXX_SPAN 1
#define _GLIBCXX_ITERATOR 1
#define _GLIBCXX_UNORDERED_MAP 1
#define _GLIBCXX_FILESYSTEM 1

#include "gtest/gtest.h"

#define EXPECT_THROW_WITHOUT_STACKTRACE(statement, expected_exception)                                                                               \
    SetPrintStacktrace(false);                                                                                                                       \
    SetPrintTransactionHistory(false);                                                                                                               \
    EXPECT_THROW(statement, expected_exception);                                                                                                     \
    SetPrintTransactionHistory(true);                                                                                                                \
    SetPrintStacktrace(true);

#define SLOW a

#endif // GTEST_EXPAND_H
