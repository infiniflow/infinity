

# infinity thrift server port
infinity_server_port = 23817

identifier_limit = 65536
database_count_limit = 65536
table_count_limit = 65536
table_column_count_limit = 65536

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
    ''.join('x' for i in range(identifier_limit + 1)),
    None,
]

invalid_vector_array = []
invalid_int_array = []
invalid_float_array = []