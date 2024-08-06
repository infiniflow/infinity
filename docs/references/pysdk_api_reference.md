---
sidebar_position: 4
slug: /python_api_reference
---
# Python API Reference

## connect

```python
infinity.connect(uri)
```

Connects to the Infinity server and gets an Infinity object. 

### Parameters

#### uri: *Required*

The `uri` here can be either a local directory in `str` format or a `NetworkAddress` object:  

- `"/path/to/save/to"` (`str`): A local directory for storing the Infinity data. Used when Infinity is deployed as a Python module. 
- `NetworkAddress`: Used in client-server mode, when you have deployed Infinity as a separate server and wish to connect to it remotely. A `NetworkAddress` object comprises two fields:
  - `"<SERVER_IP_ADDRESS>"` (`str`): The IP address of the Infinity server.  
  - `<PORT>` (`int`): The port number on which Infinity is running. Defaults to 23817.

:::caution IMPORTANT
When connecting to Infinity in a client-server mode, ensure that the version of the client *exactly* matches the version of the server. For example: 

| **Client version** | **Server version** |
| ------------------ | ------------------ |
| v0.1.0             | v0.1.0             |
| v0.1.1             | v0.1.1             |
| v0.2.0             | v0.2.0             |
| v0.2.1             | v0.2.1             |


If the versions do not match, please update your client or server accordingly to ensure compatibility. 

In client-server mode, also ensure that your server version matches the version specified in your configuration file. The matching rule is less strict than exact match: 

- The major and minor versions *must* be identical. 
- The patch version may differ. 

This allows for bug fixes without requiring configuration file changes. 

| **Configuration version** | **Compatible server version** |
| ------------------------- | ----------------------------- |
| v0.1.0                    | v0.1.0, v0.1.1                |
| v0.2.0                    | v0.2.0, v0.2.1                |

:::

### Returns

- Success: An Infinity object.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.

### Examples

#### Connect to Python module Infinity

From v0.2.1 onwards, Infinity also gives you the option to connect to the Infinity service just like calling a Python module. If you have installed Infinity via `pip install infinity-sdk==<v0.2.1_OR_HIGHER>`, you can connect to Infinity and save all related data in a local directory:

```python
infinity_obj = infinity.connect("/path/to/save/to")
```

#### Connect to Infinity in client-server mode

If you have deployed Infinity as a separate server, connect to it via its IP address. Further, if your Infinity is running on your local machine, you can also use `infinity.LOCAL_HOST` to replace `"<SERVER_IP_ADDRESS>"` in the following code snippet. 

```python
# If Infinity is deployed on the local machine, use infinity.LOCAL_HOST to replace <SERVER_IP_ADDRESS>
infinity_obj = infinity.connect(infinity.NetworkAddress("<SERVER_IP_ADDRESS>", 23817)) 
```

---

## disconnect

```python
infinity.disconnect()
```

Disconnects the client from the Infinity server in client-server mode or destructs the Infinity object and releases all associated resources when Infinity is deployed as a Python module. 

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 


### Examples

```python
infinity_obj.disconnect()
```

---

## create_database

```python
Infinity.create_database(db_name, conflict_type = ConflictType.Error)
```

Creates a database with a specified name.

### Parameters

#### db_name: `str`, *Required*

The name of the database. `database_name` requirements:

- Maximum 65,535 characters.
- Must not be empty.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Allowed characters:
  - English letters (a-z, A-Z)
  - Digits (0-9)
  - "_" (underscore)

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a database with the same name exists. 

- `Error`: Raise an error if a database with the same name exists.
- `Ignore`: Ignore the database creation requrest and keep the database with the same name as-is.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

- Success: An `infinity.local_infinity.db.LocalDatabase` object in Python module mode or an `infinity.remote_thrift.db.RemoteDatabase` object in client-server mode. 
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.


### Examples

```python
# Create a database named 'my_database':
# If the specified database already exists, raise an error. 
infinity_obj.create_database("my_database")
```

```python
# Create a database named 'my_database':
# If the specified database already exists, raise an error (same as above). 
infinity_obj.create_database("my_database", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Create a database named 'my_database':
# If the specified database already exists, silently ignore the operation and proceed. 
infinity_obj.create_database("my_database", ConflictType.Ignore)
```

---

## drop_database

```python
Infinity.drop_database(db_name, conflict_type = ConflictType.Error)
```

