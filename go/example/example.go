// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

// This is an example showing how to use the Infinity Go SDK interfaces
// Note: This example shows the interface usage only. The actual implementation
// needs to be provided by connecting to the Infinity server via thrift.

func main() {
	// Example 1: Connect to Infinity (placeholder - needs implementation)
	fmt.Println("=== Example 1: Connect to Infinity ===")

	// In actual implementation:
	// conn, err := infinity.Connect(infinity.LocalHost)
	// if err != nil {
	//     log.Fatal(err)
	// }
	// defer conn.Disconnect()

	fmt.Println("Connected to Infinity (placeholder)")

	// Example 2: Database operations
	fmt.Println("\n=== Example 2: Database Operations ===")

	// Create table
	schema := infinity.TableSchema{
		"id": &infinity.ColumnDefinition{
			Name:        "id",
			DataType:    "integer",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		"name": &infinity.ColumnDefinition{
			Name:     "name",
			DataType: "varchar",
		},
		"embedding": &infinity.ColumnDefinition{
			Name:     "embedding",
			DataType: "vector,128,float32",
		},
	}

	fmt.Printf("Table schema: %+v\n", schema)

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

	// Insert single row
	row := infinity.InsertData{
		"id":        1,
		"name":      "test",
		"embedding": []float32{0.1, 0.2, 0.3, 0.4},
	}
	fmt.Printf("Insert data: %+v\n", row)

	fmt.Println("\n=== All examples completed ===")

	// Suppress unused variable warnings
	_ = schema
	_ = indexInfo
	_ = row
	_ = log.Println
}
