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
	"os"
	"path/filepath"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

const (
	testCSVFile             = "embedding_int_dim3.csv"
	testExportCSVFile       = "export_embedding_int_dim3.csv"
	testExportCSVFilePart   = "export_embedding_int_dim3_part.csv"
	testExportJSONLFile     = "export_embedding_int_dim3.jsonl"
	testExportJSONLFilePart = "export_embedding_int_dim3_part.jsonl"
)

// TestConnection tests connect and disconnect server
func TestConnection(t *testing.T) {
	conn, err := infinity.Connect(testLocalHost)
	if err != nil {
		t.Fatalf("Failed to connect: %v", err)
	}
	if conn == nil {
		t.Fatal("Connection is nil")
	}

	_, err = conn.Disconnect()
	if err != nil {
		t.Fatalf("Failed to disconnect: %v", err)
	}
}

// TestCreateDBWithInvalidName tests db name limitation
func TestCreateDBWithInvalidName(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	dbName := ""
	_, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for empty db name, but got none")
	}
}

// TestBasic tests basic operations
func TestBasic(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// List databases and clean up if my_db exists
	resp, err := conn.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}
	// Type assert to *thriftapi.ListDatabaseResponse
	if listResp, ok := resp.(*thriftapi.ListDatabaseResponse); ok {
		for _, dbName := range listResp.DbNames {
			if dbName == "my_db" {
				conn.DropDatabase(dbName, infinity.ConflictTypeError)
			}
		}
	}

	// 1. Create database
	db, err := conn.CreateDatabase("my_db", infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// 2. List databases
	resp, err = conn.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}
	foundCount := 0
	if listResp, ok := resp.(*thriftapi.ListDatabaseResponse); ok {
		for _, dbName := range listResp.DbNames {
			if dbName == "my_db" || dbName == "default_db" {
				foundCount++
			}
		}
	}
	if foundCount != 2 {
		t.Errorf("Expected to find 2 databases (my_db and default_db), found %d", foundCount)
	}

	// 3. Drop database
	_, err = conn.DropDatabase("my_db", infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}

	// 4. Get default_db and test table operations
	db, err = conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get default_db: %v", err)
	}

	// Clean up and create table1
	tableName1 := "my_table1"
	db.DropTable(tableName1, infinity.ConflictTypeIgnore)

	schema1 := infinity.TableSchema{
		{
			Name:        "c1",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
	}

	table, err := db.CreateTable(tableName1, schema1, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table1: %v", err)
	}
	if table == nil {
		t.Fatal("Table1 is nil")
	}

	// List tables
	_, err = db.ListTables()
	if err != nil {
		t.Errorf("Failed to list tables: %v", err)
	}

	// Drop table1
	_, err = db.DropTable(tableName1, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table1: %v", err)
	}

	// 5. Test index operations
	tableName2 := "my_table2"
	db.DropTable(tableName2, infinity.ConflictTypeIgnore)

	schema2 := infinity.TableSchema{
		{Name: "c1", DataType: "vector,1024,float"},
	}

	table, err = db.CreateTable(tableName2, schema2, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table2: %v", err)
	}
	if table == nil {
		t.Fatal("Table2 is nil")
	}

	// Get table
	table, err = db.GetTable(tableName2)
	if err != nil {
		t.Fatalf("Failed to get table2: %v", err)
	}

	// Create index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop index: %v", err)
	}

	// Drop table2
	_, err = db.DropTable(tableName2, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table2: %v", err)
	}

	// 6. Test insert and search
	tableName3 := "my_table3"
	db.DropTable(tableName3, infinity.ConflictTypeIgnore)

	schema3 := infinity.TableSchema{
		{
			Name:        "c1",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		{Name: "c2", DataType: "float"},
	}

	table, err = db.CreateTable(tableName3, schema3, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table3: %v", err)
	}
	if table == nil {
		t.Fatal("Table3 is nil")
	}

	// Get table
	table, err = db.GetTable(tableName3)
	if err != nil {
		t.Fatalf("Failed to get table3: %v", err)
	}

	// Insert data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 1.1},
		{"c1": 2, "c2": 2.2},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Search with output expression
	result, err := table.Output([]string{"c1 + 0.1"}).ToResult()
	if err != nil {
		t.Errorf("Failed to search with output expression: %v", err)
	} else {
		t.Logf("Search result: %v", result)
	}

	// Search with filter
	result, err = table.Output([]string{"*"}).Filter("c1 > 1").ToResult()
	if err != nil {
		t.Errorf("Failed to search with filter: %v", err)
	} else {
		t.Logf("Filter result: %v", result)
	}

	// Drop table3
	_, err = db.DropTable(tableName3, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table3: %v", err)
	}

	// 7. Test import and export
	tableName4 := "my_table4"
	db.DropTable(tableName4, infinity.ConflictTypeIgnore)

	schema4 := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,int"},
	}

	table, err = db.CreateTable(tableName4, schema4, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table4: %v", err)
	}
	if table == nil {
		t.Fatal("Table4 is nil")
	}

	// Get table
	table, err = db.GetTable(tableName4)
	if err != nil {
		t.Fatalf("Failed to get table4: %v", err)
	}

	// Import data from CSV
	testDataDir := "/tmp/infinity_test_data/"
	csvFilePath := filepath.Join(testDataDir, testCSVFile)

	// Check if file exists, if not create test data
	if _, err := os.Stat(csvFilePath); os.IsNotExist(err) {
		// Create test directory
		os.MkdirAll(testDataDir, 0755)
		// Create test CSV file
		testData := "c1,c2\n1,[1,2,3]\n2,[4,5,6]\n3,[7,8,9]\n"
		err = os.WriteFile(csvFilePath, []byte(testData), 0644)
		if err != nil {
			t.Logf("Failed to create test CSV file: %v", err)
		}
	}

	if _, err := os.Stat(csvFilePath); err == nil {
		// Create import options for CSV
		importOpt := infinity.NewImportOption()
		importOpt.CopyFileType = infinity.CopyFileTypeCSV
		importOpt.HasHeader = true
		importOpt.Delimiter = ','

		_, err = table.ImportData(csvFilePath, importOpt)
		if err != nil {
			t.Errorf("Failed to import data: %v", err)
		}

		// Export to CSV
		exportCSVPath := filepath.Join(testDataDir, testExportCSVFile)
		os.Remove(exportCSVPath)
		exportOptCSV := infinity.NewExportOption()
		exportOptCSV.CopyFileType = infinity.CopyFileTypeCSV
		_, err = table.ExportData(exportCSVPath, exportOptCSV, nil)
		if err != nil {
			t.Errorf("Failed to export CSV data: %v", err)
		}

		// Export to JSONL
		exportJSONLPath := filepath.Join(testDataDir, testExportJSONLFile)
		os.Remove(exportJSONLPath)
		exportOptJSONL := infinity.NewExportOption()
		exportOptJSONL.CopyFileType = infinity.CopyFileTypeJSONL
		_, err = table.ExportData(exportJSONLPath, exportOptJSONL, nil)
		if err != nil {
			t.Errorf("Failed to export JSONL data: %v", err)
		}

		// Export with specific columns
		exportCSVPartPath := filepath.Join(testDataDir, testExportCSVFilePart)
		os.Remove(exportCSVPartPath)
		_, err = table.ExportData(exportCSVPartPath, exportOptCSV,
			[]string{"c2", "c1", "_row_id", "_create_timestamp", "_delete_timestamp"})
		if err != nil {
			t.Errorf("Failed to export partial CSV data: %v", err)
		}

		exportJSONLPartPath := filepath.Join(testDataDir, testExportJSONLFilePart)
		os.Remove(exportJSONLPartPath)
		_, err = table.ExportData(exportJSONLPartPath, exportOptJSONL,
			[]string{"c2", "c1", "_row_id", "_create_timestamp", "_delete_timestamp"})
		if err != nil {
			t.Errorf("Failed to export partial JSONL data: %v", err)
		}

		// Test import exported data
		exportTableName := "my_table_export"
		db.DropTable(exportTableName, infinity.ConflictTypeIgnore)

		exportTable, err := db.CreateTable(exportTableName, schema4, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to create export table: %v", err)
		}

		// Import from exported CSV
		_, err = exportTable.ImportData(exportCSVPath, importOpt)
		if err != nil {
			t.Errorf("Failed to import exported CSV data: %v", err)
		}

		// Search
		result, err = table.Output([]string{"c1"}).Filter("c1 > 1").ToResult()
		if err != nil {
			t.Errorf("Failed to search: %v", err)
		} else {
			t.Logf("Search result: %v", result)
		}

		// Drop and recreate for JSONL import test
		_, err = db.DropTable(exportTableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop export table: %v", err)
		}

		exportTable, err = db.CreateTable(exportTableName, schema4, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to recreate export table: %v", err)
		}

		// Import from exported JSONL
		importOptJSONL := infinity.NewImportOption()
		importOptJSONL.CopyFileType = infinity.CopyFileTypeJSONL
		_, err = exportTable.ImportData(exportJSONLPath, importOptJSONL)
		if err != nil {
			t.Errorf("Failed to import exported JSONL data: %v", err)
		}

		// Search again
		result, err = table.Output([]string{"c1"}).Filter("c1 > 1").ToResult()
		if err != nil {
			t.Errorf("Failed to search after JSONL import: %v", err)
		} else {
			t.Logf("Search result after JSONL import: %v", result)
		}

		// Drop export table
		_, err = db.DropTable(exportTableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop export table: %v", err)
		}

		// Cleanup export files
		os.Remove(exportCSVPath)
		os.Remove(exportJSONLPath)
		os.Remove(exportCSVPartPath)
		os.Remove(exportJSONLPartPath)
	} else {
		t.Logf("Test CSV file not found, skipping import/export tests: %s", csvFilePath)
	}

	// Final search on table4
	result, err = table.Output([]string{"c1"}).Filter("c1 > 1").ToResult()
	if err != nil {
		t.Errorf("Failed to search on table4: %v", err)
	} else {
		t.Logf("Final search result: %v", result)
	}

	// Drop table4
	_, err = db.DropTable(tableName4, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table4: %v", err)
	}
}