Deletes a database by its name. 

### Parameters

#### db_name: `str`, *Required*

Name of the database to delete. Must not be empty. 

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a database with the specified name does not exist. 

  - `Error`: Raise an error if the specified database does not exist.
  - `Ignore`: Ignore the operation and proceed regardless, if the specified database does not exist.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 

### Examples

```python
# Delete a database named 'my_database':
# If the specified database does not exist, raise an error. 
infinity_obj.drop_database("my_database")
```

```python
# Delete a database named 'my_database':
# If the specified database does not exist, raise an error (same as above). 
infinity_obj.drop_database("my_database", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Delete a database named 'my_database':
# If the specified database does not exist, silently ignore the operation and proceed.
infinity_obj.drop_database("my_database", ConflictType.Ignore)
```

---

## list_databases

```python
Infinity.list_databases()
```

Gets the names of all databases.

### Returns

A structure containing the following attributes:

- `db_names`: `list[str]` A list of all database names.
- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value: A specific error condition occurs.
- `error_msg`: `str` The error message providing additional details.

### Examples

```python
res = infinity_obj.list_databases() 
print(res.db_names) # ['my_database', 'database_1']
```

---

## get_database

```python
Infinity.get_database(db_name)
```

Retrieves a database object by its name.

### Parameters

#### db_name: `str`, *Required*

Name of the database. Must not be empty. 

### Returns

- Success: An `infinity.local_infinity.db.LocalDatabase` object in Python module mode or an `infinity.remote_thrift.db.RemoteDatabase` object in client-server mode. 
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.

### Examples

```python
db_obj=infinity_obj.get_database("my_database")
```

---

## show_database

```python
Infinity.show_database(db_name)
```

Retrieves the metadata of a database by its name.

### Parameters

#### db_name: `str` *Required*

Name of the database. Must not be empty. 

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 
- `database_name`: `str` A list of all database names.
- `store_dir`: `str` The directory holding the database files. 
- `table_count`: `int` The number of tables in the database.

### Examples

```python
metadata=infinity_obj.show_database("my_database")
metadata.database_name  #'my_database'
metadata.table_count  #0
```

---

## create_table

```python
Database.create_table(table_name, columns_definition, conflict_type = ConflictType.Error)
```

Creates a table with a specified name and defined columns.

### Parameters

#### table_name: `str`, *Required*

The name of the table. `table_name` requirements: 

- Maximum 65,535 characters.
- Must not be empty.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Allowed characters: 
  - English letters (a-z, A-Z)
  - Digits (0-9)
  - "_" (underscore)

#### columns_definition: `dict[str, dict[str, Any]]`, *Required*

Definitions for all table columns as a dictionary. Each key in the dictionary is a column name (`str`), with a corresponding 'value' dictionary defining the column's data type and default value information in key-value pairs:

- **Data type** (`"type"`)  
  The data type of the column. 
- **Default value** (`"default"`)  
  The default value for unspecified cells in that column.  


#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a table with the same name exists.

- `Error`: Raise an error if a table with the same name exists.
- `Ignore`: Ignore the table creation requrest and keep the table with the same name as-is.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_infinity.table.RemoteTable` object in client-server mode. 
- Failure: `InfinityException`:
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.

### Examples

#### Create a table with an integer column only

```python
# The `create_table`method supports creating integer columns in the following data types:
# - int8
# - int16
# - int/int32/integer
# - int64
db_obj.create_table("my_table", {"c1": {"type": "int", "default": 1}})
```

#### Create a table with a float column only

```python
# The `create_table`method supports creating float columns in the following data types:
# - float/float32
# - double/float64
db_obj.create_table("my_table", {"c1": {"type": "float64"}})
```

#### Create a table with a string column only

```python
db_obj.create_table("my_table", {"c1": {"type": "varchar"}})
```

#### Create a table with a bool column only

```python
db_obj.create_table("my_table", {"c1": {"type": "bool"}})
```

#### Create a table with a vector column only

```python
# Create a table with a vector column only:  
# - `vector`: The column is a vector column
# - `128`: The vector dimension
# - `float`: The primitive data type of the vectors. Can be `float`/`float32` or `double`/`float64`
db_obj.create_table("my_table", {"c1": {"type": "vector,128,float"}}, None)

