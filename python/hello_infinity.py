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

import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST, NetworkAddress, ConflictType
import pandas as pds


def test_english():
    try:
        infinity_obj = infinity.connect(REMOTE_HOST)
        db = infinity_obj.get_database("default_db")
        # Drop my_table if it already exists
        db.drop_table("my_table", ConflictType.Ignore)
        # Create a table named "my_table"
        table = db.create_table(
            "my_table", {
                "num": {"type": "integer"},
                "body": {"type": "varchar"},
                "vec": {"type": "vector, 4, float"}
            })
        table.insert(
            [{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
        table.insert(
            [{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])

        # `create_index()` is required before match() or fusion()
        res = table.create_index("my_index",
                                 [index.IndexInfo("body",
                                                  index.IndexType.FullText,
                                                  []),
                                  ], ConflictType.Error)
        # assert res.success

        res = table.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()

        pds_df = pds.DataFrame(res)
        json_data = pds_df.to_json()
        print("------json-------")
        print(json_data)

        table_obj = db.get_table("my_table")
        qb_result = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).to_pl()
        print("------tabular -------")
        print("------knn-------")
        print(qb_result)

        qb_result1 = table_obj.match(
            "body", "blooms", "topn=1").output(["num", "body"]).to_pl()
        print("------match-------")
        print(qb_result1)

        qb_result2 = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).match(
            "body", "blooms", "topn=1").fusion('rrf').to_pl()
        print("------knn+match-------")
        print(qb_result2)

    except Exception as e:
        print(str(e))


def test_chinese():
    """
    Checkout https://github.com/infiniflow/resource.git under /var/infinity. The jieba dict is
    /var/infinity/resource/jieba/dict/jieba.dict.utf8
    """
    try:
        infinity_obj = infinity.connect(REMOTE_HOST)
        infinity_obj.create_database("default_db", ConflictType.Ignore)
        db = infinity_obj.get_database("default_db")
        # Drop my_table if it already exists
        db.drop_table("my_table", ConflictType.Ignore)
        # Create a table named "my_table"
        table = db.create_table(
            "my_table", {
                "num": {"type": "integer"},
                "body": {"type": "varchar"},
                "vec": {"type": "vector, 4, float"}
            })
        table.insert(
            [{"num": 1, "body": "据Wccftech报道，苹果正在开发一种定制芯片，旨在为人工智能（AI）服务器提供动力。暂时还不清楚这款芯片的具体规格，以及具体的实现目标。传闻苹果已选择台积电（TSMC）的3nm制程节点来制造这款芯片，预计2025年下半年量产。如果按照量产时间和台积电的半导体工艺进度，那么对应的很可能是N3E工艺。", "vec": [1.0, 1.2, 0.8, 0.9]}])
        table.insert(
            [{"num": 2, "body": "两个月前有报道称，苹果已正式放弃了努力超过十年、投下海量资金的“泰坦计划（Project Titan）”电动车项目。苹果随后解散了大约2000人的开发团队，各人会被分配到其他地方，其中一个很重要的去处就是人工智能部门。有传言称，苹果已经将注意力转向生成式AI，希望能够为业务找到新的增长动力。", "vec": [4.0, 4.2, 4.3, 4.5]}])

        # `create_index()` is required before match() or fusion()
        res = table.create_index("my_index",
                                 [index.IndexInfo("body",
                                                  index.IndexType.FullText,
                                                  [index.InitParameter("ANALYZER", "chinese")]),
                                  ], ConflictType.Error)
        # assert res.success

        res = table.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()

        pds_df = pds.DataFrame(res)
        json_data = pds_df.to_json()
        print("------json-------")
        print(json_data)

        table_obj = db.get_table("my_table")
        qb_result = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).to_pl()
        print("------tabular -------")
        print("------knn-------")
        print(qb_result)

        qb_result1 = table_obj.match(
            "body", "芯片", "topn=1").output(["num", "body"]).to_pl()
        print("------match-------")
        print(qb_result1)

        qb_result2 = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).match(
            "body", "芯片", "topn=1").fusion('rrf').to_pl()
        print("------knn+match-------")
        print(qb_result2)

    except Exception as e:
        print(str(e))


if __name__ == '__main__':
    test_english()
    # test_chinese()
