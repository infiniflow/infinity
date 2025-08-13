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

#include <cassert>

import std;
import infinity_core;
import compilation_config;
import internal_types;
import logical_type;
import embedding_info;
import create_index_info;
import extra_ddl_info;
import knn_expr;
import column_def;
import statement_common;
import data_type;

using namespace infinity;

enum class BuildType : i8 {
    PLAIN,
    LSG,
};

enum class EncodeType : i8 {
    PLAIN,
    LVQ,
    CompressToLVQ,
};

struct BenchmarkArgs {
public:
    const static Map<String, BuildType> build_type_map;
    const static Map<String, EncodeType> encode_type_map;

public:
    BenchmarkArgs() : app_("knn_import_benchmark") {
        app_.add_option("--dataset", dataset_, "dataset")->required();
        app_.add_option("--build_type", build_type_, "build type")
            ->required(false)
            ->transform(CLI::CheckedTransformer(build_type_map, CLI::ignore_case));
        app_.add_option("--encode_type", encode_type_, "encode type")
            ->required(false)
            ->transform(CLI::CheckedTransformer(encode_type_map, CLI::ignore_case));
        app_.add_option("--ef_construction", ef_construction_, "ef construction")->required(false);
        app_.add_option("--M", M_, "M")->required(false);
        app_.add_option("--dataset_dir", dataset_dir_, "dataset dir")->required();
        app_.add_option("--infinity_dir", infinity_dir_, "infinity dir")->required(false);
        app_.add_option("--config_path", config_path_, "config path")->required(false);
    }

    void Parse(int argc, char *argv[]) {
        try {
            app_.parse(argc, argv);
        } catch (const CLI::ParseError &e) {
            UnrecoverableError(e.what());
        }

        if (dataset_ == "invalid") {
            UnrecoverableError("Invalid dataset");
        }
        if (dataset_ == "sift") {
            data_path_ = dataset_dir_ + "/benchmark/sift_1m/sift_base.fvecs";
            table_name_ = "sift_benchmark";
        } else {
            data_path_ = dataset_dir_ + "/benchmark/gist_1m/gist_base.fvecs";
            table_name_ = "gist_benchmark";
        }
    }

public:
    String dataset_ = "invalid";
    BuildType build_type_ = BuildType::PLAIN;
    EncodeType encode_type_ = EncodeType::LVQ;
    SizeT ef_construction_ = 200;
    SizeT M_ = 16;

    String dataset_dir_ = String(test_data_path());
    String infinity_dir_ = "/var/infinity";
    String config_path_;

    String data_path_;
    String table_name_;

private:
    CLI::App app_;
};

const Map<String, BuildType> BenchmarkArgs::build_type_map = {
    {"plain", BuildType::PLAIN},
    {"lsg", BuildType::LSG},
};
const Map<String, EncodeType> BenchmarkArgs::encode_type_map = {
    {"plain", EncodeType::PLAIN},
    {"lvq", EncodeType::LVQ},
    {"compress_to_lvq", EncodeType::CompressToLVQ},
};

int main(int argc, char *argv[]) {
    BenchmarkArgs args;
    args.Parse(argc, argv);

    Infinity::LocalInit(args.infinity_dir_, args.config_path_);

    std::cout << ">>> Import Benchmark Start <<<" << std::endl;

    std::vector<std::string> results;

    // hnsw benchmark
    do {
        std::vector<ColumnDef *> column_defs;

        // init column defs
        std::shared_ptr<DataType> col1_type = nullptr;
        std::string base_path = args.data_path_;
        std::string table_name = args.table_name_;
        if (args.dataset_ == "sift") {
            col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128));
        } else if (args.dataset_ == "gist") {
            col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 960));
        } else {
            UnrecoverableError("Invalid dataset");
        }
        std::cout << "Import from: " << base_path << std::endl;

        std::string col1_name = "col1";
        auto col1_def = std::make_unique<ColumnDef>(0, col1_type, col1_name, std::set<ConstraintType>());
        column_defs.emplace_back(col1_def.release());

        std::string db_name = "default_db";
        std::string index_name = "hnsw_index";

        std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
        CreateDatabaseOptions create_db_options;
        create_db_options.conflict_type_ = ConflictType::kIgnore;
        auto r1 = infinity->CreateDatabase(db_name, std::move(create_db_options), "");

        //        auto [ data_base, status1 ] = infinity->GetDatabase(db_name);
        CreateTableOptions create_tb_options;
        create_tb_options.conflict_type_ = ConflictType::kIgnore;
        auto r2 = infinity->CreateTable(db_name, table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));

        //        auto [ table, status2 ] = data_base->GetTable(table_name);

        if (!VirtualStore::Exists(base_path)) {
            std::cout << "File: " << base_path << " doesn't exist" << std::endl;
            break;
        }

        ImportOptions import_options;
        import_options.copy_file_type_ = CopyFileType::kFVECS;

        infinity::BaseProfiler profiler;
        profiler.Begin();
        QueryResult query_result = infinity->Import(db_name, table_name, base_path, import_options);
        std::cout << "Import data cost: " << profiler.ElapsedToString() << std::endl;

        auto index_info = new IndexInfo();
        index_info->index_type_ = IndexType::kHnsw;
        index_info->column_name_ = col1_name;

        {
            auto index_param_list = new std::vector<InitParameter *>();
            index_param_list->emplace_back(new InitParameter("m", std::to_string(args.M_)));
            index_param_list->emplace_back(new InitParameter("ef_construction", std::to_string(args.ef_construction_)));
            index_param_list->emplace_back(new InitParameter("metric", "l2"));
            if (args.encode_type_ == EncodeType::LVQ) {
                index_param_list->emplace_back(new InitParameter("encode", "lvq"));
            } else if (args.encode_type_ == EncodeType::PLAIN || args.encode_type_ == EncodeType::CompressToLVQ) {
                index_param_list->emplace_back(new InitParameter("encode", "plain"));
            } else {
                UnrecoverableError("Invalid encode type");
            }
            if (args.build_type_ == BuildType::LSG) {
                index_param_list->emplace_back(new InitParameter("build_type", "lsg"));
            }
            index_info->index_param_list_ = index_param_list;
        }

        String index_comment = "";
        query_result = infinity->CreateIndex(db_name, table_name, index_name, index_comment, index_info, CreateIndexOptions());

        if (!query_result.IsOk()) {
            std::cout << "Fail to create index." << profiler.ElapsedToString() << std::endl;
            profiler.End();
            break;
        }

        if (args.encode_type_ == EncodeType::CompressToLVQ) {
            OptimizeOptions options;
            options.index_name_ = index_name;
            options.opt_params_.emplace_back(new InitParameter("compress_to_lvq"));
            query_result = infinity->Optimize(db_name, table_name, options);
            if (!query_result.IsOk()) {
                std::cout << "Fail to optimize index." << profiler.ElapsedToString() << std::endl;
                profiler.End();
                break;
            }
        }

        std::cout << "Create Index cost: " << profiler.ElapsedToString() << std::endl;
        query_result = infinity->Flush();
        profiler.End();
        std::cout << "Flush data cost: " << profiler.ElapsedToString() << std::endl;
    } while (false);

    std::cout << ">>> Knn Import Benchmark End <<<" << std::endl;
    for (const auto &item : results) {
        std::cout << item << std::endl;
    }
    Infinity::LocalUnInit();
}
