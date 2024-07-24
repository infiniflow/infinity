---
sidebar_position: 4
slug: /python_api_reference
---
# Python API Reference

## connect

***class*** **infinity.connect(*uri*)**

Connect to the Infinity server and return an Infinity object. 

### Parameters

- `uri`: 
  - `NetworkAddress` A NetworkAddress object is a struct with two fields, one for the IP address (`str`) and the other for the port number (`int`). Used when Infinity is deployed as a separate server.
  - a path ('str') to store the Infinity data. Used when Infinity is deployed as a Python module. 

### Returns

- Success: An Infinity object.
- Failure: `Exception`

### Examples

- If Infinity is deployed as a separate server: 

   ```python
   # If Infinity is deployed locally, use infinity.LOCAL_HOST to replace <SERVER_IP_ADDRESS>
   infinity_obj = infinity.connect(infinity.NetworkAddress("<SERVER_IP_ADDRESS>", 23817)) 
   ```

- If Infinity is deployed as a Python module: 
   ```python
   infinity_obj = infinity.connect("/path/to/save/to")
   ```

## disconnect

**infinity.disconnect()**

Disconnect the current Infinity object from the server.

> This method is automatically called when an Infinity object is destructed.

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
infinity_obj.disconnect()
```

## create_database

**Infinity.create_database(*db_name, conflict_type = ConflictType.Error*)**

Create a database with a given name. `conflict_type` determines the approach to take when a database with the same name exists. 

### Parameters

- **db_name : str(not empty)** Name of the database.
- **conflict_type : ConflictType** `enum` The countermeasure to take when a database with the same name exists.  See `ConflictType`, which is defined in the **infinity.common** package: 
  - `Error`
  - `Ignore`

### Returns

- Success: `True`
- Failure: `Exception`

### Examples
```python
infinity_obj.create_database("my_database")
```

## drop_database

**Infinity.drop_database(*db_name, conflict_type = ConflictType.Error*)**

Drop a database by name.

### Parameters

- `db_name`: `str` Name of the database
- `conflict_type`:  `enum` The countermeasure to take when a database with the same name exists.  See `ConflictType`, which is defined in the **infinity.common** package. 
  - `Error`
  - `Ignore`

### Returns

- Success: `True`
- Failure: `Exception`

### Examples
```python
infinity_obj.drop_database("my_database")
```

## list_databases

**Infinity.list_databases()**

List all databases.

### Returns

- Success: `db_names` `list[str]`
- Failure: `Exception`

### Examples

```python
res = infinity_obj.list_databases() 
res.db_names #["my_database"]
```

## get_database

**Infinity.get_database(*db_name*)**

Retrieve a database object by name.

### Parameters

- `db_name`: `str`  Name of the database.

### Returns

- Success: A database object. 
- Failure: `Exception`

### Examples

```python
db_obj=infinity_obj.get_database("my_database")
```

## show_database

**Infinity.show_database(*db_name*)**

Retrieve the metadata of a database by name.

### Parameters

- **db_name : str** Name of the database

### Returns

- Success: Metadata of the database. See the `ShowDatabaseResponse` struct, which includes:
  - `database_name`: `str` Name of the database. 
  - `store_dir`: `str` Directory to the database file.
  - `table_count`: `int` Number of tables in the database.
- Failure: `Exception`

### Examples

```python
metadata=infinity_obj.show_database("my_database")
metadata.database_name  #'my_database'
metadata.table_count  #0
```

---

## create_table

**Database.create_table(*table_name, columns_definition, conflict_type = ConflictType.Error*)**

Create a table with a given name, defining each column in it.

### Parameters

- **table_name : str(not empty)**  Name of the table.
- **columns_definition : dict[str, str]**
A dict object whose key value pair indicates name of the column and its datatype. Especially, a vector column should be declared as `"vector, \<dimension>\, \<datatype>\"`
- **conflict_type : ConflictType** enum type defined in the `infinity.common` package:
  - `Error`
  - `Ignore`


### note
- primitive datatype:
    - int8
    - int16
    - int32/int/integer
    - int64
    - float/float32
    - double/float64
    - bool
