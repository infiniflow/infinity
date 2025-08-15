// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
#include <filesystem>
#include <stdlib.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>

#ifdef CI
module base_test;
import infinity_core;
#else
module infinity_core:ut.base_test.impl;

import :ut.base_test;
import :stl;
import :infinity_context;
import :infinity_exception;
#endif

import column_def;
import data_type;
import logical_type;

namespace infinity {

template <typename T>
std::shared_ptr<DataBlock> BaseTestWithParam<T>::MakeInputBlock(const Value &v1, const Value &v2, size_t row_cnt) {
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());

    auto input_block = std::make_shared<DataBlock>();
    {
        auto col1 = ColumnVector::Make(column_def1->type());
        col1->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col1->AppendValue(v1);
        }
        input_block->InsertVector(col1, 0);
    }
    {
        auto col2 = ColumnVector::Make(column_def2->type());
        col2->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col2->AppendValue(v2);
        }
        input_block->InsertVector(col2, 1);
    }
    input_block->Finalize();
    return input_block;
};

template <typename T>
std::shared_ptr<DataBlock> BaseTestWithParam<T>::MakeInputBlock1(size_t row_cnt) {
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block = std::make_shared<DataBlock>();
    {
        auto col1 = ColumnVector::Make(column_def1->type());
        col1->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col1->AppendValue(Value::MakeInt(i));
        }
        input_block->InsertVector(col1, 0);
    }
    {
        auto col2 = ColumnVector::Make(column_def2->type());
        col2->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col2->AppendValue(Value::MakeVarchar(fmt::format("abc_{}", i)));
        }
        input_block->InsertVector(col2, 1);
    }
    input_block->Finalize();
    return input_block;
};

template <typename T>
std::shared_ptr<DataBlock> BaseTestWithParam<T>::MakeInputBlock2(size_t row_cnt) {
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto input_block = std::make_shared<DataBlock>();
    {
        auto col1 = ColumnVector::Make(column_def1->type());
        col1->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col1->AppendValue(Value::MakeInt(2 * i));
        }
        input_block->InsertVector(col1, 0);
    }
    {
        auto col2 = ColumnVector::Make(column_def2->type());
        col2->Initialize();
        for (u32 i = 0; i < row_cnt; ++i) {
            col2->AppendValue(Value::MakeVarchar(fmt::format("abcdefghijklmnopqrstuvwxyz_{}", i)));
        }
        input_block->InsertVector(col2, 1);
    }
    input_block->Finalize();
    return input_block;
};

template <typename T>
void BaseTestWithParam<T>::CheckFilePaths(std::vector<std::string> &delete_file_paths, std::vector<std::string> &exist_file_paths) {
    auto *pm = infinity::InfinityContext::instance().persistence_manager();
    if (pm == nullptr) {
        Path data_dir = this->GetFullDataDir();
        for (auto &file_path : delete_file_paths) {
            file_path = data_dir / file_path;
        }
        for (auto &file_path : exist_file_paths) {
            file_path = data_dir / file_path;
        }
        for (const auto &file_path : delete_file_paths) {
            if (!std::filesystem::path(file_path).is_absolute()) {
                ADD_FAILURE() << "File path is not absolute: " << file_path;
            }
            EXPECT_FALSE(std::filesystem::exists(file_path));

            auto path = static_cast<Path>(file_path).parent_path();
            EXPECT_TRUE(!std::filesystem::exists(path) || std::filesystem::is_directory(path) && !std::filesystem::is_empty(path) ||
                        std::filesystem::is_directory(path) && std::filesystem::is_empty(path) && path == data_dir);
        }
        for (const auto &file_path : exist_file_paths) {
            if (!std::filesystem::path(file_path).is_absolute()) {
                ADD_FAILURE() << "File path is not absolute: " << file_path;
            }
            EXPECT_TRUE(std::filesystem::exists(file_path));
        }
    } else {
        auto local_path_map = pm->GetAllFiles();
        for (const auto &file_path : delete_file_paths) {
            auto persist_read_result = local_path_map.find(file_path);
            EXPECT_TRUE(persist_read_result == local_path_map.end());
        }
        for (const auto &file_path : exist_file_paths) {
            auto persist_read_result = local_path_map.find(file_path);
            EXPECT_FALSE(persist_read_result == local_path_map.end());
        }
    }
    delete_file_paths.clear();
    exist_file_paths.clear();
}

template class BaseTestWithParam<std::string>;
template class BaseTestWithParam<void>;
} // namespace infinity
