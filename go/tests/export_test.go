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
	"bufio"
	"encoding/binary"
	"os"
	"path/filepath"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// countLines counts the number of lines in a file
func countLines(filePath string) (int, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	count := 0
	for scanner.Scan() {
		count++
	}
	if err := scanner.Err(); err != nil {
		return 0, err
	}
	return count, nil
}

// deleteFile deletes a file if it exists
func deleteFile(filePath string) error {
	if _, err := os.Stat(filePath); err == nil {
		return os.Remove(filePath)
	}
	return nil
}

// readFvecsFile reads vectors from a binary fvecs file
// fvecs format: for each vector, first 4 bytes is the dimension (int32), followed by dimension * 4 bytes of float32 values
func readFvecsFile(filePath string) ([][]float32, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var vectors [][]float32
	for {
		// Read dimension (4 bytes, int32)
		var dim int32
		err := binary.Read(file, binary.LittleEndian, &dim)
		if err != nil {
			break // EOF or error
		}

		// Read vector data (dim * 4 bytes, float32)
		vector := make([]float32, dim)
		for i := int32(0); i < dim; i++ {
			var val float32
			err := binary.Read(file, binary.LittleEndian, &val)
			if err != nil {
				return nil, err
			}
			vector[i] = val
		}
		vectors = append(vectors, vector)
	}

	return vectors, nil
}

