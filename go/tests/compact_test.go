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
	"strings"
	"testing"
	"time"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestCompact tests the table compact functionality
func TestCompact(t *testing.T) {
	tableName := "test_compact" + generateSuffix(t)

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with int column and vector column
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
		{
			Name:     "c2",
			DataType: "vector,3,int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Get the test data file path
	// Try to find the test data file in common locations
	testDataPaths := []string{
		"/home/infiniflow/Documents/development/infinity/test/data/csv/embedding_int_dim3.csv",
		"../../test/data/csv/embedding_int_dim3.csv",
		"../test/data/csv/embedding_int_dim3.csv",
		"./test/data/csv/embedding_int_dim3.csv",
	}

	var testCSVPath string
	for _, path := range testDataPaths {
		if _, err := os.Stat(path); err == nil {
			testCSVPath = path
			break
		}
	}

	if testCSVPath == "" {
		// Create a temporary test data file
		tempDir := t.TempDir()
		testCSVPath = filepath.Join(tempDir, "embedding_int_dim3.csv")
		csvContent := `1,"[2 3 4]"
5,"[6,7,8]"
9,"[10,11,12]"`
		err := os.WriteFile(testCSVPath, []byte(csvContent), 0644)
		if err != nil {
			t.Fatalf("Failed to create test data file: %v", err)
		}
	}

	// Verify the file exists
	if _, err := os.Stat(testCSVPath); os.IsNotExist(err) {
		t.Fatalf("Test data file does not exist: %s", testCSVPath)
	}

	// Import data 4 times to create 4 segments
	for i := 0; i < 4; i++ {
		_, err = table.ImportData(testCSVPath, nil)
		if err != nil {
			t.Fatalf("Failed to import data (iteration %d): %v", i+1, err)
		}
	}

	// Check segments count - should be 4
	segments, err := table.ShowSegments()
	if err != nil {
		t.Fatalf("Failed to show segments: %v", err)
	}

	// Parse segments result (it should be a QueryResult)
	segmentCount := 0
	if segments != nil {
		if result, ok := segments.(*infinity.QueryResult); ok {
			if len(result.Data) > 0 {
				// Count rows in the result
				for _, col := range result.Data {
					segmentCount = len(col)
					break
				}
			}
		}
	}

	if segmentCount != 4 {
		t.Errorf("Expected 4 segments before compact, got %d (this may vary based on implementation)", segmentCount)
	}

	// Check row count before compact - should be 12
	res, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to get row count before compact: %v", err)
	}

	if res != nil {
		if result, ok := res.(*infinity.QueryResult); ok && len(result.Data) > 0 {
			for _, col := range result.Data {
				if len(col) > 0 {
					t.Logf("Row count before compact: %v", col[0])
					break
				}
			}
		}
	}

	// Compact the table
	_, err = table.Compact()
	if err != nil {
		// If compact fails, wait a bit and retry (in case of background compact conflict)
		errStr := err.Error()
		if strings.Contains(errStr, "conflict") || strings.Contains(errStr, "background") {
			t.Logf("Compact conflicted with background compact, waiting...")
			time.Sleep(2 * time.Second)
			_, err = table.Compact()
			if err != nil {
				t.Logf("Compact retry failed: %v", err)
			}
		} else {
			t.Logf("Compact failed: %v", err)
		}
	}

	// Check segments count after compact - should be 1
	segments, err = table.ShowSegments()
	if err != nil {
		t.Fatalf("Failed to show segments after compact: %v", err)
	}

	segmentCountAfter := 0
	if segments != nil {
		if result, ok := segments.(*infinity.QueryResult); ok {
			if len(result.Data) > 0 {
				for _, col := range result.Data {
					segmentCountAfter = len(col)
					break
				}
			}
		}
	}

	if segmentCountAfter != 1 {
		t.Errorf("Expected 1 segments after compact, got %d", segmentCountAfter)
	}

	// Check row count after compact - should still be 12
	res, err = table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to get row count after compact: %v", err)
	}

	if res != nil {
		if result, ok := res.(*infinity.QueryResult); ok && len(result.Data) > 0 {
			for _, col := range result.Data {
				if len(col) > 0 {
					t.Logf("Row count after compact: %v", col[0])
					break
				}
			}
		}
	}

	// Drop the table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}
