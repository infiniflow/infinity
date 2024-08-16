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

- `"/path/to/save/to"`: `str` - A local directory storing the Infinity data. Used when Infinity is installed as a Python module.
- `NetworkAddress`: Used in client-server mode, when you have deployed Infinity as a separate server and wish to connect to it remotely. A `NetworkAddress` object comprises two fields:
  - `"<SERVER_IP_ADDRESS>"`: `str` - The IP address of the Infinity server.  
  - `<PORT>`: `int` - The SDK port number on which the Infinity server listens. Defaults to `23817`.

:::caution IMPORTANT
When connecting to Infinity in client-server mode, ensure that the client version *exactly* matches the server version. For example:

| **Client version** | **Server version** |
| ------------------ | ------------------ |
| v0.1.0             | v0.1.0             |
| v0.1.1             | v0.1.1             |
| v0.2.0             | v0.2.0             |
| v0.2.1             | v0.2.1             |

If the versions do not match, please update your client or server to ensure compatibility.

In client-server mode, also ensure that your server version matches the version specified in your configuration file. Here, the matching rule is less strict than an exact match:

- The major and minor versions *must* be identical.
- The patch version may differ.

This allows for bug fixes without requiring changes to the configuration file.

| **Configuration version** | **Compatible server version** |
| ------------------------- | ----------------------------- |
| v0.1.0                    | v0.1.0, v0.1.1                |
| v0.2.0                    | v0.2.0, v0.2.1                |

:::

### Returns

