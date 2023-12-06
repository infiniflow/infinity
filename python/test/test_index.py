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
from infinity.common import NetworkAddress


class TestIndex:

    def test_create_index_IVFFlat(self):
        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("test_index_ivfflat", {"c1": "vector,1024,float"}, None)
        assert res.success

        table_obj = db_obj.get_table("test_index_ivfflat")
        assert table_obj

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.success

        res = table_obj.drop_index("my_index")
        assert res.success

    def test_create_index_HNSW(self):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("test_index_hnsw", {"c1": "vector,1024,float"}, None)
        assert res.success

        table_obj = db_obj.get_table("test_index_hnsw")
        assert table_obj

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.Hnsw,
                                                      [index.InitParameter("M", "16"),
                                                       index.InitParameter("ef_construction", "50"),
                                                       index.InitParameter("ef", "50"),
                                                       index.InitParameter("metric", "l2")])], None)

        assert res.success

        res = table_obj.drop_index("my_index")
        assert res.success
