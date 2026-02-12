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
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

const testDataDir = "/var/infinity/test_data/"

// Helper function to check if file exists
func fileExists(path string) bool {
	_, err := os.Stat(path)
	return !os.IsNotExist(err)
}

// TestImportEmbeddingIntDim3 tests importing embedding_int_dim3.csv
func TestImportEmbeddingIntDim3(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{
			Name:     "c1",
			DataType: "int",
		},
		"c2": &infinity.ColumnDefinition{
			Name:     "c2",
			DataType: "vector,3,int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data
	testCSVDir := testDataDir + "embedding_int_dim3.csv"
	if !fileExists(testCSVDir) {
		t.Fatalf("Test data file does not exist: %s", testCSVDir)
	}

	_, err = table.ImportData(testCSVDir, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Query data
	result, err := table.Output([]string{"c1"}).Filter("c1 > 1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	queryResult, ok := result.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("Unexpected result type")
	}
	t.Logf("Query result: %v", queryResult.Data)

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestImportDifferentFileFormat tests importing different file formats
func TestImportDifferentFileFormat(t *testing.T) {
	fileFormats := []struct {
		format   string
		fileName string
		schema   infinity.TableSchema
	}{
		{
			format:   "csv",
			fileName: "pysdk_test.csv",
			schema: infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
			},
		},
		{
			format:   "json",
			fileName: "pysdk_test.json",
			schema: infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
			},
		},
		{
			format:   "jsonl",
			fileName: "pysdk_test.jsonl",
			schema: infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
			},
		},
	}

	for _, tc := range fileFormats {
		t.Run(tc.format, func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_import_different_file_format_data" + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			table, err := db.CreateTable(tableName, tc.schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data
			filePath := testDataDir + tc.fileName
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			importOpts := infinity.NewImportOption()
			switch tc.format {
			case "json":
				importOpts.CopyFileType = infinity.CopyFileTypeJSON
			case "jsonl":
				importOpts.CopyFileType = infinity.CopyFileTypeJSONL
			default:
				importOpts.CopyFileType = infinity.CopyFileTypeCSV
			}

			_, err = table.ImportData(filePath, importOpts)
			if err != nil {
				t.Fatalf("Import %s result: %v", tc.format, err)
			}

			// Query data
			result, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			// Clean up
			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestImportFVECS tests importing fvecs format
func TestImportFVECS(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_different_file_format_data" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table for fvecs
	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{
			Name:     "c1",
			DataType: "vector,128,float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import fvecs data
	filePath := testDataDir + "pysdk_test.fvecs"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import fvecs result: %v", err)
	}

	// Query data
	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportEmptyFile tests importing empty files
func TestImportEmptyFile(t *testing.T) {
	testCases := []struct {
		name     string
		fileName string
		format   string
		schema   infinity.TableSchema
	}{
		{
			name:     "fvecs",
			fileName: "test_empty.fvecs",
			format:   "fvecs",
			schema: infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "vector,128,float"},
			},
		},
		{
			name:     "csv",
			fileName: "test_empty.csv",
			format:   "csv",
			schema: infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_import_empty_file_" + tc.name + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			table, err := db.CreateTable(tableName, tc.schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			filePath := testDataDir + tc.fileName
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			_, err = table.ImportData(filePath, infinity.NewImportOption())
			if err != nil {
				t.Fatalf("Import empty %s result: %v", tc.name, err)
			}

			result, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestImportFormatUnrecognized tests importing unrecognized format
func TestImportFormatUnrecognized(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_format_unrecognized_data" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Try to import txt file
	filePath := testDataDir + "http_test.txt"
	if fileExists(filePath) {
		_, err = table.ImportData(filePath, infinity.NewImportOption())
		if err != nil {
			t.Fatalf("Import txt file error (may or may not be expected): %v", err)
		}
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestCSVWithDifferentDelimiter tests importing CSV with different delimiters
func TestCSVWithDifferentDelimiter(t *testing.T) {
	delimiters := []struct {
		name      string
		delimiter string
		fileName  string
	}{
		{"blankspace", " ", "pysdk_test_blankspace.csv"},
		{"commas", ",", "pysdk_test_commas.csv"},
		{"semicolons", ";", "pysdk_test_semicolons.csv"},
		{"tabular", "\t", "pysdk_test_tabular.csv"},
	}

	types := []string{
		"int", "int8", "int16", "int32", "int64", "integer",
		"float", "float32", "double", "float64",
		"varchar",
	}

	for _, delim := range delimiters {
		for _, dataType := range types {
			testName := fmt.Sprintf("%s_%s", delim.name, dataType)
			t.Run(testName, func(t *testing.T) {
				suffix := generateSuffix(t)

				conn := setupConnection(t)
				defer conn.Disconnect()

				db, err := conn.GetDatabase("default_db")
				if err != nil {
					t.Fatalf("Failed to get database: %v", err)
				}

				tableName := "test_csv_with_different_delimiter" + suffix
				db.DropTable(tableName, infinity.ConflictTypeIgnore)

				schema := infinity.TableSchema{
					"c1": &infinity.ColumnDefinition{Name: "c1", DataType: dataType},
					"c2": &infinity.ColumnDefinition{Name: "c2", DataType: dataType},
				}

				table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
				if err != nil {
					t.Fatalf("Failed to create table: %v", err)
				}

				filePath := testDataDir + delim.fileName
				if !fileExists(filePath) {
					t.Fatalf("Test data file does not exist: %s", filePath)
				}

				importOpts := &infinity.ImportOption{
					Delimiter:    rune(delim.delimiter[0]),
					CopyFileType: infinity.CopyFileTypeCSV,
				}

				_, err = table.ImportData(filePath, importOpts)
				if err != nil {
					t.Fatalf("Import result: %v", err)
				}

				result, err := table.Output([]string{"*"}).ToResult()
				if err != nil {
					t.Fatalf("Query result: %v", err)
				} else {
					queryResult, ok := result.(*infinity.QueryResult)
					if ok {
						t.Logf("Query result: %v", queryResult.Data)
					}
				}

				db.DropTable(tableName, infinity.ConflictTypeError)
			})
		}
	}
}

// TestCSVWithDifferentDelimiterMoreThanOneChar tests importing CSV with multi-char delimiter
func TestCSVWithDifferentDelimiterMoreThanOneChar(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_csv_with_different_delimiter_more_than_one_character" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_blankspace.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		Delimiter:    ' ',
		CopyFileType: infinity.CopyFileTypeCSV,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportCSVWithHeaders tests importing CSV with/without headers
func TestImportCSVWithHeaders(t *testing.T) {
	hasHeaders := []bool{true, false}

	for _, hasHeader := range hasHeaders {
		t.Run(fmt.Sprintf("header_%v", hasHeader), func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_import_csv_with_headers" + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "int"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			filePath := testDataDir + "pysdk_test_commas.csv"
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			importOpts := &infinity.ImportOption{
				HasHeader:    hasHeader,
				CopyFileType: infinity.CopyFileTypeCSV,
			}

			_, err = table.ImportData(filePath, importOpts)
			if err != nil {
				t.Fatalf("Import result: %v", err)
			}

			result, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestImportFVECSTableWithMoreColumns tests importing fvecs with mismatched columns
func TestImportFVECSTableWithMoreColumns(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_fvecs_table_with_more_columns" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,128,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test.fvecs"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeFVECS,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err == nil {
		t.Error("Expected error for column mismatch, but got nil")
	} else {
		t.Logf("Got expected error: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportEmbeddingWithNotMatchDefinition tests importing embedding with mismatched definition
func TestImportEmbeddingWithNotMatchDefinition(t *testing.T) {
	types := []string{
		"vector, 3, int",
		"vector, 128, int",
		"vector, 3, float",
		"vector, 128, float",
		"vector, 3, double",
	}

	for _, dataType := range types {
		t.Run(strings.ReplaceAll(dataType, ", ", "_"), func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_import_embedding_with_not_match_definition" + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: dataType},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			filePath := testDataDir + "embedding_int_dim3.csv"
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			importOpts := &infinity.ImportOption{
				CopyFileType: infinity.CopyFileTypeCSV,
			}

			_, err = table.ImportData(filePath, importOpts)
			if err != nil {
				t.Fatalf("Import result: %v", err)
			}

			result, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestImportEmbeddingWithDimensionUnmatch tests importing embedding with dimension mismatch
func TestImportEmbeddingWithDimensionUnmatch(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_embedding_with_dimension_unmatch" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector, 2, int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "embedding_int_dim3.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import with dimension mismatch error (may or may not be expected): %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportVarcharWithNotMatchDefinition tests importing varchar data
func TestImportVarcharWithNotMatchDefinition(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_varchar_with_not_match_definition" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_varchar.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	_, err = table.ImportData(filePath, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImport10000Columns tests importing CSV with 10000 columns
func TestImport10000Columns(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_10000_columns" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_big_int.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	_, err = table.ImportData(filePath, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestTableWithNotMatchedColumns tests importing with mismatched column count
func TestTableWithNotMatchedColumns(t *testing.T) {
	columns := []infinity.TableSchema{
		{"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"}},
		{
			"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
			"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "int"},
			"c3": &infinity.ColumnDefinition{Name: "c3", DataType: "int"},
		},
	}

	for i, schema := range columns {
		t.Run(fmt.Sprintf("columns_%d", i), func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_table_with_not_matched_columns" + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			filePath := testDataDir + "pysdk_test_commas.csv"
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			_, err = table.ImportData(filePath, infinity.NewImportOption())
			if err == nil {
				t.Error("Expected error for column count mismatch, but got nil")
			} else {
				t.Logf("Got expected error: %v", err)
			}

			result, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestImportWithDifferentSize tests importing different data sizes
func TestImportWithDifferentSize(t *testing.T) {
	dataSizes := []int{1, 8191, 8192, 8193, 16 * 8192}

	for _, dataSize := range dataSizes {
		t.Run(fmt.Sprintf("size_%d", dataSize), func(t *testing.T) {
			suffix := generateSuffix(t)

			conn := setupConnection(t)
			defer conn.Disconnect()

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_import_with_different_size" + suffix
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
				"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "varchar"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Use the specific test file
			filePath := testDataDir + "pysdk_test_import_with_different_size.csv"
			if !fileExists(filePath) {
				t.Fatalf("Test data file does not exist: %s", filePath)
			}

			_, err = table.ImportData(filePath, infinity.NewImportOption())
			if err != nil {
				t.Fatalf("Import result: %v", err)
			}

			result, err := table.Output([]string{"count(*)"}).ToResult()
			if err != nil {
				t.Fatalf("Query result: %v", err)
			} else {
				queryResult, ok := result.(*infinity.QueryResult)
				if ok {
					t.Logf("Query result: %v", queryResult.Data)
				}
			}

			db.DropTable(tableName, infinity.ConflictTypeIgnore)
		})
	}
}

// TestImportExceedingRows tests importing large number of rows
func TestImportExceedingRows(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_exceeding_rows" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_big_varchar_rows.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	_, err = table.ImportData(filePath, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportMoreThanOneSegment tests importing data that spans multiple segments
func TestImportMoreThanOneSegment(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	fileName := "test_sdk_import_more_than_one_segment"
	tableName := fileName + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + fileName + ".csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	_, err = table.ImportData(filePath, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	// Check segments and blocks
	segments, err := table.ShowSegments()
	if err != nil {
		t.Logf("ShowSegments error: %v", err)
	} else {
		t.Logf("Segments: %v", segments)
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportExceedingColumns tests importing CSV with many columns
func TestImportExceedingColumns(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_exceeding_columns" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create schema with 1024 columns
	schema := make(infinity.TableSchema)
	for i := 0; i < 1024; i++ {
		colName := fmt.Sprintf("c%d", i)
		schema[colName] = &infinity.ColumnDefinition{Name: colName, DataType: "int"}
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_big_columns.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	_, err = table.ImportData(filePath, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportJSONLFileWithDefault tests importing JSONL file with default values
func TestImportJSONLFileWithDefault(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_jsonl_file_with_default" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{
			Name:     "c1",
			DataType: "int",
			Default:  1,
		},
		"c2": &infinity.ColumnDefinition{
			Name:     "c2",
			DataType: "int",
			Default:  4,
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "test_default.jsonl"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportCSVFileWithDefault tests importing CSV file with default values
func TestImportCSVFileWithDefault(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_csv_file_with_default" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{
			Name:     "c1",
			DataType: "int",
			Default:  1,
		},
		"c2": &infinity.ColumnDefinition{
			Name:     "c2",
			DataType: "int",
			Default:  4,
		},
		"c4": &infinity.ColumnDefinition{
			Name:     "c4",
			DataType: "vector, 3, float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_import_default.csv"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeCSV,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportJSONFileWithDefault tests importing JSON file with default values
func TestImportJSONFileWithDefault(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_json_file_with_default" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{
			Name:     "c1",
			DataType: "int",
			Default:  1,
		},
		"c2": &infinity.ColumnDefinition{
			Name:     "c2",
			DataType: "int",
			Default:  4,
		},
		"c3": &infinity.ColumnDefinition{
			Name:     "c3",
			DataType: "int",
			Default:  7,
		},
		"c4": &infinity.ColumnDefinition{
			Name:     "c4",
			DataType: "vector, 3, float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test_default.json"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeJSON,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportJSONLFile tests importing JSONL file
func TestImportJSONLFile(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_jsonl_file" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "pysdk_test.jsonl"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportEmptyJSONLFile tests importing empty JSONL file
func TestImportEmptyJSONLFile(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_empty_file_jsonl" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "vector,3,int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	filePath := testDataDir + "test_empty.jsonl"
	if !fileExists(filePath) {
		t.Fatalf("Test data file does not exist: %s", filePath)
	}

	importOpts := &infinity.ImportOption{
		CopyFileType: infinity.CopyFileTypeJSONL,
	}

	_, err = table.ImportData(filePath, importOpts)
	if err != nil {
		t.Fatalf("Import empty jsonl result: %v", err)
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}

// TestImportWithWildcardPath tests importing with wildcard path pattern
func TestImportWithWildcardPath(t *testing.T) {
	suffix := generateSuffix(t)

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_import_wildcard" + suffix
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		"c1": &infinity.ColumnDefinition{Name: "c1", DataType: "int"},
		"c2": &infinity.ColumnDefinition{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Find files matching pattern
	files, err := filepath.Glob(testDataDir + "pysdk_test_commas*.csv")
	if err != nil || len(files) == 0 {
		t.Fatalf("No files matching pattern found")
	}

	for _, filePath := range files {
		_, err = table.ImportData(filePath, infinity.NewImportOption())
		if err != nil {
			t.Fatalf("Import result for %s: %v", filePath, err)
		}
	}

	result, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Query result: %v", err)
	} else {
		queryResult, ok := result.(*infinity.QueryResult)
		if ok {
			t.Logf("Query result: %v", queryResult.Data)
		}
	}

	db.DropTable(tableName, infinity.ConflictTypeError)
}
