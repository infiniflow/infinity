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
import infinity
from tqdm import tqdm
import infinity.index as index
from infinity.common import ConflictType, LOCAL_HOST
from mldr_common_tools import load_corpus, fvecs_read_yield, read_mldr_sparse_embedding_yield
from infinity.errors import ErrorCode


def get_all_part_begin_ends(total_row_count: int):
    result = []
    pos_now = 0
    while pos_now < total_row_count:
        new_pos = int(input("input part end position: "))
        if pos_now >= new_pos or new_pos > total_row_count:
            print("Invalid value. Input again.")
            continue
        result.append((pos_now, new_pos))
        pos_now = new_pos
    return result


# fulltext column, dense embedding column, sparse embedding column
class InfinityClientForInsert:
    def __init__(self):
        self.test_db_name = "default_db"
        self.test_table_name_prefix = "mldr_test_table_text_dense_sparse_"
        self.test_table_schema = {"docid_col": {"type": "varchar"}, "fulltext_col": {"type": "varchar"},
                                  "dense_col": {"type": "vector,1024,float"},
                                  "sparse_col": {"type": "sparse,250002,float,int"}}
        self.infinity_obj = infinity.connect(LOCAL_HOST)
        self.infinity_db = self.infinity_obj.create_database(self.test_db_name, ConflictType.Ignore)
        self.infinity_table = None

    def create_test_table(self, language_suffix: str):
        table_name = self.test_table_name_prefix + language_suffix
        self.infinity_db.drop_table(table_name, ConflictType.Ignore)
        self.infinity_table = self.infinity_db.create_table(table_name, self.test_table_schema)
        print("Create table successfully.")

    def main(self):
        lang = input("Input language to insert: ")
        self.create_test_table(lang)
        corpus = load_corpus(lang)
        total_num = corpus.num_rows
        docid_list = corpus["docid"]
        corpus_text_list = corpus["text"]
        del corpus
        print("Expect total number of rows: {}", total_num)
        dense_embedding_dir = input("Input dense embedding data files dir: ")
        print("Input begin and end position pairs of dense embedding data to insert:")
        dense_part_begin_ends = get_all_part_begin_ends(total_num)
        sparse_embedding_dir = input("Input sparse embedding data files dir: ")
        print("Input begin and end position pairs of sparse embedding data to insert:")
        sparse_part_begin_ends = get_all_part_begin_ends(total_num)
        insert_num = int(input("Input insert row count: "))
        batch_size = int(input("Input batch size: "))
        print("Start inserting data...")
        dense_data = None
        dense_pos_part_end = 0
        dense_pair_id_next = 0
        sparse_data = None
        sparse_pos_part_end = 0
        sparse_pair_id_next = 0
        for begin_idx in tqdm(range(0, insert_num, batch_size)):
            end_idx = min(begin_idx + batch_size, insert_num)
            buffer = []
            for row_pos in range(begin_idx, end_idx):
                if row_pos == dense_pos_part_end:
                    dense_pos_part_begin, dense_pos_part_end = dense_part_begin_ends[dense_pair_id_next]
                    dense_pair_id_next += 1
                    dense_base_name = f"dense-{dense_pos_part_begin}-{dense_pos_part_end}.fvecs"
                    dense_data = fvecs_read_yield(os.path.join(dense_embedding_dir, dense_base_name))
                if row_pos == sparse_pos_part_end:
                    sparse_pos_part_begin, sparse_pos_part_end = sparse_part_begin_ends[sparse_pair_id_next]
                    sparse_pair_id_next += 1
                    sparse_base_name = f"sparse-{sparse_pos_part_begin}-{sparse_pos_part_end}.data"
                    sparse_data = read_mldr_sparse_embedding_yield(os.path.join(sparse_embedding_dir, sparse_base_name))
                insert_dict = {"docid_col": docid_list[row_pos], "fulltext_col": corpus_text_list[row_pos],
                               "dense_col": next(dense_data), "sparse_col": next(sparse_data)}
                buffer.append(insert_dict)
            self.infinity_table.insert(buffer)
            buffer.clear()
        print("Finish inserting data.")
        del dense_data
        del sparse_data
        del docid_list
        del corpus_text_list
        print("Start creating fulltext index.")
        ft_params = []
        if lang == "zh":
            ft_params.append(index.InitParameter("analyzer", "chinese"))
        res = self.infinity_table.create_index("ft_index",
                                               [index.IndexInfo("fulltext_col", index.IndexType.FullText, ft_params)],
                                               ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        print("Finish creating fulltext index.")
        print("Start creating Hnsw index...")
        res = self.infinity_table.create_index("hnsw_index", [index.IndexInfo("dense_col", index.IndexType.Hnsw,
                                                                              [index.InitParameter("M", "16"),
                                                                               index.InitParameter("ef_construction",
                                                                                                   "200"),
                                                                               index.InitParameter("ef", "200"),
                                                                               index.InitParameter("metric", "l2"),
                                                                               index.InitParameter("encode", "lvq")])],
                                               ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        print("Finish creating Hnsw index.")
        print("Start creating BMP index...")
        res = self.infinity_table.create_index("bmp_index", [index.IndexInfo("sparse_col", index.IndexType.BMP,
                                                                             [index.InitParameter("block_size", "16"),
                                                                              index.InitParameter("compress_type",
                                                                                                  "compress")])],
                                               ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        print("Finish creating BMP index.")


if __name__ == "__main__":
    infinity_client = InfinityClientForInsert()
    infinity_client.main()
