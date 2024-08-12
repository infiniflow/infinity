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

from typing import Union
# NOTICE: please check which infinity you are using, local or remote
# this statement is for local infinity
from infinity.local_infinity.types import make_match_tensor_expr
# enable the following import statement to use remote infinity
# from infinity.remote_thrift.types import make_match_tensor_expr
# from infinity.common import LOCAL_HOST


class InfinityHelperForColBERT:
    def __init__(self):
        from colbert.modeling.checkpoint import Checkpoint
        from colbert.infra import ColBERTConfig
        self.ckpt = Checkpoint("colbert-ir/colbertv2.0", colbert_config=ColBERTConfig(root="experiments"))
        from langchain.text_splitter import RecursiveCharacterTextSplitter
        self.text_splitter = RecursiveCharacterTextSplitter(chunk_size=512, chunk_overlap=0, length_function=len,
                                                            is_separator_regex=False)
        self.test_db_name = None
        self.test_table_name = None
        self.inner_col_txt = None
        self.inner_col_float = None
        self.inner_col_bit = None
        self.infinity_obj = None
        self.colbert_test_db = None
        self.colbert_test_table = None

    # create a test environment for ColBERT
    # schema: a dictionary of column names and types except the column for text which is reserved for ColBERT
    def create_test_env(self, schema: dict):
        # append two hidden columns: "INNER_HIDDEN_ColBERT_TENSOR_ARRAY_FLOAT", "INNER_HIDDEN_ColBERT_TENSOR_ARRAY_BIT"
        self.test_db_name = "colbert_test_db"
        self.test_table_name = "colbert_test_table"
        self.inner_col_txt = "inner_hidden_colbert_text_for_bm25"
        self.inner_col_float = "inner_hidden_colbert_tensor_array_float"
        self.inner_col_bit = "inner_hidden_colbert_tensor_array_bit"
        if self.inner_col_txt in schema:
            raise ValueError(f"Column name {self.inner_col_txt} is reserved for internal use.")
        if self.inner_col_float in schema:
            raise ValueError(f"Column name {self.inner_col_float} is reserved for internal use.")
        if self.inner_col_bit in schema:
            raise ValueError(f"Column name {self.inner_col_bit} is reserved for internal use.")
        schema[self.inner_col_txt] = {"type": "varchar"}
        schema[self.inner_col_float] = {"type": "tensorarray,128,float"}
        schema[self.inner_col_bit] = {"type": "tensorarray,128,bit"}
        import infinity
        from infinity import NetworkAddress
        import infinity.index as index
        from infinity.common import ConflictType
        # NOTICE: the following statement is for local infinity
        self.infinity_obj = infinity.connect("/var/infinity")
        # enable the following statement to use remote infinity
        # self.infinity_obj = infinity.connect(LOCAL_HOST)
        self.infinity_obj.drop_database(self.test_db_name, ConflictType.Ignore)
        self.colbert_test_db = self.infinity_obj.create_database(self.test_db_name)
        self.colbert_test_table = self.colbert_test_db.create_table(self.test_table_name, schema, ConflictType.Error)
        # NOTICE: the following statement is for english text
        self.colbert_test_table.create_index("test_ft_index",
                                             index.IndexInfo(self.inner_col_txt, index.IndexType.FullText),
                                             ConflictType.Error)
        # please enable the following statement to use chinese text
        # self.colbert_test_table.create_index("test_ft_index",
        #                                      index.IndexInfo(self.inner_col_txt, index.IndexType.FullText,
        #                                                      infinity.index.InitParameter("ANALYZER", "chinese")),
        #                                      ConflictType.Error)

    # clear the test environment for ColBERT
    def clear_test_env(self):
        self.colbert_test_db.drop_table(self.test_table_name)
        self.infinity_obj.drop_database(self.test_db_name)
        self.infinity_obj.disconnect()

    # insert data into the test table
    # insert_data: data to insert for a row, excluding the text for ColBERT
    # text_for_colbert: text for ColBERT
    def insert_data(self, insert_data: dict[str, Union[str, int, float, list[Union[int, float]]]],
                    text_for_colbert: str):
        if self.inner_col_txt in insert_data:
            raise ValueError(f"Column name {self.inner_col_txt} is reserved for internal use.")
        insert_data[self.inner_col_txt] = text_for_colbert
        list_float_tensor_array = []
        list_bit_tensor_array = []
        chunks = self.text_splitter.create_documents([text_for_colbert])
        text_chunks = [chunk.page_content for chunk in chunks]
        # output: split text_for_colbert into text_chunks
        print(f'split input text into {len(text_chunks)} chunks')
        subtext_tensor = self.ckpt.docFromText(text_chunks)
        for tensor in subtext_tensor:
            list_float_tensor = []
            list_bit_tensor = []
            for arr in tensor:
                if arr.dim() != 1 or arr.size(0) != 128:
                    raise ValueError("Dimension error.")
                # if torch.count_nonzero(arr) == 0:
                #     continue
                list_v = arr.tolist()
                list_float_tensor.append(list_v)
                list_bit_tensor.append([1 if x > 0 else 0 for x in list_v])
            if len(list_float_tensor) == 0 or len(list_bit_tensor) == 0:
                raise ValueError("Empty tensor.")
            list_float_tensor_array.append(list_float_tensor)
            list_bit_tensor_array.append(list_bit_tensor)
        insert_data[self.inner_col_float] = list_float_tensor_array
        insert_data[self.inner_col_bit] = list_bit_tensor_array
        self.colbert_test_table.insert([insert_data])

    def show_data(self):
        result = self.colbert_test_table.output(['*']).to_pl()
        print(result)
        return result

    def query_bm25(self, query_str: str, output_columns: list[str], top_n: int):
        if '_row_id' not in output_columns:
            output_columns.append('_row_id')
        if '_score' not in output_columns:
            output_columns.append('_score')
        query_result = self.colbert_test_table.output(output_columns).match(self.inner_col_txt, query_str,
                                                                            f'topn={top_n}').to_pl()
        print(query_result)
        return query_result

    def query_match_tensor(self, query_str: str, output_columns: list[str], top_n: int, target_col_name: str):
        if '_row_id' not in output_columns:
            output_columns.append('_row_id')
        if '_score' not in output_columns:
            output_columns.append('_score')
        query_tensor = self.ckpt.queryFromText([query_str])[0]
        if query_tensor.dim() != 2 or query_tensor.size(1) != 128:
            raise ValueError("Dimension error.")
        query_result = self.colbert_test_table.output(output_columns).match_tensor(target_col_name,
                                                                                   query_tensor.numpy(force=True),
                                                                                   'float', 'maxsim',
                                                                                   f'topn={top_n}').to_pl()
        print(query_result)
        return query_result

    def query_float(self, query_str: str, output_columns: list[str], top_n: int):
        target_col_name = self.inner_col_float
        return self.query_match_tensor(query_str, output_columns, top_n, target_col_name)

    def query_bit(self, query_str: str, output_columns: list[str], top_n: int):
        target_col_name = self.inner_col_bit
        return self.query_match_tensor(query_str, output_columns, top_n, target_col_name)

    def query_fusion(self, query_str: str, output_columns: list[str], final_top_n: int, first_stage_top_n: int,
                     target_col_name: str):
        if '_row_id' not in output_columns:
            output_columns.append('_row_id')
        if '_score' not in output_columns:
            output_columns.append('_score')
        query_tensor = self.ckpt.queryFromText([query_str])[0]
        if query_tensor.dim() != 2 or query_tensor.size(1) != 128:
            raise ValueError("Dimension error.")
        rerank_expr = make_match_tensor_expr(target_col_name, query_tensor.numpy(force=True), 'float', 'maxsim')
        query_result = self.colbert_test_table.output(output_columns).match(self.inner_col_txt, query_str,
                                                                            f'topn={first_stage_top_n}').fusion(
            'match_tensor', f'topn={final_top_n}', match_tensor_expr=rerank_expr).to_pl()
        print(query_result)
        return query_result

    def query_rerank_float(self, query_str: str, output_columns: list[str], final_top_n: int, first_stage_top_n: int):
        target_col_name = self.inner_col_float
        return self.query_fusion(query_str, output_columns, final_top_n, first_stage_top_n, target_col_name)

    def query_rerank_bit(self, query_str: str, output_columns: list[str], final_top_n: int, first_stage_top_n: int):
        target_col_name = self.inner_col_bit
        return self.query_fusion(query_str, output_columns, final_top_n, first_stage_top_n, target_col_name)
