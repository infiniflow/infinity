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
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder


def main():
    infinity_obj = infinity.connect(REMOTE_HOST)
    db = infinity_obj.get_database("default")
    db.drop_table("my_table", if_exists=True)
    db.create_table(
        "my_table", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, None)

    table = db.get_table("my_table")
    table.insert(
        [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
    table.insert(
        [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
    table.insert(
        [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])

    table.create_index("my_index",
                       [index.IndexInfo("body",
                                        index.IndexType.IRSFullText,
                                        [index.InitParameter("ANALYZER", "segmentation")]),
                        ], None)

    query_builder = InfinityThriftQueryBuilder(table)
    query_builder.output(['num', 'body', 'vec'])
    query_builder.knn('vec', [3.0] * 5, 'float', 'ip', 2)
    query_builder.match('body', 'harmful', 'topn=2')
    query_builder.fusion('rrf')
    res = query_builder.to_pl()
    print(res)


if __name__ == '__main__':
    main()
