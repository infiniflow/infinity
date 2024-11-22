import time

import infinity_http
from numpy import dtype
import pandas as pd
import time
from infinity.common import ConflictType

def do_some_operations(client : infinity_http.infinity_http) :
    table_create_insert_delete_modify(client)
    return

def do_some_operations_cluster(leader_client : infinity_http.infinity_http, other_clients : [infinity_http.infinity_http]):
    table_create_insert_delete_modify(leader_client)
    time.sleep(1)
    for client in other_clients:
        table_create_insert_delete_modify_verify(client)
    return

def table_create_insert_delete_modify_verify(client : infinity_http.infinity_http) :
    db = client.get_database("default_db")
    table = db.get_table("test_data") 
    expected = pd.DataFrame(
        {
            "c1" : (1, 3, 5, 7, 9),
            "c2" : ([[1.0, 1.0, 1.0, 1.0], [1.0, 1.0, 1.0, 1.0], [1.0, 1.0, 1.0, 1.0], [1.0, 1.0, 1.0, 1.0], [1.0, 1.0, 1.0, 1.0]])
        }
    ).astype({"c1" : dtype("int32"), "c2" : dtype("object")})

    res = table.output(["*"]).to_df()
    pd.testing.assert_frame_equal(res, expected)

def table_create_insert_delete_modify(client : infinity_http.infinity_http) :
    db = client.get_database("default_db")
    db.drop_table("test_data", ConflictType.Ignore)
    table = db.create_table("test_data", {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}})
    for i in range(0, 10):
        table.insert([{"c1": i, "c2": [1.0, 2.0, 3.0, 4.0]}])
    for i in range(0, 10, 2):
        table.delete(str.format("c1={}", i))
    for i in range(1, 10, 2):
        table.update(str.format("c1={}", i), {"c2" : [1.0, 1.0, 1.0, 1.0]})
    table_create_insert_delete_modify_verify(client)