// TestExportCSV tests exporting data to CSV format
func TestExportCSV(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_export_csv"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table with the same schema as Python test
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "integer"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV
	testCSVPath := testDataDir + "enwiki_embedding_9999.csv"
	if _, err := os.Stat(testCSVPath); os.IsNotExist(err) {
		t.Skipf("Test data file does not exist: %s", testCSVPath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		HasHeader:    false,
	}

	_, err = table.ImportData(testCSVPath, importOpts)
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Verify data count
	result, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query count: %v", err)
	}
	t.Logf("Table count result: %v", result)

	// Test 1: Basic CSV export
	exportCSVPath := filepath.Join(testDataDir, "test_export_csv.csv")
	deleteFile(exportCSVPath)

	exportOpts := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
	}

	_, err = table.ExportData(exportCSVPath, exportOpts, nil)
	if err != nil {
		t.Fatalf("Failed to export CSV: %v", err)
	}

	lineCount, err := countLines(exportCSVPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 9999 {
		t.Errorf("Expected 9999 lines, got %d", lineCount)
	}
	deleteFile(exportCSVPath)

	// Test 2: Export with offset
	exportOpts2 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		Offset:       1000,
	}

	_, err = table.ExportData(exportCSVPath, exportOpts2, nil)
	if err != nil {
		t.Fatalf("Failed to export CSV with offset: %v", err)
	}

	lineCount, err = countLines(exportCSVPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 8999 {
		t.Errorf("Expected 8999 lines (with offset 1000), got %d", lineCount)
	}
	deleteFile(exportCSVPath)

	// Test 3: Export with offset and limit
	exportOpts3 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		Offset:       1000,
		Limit:        2000,
	}

	_, err = table.ExportData(exportCSVPath, exportOpts3, nil)
	if err != nil {
		t.Fatalf("Failed to export CSV with offset and limit: %v", err)
	}

	lineCount, err = countLines(exportCSVPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 2000 {
		t.Errorf("Expected 2000 lines (with offset 1000, limit 2000), got %d", lineCount)
	}
	deleteFile(exportCSVPath)

	// Test 4: Export with offset, limit, and row_limit (creates part files)
	exportOpts4 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		Offset:       1000,
		Limit:        2000,
		RowLimit:     1500,
	}

	_, err = table.ExportData(exportCSVPath, exportOpts4, nil)
	if err != nil {
		t.Fatalf("Failed to export CSV with row_limit: %v", err)
	}

	lineCount, err = countLines(exportCSVPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 1500 {
		t.Errorf("Expected 1500 lines in main file (with row_limit 1500), got %d", lineCount)
	}
	deleteFile(exportCSVPath)

	// Check part file
	partFilePath := exportCSVPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partLineCount, err := countLines(partFilePath)
		if err != nil {
			t.Fatalf("Failed to count lines in part file: %v", err)
		}
		if partLineCount != 500 {
			t.Errorf("Expected 500 lines in part file, got %d", partLineCount)
		}
		deleteFile(partFilePath)
	}

	// Test 5: Export with row_limit only
	exportOpts5 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		RowLimit:     8192,
	}

	_, err = table.ExportData(exportCSVPath, exportOpts5, nil)
	if err != nil {
		t.Fatalf("Failed to export CSV with row_limit only: %v", err)
	}

	lineCount, err = countLines(exportCSVPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 8192 {
		t.Errorf("Expected 8192 lines (with row_limit 8192), got %d", lineCount)
	}
	deleteFile(exportCSVPath)

	// Check part file for remaining rows
	partFilePath = exportCSVPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partLineCount, err := countLines(partFilePath)
		if err != nil {
			t.Fatalf("Failed to count lines in part file: %v", err)
		}
		expectedPartLines := 9999 - 8192
		if partLineCount != expectedPartLines {
			t.Errorf("Expected %d lines in part file, got %d", expectedPartLines, partLineCount)
		}
		deleteFile(partFilePath)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestExportJSONL tests exporting data to JSONL format
func TestExportJSONL(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_export_jsonl"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table with the same schema as Python test
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "integer"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV
	testCSVPath := testDataDir + "enwiki_embedding_9999.csv"
	if _, err := os.Stat(testCSVPath); os.IsNotExist(err) {
		t.Skipf("Test data file does not exist: %s", testCSVPath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		HasHeader:    false,
	}

	_, err = table.ImportData(testCSVPath, importOpts)
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Verify data count
	result, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query count: %v", err)
	}
	t.Logf("Table count result: %v", result)

	// Test 1: Basic JSONL export
	exportJSONLPath := filepath.Join(testDataDir, "test_export_jsonl.jsonl")
	deleteFile(exportJSONLPath)

	exportOpts := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
		Delimiter:    '\t',
	}

	_, err = table.ExportData(exportJSONLPath, exportOpts, nil)
	if err != nil {
		t.Fatalf("Failed to export JSONL: %v", err)
	}

	lineCount, err := countLines(exportJSONLPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 9999 {
		t.Errorf("Expected 9999 lines, got %d", lineCount)
	}
	deleteFile(exportJSONLPath)

	// Test 2: Export with offset
	exportOpts2 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
		Delimiter:    '\t',
		Offset:       1000,
	}

	_, err = table.ExportData(exportJSONLPath, exportOpts2, nil)
	if err != nil {
		t.Fatalf("Failed to export JSONL with offset: %v", err)
	}

	lineCount, err = countLines(exportJSONLPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 8999 {
		t.Errorf("Expected 8999 lines (with offset 1000), got %d", lineCount)
	}
	deleteFile(exportJSONLPath)

	// Test 3: Export with offset and limit
	exportOpts3 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
		Delimiter:    '\t',
		Offset:       1000,
		Limit:        2000,
	}

	_, err = table.ExportData(exportJSONLPath, exportOpts3, nil)
	if err != nil {
		t.Fatalf("Failed to export JSONL with offset and limit: %v", err)
	}

	lineCount, err = countLines(exportJSONLPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 2000 {
		t.Errorf("Expected 2000 lines (with offset 1000, limit 2000), got %d", lineCount)
	}
	deleteFile(exportJSONLPath)

	// Test 4: Export with offset, limit, and row_limit (creates part files)
	exportOpts4 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
		Delimiter:    '\t',
		Offset:       1000,
		Limit:        2000,
		RowLimit:     1500,
	}

	_, err = table.ExportData(exportJSONLPath, exportOpts4, nil)
	if err != nil {
		t.Fatalf("Failed to export JSONL with row_limit: %v", err)
	}

	lineCount, err = countLines(exportJSONLPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 1500 {
		t.Errorf("Expected 1500 lines in main file (with row_limit 1500), got %d", lineCount)
	}
	deleteFile(exportJSONLPath)

	// Check part file
	partFilePath := exportJSONLPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partLineCount, err := countLines(partFilePath)
		if err != nil {
			t.Fatalf("Failed to count lines in part file: %v", err)
		}
		if partLineCount != 500 {
			t.Errorf("Expected 500 lines in part file, got %d", partLineCount)
		}
		deleteFile(partFilePath)
	}

	// Test 5: Export with row_limit only
	exportOpts5 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
		Delimiter:    '\t',
		RowLimit:     8192,
	}

	_, err = table.ExportData(exportJSONLPath, exportOpts5, nil)
	if err != nil {
		t.Fatalf("Failed to export JSONL with row_limit only: %v", err)
	}

	lineCount, err = countLines(exportJSONLPath)
	if err != nil {
		t.Fatalf("Failed to count lines: %v", err)
	}
	if lineCount != 8192 {
		t.Errorf("Expected 8192 lines (with row_limit 8192), got %d", lineCount)
	}
	deleteFile(exportJSONLPath)

	// Check part file for remaining rows
	partFilePath = exportJSONLPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partLineCount, err := countLines(partFilePath)
		if err != nil {
			t.Fatalf("Failed to count lines in part file: %v", err)
		}
		expectedPartLines := 9999 - 8192
		if partLineCount != expectedPartLines {
			t.Errorf("Expected %d lines in part file, got %d", expectedPartLines, partLineCount)
		}
		deleteFile(partFilePath)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestExportFVECS tests exporting data to FVECS format
