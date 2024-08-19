import infinity
from common import common_values
from infinity_runner import InfinityRunner
import infinity.index as index
import time


class TestMemIdx:
    def test_mem_hnsw(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        infinity_runner.init(config1)
        time.sleep(1)
        infinity_obj = infinity.connect(uri)

        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.create_table(
            "test_memidx1", {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
        )
        res = table_obj.create_index(
            "idx1",
            index.IndexInfo(
                "c2",
                index.IndexType.Hnsw,
                {"M": "16", "ef_construction": "20", "metric": "l2", "block_size": "1"},
            ),
        )
        assert res.error_code == infinity.ErrorCode.OK

        table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(5)])
        table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]}])

        # wait for the dump thread to start and memory index is reset
        time.sleep(1)

        table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(4)])

        infinity_obj.disconnect()
        infinity_runner.uninit()

        # config1 can held 6 rows of hnsw mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        infinity_runner.init(config2)
        time.sleep(1)
        infinity_obj = infinity.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_memidx1")
        data_dict, data_type_dict = (
            table_obj.output(["c1"])
            .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6)
            .to_result()
        )
        # print(data_dict["c1"])
        assert data_dict["c1"] == [4, 4, 4, 4, 4, 2]

        data_dict, data_type_dict = table_obj.output(["count(*)"]).to_result()
        # print(data_dict)
        assert data_dict["count(star)"] == [10]

        table_obj.insert([{"c1": 6, "c2": [0.3, 0.2, 0.1, 0.4]} for i in range(2)])
        # wait for memindex dump & delta checkpoint to dump
        time.sleep(5)
        table_obj.insert([{"c1": 8, "c2": [0.4, 0.3, 0.2, 0.1]}])

        infinity_obj.disconnect()
        infinity_runner.uninit()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        infinity_runner.init(config3)
        time.sleep(1)
        infinity_obj = infinity.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_memidx1")

        def check():
            data_dict, data_type_dict = (
                table_obj.output(["c1"])
                .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6)
                .to_result()
            )
            assert data_dict["c1"] == [8, 6, 6, 4, 4, 4]

            data_dict, data_type_dict = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [13]

        check()
        # wait for optimize
        time.sleep(3)
        check()

        infinity_obj.disconnect()
        infinity_runner.uninit()
        time.sleep(1)