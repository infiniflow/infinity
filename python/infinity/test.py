import unittest

from python import infinity
from python.infinity.remote.table import traverse_conditions


class MyTestCase(unittest.TestCase):

    def test_infinity(self):
        infinity_obj = infinity.connect("0.0.0.0:50051")
        assert infinity_obj is not None
        # infinity
        infinity_obj.create_database("my_db")
        print(infinity_obj.list_databases())
        infinity_obj.drop_database("my_db")
        # database
        db_obj = infinity_obj.get_database("default")
        db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)
        print(db_obj.list_tables("default"))
        db_obj.drop_table("my_table1")

        # table
        # index
        db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)
        table_obj = db_obj.get_table("my_table2")
        table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}], None)
        table_obj.drop_index("my_index")
        db_obj.drop_table("my_table2")

        # insert
        db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)
        table_obj = db_obj.get_table("my_table3")
        table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        res = table_obj.search().output(["c1", "c2"]).filter("c1>1").to_list()
        print(res)
        # import
        db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
        table_obj = db_obj.get_table("my_table4")
        table_obj.import_data("/tmp/infinity/test_data/embedding_int_dim3.csv", None)

    def test_traverse_conditions(self):
        from sqlglot import condition
        print(traverse_conditions(condition("c1>1 and c2<2 or c3=3.3")))


if __name__ == '__main__':
    unittest.main()
