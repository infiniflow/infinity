# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

"""
This example is to connect local infinity instance, create table, insert data, search the data
"""

import infinity
from infinity.common import SparseVector

try:
    # open a local directory to store the data
    infinity_instance = infinity.connect("/var/infinity")

    # connect to server with 127.0.0.1
    # infinity_instance = infinity.connect(infinity.common.LOCAL_HOST)

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table(
        "my_table",
        {
            "num": {"type": "integer"},
            "body": {"type": "varchar"},
            "vec": {"type": "vector, 4, float"},
            "sparse": {"type": "sparse,100,float,int"},
            "year": {"type": "integer"},
            "tensor": {"type": "tensor,4,float"},
        },
    )

    # Insert 4 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "body": r"unnecessary and harmful",
                "vec": [1.0, 1.2, 0.8, 0.9],
                "sparse": SparseVector([10, 20, 30], [1.1, 2.2, 3.3]),
                "year": 2024,
                "tensor": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]],
            },
            {
                "num": 2,
                "body": r"Office for Harmful Blooms",
                "vec": [4.0, 4.2, 4.3, 4.5],
                "sparse": SparseVector([40, 50, 60], [4.4, 5.5, 6.6]),
                "year": 2023,
                "tensor": [[4.0, 0.0, 4.3, 4.5], [4.0, 4.2, 4.4, 5.0]],
            },
            {
                "num": 3,
                "body": r"A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
                "vec": [4.0, 4.2, 4.3, 4.2],
                "sparse": SparseVector([70, 80, 90], [7.7, 8.8, 9.9]),
                "year": 2019,
                "tensor": [[0.9, 0.1, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]],
            },
            {
                "num": 4,
                "body": r"The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
                "vec": [4.0, 4.2, 4.3, 4.5],
                "sparse": SparseVector([20, 80, 90], [7.7, 7.8, 97.9]),
                "year": 2018,
                "tensor": [[4.0, 4.2, 4.3, 4.5], [4.0, 4.2, 4.3, 4.4]],
            },
        ]
    )

    # TODO: dense vector + sparse vector + full-text + structured data filter + tensor reranker
    # result = table_instance.output(["num", "body"]).match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).match_text("body", "blooms","topn=1").fusion(method="rrf").to_pl()

    res = table_instance.create_index(
        "my_index",
        infinity.index.IndexInfo("body", infinity.index.IndexType.FullText),
        infinity.common.ConflictType.Error,
    )

    result = (
        table_instance.output(
            ["num", "body", "vec", "sparse", "year", "tensor", "_score"]
        )
        .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "cosine", 3)
        .match_sparse(
            "sparse", SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3
        )
        .match_text("body", "blooms", 10)
        .filter("year < 2024")
        .fusion(
            method="match_tensor", topn=2,
            fusion_params={"field": "tensor", "data_type": "float",
                           "data": [[0.9, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]}
        )
        .to_pl()
    )

    print(result)
    infinity_instance.disconnect()

except Exception as e:
    print(str(e))
