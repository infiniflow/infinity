# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from dataclasses import dataclass, field
from FlagEmbedding import BGEM3FlagModel
from tqdm import tqdm
import infinity
from infinity.common import LOCAL_HOST
from mldr_common_tools import QueryArgs, check_languages, check_query_types
from mldr_common_tools import FakeJScoredDoc, get_queries_and_qids, save_result
from transformers import HfArgumentParser


@dataclass
class ModelArgs:
    fp16: bool = field(default=True, metadata={'help': 'Use fp16 in inference?'})


def get_model(model_args: ModelArgs):
    return BGEM3FlagModel("BAAI/bge-m3", use_fp16=model_args.fp16)


text_to_replace = "&|!+-():\'\"?~^"
text_to_replace_with = " " * len(text_to_replace)
query_translation_table = str.maketrans(text_to_replace, text_to_replace_with)


class InfinityClientForSearch:
    def __init__(self):
        self.test_db_name = "default_db"
        self.test_table_name_prefix = "mldr_test_table_text_dense_sparse_"
        self.test_table_schema = {"docid_col": {"type": "varchar"}, "fulltext_col": {"type": "varchar"},
                                  "dense_col": {"type": "vector,1024,float"},
                                  "sparse_col": {"type": "sparse,250002,float,int"}}
        self.infinity_obj = infinity.connect(LOCAL_HOST)
        self.infinity_db = self.infinity_obj.get_database(self.test_db_name)
        self.infinity_table = None
        self.query_funcs = {'bm25': self.bm25_query, 'dense': self.dense_query, 'sparse': self.sparse_query}

    def get_test_table(self, language_suffix: str):
        table_name = self.test_table_name_prefix + language_suffix
        self.infinity_table = self.infinity_db.get_table(table_name)
        print(f"Get table {table_name} successfully.")

    def bm25_query(self, query: str, model: BGEM3FlagModel, max_hits: int):
        query_str = query.translate(query_translation_table)
        result = self.infinity_table.output(["docid_col", "_score"]).match('fulltext_col', query_str,
                                                                           f'topn={max_hits}').to_pl()
        return result['docid_col'], result['SCORE']

    def dense_query(self, query: str, model: BGEM3FlagModel, max_hits: int):
        query_embedding = model.encode(query, return_dense=True, return_sparse=False, return_colbert_vecs=False)
        query_embedding = query_embedding['dense_vecs']
        result = self.infinity_table.output(["docid_col", "_similarity"]).knn("dense_col", query_embedding, "float",
                                                                              "ip", max_hits).to_pl()
        return result['docid_col'], result['SIMILARITY']

    def sparse_query(self, query: str, model: BGEM3FlagModel, max_hits: int):
        query_embedding = model.encode(query, return_dense=False, return_sparse=True, return_colbert_vecs=False)
        query_embedding = query_embedding['lexical_weights']
        tmp_list = []
        for p, v in query_embedding.items():
            tmp_list.append((int(p), float(v)))
        tmp_list.sort()
        query_sparse_data = {"indices": [], "values": []}
        for p, v in tmp_list:
            query_sparse_data["indices"].append(p)
            query_sparse_data["values"].append(v)
        result = self.infinity_table.output(["docid_col", "_similarity"]).match_sparse("sparse_col", query_sparse_data,
                                                                                       "ip", max_hits,
                                                                                       {"alpha": "1.0",
                                                                                        "beta": "1.0"}).to_pl()
        return result['docid_col'], result['SIMILARITY']

    def main(self, languages: list[str], query_types: list[str], model: BGEM3FlagModel, save_dir: str):
        for lang in languages:
            print(f"Start to search for language: {lang}")
            self.get_test_table(lang)
            queries, qids = get_queries_and_qids(lang, False)
            print(f"Total number of queries: {len(qids)}")
            for query_type in query_types:
                print(f"Start to search for query method: {query_type}")
                save_path = os.path.join(save_dir, f"{lang}_{query_type}.txt")
                print(f"Save search result to: {save_path}")
                result_list = []
                query_func = self.query_funcs[query_type]
                for query, qid in tqdm(zip(queries, qids), total=len(qids)):
                    docid_list, score_list = query_func(query, model=model, max_hits=1000)
                    result = []
                    for docid, score in zip(docid_list, score_list):
                        result.append(FakeJScoredDoc(docid, score))
                    result_list.append((qid, result))
                save_result(result_list, save_path, qids, max_hits=1000)


if __name__ == "__main__":
    parser = HfArgumentParser([ModelArgs, QueryArgs])
    model_args, query_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    query_args: QueryArgs
    languages = check_languages(query_args.languages)
    query_types = check_query_types(query_args.query_types)
    model = get_model(model_args=model_args)
    infinity_client = InfinityClientForSearch()
    infinity_client.main(languages=languages, query_types=query_types, model=model,
                         save_dir=query_args.query_result_dave_dir)
