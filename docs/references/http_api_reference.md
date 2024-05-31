---
sidebar_position: 3
slug: /http_api_reference
---

# HTTP API Reference

## Create database

Create a database.

#### Request

```
curl --request POST \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "create_option": "ignore_if_exists"
} '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3016,
    "error_message": "Duplicate database: {database_name}."
}
```

## Drop database

Drop a database.

#### Request

```
curl --request DELETE \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

## Show database

Show detailed information of a specified database.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "database_name": "default_db"
    "store_dir": "/var/infinity/data/nIHniKeHIB_db_default"
    "table_count": 0
}
```

- 500 Error.

```
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

## List databases

Retrieve all databases in the system.

#### Request

```
curl --request GET \
     --url localhost:23820/databases \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "databases": [
        "default_db",
        "my_db"
    ]  
}
```

## Create table

Create a table in a specified database.

#### Request

```
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

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3017,
    "error_message": "Duplicate table: {table_name} in {database_name}."
}
```

## Drop table

Drop a table from a specified database.

#### Request

```
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## List tables

List all tables in a specified database.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

## Show table

Show detailed information of a specified table.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## Show table columns

Show the column information of a specific table in a specified database.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/columns \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## Create an index

Creates an index on a specified table.

#### Request

```
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
        "ef": "50",
        "metric": "l2"
    },
     "create_option": {
        "ignore_if_exists": true
    }
} '

```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3018,
    "error_message": "Duplicate index: {index} in {table_name}."
}
```

## Drop an index

Drop an index.

#### Request

```
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "drop_option": "ignore_if_not_exists"
} '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

## Show index

Show detailed information of a specified index.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {table_name}."
}
```

## List indexes

Lists all indexes of a specified table.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

## Import data

Import data into a specified table.

#### Request

```
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

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3032,
    "error_message": "Not supported file type: docx"
}
```

## Insert data

Insert data into a specified table.

#### Request

```
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

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Delete data

Delete data in a specified table.

#### Request

```
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    filter: "a > 0"
} '
```

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "delete_row_count": 10
}
```

- 500 Error.

```
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Update

Update data in a specified table.

#### Request

```
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

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "update_row_count": 10
}
```

- 500 Error.

```
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Search data

Search data in a specified table.

#### Request

```
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

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Show segments

Show all segments of a specified table.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/ \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## Show segment details

Show details of a specified segment. 

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3070,
    "error_message": "Segment: {segment_id} doesn't exist."
}
```

## Show blocks

Show all blocks of specified segment.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/ \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## Show block details

Show details of a specified block.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

- 500 Error.

```
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

## Show block column

Show details of a specified column in a specific block.

#### Request

```
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
{
    "column_name": "c2",
    "column_id": "1",
    "data_type": "Varchar",
    "storage_path": "/var/infinity/data/nFt3IVEglM_db_default/FXgH6FA0gC_table_t2/seg_0/blk_0/1.col",
    "extra_file_count": "1",
    "extra_file_name": "col_1_out_0"
}
```

- 500 Error.

```
{
    "error_code": 3072,
    "error_message": "Block: {block_id} doesn't exist."
}
```

## Show variables

Get all global variables.

#### Request

```
curl --request GET \
     --url localhost:23820/variables \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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

## Show variable

Get a global variable.

#### Request

```
curl --request GET \
     --url localhost:23820/variables/{variable_name} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "buffer_object_count":"6"
}
```

- 500 Error.

```
{
    "error_code": 3027,
    "error_message": "No such system variable {variable_name}."
}
```

## Set a variable

Set a variable with value.

#### Request

```
curl --request POST \
     --url localhost:23820/variables/{variable_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' { "profile_record_capacity" : 120 } '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3076,
    "error_message": "Invalid command: unknown global variable {variable_name}"
}
```

## Show configs

Get all configs.

#### Request

```
curl --request GET \
     --url localhost:23820/configs \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
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
    "version":"0.2.0",
    "wal_compact_threshold":"1073741824",
    "wal_dir":"/var/infinity/wal",
    "wal_flush":"FlushAtOnce"
}
```

## Show config

Get a config.

#### Request

```
curl --request GET \
     --url localhost:23820/configs/{config_name} \
     --header 'accept: application/json'
```

#### Response

- 200 Success.

```
{
    "error_code": 0,
    "version":"0.2.0"
}
```

- 500 Error.

```
{
    "error_code": 1008,
    "error_message": "Attempt to get option: {config_name} which doesn't exist."
}
```

## Set a config

Set a config with value.

#### Request

```
curl --request POST \
     --url localhost:23820/configs/{config_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' { "log_level" : "trace" } '
```

#### Response

- 200 Success.

```
{
    "error_code": 0
}
```

- 500 Error.

```
{
    "error_code": 3028,
    "error_message": "log level value range is trace, debug, info, warning, error, critical"
}
```