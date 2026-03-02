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

package tests

import (
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestOutput tests the Output method for query building
func TestOutput(t *testing.T) {
	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_output_table"

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
		{
			Name:     "c2",
			DataType: "varchar",
		},
		{
			Name:     "c3",
			DataType: "float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil after creation")
	}

	// Test Output with multiple columns
	resultTable := table.Output([]string{"c1", "c2"})
	if resultTable == nil {
		t.Fatal("Output returned nil")
	}

	// Test chaining with Output
	resultTable = table.Output([]string{"c1", "c2", "c3"})
	if resultTable == nil {
		t.Fatal("Chained Output returned nil")
	}

	// Test Output with single column
	resultTable = table.Output([]string{"c1"})
	if resultTable == nil {
		t.Fatal("Single column Output returned nil")
	}

	// Test Output with empty columns
	resultTable = table.Output([]string{})
	if resultTable == nil {
		t.Fatal("Empty columns Output returned nil")
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeError)

	t.Logf("Test completed successfully")
}

// TestOutputChaining tests method chaining with Output
func TestOutputChaining(t *testing.T) {
	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_output_chain_table"

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "id",
			DataType: "int",
		},
		{
			Name:     "name",
			DataType: "varchar",
		},
		{
			Name:     "value",
			DataType: "float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Test method chaining
	// Note: Other methods like Filter, Limit, etc. are not fully implemented yet
	// So we just test Output chaining
	resultTable := table.
		Output([]string{"id", "name"}).
		Output([]string{"id", "name", "value"})

	if resultTable == nil {
		t.Fatal("Chained Output returned nil")
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeError)

	t.Logf("Test completed successfully")
}
