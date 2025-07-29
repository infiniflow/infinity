# System Snapshot Test Suite

This test suite provides comprehensive testing for Infinity database system snapshot functionality using pytest.

## Overview

The `test_system_snapshot.py` file contains a complete test suite for system snapshot operations including:

- **Basic Operations**: Create, list, restore, and drop system snapshots
- **Data Integrity**: Verify data consistency after snapshot operations
- **Performance Testing**: Measure snapshot creation and restore times
- **Error Handling**: Test various error conditions and edge cases
- **Concurrent Operations**: Test multiple snapshot operations running simultaneously
- **Large Scale Testing**: Test with multiple databases and large datasets

## Test Cases

### 1. Basic System Snapshot Operations (`test_system_snapshot_basic`)
- Creates multiple databases with comprehensive and simple tables
- Creates system snapshot
- Modifies data after snapshot
- Restores from snapshot and verifies data integrity
- Tests all CRUD operations on snapshots

### 2. Empty System Snapshot (`test_system_snapshot_empty_system`)
- Tests system snapshot with no databases (empty system)
- Verifies that empty system state is properly captured and restored

### 3. Large Scale System Snapshot (`test_system_snapshot_large_scale`)
- Tests with multiple databases containing large datasets
- Measures performance metrics for creation and restore operations
- Tests with comprehensive tables (all data types, indexes) and simple tables

### 4. Error Handling (`test_system_snapshot_error_handling`)
- Tests invalid snapshot names
- Tests restoring non-existent snapshots
- Tests dropping non-existent snapshots

### 5. Concurrent Operations (`test_system_snapshot_concurrent_operations`)
- Tests multiple snapshot operations running simultaneously
- Verifies system stability under concurrent load

### 6. Naming Conventions (`test_system_snapshot_naming_conventions`)
- Tests various naming patterns for snapshots
- Includes underscores, dashes, numbers, and uppercase names

### 7. Data Integrity (`test_system_snapshot_data_integrity`)
- Creates snapshots with known data
- Modifies data after snapshot creation
- Restores and verifies original data is preserved
- Ensures modified data is not present after restore

### 8. Performance Metrics (`test_system_snapshot_performance_metrics`)
- Measures snapshot creation time
- Measures snapshot restore time
- Measures snapshot drop time
- Provides performance benchmarks

### 9. Edge Cases (`test_system_snapshot_edge_cases`)
- Tests dropping all databases and restoring
- Tests with maximum data scenarios
- Tests various system states

## Usage

### Running the Tests

```bash
# Run all system snapshot tests
pytest python/test_pysdk/test_system_snapshot.py -v

# Run specific test
pytest python/test_pysdk/test_system_snapshot.py::TestSystemSnapshot::test_system_snapshot_basic -v

# Run with HTTP connection
pytest python/test_pysdk/test_system_snapshot.py --http -v

# Run with specific suffix
pytest python/test_pysdk/test_system_snapshot.py --suffix=_test -v
```

### Prerequisites

- Infinity database server running
- Python dependencies installed (pytest, polars, infinity-sdk)
- Proper connection configuration

### Test Data

The test suite creates various types of test data:

- **Comprehensive Tables**: Include all data types (int, varchar, float, bool, vector, tensor, sparse)
- **Simple Tables**: Basic data types only
- **Indexes**: Full-text, BMP, and IVF indexes
- **Large Datasets**: Up to thousands of rows for performance testing

### Verification Methods

The test suite includes comprehensive verification methods:

- `verify_table_functionality()`: Tests comprehensive tables with all operations
- `verify_simple_table_functionality()`: Tests simple tables
- `verify_database_operations()`: Tests database-level operations
- Data integrity checks comparing original vs restored data

## System Snapshot Features Tested

### Core Functionality
- ✅ Create system snapshot
- ✅ Restore system snapshot  
- ✅ List snapshots
- ✅ Drop snapshots
- ✅ Show snapshot details

### Data Types Supported
- ✅ Integer, varchar, float, boolean
- ✅ Vector columns
- ✅ Tensor columns  
- ✅ Sparse vector columns
- ✅ Primary key constraints

### Index Types
- ✅ Full-text search indexes
- ✅ BMP (Block-Max Pruning) indexes
- ✅ IVF (Inverted File) indexes

### Operations Tested
- ✅ Table creation and deletion
- ✅ Data insertion and querying
- ✅ Index creation and usage
- ✅ Vector similarity search
- ✅ Full-text search
- ✅ Database creation and deletion

## Performance Considerations

The test suite includes performance testing for:

- **Snapshot Creation Time**: Measures time to create system snapshots
- **Snapshot Restore Time**: Measures time to restore from system snapshots
- **Snapshot Drop Time**: Measures time to drop system snapshots
- **Large Scale Testing**: Tests with multiple databases and large datasets

## Error Scenarios Tested

- Invalid snapshot names
- Non-existent snapshots
- Concurrent operations
- System state changes during snapshot operations
- Edge cases with empty systems
- Maximum data scenarios

## Integration with Infinity SDK

The test suite demonstrates proper usage of the Infinity SDK for system snapshots:

```python
# Create system snapshot
result = infinity_obj.create_system_snapshot("my_snapshot")

# Restore system snapshot  
result = infinity_obj.restore_system_snapshot("my_snapshot")

# List snapshots
snapshots = infinity_obj.list_snapshots()

# Drop snapshot
result = infinity_obj.drop_snapshot("my_snapshot")
```

## Contributing

When adding new tests to this suite:

1. Follow the existing naming conventions
2. Include proper error handling
3. Add comprehensive verification methods
4. Include performance metrics where appropriate
5. Test both success and failure scenarios
6. Add proper cleanup in test methods

## Troubleshooting

### Common Issues

1. **Connection Errors**: Ensure Infinity server is running and accessible
2. **Timeout Errors**: Large scale tests may need longer timeouts
3. **Resource Errors**: Ensure sufficient system resources for large datasets
4. **Permission Errors**: Check file system permissions for snapshot directories

### Debug Mode

Run tests with increased verbosity for debugging:

```bash
pytest python/test_pysdk/test_system_snapshot.py -v -s --tb=long
```

This will show detailed output including print statements and full tracebacks. 