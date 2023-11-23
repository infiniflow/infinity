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
import unittest
from sqlglot import condition

import infinity
from infinity.infinity import NetworkAddress
from infinity.remote_thrift.table import traverse_conditions


class MyTestCreateDB(unittest.TestCase):

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        self.assertIsNotNone(infinity_obj)
        # infinity
        res = infinity_obj.create_database("my_db")
        self.assertEqual(res.success, True)
        res = infinity_obj.list_databases()
        self.assertEqual(res.success, True)
        for db in res.db_names:
            self.assertIn(db, ['my_db', 'default'])
        res = infinity_obj.drop_database("my_db")
        self.assertEqual(res.success, True)

        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)
        self.assertEqual(res.success, True)
        res = db_obj.list_tables()
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table1")
        self.assertEqual(res.success, True)

        # index
        res = db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table2")
        self.assertIsNotNone(table_obj)
        res = table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}], None)
        self.assertEqual(res.success, True)
        res = table_obj.drop_index("my_index")
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table2")
        self.assertEqual(res.success, True)

        # insert
        res = db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table3")
        self.assertIsNotNone(table_obj)
        res = table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1", "c2"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table3")
        self.assertEqual(res.success, True)

        # import
        res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table4")
        self.assertIsNotNone(table_obj)
        parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
        test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"
        self.assertTrue(os.path.exists(test_csv_dir))
        res = table_obj.import_data(test_csv_dir, None)
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table4")
        self.assertEqual(res.success, True)

        # disconnect
        res = infinity_obj.disconnect()
        self.assertEqual(res.success, True)

    def test_infinity_grpc(self):
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 50002))
        self.assertIsNotNone(infinity_obj)
        # infinity
        res = infinity_obj.create_database("my_db")
        self.assertEqual(res.success, True)
        res = infinity_obj.list_databases()
        self.assertEqual(res.success, True)
        for db in res.db_names:
            self.assertIn(db, ['my_db', 'default'])
        res = infinity_obj.drop_database("my_db")
        self.assertEqual(res.success, True)

        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)
        self.assertEqual(res.success, True)
        res = db_obj.list_tables()
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table1")
        self.assertEqual(res.success, True)

        # index
        res = db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table2")
        self.assertIsNotNone(table_obj)
        res = table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}], None)
        self.assertEqual(res.success, True)
        res = table_obj.drop_index("my_index")
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table2")
        self.assertEqual(res.success, True)

        # insert
        res = db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table3")
        self.assertIsNotNone(table_obj)
        res = table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1", "c2"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table3")
        self.assertEqual(res.success, True)

        # import
        res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table4")
        self.assertIsNotNone(table_obj)
        parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
        test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"
        self.assertTrue(os.path.exists(test_csv_dir))
        res = table_obj.import_data(test_csv_dir, None)
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table4")
        self.assertEqual(res.success, True)

        # disconnect
        res = infinity_obj.disconnect()
        self.assertEqual(res.success, True)

    def test_infinity_brpc(self):
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 50001))
        self.assertIsNotNone(infinity_obj)
        # infinity
        res = infinity_obj.create_database("my_db")
        self.assertEqual(res.success, True)
        res = infinity_obj.list_databases()
        self.assertEqual(res.success, True)
        for db in res.db_names:
            self.assertIn(db, ['my_db', 'default'])
        res = infinity_obj.drop_database("my_db")
        self.assertEqual(res.success, True)

        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)
        self.assertEqual(res.success, True)
        res = db_obj.list_tables()
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table1")
        self.assertEqual(res.success, True)

        # index
        res = db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table2")
        self.assertIsNotNone(table_obj)
        res = table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}], None)
        self.assertEqual(res.success, True)
        res = table_obj.drop_index("my_index")
        self.assertEqual(res.success, True)
        res = db_obj.drop_table("my_table2")
        self.assertEqual(res.success, True)

        # insert
        res = db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table3")
        self.assertIsNotNone(table_obj)
        res = table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1", "c2"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table3")
        self.assertEqual(res.success, True)

        # import
        res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
        self.assertEqual(res.success, True)
        table_obj = db_obj.get_table("my_table4")
        self.assertIsNotNone(table_obj)
        parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
        test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"
        self.assertTrue(os.path.exists(test_csv_dir))
        res = table_obj.import_data(test_csv_dir, None)
        self.assertEqual(res.success, True)
        # search
        res = table_obj.search().output(["c1"]).filter("c1 > 1").to_list()
        print(res)
        res = db_obj.drop_table("my_table4")
        self.assertEqual(res.success, True)

        # disconnect
        res = infinity_obj.disconnect()
        self.assertEqual(res.success, True)

    def test_traverse_conditions(self):
        res = traverse_conditions(condition("c1>1 and c2<2 or c3=3.3"))
        print(res)


if __name__ == '__main__':
    unittest.main()
