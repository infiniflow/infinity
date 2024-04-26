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
#include <csignal>
#include <cstring>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>
#ifdef ENABLE_JEMALLOC_PROF
#include <jemalloc/jemalloc.h>
#endif

import stl;
import third_party;
import compilation_config;
import local_file_system;
import profiler;
import infinity;

import internal_types;
import logical_type;
import create_index_info;
import column_def;
import data_type;
import query_options;
import extra_ddl_info;
import statement_common;
import parsed_expr;
import constant_expr;
import logger;
import match_expr;
import function_expr;
import search_expr;
import column_expr;

using namespace infinity;

void ReadJsonl(std::ifstream &input_file, SizeT lines_to_read, Vector<Tuple<char *, char *, char *>> &batch) {
    String line;
    SizeT lines_readed = 0;
    batch.clear();
    static const char *columns[3] = {"id", "title", "text"};
    while (lines_readed < lines_to_read) {
        line.clear();
        std::getline(input_file, line);
        if (input_file.eof())
            break;
        else if (line.length() == 0)
            continue;
        else {
            std::string_view json_sv(line);
            nlohmann::json json = nlohmann::json::parse(json_sv);
            char *elems[3];
            for (SizeT i = 0; i < 3; i++) {
                assert(json.contains(columns[i]));
                String val_str = json[columns[i]];
                char *val_buf = (char *)malloc(val_str.length() + 1);
                memcpy(val_buf, val_str.data(), val_str.length());
                val_buf[val_str.length()] = '\0';
                elems[i] = val_buf;
            }
            batch.push_back({elems[0], elems[1], elems[2]});
            lines_readed++;
        }
    }
}

