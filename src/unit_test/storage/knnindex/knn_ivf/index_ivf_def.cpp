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

#include "gtest/gtest.h"
#include <vector>
import base_test;
import infinity_exception;
import stl;
import index_ivf;
import internal_types;
import infinity_context;
import global_resource_usage;
import statement_common;
import defer_op;
import base_table_ref;
import data_type;
import logical_type;
import embedding_info;

using namespace infinity;

class IndexIVFTest : public BaseTest {};

const auto base_table_ref =
    MakeShared<BaseTableRef>(nullptr,
                             Vector<SizeT>{0},
                             nullptr,
                             "",
                             0,
                             MakeShared<Vector<String>>(Vector<String>{"col_ivf"}),
                             MakeShared<Vector<SharedPtr<DataType>>>(std::vector{
                                 MakeShared<DataType>(LogicalType::kEmbedding, MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemBFloat16, 128))}));

struct InitParameterHelper {
    Vector<InitParameter *> index_param_list;
    ~InitParameterHelper() { clear_ipl(); }
    void clear_ipl() {
        for (auto *ptr : index_param_list) {
            delete ptr;
        }
        index_param_list.clear();
    }
    const Vector<InitParameter *> &build_ipl(const Map<String, String> &args_map) {
        clear_ipl();
        for (const auto &[k, v] : args_map) {
            index_param_list.push_back(new InitParameter(k, v));
        }
        return index_param_list;
    }
};

TEST_F(IndexIVFTest, testplain) {
    using namespace infinity;
    Map<String, String> args_map{{"meTric", "cos"}, {"plain_storage_data_type", "f32"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["plain_storage_data_type"] = "bf16";
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["plain_storage_data_type"] = "i8";
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        EXPECT_THROW(ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf"), RecoverableException);
    }
    {
        args_map["unused_param"] = "xxx";
        EXPECT_THROW(IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map)),
                     RecoverableException);
    }
}

TEST_F(IndexIVFTest, testsq) {
    using namespace infinity;
    Map<String, String> args_map{{"metrIc", "L2"},
                                 {"centroids_num_ratio", "0.33"},
                                 {"storage_type", "Scalar_Quantization"},
                                 {"scalar_quantization_bits", "4"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map["centroids_num_ratio"] = "-0.33";
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        EXPECT_THROW(ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf"), RecoverableException);
    }
    {
        args_map["centroids_num_ratio"] = "0.33";
        args_map["Centroids_num_ratio"] = "0.33";
        EXPECT_THROW(IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map)),
                     RecoverableException);
    }
}

TEST_F(IndexIVFTest, testpq) {
    using namespace infinity;
    Map<String, String> args_map{{"Metric", "ip"},
                                 {"storage_type", "product_quAntization"},
                                 {"product_quantization_subspace_num", "32"},
                                 {"product_quanTization_subspace_bits", "12"}};
    InitParameterHelper h;
    {
        auto ivf_index = IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map));
        ivf_index->ValidateColumnDataType(base_table_ref, "col_ivf");
        std::cout << "IndexIVF::BuildOtherParamsString(): " << ivf_index->BuildOtherParamsString() << std::endl;
        std::cout << "IndexIVF::ToString(): " << ivf_index->ToString() << std::endl;
    }
    {
        args_map.erase("Metric");
        EXPECT_THROW(IndexIVF::Make(MakeShared<String>("tmp_index_name"), MakeShared<String>("test comment"), "tmp_file_name", Vector<String>{"col_ivf"}, h.build_ipl(args_map)),
                     RecoverableException);
    }
}