- vector datatype:
    - float/float32
- complex datatype:
    - varchar

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
db_obj.create_table("table_example", {
            "c1": {
                "type": "int",
                "constraints": ["primary key",],
                "default": 1
            },
            "c2": {
                "type": "vector,3,float32",
                "default": [2.0, 1.2, 3.1],
            }
        }, None)
# CREATE TABLE table_example(
#   c1 INT PRIMARY KEY DEFAULT 1
#   c2 EMBEDDING(FLOAT, 3) DEFAULT [2.0,1.2,3.1]
# );

db_obj.create_table("my_table", {
            "c1": {
                "type": "varchar", 
                "constraints": ["primary key"]
            },
            "c2": {
                "type": "float"
            }
        })
# CREATE TABLE my_table(
#   c1 VARCHAR PRIMARY KEY,
#   c2 FLOAT
# );

db_obj.create_table("test_create_embedding_table", 
                    {"c1": {"type": "vector,128,float"}}, ConflictType.Replace)
# a 128-dimensional float vector
```

## drop_table

**Database.drop_table(*table_name, conflict_type = ConflictType.Error*)**

Drop a table by name.

### Parameters

- **table_name : str(not empty)** Name of the table.
- **conflict_type : ConflictType** Enum type `ConflictType` is defined in the **infinity.common** package: 
   - `Error`
   - `Ignore`

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
db_obj.drop_table("my_table", ConflictType.Error)
```

## get_table

**Database.get_table(*table_name*)**

Retrieve a table object by name.

### Parameters

- `table_name`: `str` Name of the table to retrieve.

### Returns

- Success: A table object. 
- Failure: `Exception`. For example, when the specified table does not exist.

### Examples

```python
try:
    table_obj = db_obj.get_table("my_table")
except Exception as e:
    print(e)
```

## list_tables

**Database.list_tables()**

List all tables in the current database.

### Returns

- Success: `db_names` `list[str]`
- Failure: `Exception`

### Examples

```python
res = infinity_obj.list_tables()
res.table_names #["my_table"]
```

## show_table

**Database.show_tables()**

Get the information of all tables in the database.

### Returns

- Success: response `metadata`: `polars.DataFrame` The returned 
DataFrame contains eight columns and each row in it corresponds to a table in the database.These eight columns are:
    - `database`: `str`
    - `table`: `str`
    - `type`: `str`
    - `column_count`: `int64`
    - `block_count`: `int64`
    - `block_capacity`: `int64`
    - `segment_count`: `int64`
    - `segment_capacity`: `int64`
- Failure: `Exception`

### Examples

```python
res = db.show_tables()
res
┌──────────┬─────────────────────┬───────┬──────────────┬─────────────┬────────────────┬───────────────┬──────────────────┐
│ database ┆ table               ┆ type  ┆ column_count ┆ block_count ┆ block_capacity ┆ segment_count ┆ segment_capacity │
│ ---      ┆ ---                 ┆ ---   ┆ ---          ┆ ---         ┆ ---            ┆ ---           ┆ ---              │
│ str      ┆ str                 ┆ str   ┆ i64          ┆ i64         ┆ i64            ┆ i64           ┆ i64              │
╞══════════╪═════════════════════╪═══════╪══════════════╪═════════════╪════════════════╪═══════════════╪══════════════════╡
│ default  ┆ test_create_varchar ┆ Table ┆ 2            ┆ 0           ┆ 8192           ┆ 0             ┆ 8388608          │
│          ┆ table               ┆       ┆              ┆             ┆                ┆               ┆                  │
└──────────┴─────────────────────┴───────┴──────────────┴─────────────┴────────────────┴───────────────┴──────────────────┘

```

---

## create_index

**Table.create_index(*index_name, index_infos, conflict_type = ConflictType.Error*)**

Create an index by `IndexInfo` list.

### Parameters

