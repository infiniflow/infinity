---
sidebar_position: 4
slug: /python_api_reference
---
# API Reference

## connect

***class*** **infinity.connect(*uri = REMOTE_HOST*)**

Connect to the Infinity server and return a infinity object. 

### Parameters

- **uri : NetworkAddress**
 a NetworkAddress object is simply a struct whith 2 fields which indicate ip address(str) and port number(int) respectively. Local infinity service could be accessed by:
 `REMOTE_HOST = NetworkAddress("127.0.0.1", 23817)`
 defined in *infinity.common*, which is also default value.

### Returns

- success: An Infinity object.
- failure: `Exception`

### Examples

```python
infinity_obj = infinity.connect()
infinity_obj = infinity.connect(NetworkAddress("127.0.0.1", 23817))
```

---

## disconnect

**infinity.disconnet()**

Disconnect the current Infinity object from the server.
>automatically called when an Infinity object is destructed.

### Returns

- success: `True`
- failure: `Exception`

### Examples

```python
infinity_obj.disconnect()
```

## create_database

**Infinity.create_database(*db_name, conflict_type = ConflictType.Error*)**

Create a database using given name. Different approaches will be adopted depending on the `conflict_type` field, if database with the same name has existed. 

### Parameters

- **db_name : str(ont empty)** 
name of the database
- **confilict_type : ConflictType**
emun type which could be *Error*, *Ignore* or *Replace*, defined in *infinity.common*

### Returns

- success: `True`
- failure: `Exception`

### Examples
```python
infinity_obj.create_database("my_database")
```

## drop_database

**Infinity.drop_database(*db_name, conflict_type = ConflictType.Error*)**

Drop a database by name.

### Parameters

- **db_name : str** 
name of the database
- **confilict_type : ConflictType**
emun type which could be *Error* or *Ignore*, defined in *infinity.common*

### Returns

- success: `True`
- failure: `Exception`

### Examples
```python
infinity_obj.drop_database("my_database")
```

## list_databases
**Infinity.list_databases()**

Lists all databases.

### Details

This method lists all databases.

### Returns

- success: response `db_names` `list[str]`
- failure: `Exception`

### Examples

```python
res = infinity_obj.list_databases() 
res.db_names #["my_database"]
```

## get_database

**Infinity.get_database(*db_name*)**

Retrieve a database object by name.

### Parameters

- **db_name : str** 
name of the database

### Returns

- success: A database object. 
- failure: `Exception`

### Examples

```python
db_obj=infinity_obj.get_database("my_database")
```

## show_database

**Infinity.show_database(*db_name*)**

Get the metadata of a database by name.

### Parameters

- **db_name : str** 
name of the database

### Returns

- success: response `metadata` `ShowDatabaseResponse` 
    except from error infomation, ShowDatabaseResponse struct includes ***database_name : string, store_dir : string, table_count : int***
- failure: `Exception`

### Examples

```python
metadata=infinity_obj.show_database("my_database")
metadata.database_name  #my_database
metadata.table_count  #0
```

---

## create_table

**RemoteDatabase.create_table(*table_name, columns_definition, conflict_type = ConflictType.Error*)**

Create a table using given name, and specify defination of each column.

### Parameters

- **table_name : str(not empty)** 
name of the table to be created
- **columns_definition : dict[str, str]**
A dict object whose key value pair indicates name of the column and its datatype. Espcially, a vector column should be declared as *"vector, \<dimision>\, \<datatype>\"*
`note: ordinary datatype can be int8/int16/int32/int64/int128/`
- **confilict_type : ConflictType**
emun type which could be *Error* or *Ignore*, defined in *infinity.common*

### note
- ordinary datatype can be:
    - int8
    - int16
    - int32/int/integer
    - int64
    - int128
    - float/float32
    - double/float64
    - varchar
    - bool
- vector datatype can be:
    - bit
    - int8
    - int16
    - int32/int
    - int64
    - float/float32
    - double/float64

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples

```python
db_obj.create_table("test_create_varchar_table",
	            {"c1": "varchar", "c2": "float"})
# CREATE TABLE test_create_varchar_table(
#   c1 VARCHAR PRIMARY KEY,
#   c2 FLOAT
# );

db_obj.create_table("test_create_embedding_table", 
                    {"c1": "vector,128,float"}, ConflictType.Replace)
# a 128-dimensional float vector
```

## drop_table

**RemoteDatabase.drop_table(*table_name, conflict_type = ConflictType.Error*)**

Drops a table by name.

### Parameters

- **table_name : str(not empty)** 
name of the table
- **confilict_type : ConflictType**
emun type which could be *Error* or *Ignore*, defined in *infinity.common*

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples

```python
db_obj.drop_table("test_create_varchar_table", ConflictType.Error)
```

## get_table

**RemoteDatabase.get_table(*table_name*)**

Retrieve a table object by name.

### Parameters

- **table_name : str** 
name of the intended table.

