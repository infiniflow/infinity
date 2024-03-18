---
sidebar_position: 3
slug: /html_api_reference
---

# HTTP API Reference

## Create database

Creates a new database.

#### Request
```http request
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

- 200 Success
```json
{
    "error_code": 0
}
```

- 500 Internal Service Error
```json
{
    "error_code": 3016,
    "error_message": "Duplicate database: {database_name}."
}
```

## Drop database

Drops a database.

#### Request
```http request
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

- 200 success
```json
{
    "error_code": 0
}
```

- 500 Internal Service Error
```json
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

## Retrieve a database

Retrieves a database.

#### Request
```http request
curl --request GET \
     --url localhost:23820/databases/{database_name} \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
{
    "error_code": 0,
    "database_name": "default"
}
```

- 500 Internal Service Error
```json
{
    "error_code": 3021,
    "error_message": "{database_name} doesn't exist."
}
```

## List databases

Retrieves all databases of the system.

#### Request
```http request
curl --request GET \
     --url localhost:23820/databases \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
{
    "error_code": 0,
    "databases": [
        "default",
        "my_db"
    ]  
}
```

## Create table

creates a new table.

#### Request
```http request
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "create_option": "ignore_if_exists",
    "fields": 
    [
        "name": 
        {
            "type": "varchar",
            "constraints": ["not null"]
        },
        "age":
        {
            "type": "integer",
            "constraints": ["not null"]
        },
        "score":
        {
            "type": "integer",
            "constraints": ["not null"]
        }
    ],
    "properties": 
    [
        "bloomfilter_columns": 
        [
            "age",
            "score"
        ]
    ]
        
} '
```

#### Response

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3017,
    "error_message": "Duplicate table: {table_name} in {database_name}."
}
```

## Drop table

Drops a table.

#### Request
```http request
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

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## List tables

Lists tables of the database.

#### Request
```http request
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
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

## Retrieve a table

Retrieves a table from a database.

#### Request
```http request
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name} \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
{
    "error_code": 0,
    "columns": [
        {
            "column_name": "name",
            "column_type": "varchar",
            "constraints": ["not null"]
        },
        {
            "column_name": "age",
            "column_type": "integer",
            "constraints": ["not null"]
        },
        {
            "column_name": "score",
            "column_type": "integer",
            "constraints": ["not null"]
        }
    ]
}
```

- 501 Error
```json
{
    "error_code": 3022,
    "error_message": "Table {table_name} doesn't exist in {database_name}."
}
```

## Create index

Creates an index on the table.

#### Request
```http request
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' \
{
    "create_option": "ignore_if_exists",
    "fields": 
    [
        "image"
    ],
    "index": 
    {
        "type": "HNSW",
        "m": 16,
        "ef": 200,
        "metric": "l2"
    }
} '

```

#### Response

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3018,
    "error_message": "Duplicate index: {index} in {table_name}."
}
```

## Drop index

Drops an index.

#### Request
```http request
curl --request DELETE \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/indexes/{index_name} \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3018,
    "error_message": "Index {index_name} doesn't exist in {database_name}."
}
```

## Insert

Inserts data into a specified table.

#### Request
```http request
curl --request POST \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    [    
        {
            "name": "Tommy",
            "age": 16,
            "score": 95
        },
        {
            "name": "Jason",
            "age": 15,
            "score": 92
        },
    ]
} '
```

#### Response

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Delete

Deletes data from a specified table.

#### Request
```http request
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

- 200 success
```json
{
    "error_code": 0,
    "delete_row_count": 10
}
```

- 501 Error
```json
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Update

Updates data in a specified table.

#### Request
```http request
curl --request PUT \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    update:
    {
        "score": 95
    },
    filter: "age > 15"
} '
```

#### Response

- 200 success
```json
{
    "error_code": 0,
    "delete_row_count": 10
}
```

- 501 Error
```json
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```

## Select

Selects data from a specified table.

#### Request
```http request
curl --request GET \
     --url localhost:23820/databases/{database_name}/tables/{table_name}/docs \
     --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' \
{
    output:
    [
        "name",
        "age"
    ],
    filter: "age > 15",
    fusion: 
    {
        method: "rrf",
        match:
        {
            "title": 
            {
                "query": "rock fire",
                "operator": "and"
            }
        }
        knn:
        {
            "vector_column":
            {
                "query_vector": [1.0, 2.0],
                "ef" : 150,
                "top_k": 3
            }
        }
    }
    knn: 
    {
        vector: 
        [
            0.1,
            0.2,
            0.3,
            0.4,
        ],
        top: 1
    }
} '
```

#### Response

- 200 success

```json
{
    "error_code": 0,
    "output": [
        {
            "name": "Tommy",
            "age": 16
        }
    ]
}
```

- 501 Error
```json
{
    "error_code": 3005,
    "error_message": "Column {column_name} doesn't exist in {table_name}."
}
```


## Show variables

Gets variables.

#### Request
```http request
curl --request GET \
     --url localhost:23820/variables/{variable_name} \
     --header 'accept: application/json'
```

#### Response

- 200 success
```json
{
    "error_code": 0
}
```

- 501 Error
```json
{
    "error_code": 3027,
    "error_message": "No variable {variable_name}."
}
```