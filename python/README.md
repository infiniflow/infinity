# dependency

## python
pip install -r requirements.txt
conda install grpcio-tools

## cpp

### brpc
sudo apt-get install -y libssl-dev libgflags-dev libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev

### thrift

sudo apt-get install thrift-compiler
# build
python setup.py bdist_wheel
# install
cd dist
pip install infinity-0.0.1-py3-none-any.whl
# using

```python
import infinity

infinity_obj = infinity.connect("0.0.0.0:50051")

infinity_obj.create_database("my_db")

res = infinity_obj.list_databases()

db_obj = infinity_obj.get_database("default")

db_obj.create_table("my_table1", {"c1": "int, primary key"}, None)

db_obj.list_tables()

db_obj.drop_table("my_table1")

db_obj.create_table("my_table2", {"c1": "vector,1024,float"}, None)

table_obj = db_obj.get_table("my_table2")

table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}], None)

table_obj.drop_index("my_index")

db_obj.create_table("my_table3", {"c1": "int, primary key", "c2": "float"}, None)

table_obj = db_obj.get_table("my_table3")

table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])

res = table_obj.search().output(["c1", "c2"]).filter("c1>1").to_list()
print(res)

res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)

table_obj = db_obj.get_table("my_table4")

test_csv_dir = "/test/data/csv/embedding_int_dim3.csv"

table_obj.import_data(test_csv_dir, None)

res = table_obj.search().output(["c1"]).filter("c1 > 1").to_list()
print(res)

res = infinity_obj.disconnect()
```