### Returns

- success: A table object
- failure: `Exception`, if the table does not exist

### Examples

```python
try:
    table_obj = db_obj.get_table("test_create_varchar_table")
except Exception as e:
    print(e)
```

## list_tables
**RemoteDatabase.list_tables()**

Lists all tables.

### Returns

- success: response `db_names` `list[str]`
- failure: `Exception`

### Examples

```python
res = infinity_obj.list_tables()
res.table_names #["test_create_varchar_table"]
```

## show_table

**RemoteDatabase.show_tables()**

Get the information of each table in the database.

### Returns

- success: response `metadata`: `polars.DataFrame` 
    the returned dataframe contains 8 columns and the number of rows of it depends on how many tables the database have.These 8 columns are respectively:
    - **database : str**
    - **table : str**
    - **type : str**
    - **column_count : int64**
    - **block_count : int64**
    - **block_capacity : int64**
    - **segment_count : int64**
    - **segment_capacity : int64**
- failure: `Exception`

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

**RemoteTable.create_index(*index_name, index_infos, conflict_type = ConflictType.Error*)**

Create an index by `IndexInfo` list.

### Parameters

- **index_name : str**
- **index_infos : list[IndexInfo]**
A IndexInfo struct contains three fields, which are column_name, index_type, index_param_list respectively.
    - **column_name : str**
     Name of the column to build index on.
    - **index_type : IndexType**
    emun type which could be `IVFFlat` , `Hnsw`, `HnswLVQ` or `FullText`, defined in *infinity.index*
    `Note: The difference between Hnsw and HnswLVQ is only adopting different clustering method. The former uses K-Means while the later uses LVQ(Learning Vector Quantization)`
    - **index_param_list**
    A list of InitParameter. The InitParameter struct is like a key-value pair, with two string fields named param_name and param_value. The optional parameters of each type of index are listed below:
        - IVFFlat : `'centroids_count'`(default:`'128'`), `'metric'`(required)
        - Hnsw & HnswLVQ : `'M'`(default:`'16'`), `'ef_construction'`(default:`'50'`), `'ef'`(default:`'50'`), `'metric'`(required)
        - FullText : `'ANALYZER'`(default:`'standard'`)

        the metric field supports `ip`(inner product) and `l2`(Euclidean distance)
- **confilict_type : ConflictType**
emun type which could be `Error` , `Replace`, or `Ignore`, defined in *infinity.common*

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples

```python
db_obj.create_table("test_index_ivfflat", {
            "c1": "vector,1024,float"}, None)
db_obj.get_table("test_index_ivfflat")
table_obj.create_index("my_index",
                        [index.IndexInfo("c1",index.IndexType.IVFFlat,
	                    [
                            index.InitParameter("centroids_count", "128"),
                            index.InitParameter("metric", "l2")])], None)
```

```python
db_obj.create_table(
            "test_index_hnsw", {"c1": "vector,1024,float"}, None)

db_obj.get_table("test_index_hnsw")
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
            "test_index_fulltext", {"doctitle": "varchar", "docdate": "varchar", "body": "varchar"}, None)

db_obj.get_table("test_index_fulltext")
table_obj.create_index("my_index",
                             [index.IndexInfo("body", index.IndexType.FullText, []),
                              index.IndexInfo("doctitle", index.IndexType.FullText, []),
                              index.IndexInfo("docdate", index.IndexType.FullText, []),
                              ], None)
```

## drop_index

**RemoteTable.drop_index(*index_name, conflict_type = ConflictType.Error*)**

Drops an index by name.

### Parameters

- **index_name : str** 
The name of the index to drop.
- **confilict_type : ConflictType**
emun type which could be *Error* or *Ignore*, defined in *infinity.common*

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples

```python
table_obj.drop_index("my_index")
```

## show_index

**RemoteTable.show_index(*index_name*)**

Retrieve the metadata of an index by name.

### Parameters

- **index_name : str** 
name of the index to look up.

### Returns

- success: `metadata` : `ShowIndexResponse`
the struct `ShowIndexResponse` contains:
    - **db_name: string**
    - **table_name: string**
    - **index_name: string**
    - **index_type: string**
    - **index_column_names: string**
    - **index_column_ids: string**
    - **other_parameters: string**
    the parameters for index creation
    - **store_dir: string**
    - **segment_index_count: string**
- failure: `Exception`

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

**RemoteTable.list_indexes(*index_name*)**

List the index names built on the table

### Returns

- success: `metadata` : `ListIndexResponse`
    A filed named index_name is a list of retrived index names
- failure: `Exception`

### Examples

```python
res = table_obj.list_indexes()
res.index_names #['my_index']
```

## insert

**RemoteTable.insert(*data*)**

Insert records into the table.

### Details

This method inserts a record into a table. The inserted record is a list of `dict`.

### Parameters

