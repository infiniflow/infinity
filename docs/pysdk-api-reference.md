# API Reference

# connect

Connects to the Infinity server and gets an Infinity object.

```python
infinity_obj = infinity.connect(REMOTE_HOST)
```

## Details

This method connect to the Infinity server and return a infinity object. 

## Parameters

- `REMOTE_HOST = NetworkAddress("127.0.0.1", 9080)`

## Returns

- success: An Infinity object.
- failure: `Exception`

---

# infinity_obj.disconnect

Disconnects from the Infinity server.

```python
infinity_obj.disconnect()
```

## Details

This method disconnects the current Infinity object from the server.

> It is automatically called when an Infinity object is destructed.

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# infinity_obj.create_database

Creates a database.

```python
infinity_obj.create_database("my_database")
```

## Details

This method creates a database by name. 

## Parameters

- `db_name` : `str`

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# infinity_obj.drop_database

Drops a database.

```python
infinity_obj.drop_database("my_database")
```

## Details

This method drops a database by name.

## Parameters

- `db_name` : `str` Name of the database to drop.

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# infinity_obj.list_databases

Lists all databases.

```python
infinity_obj.list_databases()
```

## Details

This method lists all databases.

## Returns

- success:
    - response `success` is `True`.
    - response `db_names` `list[str]`
- failure: `Exception`

# infinity_obj.get_database

Gets a database object.

```python
db_obj=infinity_obj.get_database("default")
```

## Details

This method retrieves a database object by name.

## Parameters

- `db_name` : `str` The name of the database to retrieve.

## Returns

- success:
    - A database object. 
- failure: `Exception`

---

# db_obj.create_table

Creates a table.

```python
db_obj.create_table("test_create_varchar_table",
	                  {"c1": "varchar, primary key", "c2": "float"}, None)

db_obj.create_table("test_create_embedding_table", 
										{"c1": "vector,128,float"}, None)

```

## Details

### numeric or varchar

- `columns_definition` : `dict`
    - key: `column name`: `str`
    - value: <`datatype`>,<`constraint`>,<`constraint`> :`str`

### embedding

- `columns_definition` : `dict`
    - key: `column name`: `str`
    - value: vector,<`dimension`>,<`element_type`> :`str`

## Parameters

- `table_name` : `str`
- `columns_definition` : `dict[str, str]`
- `options` : None

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# db_obj.drop_table

Drops a table.

```python
db_obj.drop_table("test_create_varchar_table", if_exists=True)
```

## Details

Drops a table by name.

## Parameters

- `table_name` : `str` The name of the table to drop.
- `if_exists` : `bool`

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# db_obj.get_table

Gets a table object.

```python
table_obj = db_obj.get_table("test_create_varchar_table")
```

## Details

This method retrieves a table object by name.

## Parameters

- `table_name` : `str` The name of the intended table.

## Returns

- success:
    - A table object
- failure: `Exception`

# db_obj.list_tables

Lists all tables.

```python
db_obj.list_tables()
```

## Details

This method lists all tables in the database.

## Returns

- success:
    - response `success` is `True`
    - response `table_names` list[str]
- failure: `Exception`

---

# table_obj.create_index

Creates an index by `IndexInfo` list.

```python
db_obj.create_table("test_index_ivfflat", {
            "c1": "vector,1024,float"}, None)
db_obj.get_table("test_index_ivfflat")
table_obj.create_index("my_index",
                       [index.IndexInfo("c1",index.IndexType.IVFFlat,
	                      [		
													index.InitParameter("centroids_count", "128"),
	                        index.InitParameter("metric", "l2")
												])], None)
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
                             [index.IndexInfo("body",
							                                index.IndexType.IRSFullText,
	                                            [index.InitParameter("ANALYZER", "segmentation")]),
                              index.IndexInfo("doctitle",
                                              index.IndexType.IRSFullText,
                                              []),
                              index.IndexInfo("docdate",
                                              index.IndexType.IRSFullText,
                                              []),
                              ], None)
```

## Details

This method uses `indexInfo` to create an index.

