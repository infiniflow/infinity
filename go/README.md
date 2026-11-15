# Infinity Go SDK

This is the Go SDK for Infinity - an AI-native database built for LLM applications.

## Overview

This SDK provides Go interfaces for interacting with Infinity database. It mirrors the functionality of the Python SDK while following Go idioms and conventions.

## Project Structure

```
go/
├── common.go         # Common types (NetworkAddress, SparseVector, Array, FDE, etc.)
├── errors.go         # ErrorCode definitions
├── index.go          # Index types and related structures
├── infinity.go       # InfinityConnection interface
├── database.go       # Database interface
├── table.go          # Table interface
├── types.go          # Additional types for queries and expressions
├── utils.go          # Utility functions
├── connection_pool.go # Connection pool implementation
├── go.mod            # Go module definition
├── example/
│   └── example.go    # Example usage code
└── README.md         # This file
```

## Core Interfaces

### InfinityConnection

The main entry point for connecting to Infinity:

```go
type InfinityConnection interface {
    CreateDatabase(dbName string, conflictType ConflictType, comment string) (*Database, error)
    ListDatabases() (interface{}, error)
    ShowDatabase(dbName string) (interface{}, error)
    DropDatabase(dbName string, conflictType ConflictType) (interface{}, error)
    GetDatabase(dbName string) (*Database, error)
    ShowCurrentNode() (interface{}, error)
    Disconnect() (interface{}, error)
    // ... snapshot and config methods
}
```

### Database

Database for operations:

```go
type Database struct {
    conn   *InfinityConnection
    dbName string
}
```

### Table

Interface for table operations including data manipulation and search:

```go
type Table interface {
    // Index operations
    CreateIndex(indexName string, indexInfo *IndexInfo, conflictType ConflictType, indexComment string) (interface{}, error)
    DropIndex(indexName string, conflictType ConflictType) (interface{}, error)

    // Data operations
    Insert(data interface{}) (interface{}, error)
    ImportData(filePath string, importOptions *ImportOption) (interface{}, error)
    Delete(cond string) (interface{}, error)
    Update(cond string, data map[string]interface{}) (interface{}, error)

    // Search methods (return Table for chaining)
    MatchDense(vectorColumnName string, embeddingData VEC, embeddingDataType string, distanceType string, topN int, knnParams map[string]string) Table
    MatchText(fields string, matchingText string, topN int, extraOptions map[string]interface{}) Table
    MatchSparse(vectorColumnName string, sparseData interface{}, distanceType string, topN int, optParams map[string]string) Table
    MatchTensor(columnName string, queryData VEC, queryDataType string, topN int, extraOption map[string]interface{}) Table
    Fusion(method string, topN int, fusionParams map[string]interface{}) Table

    // Query modifiers (return Table for chaining)
    Output(columns []string) Table
    Filter(filter string) Table
    Limit(limit int) Table
    Offset(offset int) Table
    Sort(orderByExprList [][2]interface{}) Table

    // Execution methods
    ToResult() (interface{}, error)
    ToDF() (interface{}, error)
    ToPL() (interface{}, error)
    ToArrow() (interface{}, error)
    Explain(explainType ExplainType) (interface{}, error)

    // Maintenance
    Optimize() (interface{}, error)
    Compact() (interface{}, error)
    AddColumns(columnDefs TableSchema) (interface{}, error)
    DropColumns(columnNames interface{}) (interface{}, error)
}
```

## Usage Example

See `example/example.go` for comprehensive usage examples.

### Basic Usage

```go
package main

import (
    infinity "github.com/infiniflow/infinity-go-sdk"
)

func main() {
    // Connect to Infinity (implementation needed)
    // conn, err := infinity.Connect(infinity.LocalHost)
    // defer conn.Disconnect()

    // Create database
    // db, err := conn.CreateDatabase("my_db", infinity.ConflictTypeIgnore, "")

    // Create table
    schema := infinity.TableSchema{
        {
            Name:        "id",
            DataType:    "integer",
            Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
        },
        {
            Name:     "embedding",
            DataType: "vector,128,float32",
        },
    }
    // table, err := db.CreateTable("my_table", schema, infinity.ConflictTypeError)

    // Insert data
    // table.Insert(infinity.InsertData{
    //     "id": 1,
    //     "embedding": []float32{0.1, 0.2, ...},
    // })

    // Vector search
    // result, err := table.MatchDense(
    //     "embedding",
    //     []float32{0.1, 0.2, ...},
    //     "float32",
    //     "l2",
    //     10,
    //     nil,
    // ).Output([]string{"id", "_score"}).ToResult()
}
```

## Regular expression search

A full-text index built with the sparse gram analyzer answers `RegexFilter`: the literals a pattern proves mandatory become gram lookups in the index, and the regular expression then verifies the candidates that survive, so a pattern with literals only reads a fraction of the table. A full-text index only sees the rows that exist when it is built, so insert first.

```go
table, err := db.CreateTable("documents", infinity.TableSchema{
    {Name: "id", DataType: "int"},
    {Name: "doc", DataType: "varchar"},
}, infinity.ConflictTypeError)
if err != nil {
    return err
}

if _, err = table.Insert([]map[string]interface{}{
    {"id": 1, "doc": "harmful chemical reaction"},
    {"id": 2, "doc": "chemical processes are harmful"},
}); err != nil {
    return err
}

// A regular analyzer for keyword search, a sparse gram one for `regex()`.
keywordIndex := infinity.NewIndexInfo("doc", infinity.IndexTypeFullText, map[string]string{"analyzer": "standard"})
if _, err = table.CreateIndex("idx_doc", keywordIndex, infinity.ConflictTypeError, ""); err != nil {
    return err
}
// foldCase stores the lowercased form of every gram, which `(?i)` needs.
regexIndex, err := infinity.NewSparsegramIndexInfo("doc", 3, 12, true)
if err != nil {
    return err
}
if _, err = table.CreateIndex("idx_doc_sg", regexIndex, infinity.ConflictTypeError, ""); err != nil {
    return err
}

// The pattern's mandatory literals narrow the scan through the gram index.
result, err := table.Output([]string{"id", "doc"}).
    Filter(infinity.RegexFilter("doc", `(?i)chemical (reaction|processes)`)).
    ToResult()
```

A column without such an index keeps a plain scan, and a pattern that requires no literal (for example `[0-9]+`) does too.

When a column carries more than one full-text index, the choice is made per predicate: a `Filter("filter_fulltext('doc', ...)")` or a `MatchText` that names no index uses the column's regular analyzer index, while the narrowing `RegexFilter` adds goes to the sparse gram index. An index can be named explicitly, as in `filter_fulltext('doc@idx_doc_sg', 'harmful chemical', 'operator=and')`.

## Implementation Status

### Completed (Interface Layer)
- Core type definitions (common.go, errors.go)
- Interface definitions (infinity.go, database.go, table.go)
- Index types and configurations (index.go)
- Query types (types.go)
- Utility functions (utils.go)
- Connection pool (connection_pool.go)

### Pending (Thrift Integration Layer)
- Thrift client implementation
- Request/response serialization
- Connection management
- Error handling from server
- Result parsing and conversion

## License

Apache License 2.0
