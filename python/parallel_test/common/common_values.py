from infinity import NetworkAddress
import numpy as np

TEST_REMOTE_HOST = NetworkAddress("127.0.0.1", 23817)
# TEST_REMOTE_HOST = NetworkAddress("192.168.200.151", 23817)
# infinity thrift server port
infinity_server_port = 23817

identifier_limit = 65536
database_count_limit = 65536
table_count_limit = 65536
table_column_count_limit = 65536

TEST_DATA_DIR = "/test/data/"
TEST_TMP_DIR = "/var/infinity/test_data/"

types_array = [
    # # int
    "int", "int8", "int16", "int32", "int64", "integer",
    # "int128",
    # # float
    # "float", "float32", "double", "float64",
    # # string
    # "varchar",
    # # bool
    # "bool",
    # # vector
    # "vector, 3, float"
]

types_example_array = [
    # int
    1, 127, 32767, 2147483647, pow(2, 63) - 1, 10,
    # pow(2, 127) - 1,
    # float
    # float(1.1), np.float32(1/3), np.double(1/3), np.float64(1/3),
    # string
    # "^789$ test insert varchar",
    # bool
    # True,
    # vector
    # np.array([1.1, 2.2, 3.3]),
    # [1, 2, 3]
]

# db_name, table_name, index_name, column_name
invalid_name_array = [
    [],
    (),
    {},
    1,
    1.1,
    '',
    ' ',
    '12',
    'name-12',
    '12name',
    '数据库名',
    # ''.join('x' for i in range(identifier_limit + 1)),
    # None,
]

types = [
    "integer", "tinyint", "smallint", "bigint", "hugeint", "float",
    "double", "varchar", "boolean"
]
types_example = [
    1, 127, 32767, 2147483647, pow(2, 63) - 1, 10.33, 11.22, "a", True
]

check_file_data = [{"file_name": "pysdk_test_blankspace.csv"},
                   {"file_name": "pysdk_test_commas.csv"},
                   {"file_name": "pysdk_test_semicolons.csv"},
                   {"file_name": "pysdk_test_tabular.csv"}]
delimiter = [["blankspace", " "], ["commas", ","],
             ["semicolons", ";"], ["tabular", "\t"]]


create_valid_option = [
    "kError", "kIgnore", "kReplace"
]
create_invalid_option = [
    1.1, "#@$@!%string", [], {}, ()
]
drop_valid_option = [
    "kError", "kIgnore"
]
drop_invalid_option = [
    "kReplace", 1.1, "#@$@!%string", [], {}, ()
]

invalid_vector_array = []
invalid_int_array = []
invalid_float_array = []