// Helper function to check if test file exists
func testFileExists(path string) bool {
	_, err := os.Stat(path)
	return !os.IsNotExist(err)
}

// TestBasicOperations tests basic CRUD operations
func TestBasicOperations(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_basic_operations"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:        "id",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		{Name: "name", DataType: "varchar"},
		{Name: "value", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert single row
	_, err = table.Insert(map[string]interface{}{
		"id":    1,
		"name":  "test1",
		"value": 1.5,
	})
	if err != nil {
		t.Errorf("Failed to insert single row: %v", err)
	}

	// Insert multiple rows
	_, err = table.Insert([]map[string]interface{}{
		{"id": 2, "name": "test2", "value": 2.5},
		{"id": 3, "name": "test3", "value": 3.5},
	})
	if err != nil {
		t.Errorf("Failed to insert multiple rows: %v", err)
	}

	// Query all
	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query all: %v", err)
	} else {
		t.Logf("All rows: %v", result)
	}

	// Query with filter
	result, err = table.Output([]string{"id", "name"}).Filter("id > 1").ToResult()
	if err != nil {
		t.Errorf("Failed to query with filter: %v", err)
	} else {
		t.Logf("Filtered rows: %v", result)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestBasicIndexOperations tests basic index operations
func TestBasicIndexOperations(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_basic_index"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with vector column
	schema := infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "vector", DataType: "vector,128,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create HNSW index
	hnswIndex := infinity.NewIndexInfo("vector", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "50",
		"metric":          "l2",
	})
	_, err = table.CreateIndex("hnsw_idx", hnswIndex, infinity.ConflictTypeError, "")
	if err != nil {
		t.Errorf("Failed to create HNSW index: %v", err)
	}

	// Drop HNSW index
	_, err = table.DropIndex("hnsw_idx", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop HNSW index: %v", err)
	}

	// Create IVF index
	ivfIndex := infinity.NewIndexInfo("vector", infinity.IndexTypeIVF, map[string]string{
		"metric": "l2",
	})
	_, err = table.CreateIndex("ivf_idx", ivfIndex, infinity.ConflictTypeError, "")
	if err != nil {
		t.Errorf("Failed to create IVF index: %v", err)
	}

	// Drop IVF index
	_, err = table.DropIndex("ivf_idx", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop IVF index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestBasicVectorOperations tests basic vector operations
func TestBasicVectorOperations(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_basic_vector"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with vector column
	schema := infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "vector", DataType: "vector,3,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert vector data
	_, err = table.Insert([]map[string]interface{}{
		{"id": 1, "vector": []float64{1.0, 2.0, 3.0}},
		{"id": 2, "vector": []float64{4.0, 5.0, 6.0}},
		{"id": 3, "vector": []float64{7.0, 8.0, 9.0}},
	})
	if err != nil {
		t.Errorf("Failed to insert vector data: %v", err)
	}

	// Query all vectors
	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query vectors: %v", err)
	} else {
		t.Logf("Vector rows: %v", result)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestBasicDatabaseOperations tests basic database operations
func TestBasicDatabaseOperations(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	dbName := "test_basic_db"

	// Drop if exists
	conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)

	// Create database
	db, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// List databases
	resp, err := conn.ListDatabases()
	if err != nil {
		t.Errorf("Failed to list databases: %v", err)
	} else {
		found := false
		if listResp, ok := resp.(*thriftapi.ListDatabaseResponse); ok {
			for _, name := range listResp.DbNames {
				if name == dbName {
					found = true
					break
				}
			}
		}
		if !found {
			t.Errorf("Created database not found in list")
		}
	}

	// Get database
	db, err = conn.GetDatabase(dbName)
	if err != nil {
		t.Errorf("Failed to get database: %v", err)
	}

	// Show database
	showResp, err := conn.ShowDatabase(dbName)
	if err != nil {
		t.Errorf("Failed to show database: %v", err)
	} else {
		t.Logf("Database info: %v", showResp)
	}

	// Create table in new database
	tableName := "test_table"
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to create table in new database: %v", err)
	}
	if table == nil {
		t.Error("Table is nil")
	}

	// List tables
	tablesResp, err := db.ListTables()
	if err != nil {
		t.Errorf("Failed to list tables: %v", err)
	} else {
		t.Logf("Tables: %v", tablesResp)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}

	// Drop database
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop database: %v", err)
	}
}
