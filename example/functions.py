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
        {"c1": 'abcd', "c2": 'abc'}, 
        {"c1": 'test@gmail.com', "c2": 'email'}, {"c1": 'test@hotmail.com', "c2": 'email'},
        {"c1": ' abc', "c2": 'abc'}, {"c1": 'abc ', "c2": 'abc'}, {"c1": ' abc ', "c2": 'abc'}])

# varchar functions

#function char_length
res = table_obj.output(["*", "char_length(c1)"]).filter("char_length(c1) = 1").to_df()
print(res)

res = table_obj.output(["*", "char_length(c1)"]).filter("char_length(c1) = 3").to_df()
print(res)

res = table_obj.output(["*", "char_length(c1)"]).filter("char_length(c1) = 4").to_df()
print(res)

res = table_obj.output(["*", "char_length(c1)"]).filter("char_length(c1) = char_length(c2)").to_df()
print(res)

#function regex
res = table_obj.output(["*", "regex(c1, 'bc')"]).filter("regex(c1, 'bc')").to_df()
print(res)

res = table_obj.output(["*"]).filter("regex(c1, '(\w+([-+.]\w+)*)@(\w+([-.]\w+)*)\.(\w+([-.]\w+)*)')").to_df()
print(res)

#function substring
res = table_obj.output(["*", "substring(c1, 0, 2)"]).filter("substring(c1, 0, 2) = 'ab'").to_df()
print(res)

res = table_obj.output(["*", "substring(c1, 0, 4)"]).filter("substring(c1, 0, 4) = 'test'").to_df()
print(res)

#function upper and lower
res = table_obj.output(["*", "upper(c1)"]).filter("upper(c1) = 'TEST@GMAIL.COM'").to_df()
print(res)

res = table_obj.output(["*"]).filter("lower('ABC') = c1").to_df()
print(res)

#function ltrim, rtrim, trim
res = table_obj.output(["*", "ltrim(c1)"]).filter("ltrim(c1) = 'abc'").to_df()
print(res)

res = table_obj.output(["*", "rtrim(c1)"]).filter("rtrim(c1) = 'abc'").to_df()
print(res)

res = table_obj.output(["*", "trim(c1)"]).filter("trim(c1) = 'abc'").to_df()
print(res)

res = table_obj.output(["*"]).filter("trim('   abc   ') = rtrim(ltrim('   abc   '))").to_df()
print(res)

# math functions
db_obj.drop_table("function_example", ConflictType.Ignore)
db_obj.create_table("function_example",
                    {"c1": {"type": "integer"},
                        "c2": {"type": "double"}}, ConflictType.Error)
table_obj = db_obj.get_table("function_example")
table_obj.insert(
    [{"c1": 1, "c2": 2}, {"c1": 3, "c2": 4}, {"c1": 5, "c2": 6}, {"c1": 7, "c2": 8},
        {"c1": 9, "c2": 10}, {"c1": 11, "c2": 12}, {"c1": 13, "c2": 14}, {"c1": 15, "c2": 16},])

#function sqrt
res = table_obj.output(["*", "sqrt(c1)", "sqrt(c2)"]).to_df()
print(res)

res = table_obj.output(["*", "sqrt(c1)", "sqrt(c2)"]).filter("sqrt(c1) = 3").to_df()
print(res)

res = db_obj.drop_table("function_example")

infinity_obj.disconnect()