- `IndexInfo`
    - `column_name` : `str` Name of the column. Required.
    - `index_type` : `Enum`. The index type. Includes: 
        - `IVFFlat` 
        - `HnswLVQ`
        - ``Hnsw` 
        - `IRSFullText`
    - `index_param_list` : list[InitParameter]
        - `param_name` : `str`
        - `param_value` : `str`
        - example
            - `InitParameter("M", "16")`
            - `InitParameter("ef_construction", "50")`
            - `InitParameter("ef", "50")`
            - `InitParameter("metric", "l2")`
            - `InitParameter("centroids_count", "128")`

## Parameters

- `index_name` : `str`
- `index_infos` : `list[IndexInfo]`
- `options` : None

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.drop_index

Drops an index.

```python
table_obj.drop_index("my_index")
```

## Details

This method drops an index by index name.

## Parameters

- `index_name` : `str` The name of the index to drop.

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.insert

Inserts a record into the table.

```python
table_obj.insert([{"profile": [1.1, 2.2, 3.3], "age": 30, "c3": "Michael"}])
```

```python
table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
```

## Details

This method inserts a record into a table. The inserted record is a list of `dict`.

- `dict`
    - key: `column name` :str
    - value: `str` ,`int` , `float` ,`list`

## Parameters

- `data` : list[dict[str, Union[str, int, float, list[Union[int, float]]]]]

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.import

Imports data into the table.

```python
table_obj.import_data(test_csv_dir, None)
```

## Details

This method imports data into the table object. Supported file types:

- `csv` 
- `fvecs` 
- `json`

## Parameters

- `file_path` : str
- `options` : `None`

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.delete

Delete rows by condition.

```python
table_obj.delete("c1 = 1")
table_obj.delete()
```

## Details

The condition is similar to the WHERE conditions in SQL. If  condition is not specified, this method deletes all data in the table object.

## Parameters

- `cond` :  Optional[str]
- `options` : None

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.update

Updates rows by condition.

```python
table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
```

## Details

This method searches for rows that satisfy the search condition and updates them using the provided values.

- The search condition is similar to the WHERE conditions in SQL.
- Data is what needs to be updated.

## Parameters

- `cond` : `str`
- `data` : list[dict[str, Union[str, int, float]]]

## Returns

- success:
    - response `success` is `True`
- failure: `Exception`

# table_obj.query_builder

Gets a query_builder by self.

```python
query_builder=table_obj.query_builder()
```

## Details

This method retrieves a query_builder by self table.

## Returns

- success:
    - A `query_builder` object.
- failure: `Exception`

---

# query_builder.output

Specifies the columns to display in the search output. 

```python
query_builder.output(["num", "body"])
# To display all columns
query_builder.output(["*"])
# To display row ID
query_builder.output(["_row_id"])
```

## Details

This method specifies the columns to display in the search output. You must input a  list of `str` 

- To display all columns:`["*"]`
- To display row ID: `["_row_id"]`

## Parameters

- `columns` : Optional[list[str]]

## Returns

- self : `InfinityThriftQueryBuilder`

# query_builder.filter

Builds a filtering condition expression.

```python
query_builder.filter("(-7 < c1 or 9 >= c1) and (c2 = 3)")
```

## Details

This method builds a filtering expression. 

`str`:  Similar to the WHERE condition in SQL.

## Parameters

- `where` : Optional[str]

## Returns

- self : `InfinityThriftQueryBuilder`

# query_builder.knn

Builds a KNN search expression.

```python
query_builder.knn('col1', [0.1,0.2,0.3], 'float', 'l2', 100)
query_builder.knn('vec', [3.0] * 5, 'float', 'ip', 2)
```

## Details

VEC supports list or np.ndarray.

## Parameters

- `vector_column_name` : `str`
- `embedding_data` : VEC
- `embedding_data_type` : `str``
  - `float`
  - `int`

- `distance_type` : `str`
  -  `l2` 
  - `cosine` 
  - `ip` 
  - `hamming`

- `topn` : `int`

## Returns

- self : `InfinityThriftQueryBuilder`

# query_builder.match

Builds a full-text search expression.

```python
query_builder.match('body', 'harmful', 'topn=2')
```

## Details

This method builds a full-text search expression. 

## Parameters

- `fields` : `str` The text’s body
- `matching_text` : `str` The text to match.
- `options_text` : `str` `'topn=2'`: The display count is 2.

## Returns

- self : `InfinityThriftQueryBuilder`

# query_builder.fusion

Builds a fusion expression.

```python
query_builder.fusion('rrf')
```

## Details

`rrf`:  Reciprocal rank fusion method.

[Reciprocal rank fusion (RRF)](https://plg.uwaterloo.ca/~gvcormac/cormacksigir09-rrf.pdf) is a method that combines multiple result sets with different relevance indicators into one result set. RRF does not requires tuning, and the different relevance indicators do not have to be related to each other to achieve high-quality results.

## Parameters

- `method` : `str`

## Returns

- self : `InfinityThriftQueryBuilder`

# query_builder.to_result

Returns a data result.

```python
table_obj.query_builder().output(["*"]).to_result()
```

## Details

This method returns a data result of Python's built-in type.

(data_dict, data_type_dict)

## Returns

- `tuple[dict[str, list[Any]], dict[str, Any]]`

# query_builder.to_df

Returns a pandas result.

```python
table_obj.query_builder().output(["*"]).to_df()
```

## Details

This method returns a data result in pandas `DataFrame`.

## Returns

- `pandas.DataFrame`

# query_builder.to_pl

Returns a polars result.

```python
table_obj.query_builder().output(["*"]).to_pl()
```

## Details

This method returns a data result in polars `DataFrame`

## Returns

- `polars.DataFrame`

# query_builder.to_arrow

Returns a pyarrow result.

```python
table_obj.query_builder().output(["*"]).to_arrow()
```

## Details

The method returns a data result in arrow `Table`.

## Returns

- pyarrow.`Table`