SharedPtr<Infinity> CreateDbAndTable(const String &db_name, const String &table_name) {
    Vector<ColumnDef *> column_defs;
    {
        String col1_name = "id";
        auto col1_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col1_def = new ColumnDef(0, col1_type, std::move(col1_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col1_def);
    }
    {
        String col2_name = "title";
        auto col2_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col2_def = new ColumnDef(0, col2_type, std::move(col2_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col2_def);
    }
    {
        String col3_name = "text";
        auto col3_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col3_def = new ColumnDef(0, col3_type, std::move(col3_name), std::unordered_set<ConstraintType>());
        column_defs.push_back(col3_def);
    }

    String data_path = "/var/infinity";

    Infinity::LocalInit(data_path);
    // SetLogLevel(LogLevel::kTrace);

    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    CreateDatabaseOptions create_db_options;
    create_db_options.conflict_type_ = ConflictType::kIgnore;
    infinity->CreateDatabase(db_name, std::move(create_db_options));

    DropTableOptions drop_tb_options;
    drop_tb_options.conflict_type_ = ConflictType::kIgnore;
    infinity->DropTable(db_name, table_name, std::move(drop_tb_options));

    CreateTableOptions create_tb_options;
    create_tb_options.conflict_type_ = ConflictType::kIgnore;
    infinity->CreateTable(db_name, table_name, std::move(column_defs), Vector<TableConstraint *>{}, std::move(create_tb_options));
    return infinity;
}

void BenchmarkImport(SharedPtr<Infinity> infinity,
                     const String &db_name,
                     const String &table_name,
                     const String &import_from) {
    LocalFileSystem fs;
    if (!fs.Exists(import_from)) {
        LOG_ERROR(fmt::format("Data file doesn't exist: {}", import_from));
        return;
    }

    BaseProfiler profiler;

    profiler.Begin();
    ImportOptions import_options;
    import_options.copy_file_type_ = CopyFileType::kJSONL;
    infinity->Import(db_name, table_name, import_from, std::move(import_options));
    LOG_INFO(fmt::format("Import data cost: {}", profiler.ElapsedToString()));
    profiler.End();
}

void BenchmarkInsert(SharedPtr<Infinity> infinity, const String &db_name, const String &table_name, const String &insert_from, SizeT insert_batch) {
    std::ifstream input_file(insert_from);
    if (!input_file.is_open()) {
        LOG_ERROR(fmt::format("Failed to open file {}", insert_from));
        return;
    }

    BaseProfiler profiler;

    profiler.Begin();
    Vector<Tuple<char *, char *, char *>> batch_cache;
    ReadJsonl(input_file, (SizeT)(-1), batch_cache);
    SizeT num_rows = batch_cache.size();
    LOG_INFO(fmt::format("ReadJsonl {} rows cost: {}", num_rows, profiler.ElapsedToString()));
    profiler.End();

    profiler.Begin();
    Vector<String> orig_columns{"id", "title", "text"};
    ConstantExpr *const_expr = nullptr;
    SizeT num_inserted = 0;
    while (num_inserted < num_rows) {
        Vector<String> *columns = new Vector<String>(orig_columns);
        Vector<Vector<ParsedExpr *> *> *values = new Vector<Vector<ParsedExpr *> *>();
        values->reserve(insert_batch);
        for (SizeT i = 0; i<insert_batch && (num_inserted+i)<num_rows; i++){
            auto &t = batch_cache[num_inserted+i];
            auto value_list = new Vector<ParsedExpr *>(columns->size());
            const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = std::get<0>(t);
            value_list->at(0) = const_expr;
            const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = std::get<1>(t);
            value_list->at(1) = const_expr;
            const_expr = new ConstantExpr(LiteralType::kString);
            const_expr->str_value_ = std::get<2>(t);
            value_list->at(2) = const_expr;
            values->push_back(value_list);
        }
        infinity->Insert(db_name, table_name, columns, values);
        // NOTE: ~InsertStatement() has deleted or freed columns, values, value_list, const_expr, const_expr->str_value_
        num_inserted += insert_batch;
    }
    input_file.close();
    LOG_INFO(fmt::format("Insert data {} rows cost: {}", num_rows, profiler.ElapsedToString()));
    profiler.End();
}

void BenchmarkCreateIndex(SharedPtr<Infinity> infinity,
                          const String &db_name,
                          const String &table_name,
                          const String &index_name) {
    BaseProfiler profiler;
    profiler.Begin();
    auto index_info_list = new Vector<IndexInfo *>();
    auto index_info = new IndexInfo();
    index_info->index_type_ = IndexType::kFullText;
    index_info->column_name_ = "text";
    index_info->index_param_list_ = new Vector<InitParameter *>();
    index_info_list->push_back(index_info);

    auto r = infinity->CreateIndex(db_name, table_name, index_name, index_info_list, CreateIndexOptions());
    if (r.IsOk()) {
        r = infinity->Flush();
    } else {
        LOG_ERROR(fmt::format("Fail to create index {}", r.ToString()));
        return;
    }

    // NOTE: ~CreateStatement() has already deleated or freed index_info_list, index_info, index_info->index_param_list_.
    LOG_INFO(fmt::format("Create index cost: {}", profiler.ElapsedToString()));
    profiler.End();
}

void BenchmarkOptimize(SharedPtr<Infinity> infinity, const String &db_name, const String &table_name) {
    BaseProfiler profiler;
    profiler.Begin();
    infinity->Optimize(db_name, table_name);
    LOG_INFO(fmt::format("Merge index cost: {}", profiler.ElapsedToString()));
    profiler.End();
}

void BenchmarkQuery(SharedPtr<Infinity> infinity, const String &db_name, const String &table_name) {
    std::string fields = "text";
    //std::vector<std::string> query_vec = {"one of", "is", "a", "\"is a\"", "\"one of\""};// {"Animalia", "Algorithms", "Animalia Algorithms", "network space", "harmful chemical anarchism"};
    std::vector<std::string> query_vec = {"harmful chemical anarchism", "\"harmful chemical\"", "\"one of\"", "harmful chemical"};
    for (auto match_text : query_vec) {
        BaseProfiler profiler;
        profiler.Begin();
        auto *search_expr = new SearchExpr();
        {
            auto exprs = new std::vector<ParsedExpr *>();

            auto *match_expr = new MatchExpr();
            match_expr->fields_ = fields;
            match_expr->matching_text_ = match_text;
            exprs->push_back(match_expr);
            search_expr->SetExprs(exprs);
        }

        std::vector<ParsedExpr *> *output_columns = new std::vector<ParsedExpr *>();
        {
            auto select_rowid_expr = new FunctionExpr();
            select_rowid_expr->func_name_ = "row_id";
            auto select_score_expr = new FunctionExpr();
            select_score_expr->func_name_ = "score";
            output_columns->emplace_back(select_rowid_expr);
            output_columns->emplace_back(select_score_expr);
        }
        infinity->Search(db_name, table_name, search_expr, nullptr, output_columns);
        /*
        auto result = infinity->Search(db_name, table_name, search_expr, nullptr, output_columns);
        {
            auto &cv = result.result_table_->GetDataBlockById(0)->column_vectors;
            auto &column = *cv[0];
            auto result_id = reinterpret_cast<const RowID *>(column.data());
            auto cnt = column.Size();
            for (size_t i = 0; i < cnt; ++i) {
                LOG_INFO(fmt::format("result_id[{}] = {}", i, result_id[i].ToUint64()));
            }
        }
        */
        LOG_INFO(fmt::format("Query text: {}, time cost: {}", match_text, profiler.ElapsedToString()));
        profiler.End();
    }
}

void SignalHandler(int signal_number, siginfo_t *, void *) {
    switch (signal_number) {
#ifdef ENABLE_JEMALLOC_PROF
        case SIGUSR2: {
            // http://jemalloc.net/jemalloc.3.html
            int rc = mallctl("prof.dump", NULL, NULL, NULL, 0);
            printf("Dump memory profile %d\n", rc);
            break;
        }
#endif
        default: {
            // Ignore
            printf("Other type of signal: %d\n", signal_number);
        }
    }
}

void RegisterSignal() {
    struct sigaction sig_action;
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = SignalHandler;
    sigemptyset(&sig_action.sa_mask);
#ifdef ENABLE_JEMALLOC_PROF
    sigaction(SIGUSR2, &sig_action, NULL);
#endif
}
void BenchmarkMoreQuery(SharedPtr<Infinity> infinity, const String &db_name, const String &table_name, int query_times = 10) {
    BaseProfiler profiler;
    profiler.Begin();
    for (int i = 0; i < query_times; i++) {
        BenchmarkQuery(infinity, db_name, table_name);
    }
    LOG_INFO(fmt::format("run benchmark query {} times cost: {}", query_times, profiler.ElapsedToString()));
    profiler.End();
}

int main(int argc, char *argn[]) {
    argc = 3;
    const char* argv[] = {"fulltext", "--mode", "query"};
    CLI::App app{"fulltext_benchmark"};
    // https://github.com/CLIUtils/CLI11/blob/main/examples/enum.cpp
    // Using enumerations in an option
    enum class Mode : u8 { kInsert, kImport, kMerge, kQuery };
    Map<String, Mode> mode_map{
        {"insert", Mode::kInsert},
        {"import", Mode::kImport},
        {"merge", Mode::kMerge},
        {"query", Mode::kQuery}
    };
    Mode mode(Mode::kInsert);
    SizeT insert_batch = 500;
    app.add_option("--mode", mode, "Bencmark mode, one of insert, import, merge, query")
        ->required()
        ->transform(CLI::CheckedTransformer(mode_map, CLI::ignore_case));
    app.add_option("--insert-batch", insert_batch, "batch size of each insert, valid only at insert and merge mode, default value 500");
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    RegisterSignal();

    String db_name = "default_db";
    String table_name = "ft_dbpedia_benchmark";
    String index_name = "ft_dbpedia_index";
    String srcfile = test_data_path();
    srcfile += "/benchmark/dbpedia-entity/corpus10.jsonl";

// #define DEL_LOCAL_DATA
#ifdef DEL_LOCAL_DATA
    system("rm -rf /var/infinity/data /var/infinity/wal /var/infinity/log /var/infinity/tmp");
#endif

    SharedPtr<Infinity> infinity = CreateDbAndTable(db_name, table_name);

    switch (mode) {
        case Mode::kInsert: {
            BenchmarkCreateIndex(infinity, db_name, table_name, index_name);
            BenchmarkInsert(infinity, db_name, table_name, srcfile, insert_batch);
            break;
        }
        case Mode::kImport: {
            BenchmarkCreateIndex(infinity, db_name, table_name, index_name);
            BenchmarkImport(infinity, db_name, table_name, srcfile);
            break;
        }
        case Mode::kMerge: {
            BenchmarkCreateIndex(infinity, db_name, table_name, index_name);
            BenchmarkInsert(infinity, db_name, table_name, srcfile, insert_batch);
            BenchmarkOptimize(infinity, db_name, table_name);
            break;
        }
        case Mode::kQuery: {
            BenchmarkCreateIndex(infinity, db_name, table_name, index_name);
            BenchmarkInsert(infinity, db_name, table_name, srcfile, insert_batch);
            // BenchmarkOptimize(infinity, db_name, table_name);
            sleep(10);
            BenchmarkMoreQuery(infinity, db_name, table_name, 1);
            break;
        }
        default: {
            printf("Unsupported benchmark mode: %u\n", static_cast<u8>(mode));
        }
    }
    sleep(10);

    Infinity::LocalUnInit();
}