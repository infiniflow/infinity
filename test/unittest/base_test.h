//
// Created by JinHai on 2022/7/25.
//

#pragma once

#include <gtest/gtest.h>
#include <type_traits>

template <typename T>
class BaseTestWithParam : public std::conditional_t<std::is_same_v<T, void>, ::testing::Test, ::testing::TestWithParam<T>> {
public:
    ~BaseTestWithParam() override = default;
    void SetUp() override { system("rm -rf /tmp/infinity/"); }

    void TearDown() override {}
};

using BaseTest = BaseTestWithParam<void>;
