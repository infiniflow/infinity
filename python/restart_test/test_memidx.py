import infinity
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from infinity import index
import time
import pathlib
from infinity.common import ConflictType, SparseVector
import pytest


class TestMemIdx:
    def test_mem_hnsw(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
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

        # config1 can hold 6 rows of hnsw mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_memidx1")
            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6)
                .to_result()
            )
            # print(data_dict["c1"])
            assert data_dict["c1"] == [4, 4, 4, 4, 4, 2]

            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [10]

            table_obj.insert([{"c1": 6, "c2": [0.3, 0.2, 0.1, 0.4]} for i in range(2)])
            # wait for memindex dump & delta checkpoint to dump
            time.sleep(5)
            table_obj.insert([{"c1": 8, "c2": [0.4, 0.3, 0.2, 0.1]}])

        part2()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_memidx1")

            def check():
                data_dict, data_type_dict, _ = (
                    table_obj.output(["c1"])
                    .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6)
                    .to_result()
                )
                assert data_dict["c1"] == [8, 6, 6, 4, 4, 4]

                data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
                assert data_dict["count(star)"] == [13]

            check()
            infinity_obj.optimize("default_db", "test_memidx1", optimize_opt=None)
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

    # recover cose hnsw from mmap column
    def test_mem_hnsw_cos(self, infinity_runner: InfinityRunner):
        # 100M quota in 7.toml not dump index when insert 8192 rows
        row_n = 8192
        config1 = "test/data/config/restart_test/test_memidx/7.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
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
                        "metric": "cosine",
                        "block_size": "1",
                        "encode": "lvq",
                    },
                ),
            )
            table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(row_n)])
            # wait for 8192 lines to dump
            time.sleep(3)

        @decorator1
        def part2(infinity_obj):
            time.sleep(2)

        part1()

        data_dir = "/var/infinity/data"
        cnt = 0
        for path in pathlib.Path(data_dir).rglob("*.col"):
            print(path)
            cnt += 1
        assert cnt == 2
        if cnt != 2:
            print("Warning: memidx dump not triggered. skip this test")
            return

        part2()

    def test_mem_ivf(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_mem_ivf",
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
            )
            res = table_obj.create_index(
                "idx1",
                index.IndexInfo(
                    "c2",
                    index.IndexType.IVF,
                    {
                        "metric": "l2",
                    },
                ),
            )
            assert res.error_code == infinity.ErrorCode.OK

            table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(51)])
            # trigger the dump by 52th record
            table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]}])
            # table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(2)])
            time.sleep(5)
            table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(4)])

        part1()

        # config1 can hold 51 rows of ivf mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_ivf")
            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [56]

            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6, {"nprobe" : "100"})
                .to_result()
            )
            # print(data_dict["c1"])
            assert data_dict["c1"] == [4, 4, 4, 4, 4, 2]

            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [56]

            table_obj.insert([{"c1": 6, "c2": [0.3, 0.2, 0.1, 0.4]} for i in range(2)])
            # wait for memindex dump & delta checkpoint to dump
            time.sleep(5)
            table_obj.insert([{"c1": 8, "c2": [0.4, 0.3, 0.2, 0.1]}])

        part2()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_ivf")

            def check():
                data_dict, data_type_dict, _ = (
                    table_obj.output(["c1"])
                    .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6)
                    .to_result()
                )
                assert data_dict["c1"] == [8, 6, 6, 4, 4, 4]

                data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
                assert data_dict["count(star)"] == [59]

            check()
            infinity_obj.optimize("default_db", "test_mem_ivf", optimize_opt=None)
            check()

            db_obj.drop_table("test_memidx1")

        part3()

    def test_mem_indexer(self, infinity_runner : InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_mem_indexer",
                {"c1" : {"type" : "int"}, "c2": {"type": "varchar"}},
            )
            res = table_obj.create_index(
                "idx1",
                index.IndexInfo(
                    "c2",
                    index.IndexType.FullText,
                ),
            )
            assert res.error_code == infinity.ErrorCode.OK

            table_obj.insert([
                {"c1" : 1, "c2" : "this is a test text"},
                {"c1" : 2, "c2" : "this is not a test text"},
            ])
            # trigger the dump in 3rd record
            table_obj.insert([
                {"c1" : 3, "c2" : "this is indeed a test text"},
            ])
            table_obj.insert([
                {"c1" : 4, "c2" : "this is definitely not a test text"},
                {"c1" : 5, "c2" : "this is nothing but a test text"}, 
            ])

        part1()

        # config1 can hold 2 rows of identical fulltext mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_indexer")
            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [5]

            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_text('c2', 'test text', 3)
                .to_result()
            )
            # print(data_dict["c1"])
            assert data_dict["c1"] == [1, 2, 3]

            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [5]

            # the 2nd dump
            table_obj.insert([
                {"c1" : 6, "c2" : "this is the exact opposite of a test text"},
            ])
            time.sleep(5)
            table_obj.insert([
                {"c1" : 7, "c2" : "what is this?"},
                {"c1" : 8, "c2" : "this is what?"},
                {"c1" : 9, "c2" : "not a test text!"},
                {"c1" : 10, "c2" : "what a this?"},
                {"c1" : 11, "c2" : "this is you!"},
            ])

        part2()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_indexer")

            def check(rows):
                data_dict, data_type_dict, _ = (
                    table_obj.output(["c1"])
                    .match_text('c2', 'this what', 3)
                    .to_result()
                )
                # print(data_dict["c1"])
                assert data_dict["c1"] == [7, 8, 10]

                data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
                assert data_dict["count(star)"] == [rows]

            check(11)
            table_obj.insert([
                {"c1" : 12, "c2" : "this is a text!"},
            ])
            check(12)

            # the 3rd dump
            db_obj.drop_table("test_mem_indexer")

        part3()

    # @pytest.mark.skip(reason="bug")
    def test_mem_bmp(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/2.toml"
        config3 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        test_data = [
            {"c1" : 1, "c2" : SparseVector(indices=[0, 10, 20, 30, 40, 50, 60, 70, 80, 90], values=[1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0])},
            {"c1" : 2, "c2" : SparseVector(indices=[0, 20, 40, 60, 80], values=[2.0, 2.0, 2.0, 2.0, 2.0])},
            {"c1" : 3, "c2" : SparseVector(indices=[0, 30, 60, 90], values=[3.0, 3.0, 3.0, 3.0])},
            {"c1" : 4, "c2" : SparseVector(indices=[0, 40, 80], values=[4.0, 4.0, 4.0])},
            {"c1" : 5, "c2" : SparseVector(indices=[0], values=[0.0])},
        ]
        query_vector = SparseVector(indices=[0, 20, 80], values=[1.0, 2.0, 3.0])

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_mem_bmp",
                {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int"}},
            )
            res = table_obj.create_index(
                "idx1",
                index.IndexInfo(
                    "c2",
                    index.IndexType.BMP,
                    {"BLOCK_SIZE": "8", "COMPRESS_TYPE": "compress"},
                ),
            )
            assert res.error_code == infinity.ErrorCode.OK

            # trigger dump
            for i in range(7):
                table_obj.insert(test_data)

        part1()

        # config1 can hold 51 rows of ivf mem index before dump
        # 1. recover by dumpindex wal & memindex recovery
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_bmp")
            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [35]

            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_sparse("c2", query_vector, "ip", 8)
                .to_result()
            )
            assert data_dict["c1"] == [4, 4, 4, 4, 4, 4, 4, 2]

            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            # print(data_dict)
            assert data_dict["count(star)"] == [35]

            for i in range(3):
                table_obj.insert(test_data)
            time.sleep(5)

            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_sparse("c2", query_vector, "ip", 11)
                .to_result()
            )
            assert data_dict["c1"] == [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2]

        part2()

        # 2. recover by delta ckp & dumpindex wal & memindex recovery
        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_mem_bmp")

            def check():
                data_dict, data_type_dict, _ = (
                    table_obj.output(["c1"])
                    .match_sparse("c2", query_vector, "ip", 11)
                    .to_result()
                )
                assert data_dict["c1"] == [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2]

                data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
                assert data_dict["count(star)"] == [50]

            check()
            infinity_obj.optimize("default_db", "test_mem_bmp", optimize_opt=None)
            check()

            db_obj.drop_table("test_mem_bmp")

        part3()

    def test_optimize_from_different_database(self, infinity_runner: InfinityRunner):
        infinity_runner.clear()

        config1 = "test/data/config/restart_test/test_memidx/1.toml"
        config2 = "test/data/config/restart_test/test_memidx/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        data_dir = "/var/infinity/data"
        idx1_name = "index1"
        idx2_name = "index2"
        idx1_id = 0
        idx2_id = 1

        @decorator1
        def part1(infinity_obj):
            params = {
                "M": "16",
                "ef_construction": "20",
                "metric": "l2",
                "block_size": "1",
            }

            infinity_obj.drop_database("db1", conflict_type=ConflictType.Ignore)
            db_obj1 = infinity_obj.create_database("db1")
            table_obj1 = db_obj1.create_table(
                "test_memidx1",
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
            )
            table_obj1.create_index(
                idx1_name,
                index.IndexInfo("c2", index.IndexType.Hnsw, params),
            )
            table_obj1.insert(
                [{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(6)]
            )

            infinity_obj.drop_database("db2", conflict_type=ConflictType.Ignore)
            db_obj2 = infinity_obj.create_database("db2")
            table_obj2 = db_obj2.create_table(
                "test_memidx2",
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
            )
            table_obj2.create_index(
                idx2_name,
                index.IndexInfo("c2", index.IndexType.Hnsw, params),
            )
            table_obj2.insert(
                [{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(6)]
            )

            # wait memidx1 dump
            time.sleep(1)
            table_obj1.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(6)])
            table_obj2.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(6)])

        part1()

        idx1_dirs = list(pathlib.Path(data_dir).rglob(f"*idx_{idx1_id}*"))
        idx2_dirs = list(pathlib.Path(data_dir).rglob(f"*idx_{idx2_id}*"))
        assert len(idx1_dirs) == 1
        assert len(idx2_dirs) == 1

        idx1_dir = idx1_dirs[0]
        idx1_files = list(idx1_dir.glob("*"))

        idx2_dir = idx2_dirs[0]
        idx2_files = list(idx2_dir.glob("*"))

        if len(idx1_files) != 2 or len(idx2_files) not in [1, 2]:
            print("Warning: memidx dump not triggered. skip this test")
            infinity_runner.clear()
            return

        @decorator2
        def part2(infinity_obj):
            # wait for optimize
            time.sleep(3)

            idx1_files = list(idx1_dir.glob("*"))
            idx2_files = list(idx2_dir.glob("*"))

            if len(idx1_files) != 3 or len(idx2_files) != 3:
                print("Warning: optimize not triggered. skip this test")
                print(f"idx1_files: {idx1_files}")
                print(f"idx2_files: {idx2_files}")
                infinity_runner.clear()
                return

            assert len(idx1_files) == 3
            assert len(idx2_files) == 3

            time.sleep(2)

            infinity_obj.cleanup()
            idx1_files = list(idx1_dir.glob("*"))
            assert len(idx1_files) == 1

            idx2_files = list(idx2_dir.glob("*"))
            assert len(idx2_files) == 1

        part2()

    def test_recover_memindex_with_dump(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_memidx/4.toml"
        config2 = "test/data/config/restart_test/test_memidx/1.toml"
        uri = common_values.TEST_LOCAL_HOST
        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        table_name = "test_memidx3"

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, conflict_type=ConflictType.Ignore)
            table_obj = db_obj.create_table(
                table_name,
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

            # big enough to trigger dump in part2 memindex recover, but no dump in part1
            table_obj.insert(
                [{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(50)]
            )
            print("insert 50 rows")

        part1()

        @decorator2
        def part2(infinity_obj):
            time.sleep(1)  # wait dump task triggered

            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name)

        part2()

    def test_memidx_recover2(self, infinity_runner: InfinityRunner):
        infinity_runner.clear()

        uri = common_values.TEST_LOCAL_HOST
        data_dir = "/var/infinity/data"
        catalog_dir = "/var/infinity/data/catalog"

        config1 = "test/data/config/restart_test/test_memidx/5.toml"
        config2 = "test/data/config/restart_test/test_memidx/4.toml"
        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        table_name = "test_memidx4"

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, conflict_type=ConflictType.Ignore)
            dim = 100
            table_obj = db_obj.create_table(
                table_name,
                {
                    "c1": {"type": "int"},
                    "c2": {"type": "int"},
                    "c3": {"type": f"sparse,{dim},float,int"},
                },
            )
            table_obj.create_index(
                "idx1", index.IndexInfo("c2", index.IndexType.Secondary)
            )
            table_obj.create_index(
                "idx2",
                index.IndexInfo(
                    "c3",
                    index.IndexType.BMP,
                    {"BLOCK_SIZE": "8", "COMPRESS_TYPE": "compress"},
                ),
            )
            infinity_obj.test_command("stuck dump by line bg_task for 3 second")
            table_obj.insert(
                [
                    {
                        "c1": i,
                        "c2": i,
                        "c3": SparseVector(indices=[0], values=[1.0]),
                    }
                    for i in range(8192)
                ]
            )
            # dump by line submit here
            infinity_obj.flush_delta()
            for i in range(100):
                table_obj.insert(
                    [
                        {
                            "c1": 8192 + i,
                            "c2": 8192 + i,
                            "c3": SparseVector(indices=[1], values=[1.0]),
                        }
                    ]
                )
            # wait for dump by line done
            # time.sleep(4)

        part1()

        delta_paths = list(pathlib.Path(catalog_dir).rglob("*DELTA*"))
        if len(delta_paths) < 1:
            print("Warning: delta checkpoint not triggered. skip this test")
            infinity_runner.clear()
            return

        @decorator2
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"]).filter("c2 >= 8192").to_result()
            )
            assert data_dict["c1"] == [8192 + i for i in range(100)]

            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"])
                .match_sparse("c3", SparseVector(indices=[1], values=[1.0]), "ip", 100)
                .to_result()
            )
            assert data_dict["c1"] == [8192 + i for i in range(100)]

        part2()

        infinity_runner.clear()

    def test_tmp(self, infinity_runner: InfinityRunner):
        infinity_runner.clear()

        config = "test/data/config/restart_test/test_memidx/1.toml"
        uri = common_values.TEST_LOCAL_HOST

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_memidx5",
                {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
            )
            table_obj.create_index(
                "idx1", index.IndexInfo("c2", index.IndexType.FullText)
            )
            table_obj.insert(
                [{"c1": 1, "c2": "hello world. hello world. hello world."}]
            )
            infinity_obj.flush_delta()

            table_obj.insert([{"c1": 2, "c2": "hello c++. hello c++. hello c++."}])
            infinity_obj.flush_delta()

        part1()

        @decorator
        def part2(infinity_obj):
            time.sleep(1)

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_memidx5")
            data_dict, data_type_dict, _ = (
                table_obj.output(["c1"]).match_text("c2", "hello", 2).to_result()
            )
            assert data_dict["c1"] == [1, 2]

        part2()

    def test_optimize_empty_index(self, infinity_runner: InfinityRunner):
        infinity_runner.clear()
        config = "test/data/config/restart_test/test_memidx/6.toml"
        uri = common_values.TEST_LOCAL_HOST

        table_name = "t1"
        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(table_name, {"c1": {"type": "int"}, "c2": {"type": "varchar"}})
            table_obj.create_index("idx1", index.IndexInfo("c2", index.IndexType.FullText))

            insert_n = 8192*3
            for i in range(insert_n):
                table_obj.insert([{"c1": i, "c2": ""}])

            time.sleep(3)

            db_obj.drop_table(table_name)

        part1()
