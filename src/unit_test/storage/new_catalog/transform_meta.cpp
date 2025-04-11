// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <gtest/gtest.h>

import base_test;
import stl;
import third_party;
import new_catalog;
import compilation_config;
import kv_store;
import config;
import status;

using namespace infinity;

class TransformMeta : public BaseTestWithParam<String> {
public:
    void SetUp() override {
        CleanupDbDirs();
        String config_path_str = GetParam();
        config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = std::make_shared<String>(std::filesystem::absolute(config_path_str));
        }
    }

    void TearDown() override {}

protected:
    std::shared_ptr<String> config_path;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TransformMeta,
                         ::testing::Values(TransformMeta::NEW_CONFIG_PATH, TransformMeta::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TransformMeta, transform_meta00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    std::cout << String("All store key and value: ") << std::endl;
    std::cout << kv_store_ptr->ToString() << std::endl;
    std::cout << String(" -------------- ") << std::endl;

    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();
}

TEST_P(TransformMeta, transform_meta01) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_01.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    std::cout << String("All store key and value: ") << std::endl;
    std::cout << kv_store_ptr->ToString() << std::endl;
    std::cout << String(" -------------- ") << std::endl;

    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();
}

TEST_P(TransformMeta, transform_meta02) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_02.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    std::cout << String("All store key and value: ") << std::endl;
    std::cout << kv_store_ptr->ToString() << std::endl;
    std::cout << String(" -------------- ") << std::endl;

    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();
}
