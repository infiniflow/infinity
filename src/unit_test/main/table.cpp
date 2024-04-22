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

#include "unit_test/base_test.h"

import stl;
import infinity;
import query_result;
import data_block;
import value;
import query_options;

import status;
import third_party;
import logical_type;
import internal_types;
import embedding_info;
import parsed_expr;
import column_expr;
import extra_ddl_info;
import knn_expr;
import column_def;
import explain_statement;
import data_type;

class InfinityTableTest : public BaseTest {};

TEST_F(InfinityTableTest, test1) {
    using namespace infinity;

    String path = GetHomeDir();
    RemoveDbDirs();
    Infinity::LocalInit(path);

    SharedPtr<Infinity> infinity = Infinity::LocalConnect();
    {
        CreateDatabaseOptions create_db_opts;
        infinity->CreateDatabase("db1", create_db_opts);

        QueryResult result = infinity->GetDatabase("db1");
        EXPECT_TRUE(result.IsOk());
    }

    {
        std::vector<ColumnDef *> column_defs;

        // init column defs
        std::shared_ptr<DataType> col1_type =
            std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128));
        String col1_name = "col1";
        auto col1_def = std::make_unique<ColumnDef>(0, col1_type, col1_name, HashSet<ConstraintType>());
        column_defs.emplace_back(col1_def.release());

        std::shared_ptr<DataType> col2_type = std::make_shared<DataType>(LogicalType::kBigInt);
        String col2_name = "col2";
        auto col2_def = std::make_unique<ColumnDef>(0, col2_type, col2_name, HashSet<ConstraintType>());
        column_defs.emplace_back(col2_def.release());

        std::string db_name = "db1";
        std::string table_name = "knn_table";

        CreateTableOptions create_tb_options;
        create_tb_options.conflict_type_ = ConflictType::kIgnore;

        QueryResult create_result =
            infinity->CreateTable(db_name, table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));
        EXPECT_TRUE(create_result.IsOk());

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_ast = infinity->Explain(db_name, table_name, ExplainType::kAst, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_ast.IsOk());
            fmt::print("AST: {}\n", explain_ast.ToString());
        }

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_unopt = infinity->Explain(db_name, table_name, ExplainType::kUnOpt, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_unopt.IsOk());
            fmt::print("Unoptimized logical plan: {}\n", explain_unopt.ToString());
        }

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_opt = infinity->Explain(db_name, table_name, ExplainType::kOpt, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_opt.IsOk());
            fmt::print("Optimized logical plan: {}\n", explain_opt.ToString());
        }

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_phy = infinity->Explain(db_name, table_name, ExplainType::kPhysical, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_phy.IsOk());
            fmt::print("Physical plan: {}\n", explain_phy.ToString());
        }

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_fragment = infinity->Explain(db_name, table_name, ExplainType::kFragment, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_fragment.IsOk());
            fmt::print("Fragment: {}\n", explain_fragment.ToString());
        }

        {
            Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
            ColumnExpr *col2 = new ColumnExpr();
            col2->names_.emplace_back(col2_name);
            output_columns->emplace_back(col2);

            QueryResult explain_pipeline = infinity->Explain(db_name, table_name, ExplainType::kPipeline, nullptr, nullptr, output_columns);
            EXPECT_TRUE(explain_pipeline.IsOk());
            fmt::print("Pipeline: {}\n", explain_pipeline.ToString());
        }
    }

    infinity->LocalDisconnect();
    Infinity::LocalUnInit();
}