- **data** : list
    a list of dict which contains information of a record, and would have to be consistent with the table schama.
    - dict
        - key: **column name :str**
        - value: ***str, int, float, list(vector)***

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples
```python
table_obj.insert({"profile": [1.1, 2.2, 3.3], "age": 30, "c3": "Michael"})

table_obj.insert([{"c1": [1.1, 2.2, 3.3]}, {"c1": [4.4, 5.5, 6.6]}, {"c1": [7.7, 8.8, 9.9]}])
```

## import_data

**RemoteTable.import_data(*filpath, import_options = None*)**

Import data from a file into the table object

### Parameters

- **file_path : str**
- **options : dict** 
a dict which could contain three fields, 'file_type', 'delimiter' and 'header'. If these are not specifyed in the passing parameters, default value is 'csv', ',' and False repectively.  
    - file_type: str
    can be ``'csv', 'fvecs', 'json', 'jsonl'`` (default: `'csv'`)
    - delimiter : str
    used to decode csv file(defalut: `','`)
    - header : bool
    specify whether the csv file has header(defalut: `False`)

### Returns

- success:
    - response `success` is `True`
- failure: `Exception`

### Examples

```python
table_obj.import_data(test_csv_dir, None)
```

## delete

**RemoteTable.delete(*cond = None*)**

Delete rows by condition.The condition is similar to the WHERE conditions in SQL. If  condition is not specified, all the data will be removed in the table object.

### Parameters

- **cond : str**
`note : cond has only supported 'and' and 'or' conjunction expression by now. more functions like 'between and', 'in' are comming soon` 

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples
```python
table_obj.delete("c1 = 1")
table_obj.delete()
```

## update

**RemoteTable.update(*cond = None*)**

search for rows that satisfy the condition and update them using the provided values.

### Parameters

- **cond : str(not empty)**
- **data : list[dict[str, Union[str, int, float]]](not empty)**
a list of dict where key indicates column, value indicates new value.
`note: update column with vector datatype is meaningless and not supported`

### Returns

- success: response `success` is `True`
- failure: `Exception`

### Examples

```python
table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
table_obj.update("c1 > 2", [{"c2": 100, "c3": 1000}])
```

## output

**RemoteTable.output(*columns*)**
Specifies the columns to display in the search output, or perform aggragation operation and other calculations. 

```python
table_obj.output(["*"])
table_obj.output(["num", "body"])
table_obj.output(["_row_id"])
table_obj.output(["avg(c2)"])
table_obj.output(["c1+5"])
```

### Parameters

- **columns : list[str] (not empty)**
supported aggragation functions:
    - count
    - min
    - max
    - sum
    - avg
    
### Returns

- success: return self `RemoteTable`
- failure: `Exception`

## filter

**RemoteTable.filter(*cond*)**

Build a filtering condition expression.

### Parameters

- **cond : str**
`note : cond has only supported 'and' and 'or' conjunction expression by now.` 

### Returns

- success: return self `RemoteTable`
- failure: `Exception`

### Examples

```python
table_obj.filter("(-7 < c1 or 9 >= c1) and (c2 = 3)")
```

## knn

**RemoteTable.knn(*vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params = None*)**

Build a KNN search expression. Find the top n closet records to the given vector.

### Parameters

- **vector_column_name : str**
- **embedding_data : list/np.ndarray**
- **embedding_data_type : str**
- **distance_type : str**
  -  `'l2'` 
  - `'cosine'`(not available) 
  - `'ip'` 
  - `'hamming'`(not available)

- **topn : int**
- **knn_params : list**

### Returns

- success: return self `RemoteTable`
- failure: `Exception`

### Examples

```python
table_obj.knn('col1', [0.1,0.2,0.3], 'float', 'l2', 100)
table_obj.knn('vec', [3.0] * 5, 'float', 'ip', 2)
```

## match

Build a full-text search expression.

### Parameters

- **fields : str**
    The column where text is searched in, and has create fulltext index on it before. 
- **matching_text : str**
- **options_text : str**
    topn=2, retrive the two most relevent records

### Returns

- success: return self `RemoteTable`
- failure: `Exception`

### Examples
```python
table_obj.match('body', 'harmful', 'topn=2')
```

## fusion

**RemoteTable.fusion(*method, options_text = ''*)**

Build a fusion expression.

### Parameters

- **method : str**
- **method : options_text**

### Returns

- success: return self `RemoteTable`
- failure: `Exception`

### Examples

```python
table_obj.fusion('rrf')
```

### Details

`rrf`:  Reciprocal rank fusion method.

[Reciprocal rank fusion (RRF)](https://plg.uwaterloo.ca/~gvcormac/cormacksigir09-rrf.pdf) is a method that combines multiple result sets with different relevance indicators into one result set. RRF does not requires tuning, and the different relevance indicators do not have to be related to each other to achieve high-quality results.

## get result

**RemoteTable.to_result()**
**RemoteTable.to_df()**
**RemoteTable.to_pl()**
**RemoteTable.to_arrow()**

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