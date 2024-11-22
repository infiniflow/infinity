import time

import infinity_http
from numpy import dtype
import pandas as pd
import time
from infinity.common import ConflictType
from dataclasses import dataclass
from typing import Dict, Set, Tuple

class instance_state: 
    def __init__(self, client : infinity_http.infinity_http = None):
        self.db2tables : Dict[str, Set[str]] = {"default_db" : set()}
        self.dbtable2index : Dict[Tuple[str, str], Set[str]] = {}
        self.dbtable2df : Dict[Tuple[str, str], pd.DataFrame] = {}

        if client is not None:
            databases = client.list_databases().db_names
            for db_name in databases:
                self.add_database(db_name, ConflictType.Ignore)
                db_object = client.get_database(db_name)
                tables = db_object.get_all_tables()
                for table_name in tables:
                    table_object = db_object.get_table(table_name)
                    df = table_object.output(["*"]).to_df()
                    print(df)
                    self.add_table(db_name, table_name, ConflictType.Ignore)
                    self.set_table_df(db_name, table_name, df)
                    indexes = table_object.list_indexes().index_list
                    for index in indexes:
                        self.add_index(db_name, table_name, index["index_name"], ConflictType.Ignore)

    def check_db_exist(self, db_name : str):
        if db_name not in self.db2tables:
            raise InfinityException(ErrorCode.DB_NOT_EXIST, f"database {db_name} does not exist!")

    def check_db_not_exist(self, db_name : str):
        if db_name in self.db2tables:
            raise InfinityException(ErrorCode.DUPLICATE_DATABASE_NAME, f"database {db_name} already exists!")

    def check_table_exist(self, db_name : str, table_name : str):
        self.check_db_exist(db_name)
        if table_name not in self.db2tables[db_name]:
            raise InfinityException(ErrorCode.TABLE_NOT_EXIST, f"table {db_name}.{table_name} does not exist!")

    def check_table_not_exist(self, db_name : str, table_name : str):
        self.check_db_exist(db_name)
        if table_name in self.db2tables[db_name]:
            raise InfinityException(ErrorCode.DUPLICATE_TABLE_NAME, f"table {db_name}.{table_name} already exists!")

    def check_index_exist(self, db_name : str, table_name : str, index_name : str):
        self.check_table_exist(db_name, table_name)
        if index_name not in self.dbtable2index:
            raise InfinityException(ErrorCode.INDEX_NOT_EXIST, f"table {db_name}.{table_name}.{index_name} does not exist!")

    def check_index_not_exist(self, db_name : str, table_name : str, index_name : str):
        self.check_table_exist(db_name, table_name)
        if index_name in self.dbtable2index:
            raise InfinityException(ErrorCode.DUPLICATE_INDEX_NAME, f"table {db_name}.{table_name}.{index_name} already exists!")

    # operations to a instance_state()
    # add drop : database, table, index

    def add_database(self, db_name : str, conflict_type : ConflictType):
        if conflict_type == ConflictType.Ignore:
            if db_name in self.db2tables:
                return
        elif conflict_type == ConflictType.Error:
            self.check_db_not_exist(db_name)

        self.db2tables[db_name] = set()

    def drop_database(self, db_name : str, conflict_type : ConflictType):
        if conflict_type == ConflictType.Ignore:
            if db_name not in self.db2tables:
                return
        elif conflict_type == ConflictType.Error:
            self.check_db_exist(db_name)

        for table_name in self.db2tables[db_name]:
            self.dbtable2df.pop((db_name, table_name))
            self.dbtable2index.pop((db_name, table_name))
        self.db2tables.pop(db_name)

    def add_table(self, db_name : str, table_name : str, conflict_type : ConflictType):
        if conflict_type == ConflictType.Ignore:
            self.check_db_exist(db_name)
            if table_name in self.db2tables[db_name]:
                return
        elif conflict_type == ConflictType.Error:
            self.check_table_not_exist(db_name, table_name)

        self.db2tables[db_name].add(table_name)
        self.dbtable2index[(db_name, table_name)] = set()

    def drop_table(self, db_name : str, table_name : str, conflit_type : ConflictType):
        if conflit_type == ConflictType.Ignore:
            self.check_database_exist(db_name)
            if table_name not in self.db2tables:
                return
        elif conflit_type == ConflictType.Error:
            self.check_table_exist(db_name, table_name)

        self.db2tables[db_name].remove(table_name)
        self.dbtable2index.pop((db_name, table_name))
        self.dbtable2df.pop((db_name, table_name))

    def add_index(self, db_name : str, table_name : str, index_name : str, conflict_type : ConflictType):
        if conflict_type == ConflictType.Ignore:
            self.check_table_exist(db_name, table_name)
            if index_name in self.dbtable2index[(db_name, table_name)]:
                return
        elif conflict_type == ConflictType.Error:
            self.check_index_not_exist(db_name, table_name, index_name)

        self.dbtable2index[(db_name, table_name)].add(index_name)

    def drop_index(self, db_name : str, table_name : str, index_name : str, conflit_type : ConflictType):
        if conflit_type == ConflictType.Ignore:
            self.check_table_exist(db_name, table_name)
            if index_name not in self.dbtable2index[(db_name, table_name)]:
                return
        elif conflit_type == ConflictType.Error:
            self.check_index_exist(db_name, table_name, index_name)

        self.dbtable2index[(db_name, table_name)].remove(index_name)

    def get_table_df(self, db_name : str, table_name :str) -> pd.DataFrame | None:
        self.check_table_exist(db_name, table_name)
        if (db_name, table_name) in self.dbtable2df:
            return self.dbtable2df[(db_name, table_name)]
        else :
            return None

    def set_table_df(self, db_name : str, table_name :str, df : pd.DataFrame) :
        df = df.reset_index(drop=True)
        print(f"setting {db_name}.{table_name} = ")
        print(df)
        self.check_table_exist(db_name, table_name)
        self.dbtable2df[(db_name, table_name)] = df

