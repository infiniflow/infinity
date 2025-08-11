# Infinity HTTP API Usage Guide

## The AttributeError Issue

The error `AttributeError: 'database_result' object has no attribute 'table_names'` occurs because there's a fundamental difference between the **Thrift client** and the **HTTP client** in how they handle the `list_tables()` method.

## Client Differences

### Thrift Client (infinity.connect)
- **Method**: `list_tables()`
- **Returns**: Response object with `table_names` attribute
- **Usage**: `res.table_names`

### HTTP Client (infinity_http)
- **Method**: `list_tables()`
- **Returns**: `database_result` object (for status checking only)
- **Method**: `get_all_tables()`
- **Returns**: List of table names
- **Usage**: `db_instance.get_all_tables()`

## How to Fix the Error

### Option 1: Use the Correct HTTP Method
```python
from infinity_http import infinity_http

# Create HTTP client
infinity_instance = infinity_http()
db_instance = infinity_instance.get_database("db1")

# CORRECT: Use get_all_tables() for HTTP client
table_names = db_instance.get_all_tables()
print(table_names)

# INCORRECT: This will cause AttributeError
# res = db_instance.list_tables()
# print(res.table_names)  # AttributeError!
```

### Option 2: Switch to Thrift Client
```python
import infinity

# Create Thrift client
infinity_instance = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))
db_instance = infinity_instance.get_database("db1")

# CORRECT: Thrift client has table_names attribute
res = db_instance.list_tables()
print(res.table_names)
```

## Complete HTTP API Example

```python
from infinity_http import infinity_http, ConflictType

# Create HTTP client
infinity_instance = infinity_http()

# Create database
infinity_instance.create_database("test_db", ConflictType.Ignore)
db_instance = infinity_instance.get_database("test_db")

# Create table
db_instance.create_table("test_table", {
    "id": {"type": "integer", "constraints": ["PRIMARY KEY"]},
    "name": {"type": "varchar"}
}, ConflictType.Ignore)

# List tables - CORRECT way for HTTP client
table_names = db_instance.get_all_tables()
print(f"Tables: {table_names}")

# Clean up
infinity_instance.drop_database("test_db", ConflictType.Ignore)
```

## Testing the HTTP API

### 1. Run the Simple HTTP Test
```bash
cd example
python simple_http_test.py
```

### 2. Run the Full HTTP API Example
```bash
cd example
python test_http_api.py
```

### 3. Test with curl
```bash
# List databases
curl -X GET "http://localhost:23817/databases" \
  -H "Accept: application/json"

# List tables in a database
curl -X GET "http://localhost:23817/databases/default_db/tables" \
  -H "Accept: application/json"
```

## Key Points to Remember

1. **HTTP Client**: Use `get_all_tables()` to get table names
2. **Thrift Client**: Use `list_tables().table_names` to get table names
3. **HTTP Client**: `list_tables()` is for status checking only
4. **Response Format**: HTTP API returns tables in a `"tables"` field of the JSON response

## Common Mistakes

❌ **Wrong** (HTTP Client):
```python
res = db_instance.list_tables()
print(res.table_names)  # AttributeError!
```

✅ **Correct** (HTTP Client):
```python
table_names = db_instance.get_all_tables()
print(table_names)
```

✅ **Correct** (Thrift Client):
```python
res = db_instance.list_tables()
print(res.table_names)
```

## Troubleshooting

### Connection Issues
- Make sure Infinity server is running on port 23817
- Check if the server supports HTTP API
- Verify network connectivity

### Import Issues
- Ensure you're in the correct directory
- Add the python directory to your Python path
- Install required dependencies (`requests` for HTTP tests)

### Response Format Issues
- HTTP API returns JSON with specific field names
- Check the response structure before accessing fields
- Use the provided test scripts to understand the format 