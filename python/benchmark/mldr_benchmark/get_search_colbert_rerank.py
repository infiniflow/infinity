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
from itertools import chain, combinations
import time
from dataclasses import dataclass, field
from FlagEmbedding import BGEM3FlagModel
from tqdm import tqdm
import numpy as np
import struct
from mldr_common_tools import QueryArgs, check_languages, check_query_types
from mldr_common_tools import FakeJScoredDoc, get_queries_and_qids, save_result
from mldr_common_tools import query_yields, apply_funcs, get_colbert_model, save_colbert_list
from transformers import HfArgumentParser
import infinity
from infinity.common import LOCAL_HOST


@dataclass
class ModelArgs:
    colbert_model: str = field(default="jina-colbert", metadata={'help': 'The name of the colbert model to use.'})
    fp16: bool = field(default=True, metadata={'help': 'Use fp16 in inference?'})


def powerset_above_2(s: list):
    return chain.from_iterable(combinations(s, r) for r in range(2, len(s) + 1))


single_query_func_params = {'colbert': ('_score', 'SCORE'), 'bm25': ('_score', 'SCORE'),
                            'dense': ('_similarity', 'SIMILARITY'), 'sparse': ('_similarity', 'SIMILARITY')}


class InfinityClientForSearch:
    def __init__(self, with_colbert: bool):
        self.test_db_name = "default_db"
        self.test_table_name_prefix = "mldr_test_table_text_dense_sparse_"
        self.test_table_schema = {"docid_col": {"type": "varchar"}, "fulltext_col": {"type": "varchar"},
                                  "dense_col": {"type": "vector,1024,float"},
                                  "sparse_col": {"type": "sparse,250002,float,int"}}
        if with_colbert:
            self.test_table_name_prefix += "colbert_"
            self.test_table_schema["colbert_col"] = {"type": "tensor,128,float"}
            self.test_table_schema["colbert_bit_col"] = {"type": "tensor,128,bit"}
        self.infinity_obj = infinity.connect(LOCAL_HOST)
        self.infinity_db = self.infinity_obj.get_database(self.test_db_name)
        self.infinity_table = None

    def get_test_table(self, language_suffix: str):
        table_name = self.test_table_name_prefix + language_suffix
        self.infinity_table = self.infinity_db.get_table(table_name)
        print(f"Get table {table_name} successfully.")

    def common_single_query_func(self, query_type: str, query_target, max_hits: int):
        str_params = single_query_func_params[query_type]
        result_table = self.infinity_table.output(["docid_col", str_params[0]])
        result_table = apply_funcs[query_type](result_table, query_target, max_hits)
        result = result_table.to_pl()
        return result['docid_col'], result[str_params[1]]

    def fusion_query(self, query_targets_list: list, apply_funcs_list: list, max_hits: int):
        result_table = self.infinity_table.output(["docid_col", "_score"])
        for query_target, apply_func in zip(query_targets_list, apply_funcs_list):
            result_table = apply_func(result_table, query_target, max_hits)
        result_table = result_table.fusion('rrf', options_text=f'topn={max_hits}')
        result = result_table.to_pl()
        return result['docid_col'], result['SCORE']

    def main(self, languages: list[str], query_types: list[str], model_args: ModelArgs, save_dir: str):
        for lang in languages:
            print(f"Start to search for language: {lang}")
            self.get_test_table(lang)
            # enable streaming to decrease download time
            queries, qids = get_queries_and_qids(lang, False)
            print(f"Total number of queries: {len(qids)}")
            print(f"Average query chars: {sum([len(q) for q in queries]) / len(queries)}")
            for query_type in query_types:
                embedding_file = None
                if query_type in self.prepare_embedding_funcs:
                    embedding_file = os.path.join(save_dir, f"query_embedding_{lang}_{query_type}.data")
                    if not os.path.exists(embedding_file):
                        raise ValueError(f"Embedding file {embedding_file} does not exist.")
                query_yield = query_yields[query_type](queries, embedding_file)
                print(f"Start to search for query method: {query_type}")
                total_query_time: float = 0.0
                total_query_num: int = len(qids)
                result_list = []
                for query, qid in tqdm(zip(queries, qids), total=total_query_num):
                    query_target = next(query_yield)
                    time_start = time.time()
                    docid_list, score_list = self.common_single_query_func(query_type, query_target, max_hits=1000)
                    time_end = time.time()
                    total_query_time += time_end - time_start
                    result = []
                    for docid, score in zip(docid_list, score_list):
                        result.append(FakeJScoredDoc(docid, score))
                    result_list.append((qid, result))
                print(f"Average query time: {1000.0 * total_query_time / float(total_query_num)} ms.")
                save_path = os.path.join(save_dir, f"{lang}_{query_type}.txt")
                print(f"Save search result to: {save_path}")
                save_result(result_list, save_path, qids, max_hits=1000)
            if len(query_types) < 2:
                return
            # fusion ops
            print("Start to search for fusion methods.")
            for query_type_comb in powerset_above_2(query_types):
                query_type_comb_str = '_'.join(query_type_comb)
                print(f"Start to search for fusion method: {query_type_comb_str}")
                embedding_files_list = [os.path.join(save_dir, f"query_embedding_{lang}_{query_type}.data") for
                                        query_type in query_type_comb]
                query_yields_list = [query_yields[query_type](queries, embedding_file) for query_type, embedding_file in
                                     zip(query_type_comb, embedding_files_list)]
                apply_funcs_list = [apply_funcs[query_type] for query_type in query_type_comb]
                total_query_time: float = 0.0
                total_query_num: int = len(qids)
                result_list = []
                for query, qid in tqdm(zip(queries, qids), total=total_query_num):
                    query_targets_list = [next(query_yield) for query_yield in query_yields_list]
                    time_start = time.time()
                    docid_list, score_list = self.fusion_query(query_targets_list, apply_funcs_list, max_hits=1000)
                    time_end = time.time()
                    total_query_time += time_end - time_start
                    result = []
                    for docid, score in zip(docid_list, score_list):
                        result.append(FakeJScoredDoc(docid, score))
                    result_list.append((qid, result))
                print(f"Average query time: {1000.0 * total_query_time / float(total_query_num)} ms.")
                save_path = os.path.join(save_dir, f"{lang}_fusion_{query_type_comb_str}.txt")
                print(f"Save search result to: {save_path}")
                save_result(result_list, save_path, qids, max_hits=1000)


if __name__ == "__main__":
    parser = HfArgumentParser([ModelArgs, QueryArgs])
    model_args, query_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    query_args: QueryArgs
    languages = check_languages(query_args.languages)
    query_types = check_query_types(query_args.query_types)
    if 'colbert' in query_types and not query_args.with_colbert:
        raise ValueError("Colbert query type is enabled but with_colbert is False.")
    infinity_client = InfinityClientForSearch(query_args.with_colbert)
    infinity_client.main(languages=languages, query_types=query_types, model_args=model_args,
                         save_dir=query_args.query_result_dave_dir)