def check_instance_table_equal(state : instance_state, client : infinity_http.infinity_http, db_name : str, table_name : str):
    db_obj = client.get_database(db_name)
    table_obj = db_obj.get_table(table_name)
    res = table_obj.output(["*"]).to_df()
    expected = state.get_table_df(db_name, table_name)
    print("res = ")
    print(res)
    print("expected = ")
    print(expected)
    pd.testing.assert_frame_equal(res, expected)

def check_instance_equal(state : instance_state, client : infinity_http.infinity_http):
    client_state = instance_state()
    databases = client.list_databases().db_names
    for db_name in databases:
        client_state.add_database(db_name)
        db_object = client.get_database(db_name)
        tables = db_object.get_all_tables()
        for table_name in tables:
            table_object = db_object.get_table(table_name)
            df = table_object.output(["*"]).to_df()
            client_state.add_table(db_name, table_name, df)
            indexes = table_object.list_indexes().index_list
            for index in indexes:
                client_state.add_index(db_name, table_name, index["index_name"])

    assert state.db2tables == client_state.db2tables
    assert state.dbtable2index == client_state.dbtable2index
    for db_name, tables in state.db2tables.items():
        for table_name in tables:
            pd.testing.assert_frame_equal(state.dbtable2df[(db_name, table_name)], client_state.dbtable2df[(db_name, table_name)])

def do_some_operations_cluster(leader_client : infinity_http.infinity_http, other_clients : [infinity_http.infinity_http], leader_state : instance_state):
    table_create_insert_delete_modify(leader_client, leader_state)
    time.sleep(1)
    for client in other_clients:
        table_create_insert_delete_modify_verify(client, leader_state)
    return

def table_create_insert_delete_modify_verify(client : infinity_http.infinity_http, leader_state : instance_state):
    check_instance_table_equal(leader_state, client, "default_db", "test_data")

def table_create_insert_delete_modify(client : infinity_http.infinity_http, leader_state : instance_state):
    db = client.get_database("default_db")
    table = db.create_table("test_data", {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}, ConflictType.Ignore) 
    leader_state.add_table("default_db", "test_data", ConflictType.Ignore)
    table_df = leader_state.get_table_df("default_db", "test_data")
    print("got the exsiting table_df")
    print(table_df)

    for i in range(0, 10):
        table.insert([{"c1": i, "c2": [1.0, 2.0, 3.0, 4.0]}])
        df_to_insert = pd.DataFrame(
            {
                "c1" : [i],
                "c2" : [[1.0, 2.0, 3.0, 4.0]]
            }
        ).astype({"c1" : dtype("int32"), "c2" : dtype("object")})
        table_df = pd.concat([table_df, df_to_insert])

    for i in range(0, 10, 2):
        table.delete(str.format("c1={}", i))
        table_df = table_df[table_df["c1"] != i]

    # for i in range(1, 10, 2):
    #     table.update(str.format("c1={}", i), {"c2" : [1.0, 1.0, 1.0, 1.0]})
    #     table_df.loc[table_df["c1"] == i, "c2"] = [1.0, 1.0, 1.0, 1.0]

    # remember to call this every time you modified a table
    leader_state.set_table_df("default_db", "test_data", table_df)
    table_create_insert_delete_modify_verify(client, leader_state)