```

#### Create a table with a sparse vector column only

```python
from infinity.common import ConflictType
# Create a table with a vector column only:  
# - `sparse`: The column is a sparse vector column
# - `128`: The sparse vector dimension
# - `float`: The primitive data type of the sparse vectors. Can be `float`/`float32` or `double`/`float64`
# - `int`: The data type of the sparse vector indices. Can be `int8`, `int16`, `int`/`int32`/`integer`, or `int64`
db_obj.create_table("my_table", {"c1": {"type": "sparse,128,float,int"}}, ConflictType.Error)
```

#### Create a table with a tensor column only

```python
from infinity.common import ConflictType
# Create a table with a tensor column only:  
# - `tensor`: The column is a tensor column
# - `4`: Dimension of each vector unit in the tensor
# - `float64`: The primitive data type of the tensors. Can be `float`/`float32` or `double`/`float64`
db_obj.create_table("my_table", {"c1": {"type": "tensor,4,float64"}}, ConflictType.Ignore)
```

#### Create a table with a tensor array column only

```python
from infinity.common import ConflictType
# Create a table with a tensor array column only:  
# - `tensorarray`: The column is a tensor array column
# - `6`: Dimension of each vector unit in the tensor arrays
# - `float`: The primitive data type of the tensor arrays. Can be `float`/`float32` or `double`/`float64`
db_obj.create_table("my_table", {"c1": {"type": "tensorarray,6,float"}}, ConflictType.Ignore)
```

---

## drop_table

```python
Database.drop_table(table_name, conflict_type = ConflictType.Error)
```

Deletes a table from the database by its name.

### Parameters

#### table_name: `str`, *Required* 

Name of the table to delete. Must not be empty. 

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a table with the specified name does not exist. 

- `Error`: Raise an error if the specified table does not exist.
- `Ignore`: Ignore the operation and proceed regardless, if the specified table does not exist.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 

### Examples

```python
# Delete a table named 'my_table':
# If the specified table does not exist, raise an error. 
db_obj.drop_table("my_table")
```

```python
# Delete a table named 'my_table':
# If the specified table does not exist, raise an error (same as above). 
db_obj.drop_table("my_table", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Delete a table named 'my_table':
# If the specified table does not exist, silently ignore the operation and proceed.
db_obj.drop_table("my_table", ConflictType.Ignore)
```

---

## get_table

```python
Database.get_table(table_name)
```

Retrieves a table object by its name.

### Parameters

#### `table_name`: `str` *Required*

Name of the table to retrieve. Must not be empty. 

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_infinity.table.RemoteTable` object in client-server mode. 
- Failure: `InfinityException`:
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.

### Examples

```python
table_obj = db_obj.get_table("my_table")
```

---

## list_tables

```python
Database.list_tables()
```

Lists the names of all tables in the current database.

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 
- `table_names`: `list[str]` A list of table names. 

### Examples

```python
res = db_obj.list_tables()
res.table_names # ['my_table, 'tensor_table', 'sparse_table']
```

---

## create_index

```python
Table.create_index(index_name, index_infos, conflict_type = ConflictType.Error)
```

Creates index on a specified column or on multiple columns.  

:::danger NOTE
For now, it is only possible to create a full-text index on multiple columns.
:::

### Parameters

#### index_name: `str` *Required*

The name of the index. `index_name` requirements: 

- Maximum 65,535 characters.
- Must not be empty.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Allowed characters: 
  - English letters (a-z, A-Z)
  - Digits (0-9)
  - "_" (underscore)

#### index_infos: `list[IndexInfo()]`, *Required*

An `IndexInfo` structure contains three fields,`column_name`, `index_type`, and `index_param_list`.
    
- **column_name**: `str`, *Required*  
  The name of the column to build index on. Must not be empty. 
- **index_type**: `IndexType`, *Required*  
  Index type. You may want to import `infinity.index` to set `IndexType`: `from infinity.index import IndexType`  
  - `Hnsw`: An HNSW index. 
  - `EMVB`: An EMVB index. Works with tensors only.
  - `FullText`: A full-text index.  
  - `IVFFlat`: An IVFFlat index. 
  - `Secondary`: A secondary index. Works with structured data only. 
  - `BMP`: A Block-Max Pruning index. Works with sparse vectors only. 
- **index_param_list**: `list[InitParameter(str, str)]`  
  A list of `InitParameter` objects specifying parameter settings for the chosen index type. Each object handles one parameter setting. To set a specific index parameter, pass the parameter name and its corresponding value as two separate strings to the `InitParameter` object: 
  - Parameter settings for an HNSW index: 
    - `"M"`: *Optional* - Defaults to`"16"`.
    - `"ef_construction"`: *Optional* - Defaults to`"50"`.
    - `"ef"`: *Optional* - Defaults to `"50"`. 
    - `"metric"` *Required* - The distance metric to use in similarity search.
      - `"ip"`: Inner product.
      - `"l2"`: Euclidean distance.
      - `"cosine"`: Cosine similarity. 
    - `"encode"`: *Optional*
      - `"plain"`: (Default) Plain encoding. 
      - `"lvq"`: Locally-adaptive vector quantization. Works with float vector element only.  
  - Parameter settings for an EMVB index: 
    - `"pq_subspace_num"`: *Required*
      - `"8"` 
      - `"16"` (recommended) 
      - `"32"`
      - `"64"`
      - `"128"`
    - `"pq_subspace_bits"`: *Required*
      - `"8"` (recommended)
      - `"16"`
  - Parameter settings for a full-text index: 
    - `"ANALYZER"`: *Optional* 
      - `"standard"`: (Default) Standard analyzer, segmented by tokens, lowercase processing, provides stemming outputs.
      - `"chinese"`: Simplified Chinese
      - `"tradition"`: Traditional Chinese
      - `"japanese"`: Japanese
      - `"ngram"`: [N-gram](https://en.wikipedia.org/wiki/N-gram)
  - Parameter settings for an IVFFlat index:  
    - `"centroids_count"`: *Optional* - Defaults to`"128"` 
    - `"metric"`: *Required - The distance metric to use in similarity search.
      - `"ip"`: Inner product.
      - `"l2"`: Euclidean distance.
      - `"cosine"`: Cosine similarity. 
  - Parameter settings for a secondary index:  
    No parameters are required. For now, use an empty list `[]`.
  - Parameter settings for a BMP index: 
    - `block_size`: *Optional* - The size of the block in a BMP index. Range: `"1"` ~ `"256"`. Defaults to `"16"`.
    - `"compress_type"`: *Optional*  
      - `"compress"`: (Default) Store the block-max index in sparse format. Works best with small block size situations.
      - `"raw"`: Store the block-max index without compression.

:::tip NOTE
- Import the `infinity.index` package to set `IndexInfo`, `IndexType`, and `InitParameter`.
- Import the `infinity` package to set `index.IndexInfo()`.

```python
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
```
:::


#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where an index with the same name exists.

- `Error`: Raise an error if an index with the same name exists.
- `Ignore`: Ignore the index creation requrest and keep the table with the same name as-is.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

A structure containing these attributes:

  - `error_code`: `int` An error code indicating the result of the operation.  
    - `0`: The operation succeeds.  
    - A non-zero value: A specific error condition occurs.  
  - `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds.  

### Examples


#### Create an HNSW index

```python {1-4}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_hnsw" with a 1024-dimensional float vector column "c1"
table_obj = db_obj.create_table("test_index_hnsw", {"c1": {"type": "vector,1024,float"}}, None)
# Create an HNSW index named "my_index" on column "c1" with default parameter settings:
# - "M": "16", 
# - "ef_construction": "50",
# - "ef": "50", 
# - "encode": "plain"
# Only the "metric" parameter (required) is explicitly set to L2 distance. 
table_obj.create_index("my_index",[IndexInfo("c1", IndexType.Hnsw, [InitParameter("metric", "l2")])], None)
```

```python {1-4}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_hnsw" with a 1024-dimensional float vector column "c1"
table_obj = db_obj.create_table("test_index_hnsw", {"c1": {"type": "vector,1024,float"}}, None)
# Create an HNSW index named "my_index" on column "c1"
# Settings for "M", "ef_construction", "ef", and "metric" are the same as above, except:
# "encoding" is set to "lvq" 
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1",
            IndexType.Hnsw,
            [
                InitParameter("M", "16"),
                InitParameter("ef_construction", "50"),
                InitParameter("ef", "50"),
                InitParameter("metric", "l2")
                InitParameter("encode", "lvq") # "lvq" applies to float vector element only
            ]
        )
    ],
    None
)
```

#### Create a full-text index

```python {15}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_fulltext" with a varchar column "body"
table_obj = db_obj.create_table("test_index_fulltext", {"body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on column "body" with default parameter settings:
# - "ANALYZER": "standard"
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "body", 
            IndexType.FullText, 
            []
        ),
    ],
    None
)
```

```python {16}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_fulltext" with a varchar column "body"
table_obj = db_obj.create_table("test_index_fulltext", {"body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on column "body"
# Setting "ANALYZER" to "standard" (same as the above)
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "body", 
            IndexType.FullText, 
            [
                InitParameter("ANALYZER", "standard")
            ]
        ),
    ],
    None
)
```

```python {14-16}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# In the following code snippet, you will see an index built on three columns
# IMPORTANT: For now, multi-column index works with full-text index ONLY. 
# Create a table named "test_index_fulltext" with three varchar columns "doctitle", "docdate", and "body"
table_obj = db_obj.create_table("test_index_fulltext", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on three columns "doctitle", "docdate", and "body" with default parameter settings:
# - "ANALYZER": "standard"
table_obj.create_index(
    "my_index",
    [
        IndexInfo("doctitle", IndexType.FullText, []),
        IndexInfo("docdate", IndexType.FullText, []),
        IndexInfo("body", IndexType.FullText, []),
    ],
    None
)
```


#### Create an IVFFlat index

```python {17}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_ivfflat" with a vector column "c1"
table_ojbect = db_obj.create_table("test_index_ivfflat", {"c1": {"type": "vector,1024,float"}}, None)
# Create an IVFFlat index named "my_index" on column "c1" with default parameter settings:
# - "centroids_count": "128"
# Only the metric parameter (required) is explicitly set to L2 distance. 
table_obj.create_index(
    "my_index",
        [
        IndexInfo(
            "c1",
            IndexType.IVFFlat,
            [
                InitParameter("metric", "l2")
            ]
        )
        ],
    None
)
```

```python {16,17}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_ivfflat" with a vector column "c1"
table_ojbect = db_obj.create_table("test_index_ivfflat", {"c1": {"type": "vector,1024,float"}}, None)
# Create an IVFFlat index named "my_index" on column "c1"
# Explicitly settings "centroids_count" to "128" and "metric" to "l2" (same as above)
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1",
            IndexType.IVFFlat,
            [
                index.InitParameter("centroids_count", "128"),
                index.InitParameter("metric", "l2")
            ]
        )
    ],
    None
)
```

#### Create a secondary index

```python {14}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_secondary" with a varchar column "body"
table_obj = db_obj.create_table("test_index_secondary", {"c1": {"type": "varchar"}}, None)
# Create a secondary index named "my_index" on column "c1"
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1", 
            IndexType.Secondary, 
            []
        ),
    ],
    None
)
```

#### Create a BMP index

```python {16}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_bmp" with a sparse vector column "c1"
table_obj = db_obj.create_table("test_index_bmp", {"c1": {"type": "sparse,30000,float,int16"}}, None)
# Create a BMP index named "my_index" on column "c1" with default parameter settings:
# - "block_size": "16"
# - "compress_type": "compress"
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1",
            IndexType.BMP,
            []
        )
    ],
    None
)
```


```python {16,17}
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
# Create a table named "test_index_bmp" with a sparse vector column "c1"
table_obj = db_obj.create_table("test_index_bmp", {"c1": {"type": "sparse,30000,float,int16"}}, None)
# Create a BMP index named "my_index" on column "c1"
# Settings for "block_size" and "compress_type" are the same as above
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1",
            IndexType.BMP,
            [
                InitParameter("block_size", "16"),
                InitParameter("compress_type", "compress")
            ]
        )
    ],
    None
)
```

---

## drop_index

```python
Table.drop_index(index_name, conflict_type = ConflictType.Error)
```

Deletes an index by its name.

### Parameters

#### index_name: `str`, *Required*

The name of the index to delete.

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a specified index does not exist.

- `Error`: Raise an error if an index with the specified name does not exist.
- `Ignore`: Ignore the index creation requrest and keep the index with the same name as-is.

:::tip NOTE
You may want to import the `infinity.common` package to set `ConflictType`:

```python
from infinity.common import ConflictType
```
:::

:::tip NOTE
If `ConflictType` is not set, it defaults to `Error`.
:::

### Returns

A structure containing these attributes:

- `error_code`: `int` An error code indicating the result of the operation.  
  - `0`: The operation succeeds.  
  - A non-zero value: A specific error condition occurs.  
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds.  


### Examples

```python
table_obj.drop_index("my_index")
```

---

## show_index

```python
Table.show_index(index_name)
```

Retrieves the metadata of an specified index.

### Parameters

#### index_name: `str`, *Required*

The name of the index to look up.

### Returns

- Success: A structure containing the following attributes:
    - `error_code`: `int` - `0` indicating that the operation succeeds.
    - `error_msg`: `str` - An empty string.
    - `db_name`: `str` - The database name.
    - `table_name`: `str` - The table name.
    - `index_name`: `str` - The index name.
    - `index_type`: `str` - The index type.
    - `index_column_names`: `str` - Names of the columns for building the index.
    - `index_column_ids`: `str` - IDs of the columns for building the index.
    - `other_parameters`: `str` - Parameters necessary for creating the index.
    - `store_dir`: `str` - The directory holding the index files.
    - `segment_index_count`: `str` - Number of segments of the index.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value: A specific error condition occurs.
  - `error_msg`: `str` The error message providing additional details.

### Examples

```python
from infinity.index import IndexInfo
from infinity.index import IndexType
from infinity.index import InitParameter
from infinity import index
table_obj.create_index(
    "my_index",
    [
        IndexInfo(
            "c1",  
            IndexType.IVFFlat,
            [
                InitParameter("centroids_count", "128"),
                InitParameter("metric", "l2")
            ]
        )
    ], 
    ConflictType.Error
)
res = table_obj.show_index("my_index")
print(res)
```

---

## list_indexes

```python
table_obj.list_indexes()
```

Lists the indexes built on the current table.

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs.
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 
- `table_names`: `list[str]` A list of index names.

### Examples

```python
res = table_obj.list_indexes()
res.index_names # ['my_index', 'tensor_index', 'sparse_index']
```

---

## insert

```python
table_obj.insert(data)
```

Inserts rows of data into the current table. 

### Parameters

#### data: `dict[str, Any]`, *Required*  
Data to insert. Infinity supports inserting multiple rows to a table at one time in the form of `dict[str, Any]` (one row) or `list[dict[str, Any]]` (multiple rows), with each key-value pair corresponding to a column name and table cell value.

:::tip NOTE
Batch row limit: 8,192. You are allowed to insert a maximum of 8,192 rows at once. 
:::

:::tip NOTE
When inserting incomplete rows of data, ensure that all uninserted columns have default values when calling `create_table`. Otherwise, an error will occur.  
For information about setting default column values, see `create_table`.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 

### Examples

#### Insert primitives

```python
# Create a table with four primitive columns:
table_instance = db_instance.create_table("primitive_table", {
    "c1": {"type": "int8", "default": 0},
    "c2": {"type": "int16", "default": 0},
    "c3": {"type": "int", "default": 0},
    "c4": {"type": "int32", "default": 0},   # Same as int
    "c5": {"type": "integer", "default": 0}, # Same as int
    "c6": {"type": "int64", "default": 0},
    "c7": {"type": "varchar"},
    "c8": {"type": "float", "default": 1.0},
    "c9": {"type": "float32", "default": 1.0}, # Same as float
    "c10": {"type": "double", "default": 1.0},
    "c11": {"type": "float64", "default": 1.0}, # Same as double
    "c12": {"type": "bool", "default": False},
})

