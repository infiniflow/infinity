import os
import pytest
from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException

from common.utils import generate_big_int_csv, copy_data, generate_big_rows_csv, generate_big_columns_csv, generate_fvecs, \
    generate_commas_enwiki, read_fvecs_file
from http_adapter import http_adapter


@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.infinity_obj = infinity.connect(uri)
    if http:
        request.cls.infinity_obj = http_adapter()
    yield
    request.cls.infinity_obj.disconnect()

def count_lines(file_path: str):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    return len(lines)

def delete_file(file_path: str):
    if os.path.exists(file_path):
        os.remove(file_path)

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    @pytest.mark.usefixtures("skip_if_http")
    def test_export_csv(self):
        file_name = "enwiki_embedding_9999.csv"
        copy_data(file_name)

        test_csv_dir = common_values.TEST_TMP_DIR + file_name

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_export_csv", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_export_csv", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}, "num": {"type": "integer"}, "vec": {"type": "vector, 4, float"}})
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv", "delimiter" : "\t"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["count(*)"]).to_pl()
        print(res)

        test_export_csv_file_path = common_values.TEST_TMP_DIR + "test_export_csv.csv"
        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t"})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_csv_file_path) == 9999
        delete_file(test_export_csv_file_path)

        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t", "offset": 1000})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_csv_file_path) == 8999
        delete_file(test_export_csv_file_path)

        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t", "offset": 1000, "limit": 2000})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_csv_file_path) == 2000
        delete_file(test_export_csv_file_path)

        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t", "offset": 1000, "limit": 2000, "row_limit": 1500})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_csv_file_path) == 1500
        delete_file(test_export_csv_file_path)

        assert count_lines(test_export_csv_file_path+".part1") == 500
        delete_file(test_export_csv_file_path+".part1")


        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t", "row_limit": 8192})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_csv_file_path) == 8192
        delete_file(test_export_csv_file_path)

        assert count_lines(test_export_csv_file_path+".part1") == 9999 - 8192
        delete_file(test_export_csv_file_path+".part1")

        res = db_obj.drop_table("test_export_csv", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_export_jsonl(self):
        file_name = "enwiki_embedding_9999.csv"
        copy_data(file_name)

        test_csv_dir = common_values.TEST_TMP_DIR + file_name

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_export_jsonl", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_export_jsonl", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}, "num": {"type": "integer"}, "vec": {"type": "vector, 4, float"}})
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv", "delimiter" : "\t"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["count(*)"]).to_pl()
        print(res)

        test_export_jsonl_file_path = common_values.TEST_TMP_DIR + "test_export_jsonl.jsonl"
        res = table_obj.export_data(test_export_jsonl_file_path, {"file_type": "jsonl", "delimiter" : "\t"})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_jsonl_file_path) == 9999
        delete_file(test_export_jsonl_file_path)

        res = table_obj.export_data(test_export_jsonl_file_path, {"file_type": "jsonl", "delimiter" : "\t", "offset": 1000})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_jsonl_file_path) == 8999
        delete_file(test_export_jsonl_file_path)

        res = table_obj.export_data(test_export_jsonl_file_path, {"file_type": "jsonl", "delimiter" : "\t", "offset": 1000, "limit": 2000})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_jsonl_file_path) == 2000
        delete_file(test_export_jsonl_file_path)

        res = table_obj.export_data(test_export_jsonl_file_path, {"file_type": "jsonl", "delimiter" : "\t", "offset": 1000, "limit": 2000, "row_limit": 1500})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_jsonl_file_path) == 1500
        delete_file(test_export_jsonl_file_path)

        assert count_lines(test_export_jsonl_file_path+".part1") == 500
        delete_file(test_export_jsonl_file_path+".part1")


        res = table_obj.export_data(test_export_jsonl_file_path, {"file_type": "jsonl", "delimiter" : "\t", "row_limit": 8192})
        assert res.error_code == ErrorCode.OK
        assert count_lines(test_export_jsonl_file_path) == 8192
        delete_file(test_export_jsonl_file_path)

        assert count_lines(test_export_jsonl_file_path+".part1") == 9999 - 8192
        delete_file(test_export_jsonl_file_path+".part1")

        res = db_obj.drop_table("test_export_jsonl", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_export_fvecs(self):
        file_name = "enwiki_embedding_9999.csv"
        copy_data(file_name)

        test_csv_dir = common_values.TEST_TMP_DIR + file_name

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_export_fvecs", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_export_fvecs", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}, "num": {"type": "integer"}, "vec": {"type": "vector, 4, float"}})
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv", "delimiter" : "\t"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["count(*)"]).to_pl()
        print(res)

        test_export_fvecs_file_path = common_values.TEST_TMP_DIR + "test_export_fvecs.fvecs"
        res = table_obj.export_data(test_export_fvecs_file_path, {"file_type": "fvecs"}, ["vec"])
        assert res.error_code == ErrorCode.OK
        assert len(read_fvecs_file(test_export_fvecs_file_path)) == 9999
        delete_file(test_export_fvecs_file_path)

        res = table_obj.export_data(test_export_fvecs_file_path, {"file_type": "fvecs", "offset": 1000}, ["vec"])
        assert res.error_code == ErrorCode.OK
        assert len(read_fvecs_file(test_export_fvecs_file_path)) == 8999
        delete_file(test_export_fvecs_file_path)

        res = table_obj.export_data(test_export_fvecs_file_path, {"file_type": "fvecs", "offset": 1000, "limit": 2000}, ["vec"])
        assert res.error_code == ErrorCode.OK


        assert len(read_fvecs_file(test_export_fvecs_file_path)) == 2000
        delete_file(test_export_fvecs_file_path)

        res = table_obj.export_data(test_export_fvecs_file_path, {"file_type": "fvecs", "offset": 1000, "limit": 2000, "row_limit": 1500}, ["vec"])
        assert res.error_code == ErrorCode.OK
        assert len(read_fvecs_file(test_export_fvecs_file_path)) == 1500
        delete_file(test_export_fvecs_file_path)

        assert len(read_fvecs_file(test_export_fvecs_file_path+".part1")) == 500
        delete_file(test_export_fvecs_file_path+".part1")


        res = table_obj.export_data(test_export_fvecs_file_path, {"file_type": "fvecs", "row_limit": 8192}, ["vec"])
        assert res.error_code == ErrorCode.OK
        assert len(read_fvecs_file(test_export_fvecs_file_path)) == 8192
        delete_file(test_export_fvecs_file_path)

        assert len(read_fvecs_file(test_export_fvecs_file_path+".part1")) == 9999 - 8192
        delete_file(test_export_fvecs_file_path+".part1")

        res = db_obj.drop_table("test_export_fvecs", ConflictType.Error)
        assert res.error_code == ErrorCode.OK