---
sidebar_position: 3
slug: /http_api_reference
---

# HTTP API Reference

## Create database

**POST** `/databases/{database_name}`

Creates a database by its name. If the database already exists, the behavior is determined by the `create_option` parameter.

### Request

- Method: POST
- URL: `/database/{database_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
  {
      "create_option": "<option>"
  }
  ```

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
  Conflict policy in `enum<string>` for handling situations where a database with the same name exists.
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

- `error_code`: `int`  
  `0`: The operation succeeds.
- `error_msg`: `str`  
  When `error_code` is `0`, `error_msg` is an empty string.  

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3016,
    "error_message": "Duplicate database: {database_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop database

**DELETE** `/databases/{database_name}`

Deletes a database by its name. If the database does not exist, the behavior is determined by the `drop_option` parameter.

### Request

- Method: DELETE
- URL: `/database/{database_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
  {
      "drop_option": "<option>"
  }
  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show database

**GET** `/databases/{database_name}`

Shows detailed information of a specified database.

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
  A non-empty string indicating the name of the database to retrieve.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## List databases

Retrieves all databases in the system.

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

---

## Create table

Creates a table in a specified database.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
  {
      "create_option": "<option>"
  }
  ```

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
            "constraints": ["not null"],
            "id": 0
        },
        {
            "name": "age",
            "type": "integer",
            "constraints": ["not null"],
            "id": 1
        },
        {
            "name": "score",
            "type": "integer",
            "constraints": ["not null"],
            "id": 2
        },
        {
            "name": "my_vector",
            "type": "vector",
            "dimension": 1024,
            "element_type": "float",
            "id": 3
        }
    ],
    "properties": 
    [
        { 
            "bloomfilter_columns": [ "age", "score" ]
        }
    ]
        
} '
```



#### Request parameter

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3017,
    "error_message": "Duplicate table: {table_name} in {database_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop table

Deletes a table from a specified database.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
  {
      "drop_option": "<option>"
  }
  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## List tables

Lists all tables in a specified database.

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

---

## Show table

Shows detailed information of a specified table.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show table columns

Shows the column information of a specific table in a specified database.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Create an index

Creates an index on a specified table.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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
     "create_option": {
        "ignore_if_exists": true
    }
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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Duplicate index: {index} in {table_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Drop an index

Drop an index.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}/indexes/{index_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
      "drop_option": "<option>"
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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show index

Show detailed information of a specified index.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

- `error_code`: `int`  
  A non-zero value indicates a specific error condition.
- `error_msg`: `str`  
  When `error_code` is non-zero, `error_msg` provides additional details about the error.

---

## Show index segment

Show detailed information of a specified index segment.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

---

## Show index chunk

Show detailed information of a index chunk of specified index chunk.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

---

## List indexes

Lists all indexes of a specified table.

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

---

## Import data

Imports data into a specified table.

### Request

- Method: PUT
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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



### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3032,
    "error_message": "Not supported file type: docx"
}
```

---

## Export data

Exports data into a specified table.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0
}
```

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 7002,
    "error_message": "File already existed: /var/infinity/filename.csv"
}
```

---

## Insert data

Inserts data into a specified table.

### Request

- Method: POST
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

---

## Delete data

Deletes data in a specified table.

### Request

- Method: DELETE
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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

### Response

#### Status code 200

A `200` HTTP status code indicates success. The response includes a JSON object like the following:

```shell
{
    "error_code": 0,
    "delete_row_count": 10
}
```

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

---

## Update

Updates data in a specified table.

### Request

- Method: PUT
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

---

## Search data

Searches data in a specified table.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/docs`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell

  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

---

## Show segments

Shows all segments of a specified table.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/ \
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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

---

## Show segment details

Shows details of a specified segment.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3070,
    "error_message": "Segment: {segment_id} doesn't exist."
}
```

---

## Show blocks

Shows all blocks of specified segment.

### Request

- Method: GET
- URL: `/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/`
- Headers: `accept: application/json`

#### Request example

```shell
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/ \
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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

---

## Show block details

Shows details of a specified block.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

---

## Show block column

Shows details of a specified column in a specific block.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

---

## Show variables

Gets all global variables.

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

---

## Show variable

Gets a global variable.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3027,
    "error_message": "No such system variable {variable_name}."
}
```

---

## Set a variable

Sets a variable with value.

### Request

- Method: POST
- URL: `/variables/{variable_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
      "profile_record_capacity": 
  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3076,
    "error_message": "Invalid command: unknown global variable {variable_name}"
}
```

---

## Show configs

Gets all configs.

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

Gets a config.

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 1008,
    "error_message": "Attempt to get option: {config_name} which doesn't exist."
}
```

---

## Set a config

Sets a config with value.

### Request

- Method: PUT
- URL: `/config/{config_name}`
- Headers:
  - `accept: application/json`
  - `content-Type: application/json`
- Body:

  ```shell
  {
      "log_level": 
  }   
  ```

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

#### Status code 500

A `500` HTTP status code indicates an error condition. The response includes a JSON object like the following:

```shell
{
    "error_code": 3028,
    "error_message": "log level value range is trace, debug, info, warning, error, critical"
}
```

---