func TestExportFVECS(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_export_fvecs"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table with the same schema as Python test
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "integer"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV
	testCSVPath := testDataDir + "enwiki_embedding_9999.csv"
	if _, err := os.Stat(testCSVPath); os.IsNotExist(err) {
		t.Skipf("Test data file does not exist: %s", testCSVPath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
		Delimiter:    '\t',
		HasHeader:    false,
	}

	_, err = table.ImportData(testCSVPath, importOpts)
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Verify data count
	result, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query count: %v", err)
	}
	t.Logf("Table count result: %v", result)

	// Test 1: Basic FVECS export
	exportFVECSPath := filepath.Join(testDataDir, "test_export_fvecs.fvecs")
	deleteFile(exportFVECSPath)

	exportOpts := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
	}

	_, err = table.ExportData(exportFVECSPath, exportOpts, []string{"vec"})
	if err != nil {
		t.Fatalf("Failed to export FVECS: %v", err)
	}

	vectors, err := readFvecsFile(exportFVECSPath)
	if err != nil {
		t.Fatalf("Failed to read FVECS file: %v", err)
	}
	if len(vectors) != 9999 {
		t.Errorf("Expected 9999 vectors, got %d", len(vectors))
	}
	deleteFile(exportFVECSPath)

	// Test 2: Export with offset
	exportOpts2 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
		Offset:       1000,
	}

	_, err = table.ExportData(exportFVECSPath, exportOpts2, []string{"vec"})
	if err != nil {
		t.Fatalf("Failed to export FVECS with offset: %v", err)
	}

	vectors, err = readFvecsFile(exportFVECSPath)
	if err != nil {
		t.Fatalf("Failed to read FVECS file: %v", err)
	}
	if len(vectors) != 8999 {
		t.Errorf("Expected 8999 vectors (with offset 1000), got %d", len(vectors))
	}
	deleteFile(exportFVECSPath)

	// Test 3: Export with offset and limit
	exportOpts3 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
		Offset:       1000,
		Limit:        2000,
	}

	_, err = table.ExportData(exportFVECSPath, exportOpts3, []string{"vec"})
	if err != nil {
		t.Fatalf("Failed to export FVECS with offset and limit: %v", err)
	}

	vectors, err = readFvecsFile(exportFVECSPath)
	if err != nil {
		t.Fatalf("Failed to read FVECS file: %v", err)
	}
	if len(vectors) != 2000 {
		t.Errorf("Expected 2000 vectors (with offset 1000, limit 2000), got %d", len(vectors))
	}
	deleteFile(exportFVECSPath)

	// Test 4: Export with offset, limit, and row_limit (creates part files)
	exportOpts4 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
		Offset:       1000,
		Limit:        2000,
		RowLimit:     1500,
	}

	_, err = table.ExportData(exportFVECSPath, exportOpts4, []string{"vec"})
	if err != nil {
		t.Fatalf("Failed to export FVECS with row_limit: %v", err)
	}

	vectors, err = readFvecsFile(exportFVECSPath)
	if err != nil {
		t.Fatalf("Failed to read FVECS file: %v", err)
	}
	if len(vectors) != 1500 {
		t.Errorf("Expected 1500 vectors in main file (with row_limit 1500), got %d", len(vectors))
	}
	deleteFile(exportFVECSPath)

	// Check part file
	partFilePath := exportFVECSPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partVectors, err := readFvecsFile(partFilePath)
		if err != nil {
			t.Fatalf("Failed to read FVECS part file: %v", err)
		}
		if len(partVectors) != 500 {
			t.Errorf("Expected 500 vectors in part file, got %d", len(partVectors))
		}
		deleteFile(partFilePath)
	}

	// Test 5: Export with row_limit only
	exportOpts5 := &infinity.ExportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
		RowLimit:     8192,
	}

	_, err = table.ExportData(exportFVECSPath, exportOpts5, []string{"vec"})
	if err != nil {
		t.Fatalf("Failed to export FVECS with row_limit only: %v", err)
	}

	vectors, err = readFvecsFile(exportFVECSPath)
	if err != nil {
		t.Fatalf("Failed to read FVECS file: %v", err)
	}
	if len(vectors) != 8192 {
		t.Errorf("Expected 8192 vectors (with row_limit 8192), got %d", len(vectors))
	}
	deleteFile(exportFVECSPath)

	// Check part file for remaining rows
	partFilePath = exportFVECSPath + ".part1"
	if _, err := os.Stat(partFilePath); err == nil {
		partVectors, err := readFvecsFile(partFilePath)
		if err != nil {
			t.Fatalf("Failed to read FVECS part file: %v", err)
		}
		expectedPartVectors := 9999 - 8192
		if len(partVectors) != expectedPartVectors {
			t.Errorf("Expected %d vectors in part file, got %d", expectedPartVectors, len(partVectors))
		}
		deleteFile(partFilePath)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}
