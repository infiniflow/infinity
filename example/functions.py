import os
import pandas as pd
import infinity
import infinity.index as index
from numpy import dtype
from infinity.errors import ErrorCode
from infinity.common import ConflictType, SortType
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)


infinity_obj = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))
db_obj = infinity_obj.get_database("default_db")
db_obj.drop_table("function_example", ConflictType.Ignore)
db_obj.create_table("function_example",
                    {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                        "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
table_obj = db_obj.get_table("function_example")
table_obj.insert(
    [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
        {"c1": 'abc', "c2": 'abc'}, {"c1": 'bbc', "c2": 'bbc'}, {"c1": 'cbc', "c2": 'cbc'}, {"c1": 'dbc', "c2": 'dbc'},
        {"c1": 'abcd', "c2": 'abc'}])

#function char_length
res = table_obj.output(["*"]).filter("char_length(c1) = 1").to_df()
print(res)

res = table_obj.output(["*"]).filter("char_length(c1) = 3").to_df()
print(res)

res = table_obj.output(["*"]).filter("char_length(c1) = 4").to_df()
print(res)

res = table_obj.output(["*"]).filter("char_length(c1) = char_length(c2)").to_df()
print(res)

res = db_obj.drop_table("function_example")

infinity_obj.disconnect()