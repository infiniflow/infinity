# dependency

## python

```shell
pip install -r requirements.txt
```

## cpp

### thrift

```shell
sudo apt-get install thrift-compiler
```

# build

```shell
python setup.py bdist_wheel
```

# install
```shell
pip install dist/infinity-0.0.1-py3-none-any.whl
```
# using

```python
import infinity
from infinity import NetworkAddress

infinity_obj = infinity.connect(NetworkAddress('127.0.0.1', 9080))

# infinity
res = infinity_obj.create_database("my_db")

res = infinity_obj.list_databases()

res = infinity_obj.drop_database("my_db")

db_obj = infinity_obj.get_database("default")
res = db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)

res = db_obj.list_tables()

res = db_obj.drop_table("my_table1")

# index
res = db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)

table_obj = db_obj.get_table("my_table2")

res = table_obj.create_index("my_index",
                             [index.IndexInfo("c1",
                                              index.IndexType.IVFFlat,
                                              [index.InitParameter("centroids_count", "128"),
                                               index.InitParameter("metric", "l2")])], None)

res = table_obj.drop_index("my_index")

res = db_obj.drop_table("my_table2")

# insert
res = db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)

table_obj = db_obj.get_table("my_table3")

res = table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])

res = db_obj.create_table("test_insert_varchar", {"c1": "varchar"}, None)

table_obj = db_obj.get_table("test_insert_varchar")

res = table_obj.insert([{"c1": "test_insert_varchar"}])

res = db_obj.create_table("test_insert_embedding", {"c1": "vector,3,int"}, None)

table_obj = db_obj.get_table("test_insert_embedding")

res = table_obj.insert([{"c1": [4, 5, 6]}])

res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {"c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])

# search

res = table_obj.search().output(["c1 + 0.1"]).to_df()

res = table_obj.search().output(["*"]).filter("c1 > 1").to_df()

# import
res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
table_obj = db_obj.get_table("my_table4")
parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"

res = table_obj.import_data(test_csv_dir, None)

# search
res = table_obj.search().output(["c1"]).filter("c1 > 1").to_df()

res = db_obj.drop_table("my_table4")


res = db_obj.create_table("table_4", {"c1": "int, primary key, not null", "c2": "int", "c3": "int"}, None)
       
table_obj = db_obj.get_table("table_4")

res = table_obj.insert(
    [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
     {"c1": 4, "c2": 40, "c3": 400}])

res = table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])

res = table_obj.delete("c1 = 1")

res = table_obj.delete()

# disconnect
res = infinity_obj.disconnect()

```