- **index_name : str**
- **index_infos : list[IndexInfo]**
  A IndexInfo struct contains three fields,`column_name`, `index_type`, and `index_param_list`.
    - **column_name : str** Name of the column to build index on.
    - **index_type : IndexType**
      enum type: `IVFFlat` , `Hnsw`, `HnswLVQ`, `FullText`, or `BMP`. Defined in `infinity.index`.
      `Note: The difference between Hnsw and HnswLVQ is only adopting different clustering method. The former uses K-Means while the later uses LVQ(Learning Vector Quantization)`
    - **index_param_list**
      A list of InitParameter. The InitParameter struct is like a key-value pair, with two string fields named param_name and param_value. The optional parameters of each type of index are listed below:
        - `IVFFlat`: `'centroids_count'`(default:`'128'`), `'metric'`(required)
        - `Hnsw`: `'M'`(default:`'16'`), `'ef_construction'`(default:`'50'`), `'ef'`(default:`'50'`), `'metric'`(required)
        - `HnswLVQ`: 
          - `'M'`(default:`'16'`)
          - `'ef_construction'`(default:`'50'`)
          - `'ef'`(default:`'50'`)
          - `'metric'`(required)
             - `ip`: Inner product
             - `l2`: Euclidean distance
        - `FullText`: `'ANALYZER'`(default:`'standard'`)
        - `BMP`: 
          - `block_size=1~256`(default: 16): The size of the block in BMP index
          - `compress_type=[compress|raww]` (default: `compress`): If set to `compress`, the block max is stored in the sparse format, which is suitable for small "block size".
- `conflict_type`: `Enum`. See `ConflictType`, which is defined in the **infinity.common** package. 
          - `Error`
          - `Ignore`

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
db_obj.create_table("test_index_ivfflat", {
            "c1": {"type": "vector,1024,float"}}, None)
table_obj = db_obj.get_table("test_index_ivfflat")
table_obj.create_index("my_index",
                        [index.IndexInfo("c1",index.IndexType.IVFFlat,
	                    [
                            index.InitParameter("centroids_count", "128"),
                            index.InitParameter("metric", "l2")])], None)
```

```python
db_obj.create_table(
            "test_index_hnsw", {"c1": {"type": "vector,1024,float"}}, None)

table_obj = db_obj.get_table("test_index_hnsw")
table_obj.create_index("my_index",
                       [index.IndexInfo("c1",index.IndexType.Hnsw,
                       [
                          index.InitParameter("M", "16"),
                          index.InitParameter("ef_construction", "50"),
                          index.InitParameter("ef", "50"),
                          index.InitParameter("metric", "l2")
                        ])], None)
```

```python
db_obj.create_table(
            "test_index_fulltext", {
                "doctitle": {"type": "varchar"},
                "docdate": {"type": "varchar"},
                "body": {"type": "varchar"}
            }, None)

table_obj = db_obj.get_table("test_index_fulltext")
table_obj.create_index("my_index",
                             [index.IndexInfo("body", index.IndexType.FullText, []),
                              index.IndexInfo("doctitle", index.IndexType.FullText, []),
                              index.IndexInfo("docdate", index.IndexType.FullText, []),
                              ], None)
```

```python
db_obj.create_table(
            "test_index_bmp", {
                "c1": {"type": "sparse,30000,float,int16"}
            }, None)
table_obj = db_obj.get_table("test_index_bmp")
table_obj.create_index("my_index",
                             [index.IndexInfo("c1", index.IndexType.BMP,
                              [
                                  index.InitParameter("block_size", "16"),
                                  index.InitParameter("compress_type", "compress")
                              ])], None)
