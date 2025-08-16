// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#ifdef CI
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.index_ivf_def;

import :ut.base_test;
import :infinity_exception;
import :index_ivf;
import :infinity_context;
import :defer_op;
import :base_table_ref;
import :meta_info;
import :block_index;
#endif

import internal_types;
import global_resource_usage;
import statement_common;
import data_type;
import logical_type;
import embedding_info;

using namespace infinity;

class IndexIVFTest : public BaseTest {};

const auto base_table_ref = std::make_shared<BaseTableRef>(
    nullptr,
    std::vector<size_t>{0},
    nullptr,
    "",
    0,
    std::make_shared<std::vector<std::string>>(std::vector<std::string>{"col_ivf"}),
    std::make_shared<std::vector<std::shared_ptr<DataType>>>(
        std::vector{std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemBFloat16, 128))}));

struct InitParameterHelper {
    std::vector<InitParameter *> index_param_list;
    ~InitParameterHelper() { clear_ipl(); }
    void clear_ipl() {
        for (auto *ptr : index_param_list) {
            delete ptr;
        }
        index_param_list.clear();
    }
    const std::vector<InitParameter *> &build_ipl(const std::map<std::string, std::string> &args_map) {
        clear_ipl();
        for (const auto &[k, v] : args_map) {
            index_param_list.push_back(new InitParameter(k, v));
        }
        return index_param_list;
    }
};

TEST_F(IndexIVFTest, testplain) {
    using namespace infinity;
    std::map<std::string, std::string> args_map{{"meTric", "cos"}, {"plain_storage_data_type", "f32"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["plain_storage_data_type"] = "bf16";
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["plain_storage_data_type"] = "i8";
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        EXPECT_THROW_WITHOUT_STACKTRACE(ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf"), RecoverableException);
    }
    {
        args_map["unused_param"] = "xxx";
        EXPECT_THROW_WITHOUT_STACKTRACE(IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                                       std::make_shared<std::string>("test comment"),
                                                       "tmp_file_name",
                                                       std::vector<std::string>{"col_ivf"},
                                                       h.build_ipl(args_map)),
                                        RecoverableException);
    }
}

TEST_F(IndexIVFTest, testsq) {
    using namespace infinity;
    std::map<std::string, std::string> args_map{{"metrIc", "L2"},
                                                {"centroids_num_ratio", "0.33"},
                                                {"storage_type", "Scalar_Quantization"},
                                                {"scalar_quantization_bits", "4"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["centroids_num_ratio"] = "-0.33";
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        EXPECT_THROW_WITHOUT_STACKTRACE(ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf"), RecoverableException);
    }
    {
        args_map["centroids_num_ratio"] = "0.33";
        args_map["Centroids_num_ratio"] = "0.33";
        EXPECT_THROW_WITHOUT_STACKTRACE(IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                                       std::make_shared<std::string>("test comment"),
                                                       "tmp_file_name",
                                                       std::vector<std::string>{"col_ivf"},
                                                       h.build_ipl(args_map)),
                                        RecoverableException);
    }
}

TEST_F(IndexIVFTest, testpq) {
    using namespace infinity;
    std::map<std::string, std::string> args_map{{"Metric", "ip"},
                                                {"storage_type", "product_quAntization"},
                                                {"product_quantization_subspace_num", "32"},
                                                {"product_quanTization_subspace_bits", "12"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                        std::make_shared<std::string>("test comment"),
                                        "tmp_file_name",
                                        std::vector<std::string>{"col_ivf"},
                                        h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map.erase("Metric");
        EXPECT_THROW_WITHOUT_STACKTRACE(IndexIVF::Make(std::make_shared<std::string>("tmp_index_name"),
                                                       std::make_shared<std::string>("test comment"),
                                                       "tmp_file_name",
                                                       std::vector<std::string>{"col_ivf"},
                                                       h.build_ipl(args_map)),
                                        RecoverableException);
    }
}
