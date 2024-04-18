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

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

import compilation_config;
import local_file_system;
import file_system;
import file_system_type;

import third_party;
import profiler;
import infinity;
import search_expr;
import parsed_expr;
import match_expr;
import function_expr;

using namespace infinity;

bool ReadJsonl(const std::string &query_str, size_t &start_pos, size_t query_size, std::pair<std::string, std::string> &result) {
    size_t end_pos = query_str.find('\n', start_pos);
    if (end_pos == std::string::npos) {
        end_pos = query_size;
    }
    if (end_pos == start_pos) {
        return false;
    }
    std::string_view json_sv(query_str.data() + start_pos, end_pos - start_pos);
    start_pos = end_pos + 1;

    nlohmann::json json = nlohmann::json::parse(json_sv);
    result = {json["_id"], json["text"]};
    return true;
}

bool ReadCSVCol3(const std::string &groundtruth_str, size_t &start_pos, size_t groundtruth_size, std::pair<std::string, std::string> &result) {
    size_t end_pos = groundtruth_str.find('\n', start_pos);
    if (end_pos == std::string::npos) {
        end_pos = groundtruth_size;
    }
    if (end_pos == start_pos) {
        return false;
    }
    std::string_view csv_sv(groundtruth_str.data() + start_pos, end_pos - start_pos);
    start_pos = end_pos + 1;

    size_t pos1 = csv_sv.find('\t');
    std::string query_id(csv_sv.data(), pos1);
    size_t pos2 = csv_sv.find('\t', pos1 + 1);
    std::string corpus_id(csv_sv.data() + pos1 + 1, pos2 - pos1 - 1);
    result = {std::move(query_id), std::move(corpus_id)};
    return true;
}

int main(int argn, char *argv[]) {
    if (argn != 2) {
        std::cout << "Usage: " << argv[0] << " test_number" << std::endl;
        return 1;
    }
    size_t test_n = std::stoull(argv[1]);

    std::string query_path = test_data_path();
    query_path += "/benchmark/dbpedia-entity/queries.jsonl";
    std::string groundtruth_path = test_data_path();
    groundtruth_path += "/benchmark/dbpedia-entity/qrels/test.tsv";

    LocalFileSystem fs;
    if (!fs.Exists(query_path) || !fs.Exists(groundtruth_path)) {
        std::cout << "Data file: " << query_path << " or " << groundtruth_path << " is not existed." << std::endl;
        return 1;
    }

    std::string db_name = "default";
    std::string table_name = "ft_dbpedia_benchmark";
    std::string index_name = "ft_dbpedia_index";

    std::string infinity_path = "/var/infinity";
    Infinity::LocalInit(infinity_path);

    std::unique_ptr<FileHandler> query_file = fs.OpenFile(query_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    std::unique_ptr<FileHandler> groundtruth_file = fs.OpenFile(groundtruth_path, FileFlags::READ_FLAG, FileLockType::kReadLock);

    std::unordered_multimap<std::string, std::string> groundtruth_map;
    {
        size_t groundtruth_size = fs.GetFileSize(*groundtruth_file);
        std::string groundtruth_str(groundtruth_size + 1, 0);
        fs.Read(*groundtruth_file, groundtruth_str.data(), groundtruth_size);
        size_t start_pos1 = 0;

        std::pair<std::string, std::string> groundtruth;
        while (ReadCSVCol3(groundtruth_str, start_pos1, groundtruth_size, groundtruth)) {
            auto &[query_id, corpus_id] = groundtruth;
            groundtruth_map.emplace(std::move(query_id), std::move(corpus_id));
        }
    }

    std::vector<std::pair<std::string, std::string>> query_vec;
    {
        size_t query_size = fs.GetFileSize(*query_file);
        std::string query_str(query_size + 1, 0);
        fs.Read(*query_file, query_str.data(), query_size);

        size_t start_pos = 0;
        std::pair<std::string, std::string> query;
        while (ReadJsonl(query_str, start_pos, query_size, query)) {
            auto &[id, text] = query;
            query_vec.emplace_back(std::move(id), std::move(text));
        }
    }

    std::string fields = "text";
    BaseProfiler profiler;
    for (size_t i = 0; i < test_n; ++i) {
        profiler.Begin();
        std::cout << "Test " << i << std::endl;

        std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
//        auto [ data_base, status1 ] = infinity->GetDatabase(db_name);
//        auto [ table, status2 ] = data_base->GetTable(table_name);

        size_t correct_n = 0;
        size_t total_n = 0;

        for (auto &[query_id, match_txt] : query_vec) {
            auto *search_expr = new SearchExpr();
            {
                auto exprs = new std::vector<ParsedExpr *>();

                auto *match_expr = new MatchExpr();
                match_expr->fields_ = fields;
                match_expr->matching_text_ = match_txt;

                exprs->push_back(match_expr);

                search_expr->SetExprs(exprs);
            }
            auto output_columns = new std::vector<ParsedExpr *>();
            {
                auto select_rowid_expr = new FunctionExpr();
                select_rowid_expr->func_name_ = "_id";
                output_columns->emplace_back(select_rowid_expr);
            }

            auto result = infinity->Search(db_name, table_name, search_expr, nullptr, output_columns);
            {
                auto &cv = result.result_table_->GetDataBlockById(0)->column_vectors;
                auto &column = *cv[0];
                auto result_id = reinterpret_cast<const std::string *>(column.data());

                total_n += column.Size();

                auto range = groundtruth_map.equal_range(query_id);

                auto it = range.first;
                for (size_t i = 0; i < column.Size() && it != range.second; ++i, ++it) {
                    if (result_id[i] == it->second) {
                        ++correct_n;
                    }
                }
            }
        }

        std::cout << "Total Time: " << profiler.ElapsedToString() << std::endl;
        std::cout << "correctness: " << float(correct_n) / total_n << std::endl;
    }
}