# Insert an incomplete row, with remaining cells defaulting to their column defaults:
table_instance.insert({"c1": 1, "c7": "Tom", "c12": True})
```

#### Insert vectors

```python
# Create a table with a integer column and a 3-d vector column:
table_obj = db_obj.create_table("vector_table", {"c1": {"type": "integer", "default": 2024}, "vector_column": {"type": "vector,3,float"}})

# Insert one incomplete row into the table:
# Note that the 'c1' cell defaults to 0. 
table_obj.insert({"vector_column": [1.1, 2.2, 3.3]})

# Insert two incomplete rows into the table:
# Note that the 'c1' cells default to 0. 
table_obj.insert([{"vector_column": [1.1, 2.2, 3.3]}, {"vector_column": [4.4, 5.5, 6.6]}])
```

#### Insert sparse vectors

```python
# Create a table with a integer column and a 100-d sparse vector column:
table_obj = db_obj.create_table("sparse_vector_table", {"c1": {"type": "integer"}, "sparse_column": {"type": "sparse,100,float,int"}})

# Insert one row into the table:
# `indices` specifies the correspoing indices to the values in `values`.
# Note that the second row sets "c1" as 2024 by default. 
table_obj.insert([{"c1": 2022, "sparse_column": {"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]}, {"sparse_column":  {"indices": [70, 80, 90], "values": [7.7, 8.8, 9.9]}}}])
```

#### Insert tensors

```python
# Create a table with a tensor column: 
table_obj = db_obj.create_table("tensor_table", {"c1": {"type": "integer", "default": 2024}, "tensor_column": {"type": "tensor,4,float"}})

# Insert one row into the table:
table_instance.insert([{"tensor_column": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]}])
```

#### Insert tensor arrays

```python
# Creat a table with only one tensor array column:
table_obj = db_obj.create_table("tensor_array_table", {"tensor_array_column": {"type": "tensorarray,2,float"}})
table_obj.insert([{"tensor_array_column": [[[1.0, 2.0], [3.0, 4.0]], [[5.0, 6.0]]]}])
```

---

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

#### import_options: `dict[str, bool | str]`

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

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 

### Examples

#### Import a csv file

```python
table_obj.import_data(os.getcwd() + "/your_file.csv", {"header": False, "file_type": "csv", "delimiter": "\t"})
```

#### Import a jsonl file

```python
table_obj.import_data(os.getcwd() + "/your_file.jsonl", {"file_type": "csv"})
```

---

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
  
#### export_options: `dict[str, Any]`

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

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds. 
  - A non-zero value: A specific error condition occurs. 
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds. 

### Examples

#### Export your table to a csv file

```python
table_obj.export_data(os.getcwd() + "/export_data.csv", {"header": True, "file_type": "csv", "delimiter": ",", "offset": 2, "limit": 7, "row_limit": 3}, ["num", "name", "score"])
```

#### Export your table to a jsonl file

```python
table_obj.export_data(os.getcwd() + "/export_data.jsonl", {"file_type": "jsonl", "offset": 1, "limit": 8, "row_limit": 2}, ["num", "name", "score"])
```

---

## delete

```python
Table.delete(cond = None)
```

Deletes rows by condition.

### Parameters

#### cond: `str`, *Optional*

A condition or filter that determines which rows to delete from the table. The parameter can be an expression, a function, or any other form of conditional logic that evaluates to `True` for the rows that should be deleted. If `cond` is not specified or set to `None`, the method will delete all rows in the table.

:::tip NOTE
The `cond` parameter currently supports 'and' and 'or' logical expressions only. Additional expressions like 'between' and 'in' will be available soon.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value: A specific error condition occurs.
- `error_msg`: `str` The error message providing additional details. It is an empty string if the operation succeeds.

### Examples

#### Remove all rows in the table

```python
# Clear all data in the current table
table_obj.delete()
```

```python
# Clear all data in the current table
table_obj.delete(None)

```

#### Conditional row deletion

```python
# Create a table named "my_table" with two columns:
# - Integer column "c1"
# - Vector column "vec"
table_obj = db_instance.create_table("my_table", {"c1": {"type": "integer"}, "vec": {"type": "vector,4,float"},})
# Insert two rows of data into the "my_table"
table_obj.insert(
[
        {
            "c1": 1,
            "vec": [1.0, 1.2, 0.8, 0.9],
        },
        {
            "c1": 2,
            "vec": [4.0, 4.2, 4.3, 4.5],
        },
    ]
)
# Delete rows whose "c1" equal 1
table_obj.delete("c1 = 1")
```



---

## update

```python
Table.update(cond = None)
```

Searches for rows that match the specified condition and update them accordingly.

### Parameters

#### cond: `str` 

Must not be empty.

#### data: `list[dict[str, Union[str, int, float]]]`

A list of dict where key indicates column, value indicates new value. Must not be empty.

> Infinity does not support updating column with vector data type.

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
table_obj.update("c1 > 2", [{"c2": 100, "c3": 1000}])
```

---

## output

```python
Table.output(columns)
```
Specify the columns to display in the search output, or perform aggregation operations or arithmetic calculations. 

### Parameters

#### columns: `list[str]`, *Required* 

Must not be empty. Supported aggragation functions:

- count
- min
- max
- sum
- avg
  
### Returns

- Success: self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.output(["*"])
table_obj.output(["num", "body"])
table_obj.output(["_row_id"])
table_obj.output(["avg(c2)"])
table_obj.output(["c1+5"])
```

---

## filter

```python
Table.filter(cond)
```

Creates a filtering condition expression.

### Parameters

#### cond: `str`
`cond` has only supported 'and' and 'or' conjunction expression for now. 

### Returns

- Success: self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.filter("(-7 < c1 or 9 >= c1) and (c2 = 3)")
```

---

## knn

```python
Table.knn(vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params = None)
```

Builds a KNN search expression. Find the top n closet rows to the given vector.

### Parameters

#### vector_column_name: `str`, *Required*

The name of the vector column to search.

#### embedding_data: `list/np.ndarray`, *Required*

#### embedding_data_type: `str`, *Required*

#### distance_type: `str`, *Required*

The distance metric to use in similarity search.

- `"ip"`: Inner product.
- `"l2"`: Euclidean distance.
- `"cosine"`: Cosine similarity. 

#### topn: `int`

#### knn_params: `list`, *Optional*

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples

```python
table_obj.knn('col1', [0.1,0.2,0.3], 'float', 'l2', 100)
table_obj.knn('vec', [3.0] * 5, 'float', 'ip', 2)
```

---

## match sparse

```python
Table.match_sparse(vector_column_name, sparse_data, distance_type, topn, opt_params = None)
```

### Parameters

#### vector_column_name: `str`

#### sparse_data: `{"indices": list[int], "values": Union(list[int], list[float])}`

#### distance_type: `str`
  -  `'ip'`

#### topn: `int`
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

---

## match

Creates a full-text search expression.

### Parameters

#### fields: `str`

The column where text is searched, and has create full-text index on it before. 

#### matching_text: `str`

#### options_text: `str`
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
    r'title:(quick OR brown) AND body:foobar', # search `(quick OR brown)` in the `title` field. keep fields empty.
]
for question in questions:
    table_obj.match('body', question, 'topn=2')
```

---

## match tensor

```python
Table.match_tensor(vector_column_name, tensor_data, tensor_data_type, method_type, topn, extra_option)
```

Builds a KNN tensor search expression. Find the top n closet rows to the given tensor according to chosen method.

For example, find k most match tensors generated by ColBERT.

### Parameters

#### vector_column_name: `str`


#### tensor_data: `list/np.ndarray`


#### tensor_data_type: `str`


#### method_type: `str`

-  `'maxsim'`

#### extra_option: `str` 

Options seperated by ';'
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

---

## fusion

```python
Table.fusion(method, options_text = '')
```

Builds a fusion expression.

### Parameters

#### method: `str`

The supported methods, including:

- `"rrf"`
- `"weighted_sum"`
- `"match_tensor"`

#### options_text: `str`

- `Common options`:
    - 'topn=10': Retrieve the 10 most relevant rows. The defualt value is `100`.

- `rrf-specific options`:
    - 'rank_constant=30': The default value is `60`.

- `weighted_sum-specific options`:
    - 'weights=1,2,0.5': The weights of children scorers. The default weight of each weight is `1.0`.

### Returns

- Success: Self `Table`
- Failure: `Exception`

### Examples

:::caution IMPORTANT
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

---

## optimize

```python
Table.optimize(index_name, opt_params)
```

### Parameters

#### index_name: `str`

#### opt_params: `dict[str, str]`
    
#### Common options: `str`
  - 'topk=10': Optimize the BMP index for top 10. Used only when the index is BMP.

### Returns

- Success: `True`
- Failure: `Exception`

### Examples

```python
table_obj.optimize('bmp_index_name', {'topk': '10'})
```

---

## get result

```python
Table.to_result()
```

```python
Table.to_df()
```

```python
Table.to_pl()
```

```python
Table.to_arrow()
```

After querying, these four methods above can get result into specific type. 
`Note: output method must be executed before get result`

### Returns

- **to_result() : tuple[dict[str, list[Any]], dict[str, Any]]**
Python's built-in type
- **to_df() : pandas.DataFrame**
- **to_pl() : polars.DataFrame**
- **to_arrow() : pyarrow.Table**

### Examples

```python
res = table_obj.output(['c1', 'c1']).to_df()

res = table_obj.output(['*'])
               .knn('vec', [3.0, 2.8, 2.7, 3.1], 'float', 'ip', 1)
               .match('doctitle, num, body', 'word', match_param_3)
               .fusion('rrf')
               .to_pl()
```

---
