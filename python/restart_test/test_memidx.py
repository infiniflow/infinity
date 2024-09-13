import infinity
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from infinity import index
import time


class TestMemIdx:
    def test_mem_hnsw(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        @infinity_runner_decorator_factory(config1, uri, infinity_runner)
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_memidx1",
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
            )
            res = table_obj.create_index(
                "idx1",
                index.IndexInfo(
                    "c2",
                    index.IndexType.Hnsw,
                    {
                        "M": "16",
                        "ef_construction": "20",
                        "metric": "l2",
                        "block_size": "1",
                    },
                ),
            )
            assert res.error_code == infinity.ErrorCode.OK

            table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(5)])
            table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]}])

            # wait for the dump thread to start and memory index is reset
            time.sleep(5)

            table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(4)])

        part1()

        # config1 can held 6 rows of hnsw mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        @infinity_runner_decorator_factory(config2, uri, infinity_runner)
        def part2(infinity_obj):
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

        part2()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        infinity_runner.init(config3)
        infinity_obj = InfinityRunner.connect(uri)

        @infinity_runner_decorator_factory(config3, uri, infinity_runner)
        def part3(infinity_obj):
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

            db_obj.drop_table("test_memidx1")

        part3()


# create table test_memidx1(c1 int, c2 embedding(float, 4));
# create index idx1 on test_memidx1(c2) using hnsw with(m=16, ef_construction=200, metric=l2,block_size=1);
# insert into test_memidx1 values(2, [0.1,0.2,0.3,-0.2]),(2, [0.1,0.2,0.3,-0.2]),(2, [0.1,0.2,0.3,-0.2]),(2, [0.1,0.2,0.3,-0.2]),(2, [0.1,0.2,0.3,-0.2]);
# insert into test_memidx1 values(4,[0.2,0.1,0.3,0.4]);
# # wait 5s
# insert into test_memidx1 values(4,[0.2,0.1,0.3,0.4]),(4,[0.2,0.1,0.3,0.4]),(4,[0.2,0.1,0.3,0.4]),(4,[0.2,0.1,0.3,0.4]);

# select c1 from test_memidx1 search match vector(c2, [0.3,0.3,0.2,0.2],'float','l2',6);
# # result: 4, 4, 4, 4, 4, 2
# select count(*) from test_memidx1;
# # result: 10
# insert into test_memidx1 values(6,[0.3,0.2,0.1,0.4]),(6,[0.3,0.2,0.1,0.4]);
# # wait 5s
# insert into test_memidx1 values(8,[0.4,0.3,0.2,0.1]);

# select c1 from test_memidx1 search match vector(c2, [0.3,0.3,0.2,0.2],'float','l2',6);
# # result: 8, 6, 6, 4, 4, 4
# select count(*) from test_memidx1;
# # result: 13
# # wait 3s
# select c1 from test_memidx1 search match vector(c2, [0.3,0.3,0.2,0.2],'float','l2',6);
# # result: 8, 6, 6, 4, 4, 4
# select count(*) from test_memidx1;
# # result: 13
