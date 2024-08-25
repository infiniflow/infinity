---
sidebar_position: 3
slug: /http_api_reference
---

# HTTP API Reference

## Create database

**POST** `/databases/{database_name}`

Creates a database by its name. If the database already exists, the action taken depends on the `create_option` parameter.

### Request

- Method: POST
- URL: `/database/{database_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"create_option"`: `enum<string>`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "create_option": "ignore_if_exists"
} ' 
```

#### Request parameter

- `database_name`: (*Path parameter*), `string`, *Required*  
  A non-empty string indicating the name of the database, which must adhere to the following requirements:  
  - Permitted characters include:
    - English letters (a-z, A-Z)
    - Digits (0-9)
    - "_" (underscore)
  - Must begin with an English letter or underscore.
  - Maximum 65,535 characters.
  - Case-insensitive.
- `create_option`: (*Body parameter*), `enum<string>`, *Optional*  
  - `"error"`: (Default) Raise an error if a database with the same name exists.
  - `"ignore_if_exist"`: Ignore the database creation requrest and keep the existing database with the same name.

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0 
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3016,
    "error_message": "Duplicate database: {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop database

**DELETE** `/databases/{database_name}`

Deletes a database by its name. If the database does not exist, the action taken depends on the `drop_option` parameter.

### Request

- Method: DELETE
- URL: `/database/{database_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"drop_option"`: `enum<string>`

#### Request example

```shell
curl --request DELETE \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

#### Request parameter

- `database_name`: (*Path parameter*), `string`, *Required*  
  A non-empty string indicating the name of the database to delete.
- `drop_option`: (*Body parameter*), `enum<string>`, *Optional*  
  Conflict policy in `enum<string>` for handling situations where a database with the specified name does not exist.
  - `"error"`: (Default) Raise an error if the specified database does not exist.
  - `"ignore_if_not exists"`: Ignore the operation and proceed regardless, if the specified database does not exist.

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show database

**GET** `/databases/{database_name}`

Shows detailed information about a specified database.

### Request

- Method: GET
- URL: `/database/{database_name}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json'
```

#### Request parameter

- `database_name`: (*Path parameter*), `string`, *Required*  
  A non-empty string specifying the name of the database to retrieve.

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "database_name": "default_db"
    "store_dir": "/var/infinity/data/nIHniKeHIB_db_default"
    "table_count": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.
- `database_name`: `string`  
  The name of the retrieved database.
- `store_dir`: `string`  
  The directory path where the database is stored.
- `table_count`: `integer`  
  The number of tables present in the database.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## List databases

**GET** `/databases`

Retrieves a list of all available databases within the Infinity system.

### Request

- Method: GET
- URL: `/databases`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "databases": [
        "default_db",
        "my_db"
    ]  
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.
- `databases`: `list[string]`  
  An array of strings representing the names of the databases in the system.

---

## Create table

**POST** `/databases/{database_name}/tables/{table_name}`

Creates a table with a specified name and defined fields (columns) within a given database. If the table already exists, the action taken depends on the `create_option` parameter.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"create_option"`: `enum<string>`
  - `"fields"`: `object[]`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "create_option": "ignore_if_exists",
    "fields": 
    [
        {
            "name": "name",
            "type": "varchar",
        },
        {
            "name": "score",
            "type": "float"
        },
        {
            "name": "my_vector",
            "type": "vector",
            "dimension": 1024
        }
    ]       
} '
```


#### Request parameters

- `database_name`: (*Path parameter*), `string`, *Required*  
  A non-empty string that specifies the name of the database to retrieve.
- `table_name`: (*Path parameter*), `string`, *Required*  
  A non-empty string indicating the name of the table to create, which must adhere to the following requirements:  
  - Permitted characters include:
    - English letters (a-z, A-Z)
    - Digits (0-9)
    - "_" (underscore)
  - Must begin with an English letter or underscore.
  - Maximum 65,535 characters.
  - Case-insensitive.
- `create_option`: (*Body parameter*), `enum<string>`, *Optional*  
  - `"error"`: (Default) Raise an error if a table with the same name exists.
  - `"ignore_if_exist"`: Ignore the table creation request and keep the existing table with the same name.
- `field`: (*Body parameter*), `object[]`, *Required*
  - `"name"`: `string`, *Required*
  - `"type"`: `string`, *Required*

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3017,
    "error_message": "Duplicate table: {table_name} in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop table

**DELETE** `/databases/{database_name}/tables/{table_name}`

Deletes a table from a specified database. If the table does not exist, the action taken depends on the `drop_option` parameter.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"drop_option"`: `enum<string>`

```shell
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## List tables

**GET** `/databases/{database_name}/tables`

Retrieves a list of all available tables in a specified database.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "tables": 
    [
        {
            "name": "table1",
            "type": "table",
            "column_count": 3,
            "block_count": 21,
            "block_capacity": 8192,
            "segment_count": 3,
            "segment_capacity": 8388608
        },
        {
            "name": "table2",
            "type": "table",
            "column_count": 5,
            "block_count": 13,
            "block_capacity": 8192,
            "segment_count": 1,
            "segment_capacity": 8388608
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

---

## Show table

**GET** `/databases/{database_name}/tables/{table_name}`

Shows detailed information about a specified table within a given database.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "database_name": "default_db",
    "table_name": "my_table",
    "store_dir": "/var/infinity/data/nIHniKeHIB_db_default/h1abZcWuBs_table_my_table",
    "column_count" : 3,
    "segment_count" : 1,
    "row_count" : 5
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show table columns

**GET** `/databases/{database_name}/tables/{table_name}/columns`

Shows the column information about a specified table within a given database.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/columns`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/columns \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "columns": [
        {
            "column_name": "name",
            "column_type": "varchar",
            "constraints": ""
        },
        {
            "column_name": "age",
            "column_type": "integer",
            "constraints": ""
        },
        {
            "column_name": "score",
            "column_type": "integer",
            "constraints": ""
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Create index

**POST** `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`

Creates an index on a specified table. If an index with the same name exists, the action taken depends on the `create_option` parameter.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"fields"`: `list[string]`
  - `"index"`: `object`
  - `"create_option"`: `enum<string>`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
--data '
{
    "fields": 
    [
        "col1"
    ],
    "index": 
    {
        "type": "HNSW",
        "M": "16",
        "ef_construction": "50",
        "metric": "l2"
    },
     "create_option": "ignore_if_exists"
} '

```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Duplicate index: {index} in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop an index

**DELETE** `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`

Deletes an index by its name.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"drop_option"`: `enum<string>`
  ```

#### Request example

```shell
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show index

**GET** `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`

Shows detailed information about a specified index.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`
- Headers: `accept: application/json`

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "database_name": "default_db",
    "table_name": "test_index_tbl",
    "index_name": "idx1",
    "index_column_ids": "0",
    "index_column_names": "col1",
    "index_type": "IVFFlat",
    "other_parameters": "metric = l2, centroids_count = 128",
    "segment_index_count": "0",
    "storage_directory": "/var/infinity/data/yjamyYqzzt_db_default/CxmfWOUCdN_table_test_index_tbl/inlt9JpOyy_index_idx1"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show index segment

**GET** `/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}`

Shows detailed information about a specified index segment.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code":0,
    "chunk_count":"1",
    "index_segment_size":"290.41MB",
    "segment_id":"0",
    "storage_path":"/var/infinity/data/XScyuGgMEf_db_default_db/X7Ilrg2jG1_table_sift_benchmark/gWao7rl6u4_index_hnsw_index"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show index chunk

**DELETE** `/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}/chunk/{chunk_id}`

Shows detailed information about a specified index chunk.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}/chunk/{chunk_id}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}/chunk/{chunk_id} \
     --header 'accept: application/json'
```

#### Request parameters

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code":0,
    "deprecate_timestamp":"18446744073709551615",
    "file_name":"",
    "row_count":"1000000",
    "start_row":"0"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## List indexes

**GET** `/databases/{database_name}/tables/{table_name}/indexes`

Retrieves a list of all indexes created on a given table.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/indexes`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "tables": 
    [
        {
            "name": "index1",
            "index_type": "HNSW",
            "columns": ["column1"]
        },
        {
            "name": "index2",
            "index_type": "HNSW",
            "columns": ["column2"]
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

---

## Import data

**PUT** `/databases/{database_name}/tables/{table_name}`

Imports data from a selected file into a specified table.

### Request

- Method: PUT
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"file_path"`: `string`
  - `"file_type"`: `string`  
  - `"header"`: `boolean`
  - `"delimiter"`: `string`

#### Request example

```shell
curl --request PUT \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "file_path":"./filename.json",
    "file_type":"csv",
    "header":false,
    "delimiter":","
} '
```

#### Request parameters

- `"file_path"`: `string`  
  Absolute path to the file for export. Supported file types include:  
  - `csv`
  - `json`
  - `jsonl`

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3032,
    "error_message": "Not supported file type: docx"
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Export data

**GET** `/databases/{database_name}/tables/{table_name}`

Exports data from a specified table to a specified file.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"file_path"`: `string`
  - `"file_type"`: `string`
  - `"header"`: `boolean`  
  - `"delimiter"`: `string`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/table/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "file_path":"/var/infinity/filename.csv",
    "file_type":"csv",
    "header":false,
    "delimiter":","
} '
```

#### Request parameters


- `"header"`: `boolean`, *Optional*  
  Whether to display table header or not. Works with **.csv** files only:  
  - `True`: Display table header.
  - `False`: (Default) Do not display table header.

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 7002,
    "error_message": "File already existed: /var/infinity/filename.csv"
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Insert data

**POST** `/databases/{database_name}/tables/{table_name}/docs`

Inserts rows of data into a specified table.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `object[]`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
[    
    {
        "name": "Tom",
        "age": 16,
        "score": 95
    },
    {
        "name": "Jason",
        "age": 15,
        "score": 92
    },
] '
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Delete data

**DELETE** `/databases/{database_name}/tables/{table_name}/docs`

Deletes rows from a table based on the specified condition.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"filter"`: `string`

#### Request example

```shell
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    filter: "a > 0"
} '
```

#### Request parameters

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "delete_row_count": 10
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Update

**PUT** `/databases/{database_name}/tables/{table_name}/docs`

Searches for rows that match the specified condition and updates them accordingly.

### Request

- Method: PUT
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"update"`: `object`
  - `"filter"`: `string`