```

## drop_index

**Table.drop_index(*index_name, conflict_type = ConflictType.Error*)**

Drop an index by name.

### Parameters

- `index_name`: `str` Name of the index to drop.
- `conflict_type`: `enum`. See `ConflictType`, which is defined in the **infinity.common** package. 
  - `Error`
  - `Ignore`

### Returns

- Success:`True`
- Failure: `Exception`

### Examples

```python
table_obj.drop_index("my_index")
```

## show_index

**Table.show_index(*index_name*)**

Retrieve the metadata of an index by name.

### Parameters

- **index_name : str** Name of the index to look up.

### Returns

- Success: `metadata` : `ShowIndexResponse`
  the struct `ShowIndexResponse` contains:
    - **db_name: string** Name of the database
    - **table_name: string**
    - **index_name: string**
    - **index_type: string**
    - **index_column_names: string**
    - **index_column_ids: string**
    - **other_parameters: string** Parameters necessary for index creation.
    - **store_dir: string**
    - **segment_index_count: string**
- Failure: `Exception`

### Examples

```python
res = table_obj.create_index("my_index",[index.IndexInfo("c1",  index.IndexType.IVFFlat,
        [index.InitParameter("centroids_count", "128"),index.  InitParameter("metric", "l2")])], 
        ConflictType.Error)
assert res.error_code == ErrorCode.OK
res = table_obj.show_index("my_index")
print(res)
#ShowIndexResponse(error_code=0, error_msg='', db_name="default_db", table_name='test_create_index_show_index', index_name='my_index',
#index_type='IVFFlat', index_column_names='c1', index_column_ids='0', other_parameters='metric = l2, centroids_count = 128', store_dir='/var/
#infinity/data/7SJK3mOSl2_db_default/f3AsBt7SRC_table_test_create_index_show_index/1hbFtMVaRY_index_my_index', segment_index_count='0')
```

## list_indexes

**Table.list_indexes(*index_name*)**

List the indexes built on the table.

### Returns

- Success: `metadata` : Metadata of the table. See `ListIndexResponse`. A field named index_name is a list of the retrieved index names.
- Failure: `Exception`

### Examples

```python
res = table_obj.list_indexes()
res.index_names #['my_index']
```

## insert

```python
Table.insert(data)
```

Inserts rows (rows) of data into the current table. 

### Parameters

**data** : `json`, *Required*  
Data to insert. Infinity supports inserting multiple rows to a table at one time in the form of `json` (one record) or `json` list (multiple rows), with each key-value pair corresponding to a column name and table cell value.

:::tip NOTE
Batch row limit: 8,192. You are allowed to insert a maximum of 8,192 rows at once. 
:::

:::note
When inserting incomplete rows of data, ensure that all uninserted columns have default values when calling `create_table`. Otherwise, an error will occur.  
For information about setting default column values, see `create_table`.
:::

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

#### Insert primitives

```python {12,14}
# Create a table with four primitive columns:
table_instance = db_instance.create_table("primitive_table", {
    "c1": {"type": "int8"},
    "c2": {"type": "int16"},
    "c3": {"type": "int"},
    "c4": {"type": "int32"},   # Same as int
    "c5": {"type": "integer"}, # Same as int
    "c6": {"type": "int64"},
    "c7": {"type": "varchar"},
    "c8": {"type": "float"},
    "c8": {"type": "float32"}, # Same as float
    "c8": {"type": "double"},
    "c8": {"type": "float64"}, # Same as double
    "c9": {"type": "bool", "default": False},
})

# Insert a complete row into the table:
table_instance.insert("c1": 1, "c2": "Tom", "c3": 90.5, "c4": True)

# Insert an incomplete row, with the "c4" column defaulting to False:
table_instance.insert("c1": 2, "c2": "Jeffery", "c3": 88.0)
```

#### Insert vectors

```python
# Create a table with a integer column and a 3-d vector column:
table_instance = db_instance.create_table("vector_table", {"c1": {"type": "integer", "default": 2024}, "vector_column": {"type": "vector,3,float"}})

# Insert one complete row into the table:
table_obj.insert({"c1": 2023, "vector_column": [1.1, 2.2, 3.3]})

# Insert three rows into the table:
table_obj.insert([{"vector_column": [1.1, 2.2, 3.3]}, {"vector_column": [4.4, 5.5, 6.6]}, {"vector_column": [7.7, 8.8, 9.9]}])
```
#### Insert sparse vectors

```python
# Create a table with a integer column and a 100-d sparse vector column:
table_instance = db_instance.create_table("sparse_vector_table", {"c1": {"type": "integer"}, "sparse_column": {"type": "sparse,100,float,int"}})

