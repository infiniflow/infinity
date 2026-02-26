// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package main

import (
	"fmt"
	"log"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func main() {
	// Example 1: Connect to Infinity
	fmt.Println("=== Example 1: Connect to Infinity ===")

	conn, err := infinity.Connect(infinity.LocalHost)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	fmt.Println("Connected to Infinity successfully")

	// Disconnect when done
	defer func() {
		_, err := conn.Disconnect()
		if err != nil {
			log.Printf("Error disconnecting: %v", err)
		} else {
			fmt.Println("Disconnected from Infinity")
		}
	}()

	// Example 2: Database operations
	fmt.Println("\n=== Example 2: Database Operations ===")

	// Create a database
	db, err := conn.CreateDatabase("my_db", infinity.ConflictTypeIgnore, "")
	if err != nil {
		log.Printf("CreateDatabase error (may already exist): %v", err)
		// Try to get existing database
		db, err = conn.GetDatabase("my_db")
		if err != nil {
			log.Fatalf("Failed to get database: %v", err)
		}
	}
	fmt.Println("Database 'my_db' ready")

	// List all databases
	dbList, err := conn.ListDatabases()
	if err != nil {
		log.Printf("ListDatabases error: %v", err)
	} else {
		fmt.Printf("Databases: %v\n", dbList)
	}

	// Show database details
	dbInfo, err := conn.ShowDatabase("my_db")
	if err != nil {
		log.Printf("ShowDatabase error: %v", err)
	} else {
		fmt.Printf("Database info: %v\n", dbInfo)
	}

	// Example 3: Table operations
	fmt.Println("\n=== Example 3: Table Operations ===")

	// Create a table with schema
	schema := infinity.TableSchema{
		{
			Name:        "id",
			DataType:    "integer",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		{
			Name:     "name",
			DataType: "varchar",
		},
		{
			Name:     "embedding",
			DataType: "vector,128,float32",
		},
	}

	// Create table
	table, err := db.CreateTable("my_table", schema, infinity.ConflictTypeIgnore)
	if err != nil {
		log.Printf("CreateTable error (may already exist): %v", err)
		// Try to get existing table
		table, err = db.GetTable("my_table")
		if err != nil {
			log.Fatalf("Failed to get table: %v", err)
		}
	}
	fmt.Println("Table 'my_table' ready")

	// List tables
	tables, err := db.ListTables()
	if err != nil {
		log.Printf("ListTables error: %v", err)
	} else {
		fmt.Printf("Tables: %v\n", tables)
	}

	// Show table details
	tableInfo, err := db.ShowTable("my_table")
	if err != nil {
		log.Printf("ShowTable error: %v", err)
	} else {
		fmt.Printf("Table info: %v\n", tableInfo)
	}

	// Example 4: Index operations
	fmt.Println("\n=== Example 4: Index Operations ===")

	// Create an index
	indexInfo := infinity.NewIndexInfo(
		"embedding",
		infinity.IndexTypeHnsw,
		map[string]string{
			"metric":          "l2",
			"m":               "16",
			"ef_construction": "200",
		},
	)
	fmt.Printf("Index info: %s\n", indexInfo.String())

	// Create index on table
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeIgnore, "")
	if err != nil {
		log.Printf("CreateIndex error (may already exist): %v", err)
	} else {
		fmt.Println("Index 'my_index' created")
	}

	// List indexes
	indexes, err := table.ListIndexes()
	if err != nil {
		log.Printf("ListIndexes error: %v", err)
	} else {
		fmt.Printf("Indexes: %v\n", indexes)
	}

	// Example 5: Data operations
	fmt.Println("\n=== Example 5: Data Operations ===")

	// Insert single row
	row := infinity.InsertData{
		"id":        1,
		"name":      "test",
		"embedding": []float32{0.1, 0.2, 0.3, 0.4},
	}
	_, err = table.Insert(row)
	if err != nil {
		log.Printf("Insert error: %v", err)
	} else {
		fmt.Println("Inserted single row")
	}

	// Insert multiple rows
	rows := []infinity.InsertData{
		{"id": 2, "name": "test2", "embedding": []float32{0.5, 0.6, 0.7, 0.8}},
		{"id": 3, "name": "test3", "embedding": []float32{0.9, 1.0, 1.1, 1.2}},
	}
	_, err = table.Insert(rows)
	if err != nil {
		log.Printf("Insert batch error: %v", err)
	} else {
		fmt.Println("Inserted multiple rows")
	}

	// Import data from file (commented out - needs actual file)
	// importOpt := infinity.NewImportOption()
	// importOpt.HasHeader = true
	// importOpt.Delimiter = ','
	// importOpt.CopyFileType = infinity.CopyFileTypeCSV
	// _, err = table.ImportData("/path/to/data.csv", importOpt)
	// if err != nil {
	//     log.Printf("ImportData error: %v", err)
	// }

	// Example 6: Search operations
	fmt.Println("\n=== Example 6: Search Operations ===")

	// Vector search (KNN)
	queryVec := infinity.Float32Vector{0.1, 0.2, 0.3, 0.4}
	result, err := table.MatchDense(
		"embedding",
		queryVec,
		"float32",
		"l2",
		10,
		nil,
	).Output([]string{"id", "name", "_score"}).ToResult()
	if err != nil {
		log.Printf("MatchDense error: %v", err)
	} else {
		fmt.Printf("Vector search result: %v\n", result)
	}

	// Full-text search
	result, err = table.MatchText(
		"name",
		"test",
		10,
		nil,
	).Output([]string{"id", "name"}).ToResult()
	if err != nil {
		log.Printf("MatchText error: %v", err)
	} else {
		fmt.Printf("Full-text search result: %v\n", result)
	}

	// Filter and sort
	result, err = table.Filter("id > 1").
		Sort([][2]interface{}{{"id", infinity.SortTypeDesc}}).
		Limit(100).
		Offset(0).
		Output([]string{"id", "name"}).
		ToResult()
	if err != nil {
		log.Printf("Filter/Sort query error: %v", err)
	} else {
		fmt.Printf("Filter/Sort result: %v\n", result)
	}

	// Example 7: Query execution with different output formats
	fmt.Println("\n=== Example 7: Query Execution ===")

	// Get query string
	queryStr := table.MatchDense("embedding", infinity.Float32Vector(queryVec), "float32", "l2", 10, nil).ToString()
	fmt.Printf("Query string: %s\n", queryStr)

	// Example 8: Explain query
	fmt.Println("\n=== Example 8: Explain Query ===")

	plan, err := table.MatchDense("embedding", infinity.Float32Vector(queryVec), "float32", "l2", 10, nil).
		Explain(infinity.ExplainTypePhysical)
	if err != nil {
		log.Printf("Explain error: %v", err)
	} else {
		fmt.Printf("Execution plan: %v\n", plan)
	}

	// Example 9: Table maintenance
	fmt.Println("\n=== Example 9: Table Maintenance ===")

	// Optimize table
	_, err = table.Optimize()
	if err != nil {
		log.Printf("Optimize error: %v", err)
	} else {
		fmt.Println("Table optimized")
	}

	// Compact table
	_, err = table.Compact()
	if err != nil {
		log.Printf("Compact error: %v", err)
	} else {
		fmt.Println("Table compacted")
	}

	// Add columns
	_, err = table.AddColumns(infinity.TableSchema{
		{Name: "new_col", DataType: "varchar"},
	})
	if err != nil {
		log.Printf("AddColumns error: %v", err)
	} else {
		fmt.Println("Column added")
	}

	// Example 10: Connection pool
	fmt.Println("\n=== Example 10: Connection Pool ===")

	// Create connection pool
	pool, err := infinity.NewConnectionPool(
		infinity.ConnectionPoolConfig{
			URI:         infinity.LocalHost,
			InitialSize: 10,
		},
		func(uri infinity.URI) (*infinity.InfinityConnection, error) {
			return infinity.Connect(uri)
		},
	)
	if err != nil {
		log.Printf("Failed to create connection pool: %v", err)
		return
	}
	defer pool.Close()

	// Get connection from pool
	poolConn, err := pool.Get()
	if err != nil {
		log.Printf("Pool Get error: %v", err)
	} else {
		fmt.Println("Got connection from pool")
		// Return connection to pool
		pool.Put(poolConn)
		fmt.Println("Returned connection to pool")
	}

	fmt.Println("\n=== All examples completed ===")
}