#### Request example

```shell
curl --request PUT \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    "update":
    {
        "score": 95
    },
    "filter": "age > 15"
} '
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "update_row_count": 10
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Search data

**GET** `/databases/{database_name}/tables/{table_name}/docs`

Searches for data in a specified table. The search can range from a simple vector search, sparse vector search, or full-text search to complex hybrid searches involving reranking methods.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"output"`: `list[string]`
  - `"filter"`: `string`
  - `"fusion"`: `object`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    "output":
    [
        "name",
        "age"
    ],
    "filter": "age > 15",
    "fusion": 
    {
        "method": "rrf",
        "match":
        {
            "fields": "body", 
            "query": "bloom",
            "operator": "topn=1"
        }
        "knn":
        {
            "fields": "vector_column",
            "query_vector": [1.0, 2.0],
            "ef" : "150",
            "top_k": 3,
            "metric_type": L2
        }
    }
} '
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "output": [
        {
            "name": "Tom",
            "age": 16
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show segments

**GET** `/databases/{database_name}/tables/{table_name}/segments`

Shows all segments in a specified table.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments \
     --header 'accept: application/json'
```
#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "table_name": "test1",
    "segments": [
        {
            "id": "0",
            "size": "192.07KB",
            "status": "Unsealed"
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show segment details

**GET** `/databases/{database_name}/tables/{table_name}/segments/{segment_id}`

Shows the detailed information about a specified segment.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/{segment_id}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "actual_row_count": "4",
    "block_count": "1",
    "column_count": "2",
    "dir": "/var/infinity/data/BCvJAOlTOt_db_default/wUxDzXjGjE_table_test1/seg_0",
    "id": "0",
    "room": "8388604",
    "row_capacity": "8388608",
    "row_count": "4",
    "size": "192.08KB",
    "status": "Unsealed"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3070,
    "error_message": "Segment: {segment_id} doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show blocks

**GET** `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks`

Shows all blocks of a specified segment.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks \
     --header 'accept: application/json'
```

#### Request parameters

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "segment_id": 0,
    "blocks": [
        {
            "id": "0",
            "row_count": "2",
            "size": "192.07KB"
        }
    ]
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show block details

**GET** `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}`

Shows the detailed information about a specified block.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "id": "0",
    "checkpoint_row_count": "4",
    "checkpoint_ts": "209",
    "column_count": "2",
    "path": "/var/infinity/data/CS4HkE8F0H_db_default/IF5Luj1FdG_table_test1/seg_0/blk_0/version",
    "row_capacity": "8192",
    "row_count": "4",
    "size": "192.08KB"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show block column

**GET** `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id}`

Shows the detailed information about a specified column in a block.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "column_name": "c2",
    "column_id": "1",
    "data_type": "Varchar",
    "storage_path": "/var/infinity/data/nFt3IVEglM_db_default/FXgH6FA0gC_table_t2/seg_0/blk_0/1.col",
    "extra_file_count": "1",
    "extra_file_name": "col_1_out_0"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show variables

**GET** `/variables`

Retrieves all global variables in the Infinity system.

### Request

- Method: GET
- URL: `/variables`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/variables \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code":0,
    "active_txn_count":"1",
    "active_wal_filename":"/var/infinity/wal/wal.log",
    "buffer_object_count":"6",
    "buffer_usage":"0B/4.00GB",
    "current_timestamp":"16774",
    "delta_log_count":"1",
    "next_transaction_id":"6",
    "profile_record_capacity":"128",
    "query_count":"0",
    "schedule_policy":"round robin",
    "session_count":"1",
    "total_commit_count":"0",
    "total_rollback_count":"0",
    "unused_buffer_object":"0"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

---

## Show variable

**GET** `/variables/{variable_name}`

Retrieves the value of a global variable.

### Request

- Method: GET
- URL: `/variables/{variable_name}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/variables/{variable_name} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "buffer_object_count":"6"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3027,
    "error_message": "No such system variable {variable_name}."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Set a variable

**POST** `/variables/{variable_name}`

Assigns a value to a global variable.

### Request

- Method: POST
- URL: `/variables/{variable_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"profile_record_capacity"`: `integer`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/variables/{variable_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' { "profile_record_capacity" : 120 } '
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3076,
    "error_message": "Invalid command: unknown global variable {variable_name}"
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show configs

**GET** `/configs`

Retrieves all configs in the Infinity system.

### Request

- Method: GET
- URL: `/configs`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/configs \
     --header 'accept: application/json'
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "buffer_manager_size":"4294967296",
    "cleanup_interval":"10",
    "client_port":"23817",
    "compact_interval":"10",
    "connection_pool_size":"256",
    "cpu_limit":"16",
    "data_dir":"/var/infinity/data",
    "delta_checkpoint_interval":"5",
    "delta_checkpoint_threshold":"67108864",
    "error_code":0,
    "full_checkpoint_interval":"30",
    "http_port":"23820",
    "log_dir":"/var/infinity/log",
    "log_file_max_size":"1073741824",
    "log_file_rotate_count":"8",
    "log_filename":"infinity.log",
    "log_level":"Info",
    "log_to_stdout":"False",
    "mem_index_capacity":"1048576",
    "optimize_interval":"10",
    "postgres_port":"5432",
    "resource_dir":"/var/infinity/resource",
    "server_address":"0.0.0.0",
    "temp_dir":"/var/infinity/tmp",
    "time_zone":"UTC+8",
    "version":"0.3.0",
    "wal_compact_threshold":"1073741824",
    "wal_dir":"/var/infinity/wal",
    "wal_flush":"FlushAtOnce"
}
```

---

## Show config

**GET** `/configs/{config_name}`

Retrieves the value of a config in the Infinity system.

### Request

- Method: GET
- URL: `/configs/{config_name}`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/configs/{config_name} \
     --header 'accept: application/json'
```

#### Request parameters



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "version":"0.3.0"
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 1008,
    "error_message": "Attempt to get option: {config_name} which doesn't exist."
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Set a config

**PUT** `/configs/{config_name}`

Assigns a value to a config.

### Request

- Method: PUT
- URL: `/config/{config_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:
  - `"log_level"`: `enum<string>`

#### Request example

```shell
curl --request POST \
     --url localhost:23820/configs/{config_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' { "log_level": "trace" } '
```

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

- `error_code`: `integer`  
  `0`: The operation succeeds.

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3028,
    "error_message": "log level value range is trace, debug, info, warning, error, critical"
}
```

- `error_code`: `integer`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `string`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---