# Insert three rows into the table:
# `indices` specifies the correspoing indices to the values in `values`.
# Note that the second row sets "c1" as 2024 by default. 
table_instance.insert([{"c1": 2022, "sparse_column": {"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]}, {"sparse_column":  {"indices": [70, 80, 90], "values": [7.7, 8.8, 9.9]}}}])
```

#### Insert tensors

```python
# Create a table with a tensor column: 
table_instance = db_instance.create_table("tensor_table", {"c1": {"type": "integer", "default": 2024}, "tensor_column": {"type": "tensor,4,float"}})

# Insert one row into the table, with the "c1" column defaulting to 2024:
table_instance.insert([{"tensor_column": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]}])
```

#### Insert tensor arrays

```python
# Creat a table with only one tensor array column:
table_instance = db_instance.create_table("tensor_array_table", {"tensor_array_column": {"type": "tensorarray,2,float"}})

table_instance.insert([{"tensor_array_column": [[[1.0, 2.0], [3.0, 4.0]], [[5.0, 6.0]]]}])
```

## import_data

```python
Table.import_data(filepath, import_options)
```

Imports data from a specified file into the current table. 

### Parameters

#### file_path: `str`, *Required*

Absolute path to the file for export. Supported file types include: 
- `csv`
- `json`
- `jsonl`

#### import_options: `json`

Example: `{"header":True, "delimiter": "\t", file_type}`

- **header**: `bool`  
  Whether to display table header or not. Works with **.csv** files only:
  - `True`: Display table header. 
  - `False`: (Default) Do not display table header. 

- **delimiter**: `str`, *Optional*, Defaults to ","  
  Delimiter to separate columns. Works with **.csv** files only.

- **file_type**: `str`, *Required*  
  The type of the imported file. Supported file types include:
  - `csv`
  - `json`
  - `jsonl`



### Returns

- Success: `True`
- Failure: `Exception`

### Examples

#### Import a csv file

```python
table_instance.import_data(os.getcwd() + "/your_file.csv", {"header": False, "file_type": "csv", "delimiter": "\t"})
```

#### Import a jsonl file

```python
table_instance.import_data(os.getcwd() + "/your_file.jsonl", {"file_type": "csv"})
```

## export_data

```python
Table.export_data(filepath, export_options, columns = None)
```

Exports the current table to a specified file. 

### Parameters

#### file_path: `str`, *Required*

Absolute path to the file for export. Supported file types include: 

- `csv`
- `jsonl`
  
#### export_options: `json`

Example: `{"header": False, "delimiter": "\t", "file_type": "jsonl", "offset": 2, "limit": 5}`

- **header**: `bool`, *Optional*  
  Whether to display table header or not. Works with **.csv** files only:
  - `True`: Display table header. 
  - `False`: (Default) Do not display table header. 

- **delimiter**: `str`, *Optional*, Defaults to ","  
  Delimiter to separate columns. Works with **.csv** files only.

- **file_type**: `str`, *Required*  
  The type of the exported file. Supported file types include:
  - `csv`
  - `jsonl`
  
- **offset**: `int`, *Optional*  
  Index specifying the starting row for export. Usually used in conjunction with `limit`. If not specified, the file export starts from the first row. 

- **limit**: `int`, *Optional*  
  The maximum number of rows to export. Usually used in conjunction with `offset`. If the table's row count exceeds `offset` + `limit`, the excess rows are excluded from the export.

- **row_limit**: `int`, *Optional*  
  Used when you have a large table and need to break the output file into multiple parts. This argument sets the row limit for each part. If you specify **test_export_file.csv** as the file name, the exported files will be named **test_export_file.csv**, **test_export_file.csv.part1**, **test_export_file.csv.part2**, and so one. 

#### columns: `[str]`, *Optional*

Columns to export to the output file, for example, `["num", "name", "score"]`. If not specified, the entire table is exported. 

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
table_instance.export_data(os.getcwd() + "/export_data.jsonl",
                            {"header": False, "file_type": "jsonl", "delimiter": ",", "row_limit": 2}, ["num", "name", "score"])
```

## delete

**Table.delete(*cond = None*)**

Delete rows by condition.The condition is similar to the WHERE conditions in SQL. If  `cond` is not specified, all the data will be removed in the table object.

### Parameters

- **cond : str**
`note : cond has only supported 'and' and 'or' conjunction expression by now. more functions like 'between and', 'in' are comming soon` 

### Returns

- Success: `True`
- Failure: `Exception`

### Examples
```python
table_obj.delete("c1 = 1")
table_obj.delete()
```

## update

**Table.update(*cond = None*)**

Search for rows that match the specified condition and update them accordingly.

### Parameters

- **cond : str(not empty)**
- **data : list[dict[str, Union[str, int, float]]](not empty)**
a list of dict where key indicates column, value indicates new value.
> Infinity does not support updating column with vector datatype.

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
table_obj.update("c1 > 2", [{"c2": 100, "c3": 1000}])
```

## output

**Table.output(*columns*)**
Specify the columns to display in the search output, or perform aggregation operations or arithmetic calculations. 

```python
table_obj.output(["*"])
table_obj.output(["num", "body"])
table_obj.output(["_row_id"])
table_obj.output(["avg(c2)"])
table_obj.output(["c1+5"])
```

### Parameters

- **columns : list[str] (not empty)**
  Supported aggragation functions:
    - count
    - min
    - max
    - sum
    - avg
  
### Returns

- Success: self `Table`
- Failure: `Exception`

## filter

**Table.filter(*cond*)**

Create a filtering condition expression.

### Parameters

- **cond : str**
`cond` has only supported 'and' and 'or' conjunction expression for now. 

### Returns

- Success: self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.filter("(-7 < c1 or 9 >= c1) and (c2 = 3)")
```

## knn

**Table.knn(*vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params = None*)**

Build a KNN search expression. Find the top n closet rows to the given vector.

### Parameters

- **vector_column_name : str**
- **embedding_data : list/np.ndarray**
- **embedding_data_type : str**
- **distance_type : str**
  -  `'l2'` 
  - `'ip'`
  - `'cosine'`
  - `'hamming'`(not available)

- **topn : int**
- **knn_params : list**

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.knn('col1', [0.1,0.2,0.3], 'float', 'l2', 100)
table_obj.knn('vec', [3.0] * 5, 'float', 'ip', 2)
```

## match sparse

**Table.match_sparse(*vector_column_name, sparse_data, distance_type, topn, opt_params = None*)**

### Parameters

- **vector_column_name : str**
- **sparse_data** : `{"indices": list[int], "values": Union(list[int], list[float])}`
- **distance_type : str**
  -  `'ip'`
- **topn : int**
- **opt_params : dict[str, str]**
    common options:
      - 'alpha=0.0~1.0'(default: 1.0): A "Termination Conditions" parameter. The smaller the value, the more aggressive the pruning.
      - 'beta=0.0~1.0'(default: 1.0): A "Query Term Pruning" parameter. The smaller the value, the more aggressive the pruning.

### Returns
- Success: Self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.match_sparse('col1', {"indices": [0, 10, 20], "values": [0.1, 0.2, 0.3]}, 'ip', 100)
table_obj.match_sparse('col1_with_bmp_index', {"indices": [0, 10, 20], "values": [0.1, 0.2, 0.3]}, 'ip', 100, {"alpha": "1.0", "beta": "1.0"})
```

## match

Create a full-text search expression.

### Parameters

- **fields : str**
    The column where text is searched, and has create full-text index on it before. 
- **matching_text : str**
- **options_text : str**
    'topn=2': Retrieve the two most relevant rows. The `topn` is `10` by default.

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples
```python
questions = [
    r"blooms",  # single term
    r"Bloom filter",  # OR multiple terms
    r'"Bloom filter"',  # phrase: adjacent multiple terms
    r"space efficient",  # OR multiple terms
    r"space\-efficient",  # Escape reserved character '-', equivalent to: `space efficient`
    r'"space\-efficient"',  # phrase and escape reserved character, equivalent to: `"space efficient"`
    r'"harmful chemical"~10',  # sloppy phrase, refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-match-query-phrase.html
]
for question in questions:
    table_obj.match('body', question, 'topn=2')
```

## match tensor

**Table.match_tensor(*vector_column_name, tensor_data, tensor_data_type, method_type, topn, extra_option)**

Build a KNN tensor search expression. Find the top n closet rows to the given tensor according to chosen method.

For example, find k most match tensors generated by ColBERT.

### Parameters

- **vector_column_name : str**
- **tensor_data : list/np.ndarray**
- **tensor_data_type : str**
- **method_type : str**
    -  `'maxsim'`

- **extra_option : str** options seperated by ';'
    - `'topn'`
    - **EMVB index options**
        - `'emvb_centroid_nprobe'`
        - `'emvb_threshold_first'`
        - `'emvb_n_doc_to_score'`
        - `'emvb_n_doc_out_second_stage'`
        - `'emvb_threshold_final'`

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples

```python
match_tensor('t', [[1.0, 0.0, 0.0, 0.0], [1.0, 0.0, 0.0, 0.0]], 'float', 'maxsim', 'topn=2')
match_tensor('t', [[1.0, 0.0, 0.0, 0.0], [1.0, 0.0, 0.0, 0.0]], 'float', 'maxsim', 'topn=10;emvb_centroid_nprobe=4;emvb_threshold_first=0.4;emvb_threshold_final=0.5')
```

## fusion

**Table.fusion(*method, options_text = ''*)**

Build a fusion expression.

### Parameters

- **method : str**
    The supported methods are: rrf, weighted_sum, match_tensor
- **options_text : str**

    Common options:

    - 'topn=10': Retrieve the 10 most relevant rows. The defualt value is `100`.

    Dedicated options of rrf:

    - 'rank_constant=30': The default value is `60`.

    Dedicated options of weighted_sum:

    - 'weights=1,2,0.5': The weights of children scorers. The default weight of each weight is `1.0`.

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples

:::alert IMPORTANT
Ensure that you import the following when using `make_match_tensor_expr`:

```python
from infinity.remote_thrift.types import make_match_tensor_expr
```
:::

```python
table_obj.fusion('rrf')
table_obj.fusion('rrf', 'topn=10')
table_obj.fusion('weighted_sum', 'weights=1,2,0.5')
table_obj.fusion('match_tensor', 'topn=2', make_match_tensor_expr('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], 'float', 'maxsim'))
```

### Details

`rrf`:  Reciprocal rank fusion method.

[Reciprocal rank fusion (RRF)](https://plg.uwaterloo.ca/~gvcormac/cormacksigir09-rrf.pdf) is a method that combines multiple result sets with different relevance indicators into one result set. RRF does not requires tuning, and the different relevance indicators do not have to be related to each other to achieve high-quality results.

## optimize

**Table.optimize(*index_name, opt_params*)**

### Parameters

- **index_name : str**
- **opt_params : dict[str, str]**
    Common options:
  - 'topk=10': Optimize the BMP index for top 10. Used only when the index is BMP.

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
table_obj.optimize('bmp_index_name', {'topk': '10'})
```

## get result

**Table.to_result()**
**Table.to_df()**
**Table.to_pl()**
**Table.to_arrow()**

After querying, these four methods above can get result into specific type. 
`Note: output method must be executed before get result`

### Returns

- **to_result() : tuple[dict[str, list[Any]], dict[str, Any]]**
Python's built-in type
- **to_df() : pandas.DataFrame**
- **to_pl() : polars.DataFrame**
- **to_arrow() : pyarrow.Table**

### Examples

```
res = table_obj.output(['c1', 'c1']).to_df()

res = table_obj.output(['*'])
               .knn('vec', [3.0, 2.8, 2.7, 3.1], 'float', 'ip', 1)
               .match('doctitle, num, body', 'word', match_param_3)
               .fusion('rrf')
               .to_pl()
```