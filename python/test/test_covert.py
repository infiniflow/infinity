import infinity
from infinity.common import REMOTE_HOST


class TestCovert:
    def test_to_pl(self):
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_to_pl", True)
        db_obj.create_table("test_to_pl", {
            "c1": "int", "c2": "float"}, None)

        table_obj = db_obj.get_table("test_to_pl")
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res = table_obj.output(["c1", "c2"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_pl()
        print(res)


    def test_to_pa(self):
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_to_pa", True)
        db_obj.create_table("test_to_pa", {
            "c1": "int", "c2": "float"}, None)

        table_obj = db_obj.get_table("test_to_pa")
        table_obj.insert([{"c1": 1, "c2": 2.0}])
        print()
        res = table_obj.output(["c1", "c2"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_arrow()
        print(res)