- Success: An `infinity.local_infinity.infinity.LocalInfinityConnection` object in Python module mode or an `infinity.remote_thrift.infinity.RemoteThriftInfinityConnection` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` - A non-zero value indicating a specific error condition.
  - `error_msg`: `str` - A message providing additional details about the error.

### Examples

#### Connect to Python module Infinity

From v0.2.1 onwards, Infinity also gives you the option to connect to the Infinity service just like calling a Python module. If you have installed Infinity via `pip install infinity-sdk==<v0.2.1_OR_HIGHER>`, you can connect to Infinity and save all related data in a local directory:

```python
import infinity
infinity_object = infinity.connect("/path/to/save/to")
```

#### Connect to Infinity in client-server mode

If you have deployed Infinity as a separate server, connect to it via its IP address. If your Infinity is running on your local machine, you can also use `infinity.common.LOCAL_HOST` to replace `"<SERVER_IP_ADDRESS>"` in the following code snippet.

```python
import infinity
# If Infinity is deployed on the local machine, use infinity.LOCAL_HOST to replace <SERVER_IP_ADDRESS>
infinity_object = infinity.connect(infinity.NetworkAddress("<SERVER_IP_ADDRESS>", 23817)) 
```

---

## disconnect

```python
infinity_object.disconnect()
```

Disconnects the client from the Infinity server in client-server mode or destructs the Infinity object and releases all associated resources when Infinity is installed as a Python module.

### Returns

A structure containing the following attributes:

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

```python
infinity_object.disconnect()
```

---

## create_database

```python
infinity_object.create_database(db_name, conflict_type = ConflictType.Error)
```

Creates a database with a specified name.

### Parameters

#### db_name: `str`, *Required*

A non-empty string indicating the name of the database, which must adhere to the following requirements:

- Maximum 65,535 characters.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Permitted characters include:
  - English letters (a-z, A-Z)
  - Digits (0-9)
  - "_" (underscore)

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a database with the same name exists.

- `Error`: Raise an error if a database with the same name exists.
- `Ignore`: Ignore the database creation requrest and keep the existing database with the same name.

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
  - `error_code`: `int` - A non-zero value indicating a specific error condition.
  - `error_msg`: `str` - A message providing additional details about the error.

### Examples

```python
# Create a database named 'my_database':
# If the specified database already exists, raise an error. 
infinity_object.create_database("my_database")
```

```python
# Create a database named 'my_database':
# If the specified database already exists, raise an error (same as above). 
infinity_object.create_database("my_database", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Create a database named 'my_database':
# If the specified database already exists, silently ignore the operation and proceed. 
infinity_object.create_database("my_database", ConflictType.Ignore)
```

---

## drop_database

```python
infinity_object.drop_database(db_name, conflict_type = ConflictType.Error)
```

Deletes a database by its name.

### Parameters

#### db_name: `str`, *Required*

A non-empty string indicating the name of the database to delete.

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

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

```python
# Delete a database named 'my_database':
# If the specified database does not exist, raise an error. 
infinity_object.drop_database("my_database")
```

```python
# Delete a database named 'my_database':
# If the specified database does not exist, raise an error (same as above). 
infinity_object.drop_database("my_database", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Delete a database named 'my_database':
# If the specified database does not exist, silently ignore the operation and proceed.
infinity_object.drop_database("my_database", ConflictType.Ignore)
```

---

## list_databases

```python
Infinity.list_databases()
```

Retrieves a list of all available databases within the Infinity system.

### Returns

A structure containing the following attributes:

- `db_names`: `list[str]` A list of strings indicating the names of all available databases.
- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` A message providing additional details about the error.

### Examples

```python
res = infinity_object.list_databases() 
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

A non-empty string indicating the name of the database to retrieve.

### Returns

- Success: An `infinity.local_infinity.db.LocalDatabase` object in Python module mode or an `infinity.remote_thrift.db.RemoteDatabase` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` - A non-zero value indicating a specific error condition.
  - `error_msg`: `str` - A message providing additional details about the error.

### Examples

```python
db_object = infinity_object.get_database("my_database")
```

---

## create_table

```python
db_object.create_table(table_name, columns_definition, conflict_type = ConflictType.Error)
```

Creates a table with a specified name and defined columns.

### Parameters

#### table_name: `str`, *Required*

A non-empty string indicating the name of the table, which must adhere to the following requirements:

- Maximum 65,535 characters.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Permitted characters include:
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
- `Ignore`: Ignore the table creation requrest and keep the existing table with the same name.

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
  - `error_code`: `int` - A non-zero value indicating a specific error condition.
  - `error_msg`: `str` - A message providing additional details about the error.

### Examples

#### Create a table with an integer column only

```python
# The `create_table`method supports creating integer columns in the following data types:
# - int8
# - int16
# - int/int32/integer
# - int64
db_object.create_table("my_table", {"c1": {"type": "int", "default": 1}})
```

#### Create a table with a float column only

```python
# The `create_table`method supports creating float columns in the following data types:
# - float/float32
# - double/float64
db_object.create_table("my_table", {"c1": {"type": "float64"}})
```

#### Create a table with a string column only

```python
db_object.create_table("my_table", {"c1": {"type": "varchar"}})
```

#### Create a table with a bool column only

```python
db_object.create_table("my_table", {"c1": {"type": "bool"}})
```

#### Create a table with a vector column only

```python
# Create a table with a vector column only:  
# - `vector`: The column is a vector column
# - `128`: The vector dimension
# - `float`: The primitive data type of the vectors. Can be `float`/`float32` or `double`/`float64`
db_object.create_table("my_table", {"c1": {"type": "vector,128,float"}}, None)

```

#### Create a table with a sparse vector column only

```python
from infinity.common import ConflictType
# Create a table with a vector column only:  
# - `sparse`: The column is a sparse vector column
# - `128`: The sparse vector dimension
# - `float`: The primitive data type of the sparse vectors. Can be `float`/`float32` or `double`/`float64`
# - `int`: The data type of the sparse vector indices. Can be `int8`, `int16`, `int`/`int32`/`integer`, or `int64`
db_object.create_table("my_table", {"c1": {"type": "sparse,128,float,int"}}, ConflictType.Error)
```

#### Create a table with a tensor column only

```python
from infinity.common import ConflictType
# Create a table with a tensor column only:  
# - `tensor`: The column is a tensor column
# - `4`: Dimension of each vector unit in the tensor
# - `float64`: The primitive data type of the tensors. Can be `float`/`float32` or `double`/`float64`
db_object.create_table("my_table", {"c1": {"type": "tensor,4,float64"}}, ConflictType.Ignore)
```

#### Create a table with a tensor array column only

```python
from infinity.common import ConflictType
# Create a table with a tensor array column only:  
# - `tensorarray`: The column is a tensor array column
# - `6`: Dimension of each vector unit in the tensor arrays
# - `float`: The primitive data type of the tensor arrays. Can be `float`/`float32` or `double`/`float64`
db_object.create_table("my_table", {"c1": {"type": "tensorarray,6,float"}}, ConflictType.Ignore)
```

---

## drop_table

```python
db_object.drop_table(table_name, conflict_type = ConflictType.Error)
```

Deletes a table from the database by its name.

### Parameters

#### table_name: `str`, *Required*

A non-empty string indicating the name of the table to delete.

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

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

```python
# Delete a table named 'my_table':
# If the specified table does not exist, raise an error. 
db_object.drop_table("my_table")
```

```python
# Delete a table named 'my_table':
# If the specified table does not exist, raise an error (same as above). 
db_object.drop_table("my_table", infinity.common.ConflictType.Error)
```

```python
from infinity.common import ConflictType
# Delete a table named 'my_table':
# If the specified table does not exist, silently ignore the operation and proceed.
db_object.drop_table("my_table", ConflictType.Ignore)
```

---

## get_table

```python
db_object.get_table(table_name)
```

Retrieves a table object by its name.

### Parameters

#### `table_name`: `str`, *Required*

A non-empty string indicating the name of the table to retrieve.

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_infinity.table.RemoteTable` object in client-server mode.
- Failure: `InfinityException`:
  - `error_code`: `int` - A non-zero value indicating a specific error condition.
  - `error_msg`: `str` - A message providing additional details about the error.

### Examples

```python
table_object = db_object.get_table("my_table")
```

---

## list_tables

```python
db_object.list_tables()
```

Retrieves a list of all available tables within the current database.

### Returns

A structure containing the following attributes:

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.
- `table_names`: `list[str]` - A list of strings indicating the names of all available tables in the current database.

### Examples

```python
res = db_object.list_tables()
res.table_names # ['my_table, 'tensor_table', 'sparse_table']
```

---

## create_index

```python
table_object.create_index(index_name, index_info, conflict_type = ConflictType.Error)
```

Creates index on a specified column.

### Parameters

#### index_name: `str` *Required*

A non-empty string indicating the name of the index, which must adhere to the following requirements:

- Maximum 65,535 characters.
- Case-insensitive.
- Must begin with an English letter or underscore.
- Permitted characters include:
  - English letters (a-z, A-Z)
  - Digits (0-9)
  - "_" (underscore)

#### index_info: `IndexInfo()`, *Required*

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
    - `"centroids_count"`: *Optional* - Defaults to`"128"`.
    - `"metric"`: *Required* - The distance metric to use in similarity search.
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
Import the `infinity.index` package to set `IndexInfo`, `IndexType`, and `InitParameter`.

```python
from infinity.index import IndexInfo, IndexType, InitParameter
```

:::

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where an index with the same name exists.

- `Error`: Raise an error if an index with the same name exists.
- `Ignore`: Ignore the index creation requrest and keep the existing table with the same name.

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

- `error_code`: `int` - An error code indicating the result of the operation.  
  - `0`: The operation succeeds.  
  - A non-zero value indicating a specific error condition.  
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.  

### Examples

#### Create an HNSW index

```python {1}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_hnsw" with a 1024-dimensional float vector column "c1"
table_object = db_object.create_table("test_index_hnsw", {"c1": {"type": "vector,1024,float"}}, None)
# Create an HNSW index named "my_index" on column "c1" with default parameter settings:
# - "M": "16", 
# - "ef_construction": "50",
# - "encode": "plain"
# Only the "metric" parameter (required) is explicitly set to L2 distance. 
table_object.create_index("my_index",IndexInfo("c1", IndexType.Hnsw, [InitParameter("metric", "l2")]), None)
```

```python {1}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_hnsw" with a 1024-dimensional float vector column "c1"
table_object = db_object.create_table("test_index_hnsw", {"c1": {"type": "vector,1024,float"}}, None)
# Create an HNSW index named "my_index" on column "c1"
# Settings for "M", "ef_construction", "ef", and "metric" are the same as above, except:
# "encoding" is set to "lvq" 
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1",
            IndexType.Hnsw,
            [
                InitParameter("M", "16"),
                InitParameter("ef_construction", "50"),
                InitParameter("ef", "50"),
                InitParameter("metric", "l2"),
                InitParameter("encode", "lvq") # "lvq" applies to float vector element only
            ]
        ),
    None
)
```

#### Create a full-text index

```python {12}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_fulltext" with a varchar column "body"
table_object = db_object.create_table("test_index_fulltext", {"body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on column "body" with default parameter settings:
# - "ANALYZER": "standard"
table_object.create_index(
    "my_index",
        IndexInfo(
            "body", 
            IndexType.FullText, 
            []
        ),
    None
)
```

```python {13}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_fulltext" with a varchar column "body"
table_object = db_object.create_table("test_index_fulltext", {"body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on column "body"
# Setting "ANALYZER" to "standard" (same as the above)
table_object.create_index(
    "my_index",
        IndexInfo(
            "body", 
            IndexType.FullText, 
            [
                InitParameter("ANALYZER", "standard")
            ]
        ),
    None
)
```

```python {11-13}
from infinity.index import IndexInfo, IndexType, InitParameter
# In the following code snippet, you will see an index built on three columns
# IMPORTANT: For now, multi-column index works with full-text index ONLY. 
# Create a table named "test_index_fulltext" with three varchar columns "doctitle", "docdate", and "body"
table_object = db_object.create_table("test_index_fulltext", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}}, None)
# Create a full-text index named "my_index" on three columns "doctitle", "docdate", and "body" with default parameter settings:
# - "ANALYZER": "standard"
table_object.create_index(
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

```python {14}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_ivfflat" with a vector column "c1"
table_ojbect = db_object.create_table("test_index_ivfflat", {"c1": {"type": "vector,1024,float"}}, None)
# Create an IVFFlat index named "my_index" on column "c1" with default parameter settings:
# - "centroids_count": "128"
# Only the metric parameter (required) is explicitly set to L2 distance. 
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1",
            IndexType.IVFFlat,
            [
                InitParameter("metric", "l2")
            ]
        ),
    None
)
```

```python {13,14}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_ivfflat" with a vector column "c1"
table_ojbect = db_object.create_table("test_index_ivfflat", {"c1": {"type": "vector,1024,float"}}, None)
# Create an IVFFlat index named "my_index" on column "c1"
# Explicitly settings "centroids_count" to "128" and "metric" to "l2" (same as above)
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1",
            IndexType.IVFFlat,
            [
                InitParameter("centroids_count", "128"),
                InitParameter("metric", "l2")
            ]
        ),
    None
)
```

#### Create a secondary index

```python {11}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_secondary" with a varchar column "body"
table_object = db_object.create_table("test_index_secondary", {"c1": {"type": "varchar"}}, None)
# Create a secondary index named "my_index" on column "c1"
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1", 
            IndexType.Secondary, 
            []
        ),
    None
)
```

#### Create a BMP index

```python {13}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_bmp" with a sparse vector column "c1"
table_object = db_object.create_table("test_index_bmp", {"c1": {"type": "sparse,30000,float,int16"}}, None)
# Create a BMP index named "my_index" on column "c1" with default parameter settings:
# - "block_size": "16"
# - "compress_type": "compress"
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1",
            IndexType.BMP,
            []
        ),
    None
)
```

```python {13,14}
from infinity.index import IndexInfo, IndexType, InitParameter
# Create a table named "test_index_bmp" with a sparse vector column "c1"
table_object = db_object.create_table("test_index_bmp", {"c1": {"type": "sparse,30000,float,int16"}}, None)
# Create a BMP index named "my_index" on column "c1"
# Settings for "block_size" and "compress_type" are the same as above
table_object.create_index(
    "my_index",
        IndexInfo(
            "c1",
            IndexType.BMP,
            [
                InitParameter("block_size", "16"),
                InitParameter("compress_type", "compress")
            ]
        ),
    None
)
```

---

## drop_index

```python
table_object.drop_index(index_name, conflict_type = ConflictType.Error)
```

Deletes an index by its name.

### Parameters

#### index_name: `str`, *Required*

A non-empty string indicating the name of the index to delete.

#### conflict_type: `ConflictType`, *Optional*

Conflict policy in `enum` for handling situations where a specified index does not exist.

- `Error`: Raise an error if an index with the specified name does not exist.
- `Ignore`: Ignore the index creation requrest.

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

- `error_code`: `int` - An error code indicating the result of the operation.  
  - `0`: The operation succeeds.  
  - A non-zero value indicating a specific error condition.  
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.  


### Examples

```python
table_object.drop_index("my_index")
```

---

## list_indexes

```python
table_object.list_indexes()
```

Retrieves a list of all available indexes built on the current table.

### Returns

A structure containing the following attributes:

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.
- `table_names`: `list[str]` - A list of strings indicating the names of all available indexes.

### Examples

```python
res = table_object.list_indexes()
res.index_names # ['my_index', 'tensor_index', 'sparse_index']
```

---

## insert

```python
table_object.insert(data)
```

Inserts rows of data into the current table.

### Parameters

#### data: `dict[str, Any]`, *Required*

Data to insert. Infinity supports inserting multiple rows to a table at one time in the form of `dict[str, Any]` (one row) or `list[dict[str, Any]]` (multiple rows), with each key-value pair corresponding to a column name and table cell value.

:::tip NOTE
Batch row limit: 8,192. You are allowed to insert a maximum of 8,192 rows at once.
:::

:::tip NOTE
When inserting incomplete rows of data, ensure that all uninserted columns have default values when calling `create_table()`. Otherwise, an error will occur.  
For information about setting default column values, see `create_table()`.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` - An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` - A message providing additional details about the error. It is an empty string if the operation succeeds.

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
table_object = db_object.create_table("vector_table", {"c1": {"type": "integer", "default": 2024}, "vector_column": {"type": "vector,3,float"}})

# Insert one incomplete row into the table:
# Note that the 'c1' cell defaults to 0. 
table_object.insert({"vector_column": [1.1, 2.2, 3.3]})

# Insert two incomplete rows into the table:
# Note that the 'c1' cells default to 0. 
table_object.insert([{"vector_column": [1.1, 2.2, 3.3]}, {"vector_column": [4.4, 5.5, 6.6]}])
```

#### Insert sparse vectors

```python
# Create a table with a integer column and a 100-d sparse vector column:
table_object = db_object.create_table("sparse_vector_table", {"c1": {"type": "integer"}, "sparse_column": {"type": "sparse,100,float,int"}})

# Insert one row into the table:
# `indices` specifies the correspoing indices to the values in `values`.
# Note that the second row sets "c1" as 2024 by default. 
table_object.insert([{"c1": 2022, "sparse_column": {"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]}, {"sparse_column":  {"indices": [70, 80, 90], "values": [7.7, 8.8, 9.9]}}}])
```

#### Insert tensors

```python
# Create a table with a tensor column: 
table_object = db_object.create_table("tensor_table", {"c1": {"type": "integer", "default": 2024}, "tensor_column": {"type": "tensor,4,float"}})

# Insert one row into the table:
table_instance.insert([{"tensor_column": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]}])
```

#### Insert tensor arrays

```python
# Creat a table with only one tensor array column:
table_object = db_object.create_table("tensor_array_table", {"tensor_array_column": {"type": "tensorarray,2,float"}})
table_object.insert([{"tensor_array_column": [[[1.0, 2.0], [3.0, 4.0]], [[5.0, 6.0]]]}])
```

---

## import_data

```python
table_object.import_data(filepath, import_options)
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
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

#### Import a csv file

```python
table_object.import_data(os.getcwd() + "/your_file.csv", {"header": False, "file_type": "csv", "delimiter": "\t"})
```

#### Import a jsonl file

```python
table_object.import_data(os.getcwd() + "/your_file.jsonl", {"file_type": "csv"})
```

---

## export_data

```python
table_object.export_data(filepath, export_options, columns = None)
```

Exports the current table to a specified file.

### Parameters

#### file_path: `str`, *Required*

Absolute path to the file for export. Supported file types include:

- `csv`
- `jsonl`
  
#### export_options: `dict[str, Any]`, *Required*

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
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

#### Export your table to a csv file

```python
table_object.export_data(os.getcwd() + "/export_data.csv", {"header": True, "file_type": "csv", "delimiter": ",", "offset": 2, "limit": 7, "row_limit": 3}, ["num", "name", "score"])
```

#### Export your table to a jsonl file

```python
table_object.export_data(os.getcwd() + "/export_data.jsonl", {"file_type": "jsonl", "offset": 1, "limit": 8, "row_limit": 2}, ["num", "name", "score"])
```

---

## delete

```python
table_object.delete(cond = None)
```

Deletes rows from the table based on the specified condition.

### Parameters

#### cond: `str` (non-empty), *Optional*

A condition or filter that determines which rows to delete from the table. The parameter can be an expression, a function, or any other form of conditional logic that evaluates to `True` for the rows that should be deleted. If `cond` is not specified or set to `None`, the method will delete all rows in the table.

:::tip NOTE

- The `cond` parameter currently supports 'and' and 'or' logical expressions only.
- `cond` must not be an empty string.
:::

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

#### Remove all rows in the table

```python
# Clear all data in the current table
table_object.delete()
```

```python
# Clear all data in the current table
table_object.delete(None)

```

#### Conditional row deletion

```python
# Create a table named "my_table" with two columns:
# - Integer column "c1"
# - Vector column "vec"
table_object = db_instance.create_table("my_table", {"c1": {"type": "integer"}, "vec": {"type": "vector,4,float"},})
# Insert two rows of data into the "my_table"
table_object.insert([{"c1": 90, "vec": [1.0, 1.2, 0.8, 0.9],}, {"c1": 80, "vec": [4.0, 4.2, 4.3, 4.5],},])
# Delete rows where "c1" equals 1
table_object.delete("c1 = 90")
```

```python
# Create a table named "my_table" with one integer column "c1"
table_object = db_instance.create_table("my_table", {"c1": {"type": "integer"}})
# Insert three rows of data into the "my_table"
table_object.insert([{"c1": 90}, {"c1": 80}, {"c1": 95}])
# Delete rows where "c1" is between 70 and 90 (inclusive)
table_object.delete("c1 >= 70 and c1 <= 90")
```

---

## update

```python
table_object.update(cond = None, data)
```

Searches for rows that match the specified condition and updates them accordingly.

### Parameters

#### cond: `str`, *Required*

A non-empty string that defines the condition for selecting rows to update. It represents a logical expression, a function, or any other form of conditional logic that evaluates to `True` for the rows that should be updated.

#### data: `list[dict[str, Any]]]` (non-empty), *Required*

A list of dictionaries where each key indicates a column name and each value indicates the new value for the corresponding cell. This list must not be empty.

### Returns

A structure containing the following attributes:

- `error_code`: `int` An error code indicating the result of the operation.
  - `0`: The operation succeeds.
  - A non-zero value indicating a specific error condition.
- `error_msg`: `str` A message providing additional details about the error. It is an empty string if the operation succeeds.

### Examples

```python
# Update rows where column "c1" equals 1, setting "c2" to 90 and "c3" to 900
table_object.update("c1 = 1", [{"c2": 90, "c3": 900}])
```

```python
# Update rows where column "c1" is greater than 2, setting "c2" to 100 and "c3" to 1,000
table_object.update("c1 > 2", [{"c2": 100, "c3": 1000}])
```

---

## output

```python
table_object.output(columns)
```

This method allows you to customize the output of your query by selecting specific columns, applying aggregation functions, or performing arithmetic operations.

### Parameters

#### columns: `list[str]`, *Required*

A non-empty list of strings specifying the columns to include in the output. Each string in the list can represent:

- A user-defined column name: The name of the column to include in the output, e.g., `"body"`.
- All user-defined columns: Use a wildcard `"*"` to select all columns.
- A special system column: system-generated columns include:
  - `_row_id`:  An automatically generated, unique identifier for each row in the table. It serves as a unique key for each row but does not necessarily correspond to the actual row number. When the data in a row is updated, the `_row_id` for that row is also changed to reflect the update.
  - `_score`: A BM25 score used in full-text search.
  - `_similarity`: Used by IP and cosine metric in dense or sparse vector search.
  - `_distance`: Used by L2 metric in dense vector search.
- An aggregation function: Apply an aggregation operation on specified columns. Supported aggragation functions include:
  - `count`
  - `min`
  - `max`
  - `sum`
  - `avg`
- An arithmetic function: Apply an arithmetic operation on specified columns (e.g., `c1+5`).

:::tip NOTE
The list must contain at least one element. Empty lists are not allowed.
:::
  
### Returns

An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.

:::tip NOTE
This method specifies the projection columns for the current table but does not directly produce displayable data. To display the query results, use `output()` in conjunction with methods like `to_result()`, `to_df()`, `to_pl()`, or `to_arrow()` to materialize the data.
:::

### Examples

#### Select columns to display

```python
# Select all columns
table_object.output(["*"]).to_pl()
```

```python
# Select columns "num" and "body"
table_object.output(["num", "body"]).to_df()
```

```python
# Select a system-generated column "_row_id"
table_object.output(["_row_id"]).to_pl()
```

#### Perform aggregation or arithmetic operations on selected columns

```python
# Specify that the output should display the average value of all cells in column "c2"
table_object.output(["avg(c2)"]).to_pl()
```

```python
# Select column "c1" and request all cells in this column to be displayed with their original values increased by 5
table_object.output(["c1+5"]).to_pl()
```

```python
# Specify that the output should display the result of an arithmetic operation combining two aggregation functions
table_object.output(["min(c1) + max(c2)"]).to_pl()
```

```python
# Specify that the output should display the row number of the current table
table_object.output(["count(*)"]).to_pl()
```

```python
# Select column "num" and request all cells in this column to be displayed with their original values divided by 10
table_object.output(["num / 10"]).to_pl()
```

```python
# Select column "num" and display all its cells as absolute values
table_object.output(["abs(num)"]).to_pl()
```

```python
# Specify that the output should display the result of three multiplied by five
# Note that no columns are involved in this example!
# Either of the following works: 
table_object.output(["3 * 5"]).to_pl()
```

---

## filter

```python
table_object.filter(cond)
```

Creates a filtering condition expression for the current table.

:::tip NOTE
This method creates a filtering condition for your query. To display the results, you must chain it with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

### Parameters

#### cond: `str`, *Required*

A non-empty string representing the filter condition. It comprises one or multiple expressions combined by 'and' or 'or' logical operators, where each expression uses comparison operators to set criteria for keeping or removing rows.

:::tip NOTE
Currently, only 'and' and 'or' logical expressions are supported.
:::

### Returns

An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.

:::tip NOTE
This method specifies a filtering condition for the rows in the current table but does not directly produce displayable data. To display the query results, use `filter()` in conjunction with methods like `to_result()`, `to_df()`, `to_pl()`, or `to_arrow()` to materialize the data.
:::

### Examples

```python
table_object.output(["c1", "c2"]).filter("(-7 < c1 or 9 >= c1) and (c2 = 3)").to_pl()
```

```python
table_object.output(["*"]).filter("c2 = 3").to_pl()
```

---

## match_dense

```python
table_object.match_dense(vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params = None)
```

Creates a dense vector search expression to identify the top n closest rows to the given dense vector. Suitable for working with dense vectors (dense embeddings).

:::tip NOTE
To display your query results, you must chain this method with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

### Parameters

#### vector_column_name: `str`, *Required*

A non-empty string indicating the name of the vector column to search on.

#### embedding_data: `list/np.ndarray`, *Required*

The query vector data to compare against. This should be provided as a list or a one-dimensional NumPy array of numerical values.

#### embedding_data_type: `str`, *Required*

Specifies the data type of the embedding vector. Commonly used types (values) include:

- `"float"`
- `"uint8"`.

#### distance_type: `str`, *Required*

The distance metric to use in similarity search.

- `"ip"`: Inner product.
- `"l2"`: Euclidean distance.
- `"cosine"`: Cosine similarity.

#### topn: `int`, *Required*

An integer indicating the number of nearest neighbours to return.

#### knn_params: `dict[str, str]`, *Optional*

A dictionary representing additional parameters for the KNN or ANN search.

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value indicating a specific error condition.
  - `error_msg`: `str` A message providing additional details about the error.

### Examples

#### Perform a brute-force vector search

```python
# Find the 100 nearest neighbors using Euclidean distance
# If no vector index is created on the column being queried, then the vector search defaults to a brute-force search.
# In such case, set `knn_params` to `None` or leave it blank.
table_object.match_dense("vec", [0.1,0.2,0.3], "float", "l2", 100)
```

:::caution NOTE
`knn_params` settings will not take effect here because no index has been created.
:::

#### Perform a vector search in HNSW

1. Ensure that you have successfully built an HNSW index. If you are uncertain, you can rebuild the index, setting `ConflictType` to `Ignore`.
2. Set the `ef` value as follows:

```python
from infinity.index import IndexInfo, IndexType, InitParameter
table_object.create_index("my_index", IndexInfo("vec", IndexType.Hnsw, [InitParameter("ef_construction", "50"), InitParameter("ef", "50")]))
# Find the 2 nearest neighbors using cosine distance
# If an HNSW index is successfully built on the column being queried, then the vector search uses this index,
# regardless of whether `knn_params` is set.
# If you leave `knn_params` blank, the search takes the `"ef"` value set in `create_index()`.
table_object.match_dense("vec", [1, 2, 3], "uint8", "cosine", 2)
```

```python
from infinity.index import IndexInfo, IndexType, InitParameter
table_object.create_index("my_index", IndexInfo("vec", IndexType.Hnsw, [InitParameter("ef_construction", "50"), InitParameter("ef", "50")]))
# Find the 2 nearest neighbors using inner product distance
# If an HNSW index is successfully built on the column being queried, then the vector search uses this index,
# regardless of whether `knn_params` is set.
# You can specify the value of `"ef"` in `knn_params`, which overrides the value set in `create_index()`
table_object.match_dense("vec", [0.1,0.2,0.3], "float", "ip", 2, {"ef": "100"})
```

:::tip NOTE
If the HNSW index is not created successfully, the search will fall back to a brute-force search.
:::

---

## match_sparse

```python
table_object.match_sparse(vector_column_name, sparse_data, distance_type, topn, opt_params)
```

Creates a sparse vector search expression to identify the top n closest rows to the given sparse vector. Suitable for working with sparse vectors (sparse embeddings).

:::tip NOTE
To display your query results, you must chain this method with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

### Parameters

#### vector_column_name: `str`, *Required*

A non-empty string indicating the name of the column to query on.

#### sparse_data: `SparseVector(list[int], list[int] | list[float])`, *Required*

The query sparse vector data to compare against. The `sparse_data` parameter should be provided as a SparseVector object, which has two members:

- `indices`: A list of the indices, each corresponding to a non-zero value in the sparse vector.
- `values`: A list of the corresponding values for each index in the `indices` list.

:::tip NOTE
If you have a dictionary of indices and values, you can create a SparseVector object using the `SparseVector` class. For example:

```python
from infinity.common import SparseVector
dic_sparse_vector = {"indices": [0, 10, 20], "values": [0.1, 0.2, 0.3]}
sparse_vector = SparseVector(**dic_sparse_vector)
```

:::

#### distance_type: `str`, *Required*

 A non-empty string indicating the distance type for the search. Currently, only `"ip"` (inner product) is supported.

#### topn: `int`, *Required*

An integer indicating the number of nearest neighbours to return.

#### opt_params: `dict[str, str]`, *Required*

A dictionary representing additional parameters for the sparse vector search. Following are parameters for the BMP index:

- `"alpha"`: `str`  
  `"0.0"` ~ `"1.0"` (default: `"1.0"`) - A "Termination Conditions" parameter. The smaller the value, the more aggressive the pruning.
- `"beta"`: `str`  
  `"0.0"` ~ `"1.0"` (default: `"1.0"`) - A "Query Term Pruning" parameter. The smaller the value, the more aggressive the pruning.

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value indicating a specific error condition.
  - `error_msg`: `str` A message providing additional details about the error.

### Examples

#### Perform a brute-force sparse vector search

```python
# As demonstrated in the following example:
# The sparse vector search is performed on column "sparse_column" to find the 100 nearest neighbors using inner product
# SparseVector(**{"indices": [0, 10, 20], "values": [0.1, 0.2, 0.3]}) represents the sparse vector to compare against:
# - 0: the index of 0.1
# - 10: the index of 0.2
# - 20: the index of 0.3
# If no sparse vector index is created on the column being queried, then the search defaults to a brute-force search.
# In such case, set `opt_params` to `None` or leave it blank.
from infinity.common import SparseVector
table_object.match_sparse('sparse', SparseVector([0, 10, 20], [0.1, 0.2, 0.3]), 'ip', 100)
```

:::caution NOTE
`opt_params` settings will not take effect here because no index has been created.
:::

#### Perform a sparse vector search in BMP

```python
from infinity.index import IndexInfo, IndexType, InitParameter
table_object.create_index("my_index", [IndexInfo("sparse", IndexType.BMP)])
# Find the 100 nearest neighbors using inner product
# If a BMP index is successfully built on the column being queried, then the sparse vector search uses this index,
# regardless of whether `opt_params` is set.
# If you leave `opt_params` blank, the search takes the default settings for `"alpha"` and `"beta"`.
from infinity.common import SparseVector
table_object.match_sparse('sparse', SparseVector([0, 10, 20], [0.1, 0.2, 0.3]), 'ip', 100, {"alpha": "1.0", "beta": "1.0"})
```

```python
from infinity.index import IndexInfo, IndexType, InitParameter
table_object.create_index("my_index", IndexInfo("sparse", IndexType.BMP))
# Find the 100 nearest neighbors using inner product
# If a BMP index is successfully built on the column being queried, then the sparse vector search uses this index,
# regardless of whether `opt_params` is set.
# You can set the values of `"alpha"` or `"beta"` in `opt_params`, which overrides the default settings.
from infinity.common import SparseVector
table_object.match_sparse('sparse', SparseVector([0, 10, 20], [8, 10, 66]), 'ip', 100, {"alpha": "1.0", "beta": "1.0"})
```

---

## match_text

```python
table_object.match_text(fields, matching_text, topn, extra_options)
```

Creates a full-text search expression on the specified field(s)/column(s) to identify the most relevant rows.

:::tip NOTE
To display your query results, you must chain this method with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

### Parameters

#### fields: `str`, *Required*

A non-empty, comma-separated string of column names on which the full-text search will be performed.

:::danger NOTE
Ensure that a full-text index has been successfully built on each column involved before executing a full-text search; otherwise, an error will occur.
:::

:::tip NOTE
To display your query results, you must chain this method with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

#### matching_text: `str`, *Required*

A non-empty text string to search for. You can use various search options within the matching text, including:

- Single terms: `"blooms"`
- OR multiple terms: `"Bloom OR filter"`, `"Bloom || filter"` or just `"Bloom filter"`
- Phrase search: `'"Bloom filter"'`
- AND multiple terms: `"space AND efficient"`, `"space && efficient"` or `"space + efficient"`
- Escaping reserved characters: `"space\-efficient"`
- Sloppy phrase search: `'"harmful chemical"~10'`
- Field-specific search: `"title:(quick OR brown) AND body:foobar"`

#### topn: `int`, *Required*

Specifies the number of the most relevant rows to retrieve, e.g., assign `10` to obtain the ten most relevant rows.

#### extra_options: `dict`, *Optional*

An optional dictionary specifying the following search options:

- **"default_field"**: `str`, *Optional*
  - If `"fields"` is an empty string, this parameter specifies the default field to search on.
- **"operator"**: `str`, *Optional*
  - If not specified, the search follows Infinity's full-text search syntax, meaning that logical and arithmetic operators, quotation marks and escape characters will function as full-text search operators, such as:
    - AND operator: `AND`, `&&`, `+`
    - OR operator: `OR`, `||`
    - NOT operator: `NOT`, `!`, `-`
    - PAREN operator: `(`, `)`, need to appear in pairs, and can be nested.
    - COLON operator: `:`: Used to specify field-specific search, e.g., `body:foobar` searches for `foobar` in the `body` field.
    - CARAT operator: `^`: Used to boost the importance of a term, e.g., `quick^2 brown` boosts the importance of `quick` by a factor of 2, making it twice as important as `brown`.
    - TILDE operator: `~`: Used for sloppy phrase search, e.g., `"harmful chemical"~10` searches for the phrase `"harmful chemical"` within a tolerable distance of 10 words.
    - SINGLE_QUOTED_STRING: Used to search for a phrase, e.g., `'Bloom filter'`.
    - DOUBLE_QUOTED_STRING: Used to search for a phrase, e.g., `"Bloom filter"`.
    - Escape characters: Used to escape reserved characters, e.g., `space\-efficient`. Starting with a backslash `\` will escape the following characters:   
      `' '`, `'+'`, `'-'`, `'='`, `'&'`, `'|'`, `'!'`, `'('`, `')'`, `'{'`, `'}'`, `'['`, `']'`, `'^'`, `'"'`, `'~'`, `'*'`, `'?'`, `':'`, `'\'`, `'/'`
  - If specified, Infinity's full-text search syntax will not take effect, and the specified operator will be interpolated into `matching_text`.
    Useful for searching text including code numbers like `"A01-233:BC"`.
    - `{"operator": "or"}`: Interpolates the `OR` operator between words in `matching_text` to create a new search text.
      For example, reinterprets `"A01-233:BC"` as `'"A01" OR "-233" OR "BC"'`.
    - `{"operator": "and"}`: Interpolates the `AND` operator between words in `matching_text` to create a new search text.
      For example, reinterprets `"A01-233:BC"` as `'"A01" AND "-233" AND "BC"'`.

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value indicating a specific error condition.
  - `error_msg`: `str` A message providing additional details about the error.

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
    table_object.match_text('body', question, 2)
    table_object.match_text('', question, 2, {'default_field': 'body'})
```

---

## fusion

```python
table_object.fusion(method, topn, fusion_params = None)
```

Creates a reranking expression for multiple retrieval ways to identify the top n closest rows.

:::tip NOTE
To display your query results, you must chain this method with `output(columns)`, which specifies the columns to output, and a method such as `to_pl()`, `to_df()`, or `to_arrow()` to format the query results.
:::

### Parameters

#### method: `str`, *Required*

A non-empty string indicating the reranking methods to use:

- `"rrf"`: [Reciprocal rank fusion](https://plg.uwaterloo.ca/~gvcormac/cormacksigir09-rrf.pdf)  
  RRF is a method for combining multiple result sets with varying relevance indicators into a single result set. It requires no tuning, and the relevance indicators need not be related to achieve high-quality results. RRF is particularly useful when you are uncertain of the relative importance of each retrieval way.  
  RRF uses the following formula to calculate the score for ranking each document:  

  ```python
  score = 0.0
  for q in queries:
      if d in result(q):
          score += 1.0 / ( k + rank( result(q), d ) )
  return score

  # Where
  # k is the ranking constant,
  # q is a query in a set of queries,
  # d is a document in the result set of q,
  # result(q) is the result set of q, and
  # rank( result(q), d ) is the rank of d within the result(q), starting from 1.
  ```

- `"weighted_sum"`  
  The weighted sum approach assigns different weights to different retrieval ways, allowing you to emphasize specific ways. This is particularly useful when you are certain of each path's relative importance.  
- `"match_tensor"`  
  Infinity's tensor-based late interaction reranking approach.  

#### topn: `int`, *Required*

An integer indicating the number of the most relevant rows to retrieve.

#### fusion_params: `dict[str, Any]`, *Optional*

A dictionary representing additional options for the selected reranking method:

- **RRF-specific options**: *Optional*  
  Settings when employing RRF for reranking.  
  - `"rank_constant"`: The smoothing constant for RRF reranking, e.g., `{"rank_constant": 60}`. Defaults to `60`.

- **weighted_sum-specific options**: *Optional*  
  Settings when employing Weighted Sum for reranking.  
  - `"weights"`: Specifies the weight for each retrieval way. For example, `{"weights": "1,2,0.5"}` sets weights of `1`, `2`, and `0.5` for the first, second, and third retrieval ways, respectively. The default weight of each retrieval way is `1.0`. If `"weight"` is not specified, all retrieval ways will be assiged the default weight of `1.0`.

- **match_tensor-specific options**: *Optional*  
  Settings when employing match_tensor for reranking.
  - `"field"`: The name of the tensor column for reranking.
  - `"data"`: The tensor data to compare against. This should be provided as a list of lists or a two-dimensional NumPy
    array of numerical values.
  - `"data_type"`: The element data type of the query tensor. Usually `"float"`.

### Returns

- Success: An `infinity.local_infinity.table.LocalTable` object in Python module mode or an `infinity.remote_thrift.table.RemoteTable` object in client-server mode.
- Failure: `InfinityException`
  - `error_code`: `int` A non-zero value indicating a specific error condition.
  - `error_msg`: `str` A message providing additional details about the error.

### Examples

The following code snippets illustrate the use of fused reranking in a three-way retrieval.

#### Use RRF for reranking

```python {6}
from infinity.common import SparseVector
table_object.output(["num", "body", "vec", "sparse", "year", "tensor", "_score"])
            .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "cosine", 3)
            .match_sparse("sparse", SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3)
            .match_text("body", "blooms", 10)
            .filter("year < 2024")
            .fusion("rrf", 2)
            .to_pl()
```

```python {6}
from infinity.common import SparseVector
table_object.output(["num", "body", "vec", "sparse", "year", "tensor", "_score"])
            .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "cosine", 3)
            .match_sparse("sparse", SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3)
            .match_text("body", "blooms", 10)
            .filter("year < 2024")
            .fusion("rrf", 2, {"rank_constant": 30})
            .to_pl()
```

#### Use Weighted Sum for reranking

```python {6}
from infinity.common import SparseVector
table_object.output(["num", "body", "vec", "sparse", "year", "tensor", "_score"])
            .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "cosine", 3)
            .match_sparse("sparse", SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3)
            .match_text("body", "blooms", 10)
            .filter("year < 2024")
            .fusion("weighted_sum", 2, {"weights": "1,2,0.5"})
            .to_pl()
```

#### Use tensor reranking

```python {8}
from infinity.common import SparseVector
table_object.output(["num", "body", "vec", "sparse", "year", "tensor", "_score"])
            .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "cosine", 3)
            .match_sparse("sparse", SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3)
            .match_text("body", "blooms", 10)
            .filter("year < 2024")
            .fusion("match_tensor", 2, {"field": "tensor", "data_type": "float", "data": [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]]})
            .to_pl()
```

---

## to_result


```python
table_object.to_result()
```

Returns the query result as a tuple.

:::tip NOTE
Call `to_result()` in a chain after (not necessarily "immediately after") `output(columns)` on the same table object.
:::

:::caution NOTE
We recommend calling `to_df()`, `to_pl()`, or `to_arrow()` to format your results.
:::

### Returns 

`tuple[dict[str, list[Any]], dict[str, Any]]`

## to_df

```python
table_object.to_df()
```

Returns the query result in pandas DataFrame format.

:::tip NOTE
Call `to_df()` in a chain after (not necessarily "immediately after") `output(columns)` on the same table object.
:::

### Examples

```python
# Format columns "c1" and C2" of the current table into a pandas DataFrame
res = table_object.output(["c1", "c2"]).to_df()
```

### Returns

A `pandas.DataFrame` object.

## to_pl

```python
table_object.to_pl()
```

Returns the query result in Polas DataFrame format.

:::tip NOTE
Call `to_pl()` in a chain after (not necessarily "immediately after") `output(columns)` on the same table object.
:::

### Returns

A `polas.DataFrame` object.

### Examples

```python
# Format a vector search result into a Polas DataFrame. 
res = table_object.output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 10)
               .to_pl()
```

## to_arrow

```python
table_object.to_arrow()
```

Returns the query result in Apache Arrow Table format.

:::tip NOTE
Call `to_arrow()` in a chain after (not necessarily "immediately after") `output(columns)` on the same table object.
:::

### Returns

A `pyarrow.Table` object.

### Examples

```python
# Format the current table object into an Apache Arrow Table. 
res = table_object.output(["*"])
               .filter("score >= 90")
               .to_pl()
```

---
