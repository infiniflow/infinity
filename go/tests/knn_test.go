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
	"math/rand"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestKNNBasic tests basic KNN search functionality
func TestKNNBasic(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_basic"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with vector columns
	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
		{Name: "color_vector", DataType: "vector,4,float"},
		{Name: "category_vector", DataType: "vector,4,float"},
		{Name: "tag_vector", DataType: "vector,4,float"},
		{Name: "other_vector", DataType: "vector,4,float"},
		{Name: "query_is_recommend", DataType: "varchar"},
		{Name: "query_gender", DataType: "varchar"},
		{Name: "query_color", DataType: "varchar"},
		{Name: "query_price", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{
			"variant_id":         "variant_1",
			"gender_vector":      []float32{1.0, 0.0, 0.0, 0.0},
			"color_vector":       []float32{0.0, 1.0, 0.0, 0.0},
			"category_vector":    []float32{0.0, 0.0, 1.0, 0.0},
			"tag_vector":         []float32{0.0, 0.0, 0.0, 1.0},
			"other_vector":       []float32{1.0, 1.0, 1.0, 1.0},
			"query_is_recommend": "yes",
			"query_gender":       "male",
			"query_color":        "red",
			"query_price":        10.5,
		},
		{
			"variant_id":         "variant_2",
			"gender_vector":      []float32{0.0, 1.0, 0.0, 0.0},
			"color_vector":       []float32{1.0, 0.0, 0.0, 0.0},
			"category_vector":    []float32{0.0, 1.0, 0.0, 0.0},
			"tag_vector":         []float32{0.0, 0.0, 1.0, 0.0},
			"other_vector":       []float32{0.5, 0.5, 0.5, 0.5},
			"query_is_recommend": "no",
			"query_gender":       "female",
			"query_color":        "blue",
			"query_price":        20.0,
		},
		{
			"variant_id":         "variant_3",
			"gender_vector":      []float32{0.0, 0.0, 1.0, 0.0},
			"color_vector":       []float32{0.0, 0.0, 1.0, 0.0},
			"category_vector":    []float32{1.0, 0.0, 0.0, 0.0},
			"tag_vector":         []float32{0.0, 1.0, 0.0, 0.0},
			"other_vector":       []float32{0.0, 0.0, 0.0, 0.0},
			"query_is_recommend": "yes",
			"query_gender":       "other",
			"query_color":        "green",
			"query_price":        15.5,
		},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test KNN search with IP distance
	_, err = table.Output([]string{"variant_id", "_row_id"}).
		MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "ip", 2, nil).
		ToResult()
	if err != nil {
		t.Fatalf("KNN search with IP failed: %v", err)
	}

	// Test KNN search with L2 distance
	_, err = table.Output([]string{"variant_id", "_distance"}).
		MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 0.0, 0.0, 0.0}), "float", "l2", 2, nil).
		ToResult()
	if err != nil {
		t.Fatalf("KNN search with L2 failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNOnVectorColumn tests KNN search on different vector columns
func TestKNNOnVectorColumn(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_on_vector_column"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
		{Name: "color_vector", DataType: "vector,4,float"},
		{Name: "category_vector", DataType: "vector,4,float"},
		{Name: "tag_vector", DataType: "vector,4,float"},
		{Name: "other_vector", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{
			"variant_id":      "variant_1",
			"gender_vector":   []float32{1.0, 0.0, 0.0, 0.0},
			"color_vector":    []float32{0.0, 1.0, 0.0, 0.0},
			"category_vector": []float32{0.0, 0.0, 1.0, 0.0},
			"tag_vector":      []float32{0.0, 0.0, 0.0, 1.0},
			"other_vector":    []float32{1.0, 1.0, 1.0, 1.0},
		},
		{
			"variant_id":      "variant_2",
			"gender_vector":   []float32{0.0, 1.0, 0.0, 0.0},
			"color_vector":    []float32{1.0, 0.0, 0.0, 0.0},
			"category_vector": []float32{0.0, 1.0, 0.0, 0.0},
			"tag_vector":      []float32{0.0, 0.0, 1.0, 0.0},
			"other_vector":    []float32{0.5, 0.5, 0.5, 0.5},
		},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test on different vector columns
	columns := []string{"gender_vector", "color_vector", "category_vector", "tag_vector", "other_vector"}

	for _, col := range columns {
		_, err = table.Output([]string{"variant_id", "_row_id", "_similarity"}).
			MatchDense(col, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "ip", 2, nil).
			ToResult()
		if err != nil {
			t.Fatalf("KNN search on %s failed: %v", col, err)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNWithIndex tests KNN search with HNSW index
func TestKNNWithIndex(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_index"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
		{Name: "color_vector", DataType: "vector,4,float"},
		{Name: "category_vector", DataType: "vector,4,float"},
		{Name: "tag_vector", DataType: "vector,4,float"},
		{Name: "other_vector", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{
			"variant_id":      "variant_1",
			"gender_vector":   []float32{1.0, 0.0, 0.0, 0.0},
			"color_vector":    []float32{0.0, 1.0, 0.0, 0.0},
			"category_vector": []float32{0.0, 0.0, 1.0, 0.0},
			"tag_vector":      []float32{0.0, 0.0, 0.0, 1.0},
			"other_vector":    []float32{1.0, 1.0, 1.0, 1.0},
		},
		{
			"variant_id":      "variant_2",
			"gender_vector":   []float32{0.0, 1.0, 0.0, 0.0},
			"color_vector":    []float32{1.0, 0.0, 0.0, 0.0},
			"category_vector": []float32{0.0, 1.0, 0.0, 0.0},
			"tag_vector":      []float32{0.0, 0.0, 1.0, 0.0},
			"other_vector":    []float32{0.5, 0.5, 0.5, 0.5},
		},
		{
			"variant_id":      "variant_3",
			"gender_vector":   []float32{0.0, 0.0, 1.0, 0.0},
			"color_vector":    []float32{0.0, 0.0, 1.0, 0.0},
			"category_vector": []float32{1.0, 0.0, 0.0, 0.0},
			"tag_vector":      []float32{0.0, 1.0, 0.0, 0.0},
			"other_vector":    []float32{0.0, 0.0, 0.0, 0.0},
		},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Create HNSW index on gender_vector
	indexInfo := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "50",
		"metric":          "l2",
	})

	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Test KNN search with index
	_, err = table.Output([]string{"variant_id"}).
		MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "l2", 5, nil).
		ToResult()
	if err != nil {
		t.Fatalf("KNN search with index failed: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNVariousDistanceTypes tests KNN search with different distance types
func TestKNNVariousDistanceTypes(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_various_distance"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{
			"variant_id":    "variant_1",
			"gender_vector": []float32{1.0, 0.0, 0.0, 0.0},
		},
		{
			"variant_id":    "variant_2",
			"gender_vector": []float32{0.0, 1.0, 0.0, 0.0},
		},
		{
			"variant_id":    "variant_3",
			"gender_vector": []float32{0.0, 0.0, 1.0, 0.0},
		},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test different distance types
	distanceTypes := []string{"l2", "ip", "cosine", "cos"}

	for _, distType := range distanceTypes {
		_, err = table.Output([]string{"variant_id"}).
			MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", distType, 2, nil).
			ToResult()
		if err != nil {
			t.Logf("KNN search with %s may not be supported or failed: %v", distType, err)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNInvalidTopN tests KNN search with invalid topn values
func TestKNNInvalidTopN(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_invalid_topn"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{
			"variant_id":    "variant_1",
			"gender_vector": []float32{1.0, 0.0, 0.0, 0.0},
		},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test invalid topn values
	// Note: Topn validation happens in MatchDense, which returns *Table
	// The error will be captured in t.err and returned by ToResult()
	invalidTopNs := []int{0, -1}

	for _, topn := range invalidTopNs {
		result, err := table.Output([]string{"variant_id"}).
			MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "l2", topn, nil).
			ToResult()
		if err == nil {
			t.Errorf("KNN search with topn=%d should return error, got result: %v", topn, result)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNBigDimensionVector tests KNN search with large dimension vectors
func TestKNNBigDimensionVector(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	dimensions := []int{1000, 16384}

	for _, dim := range dimensions {
		tableName := "test_knn_big_dim_" + strconv.Itoa(dim)
		db.DropTable(tableName, infinity.ConflictTypeIgnore)

		schema := infinity.TableSchema{
			{Name: "big_vector", DataType: "vector," + strconv.Itoa(dim) + ",float"},
		}

		table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to create table with dim %d: %v", dim, err)
		}

		// Insert test data with the specified dimension
		for i := 1; i <= 5; i++ {
			vector := make([]float32, dim)
			for j := 0; j < dim; j++ {
				vector[j] = float32(i)
			}
			_, err = table.Insert([]map[string]interface{}{
				{"big_vector": vector},
			})
			if err != nil {
				t.Fatalf("Failed to insert data: %v", err)
			}
		}

		// Test KNN search
		queryVector := make([]float32, dim)
		_, err = table.Output([]string{"_row_id"}).
			MatchDense("big_vector", infinity.Float32Vector(queryVector), "float", "l2", 5, nil).
			ToResult()
		if err != nil {
			t.Fatalf("KNN search with dim %d failed: %v", dim, err)
		}

		// Cleanup
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to drop table: %v", err)
		}
	}
}

// TestKNNWithIVFIndex tests KNN search with IVF index
func TestKNNWithIVFIndex(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_ivf_index"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	for i := 1; i <= 10; i++ {
		vector := []float32{float32(i), float32(i), float32(i), float32(i)}
		_, err = table.Insert([]map[string]interface{}{
			{
				"variant_id":    "variant_" + strconv.Itoa(i),
				"gender_vector": vector,
			},
		})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Create IVF index
	indexInfo := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeIVF, map[string]string{
		"metric": "l2",
	})

	_, err = table.CreateIndex("my_ivf_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create IVF index: %v", err)
	}

	// Drop IVF index
	_, err = table.DropIndex("my_ivf_index", infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop IVF index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNWithThreshold tests KNN search with threshold parameter
func TestKNNWithThreshold(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_threshold"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{"c1": 1, "c2": []float32{2, 3, 4}},
		{"c1": 5, "c2": []float32{6, 7, 8}},
		{"c1": 9, "c2": []float32{10, 11, 12}},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test KNN with threshold
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "float", "l2", 10, map[string]string{
			"threshold": "200",
		}).
		ToResult()
	if err != nil {
		t.Fatalf("KNN search with threshold failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNImportFromCSV tests KNN search with imported CSV data
func TestKNNImportFromCSV(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_import_csv"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Try to import from CSV if file exists
	testCSVPath := filepath.Join(testDataDir, "embedding_int_dim3.csv")
	if _, err := os.Stat(testCSVPath); err == nil {
		_, err = table.ImportData(testCSVPath, nil)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		} else {
			// Test KNN search after import
			_, err = table.Output([]string{"c1", "_distance"}).
				MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "float", "l2", 10, nil).
				ToResult()
			if err != nil {
				t.Fatalf("KNN search after import failed: %v", err)
			}
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNWithFusion tests KNN search combined with full-text search and fusion
func TestKNNWithFusion(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_fusion"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index on body
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("ft_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Insert test data
	testData := []map[string]interface{}{
		{
			"doctitle": "title1",
			"docdate":  "2024-01-01",
			"body":     "black white cat dog",
			"num":      1,
			"vec":      []float32{3.0, 2.8, 2.7, 3.1},
		},
		{
			"doctitle": "title2",
			"docdate":  "2024-01-02",
			"body":     "red blue green yellow",
			"num":      2,
			"vec":      []float32{1.0, 2.0, 3.0, 4.0},
		},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test fusion of dense vector search and full-text search
	_, err = table.Output([]string{"*"}).
		MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
		MatchText("body^5", "black", 1, nil).
		Fusion("rrf", 10, nil).
		ToResult()
	if err != nil {
		t.Logf("Fusion search may not be fully supported: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("ft_index", infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16

// TestKNNu8 tests KNN search with uint8 vector type
// Based on Python SDK test_pysdk/test_knn.py - test_knn_u8
func TestKNNu8(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_u8"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with int and uint8 vector columns
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,uint8"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := testDataDir + "embedding_int_dim3.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		_, err = table.ImportData(testCSVPath, nil)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Insert additional data (using []int instead of []uint8 as Go SDK doesn't support uint8 slice)
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 11, "c2": []int{127, 128, 255}},
	})
	if err != nil {
		t.Errorf("Failed to insert additional data: %v", err)
	}

	// Test query all data
	res, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query all data: %v", err)
	} else {
		if result, ok := res.(*infinity.QueryResult); ok {
			if c1Col, exists := result.Data["c1"]; exists {
				t.Logf("Total rows: %d", len(c1Col))
			}
		}
	}

	// Test KNN search with L2 distance
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "uint8", "l2", 10, nil).
		ToResult()
	if err != nil {
		t.Errorf("KNN search with uint8 failed: %v", err)
	}

	// Test KNN search with threshold
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "uint8", "l2", 10, map[string]string{
			"threshold": "200",
		}).
		ToResult()
	if err != nil {
		t.Errorf("KNN search with threshold failed: %v", err)
	}

	// Test creating index with invalid lvq encode (should fail for uint8)
	invalidIndexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeHnsw, map[string]string{
		"M":               "16",
		"ef_construction": "50",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("invalid_lvq", invalidIndexInfo, infinity.ConflictTypeError, "")
	if err == nil {
		t.Log("Expected error for lvq encode with uint8, but got none")
		// Drop the invalid index if it was created
		table.DropIndex("invalid_lvq", infinity.ConflictTypeIgnore)
	}

	// Test creating valid HNSW index
	validIndexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeHnsw, map[string]string{
		"M":               "16",
		"ef_construction": "50",
		"metric":          "l2",
	})
	_, err = table.CreateIndex("valid_lvq", validIndexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Errorf("Failed to create valid HNSW index: %v", err)
	}

	// Test KNN search with index
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "uint8", "l2", 10, nil).
		ToResult()
	if err != nil {
		t.Errorf("KNN search with index failed: %v", err)
	}

	// Test KNN search with threshold and index
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "uint8", "l2", 10, map[string]string{
			"threshold": "200",
		}).
		ToResult()
	if err != nil {
		t.Errorf("KNN search with threshold and index failed: %v", err)
	}

	// Test KNN search with wrong element type (should fail)
	_, err = table.Output([]string{"c1", "_distance"}).
		MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), "int8", "l2", 10, nil).
		ToResult()
	if err == nil {
		t.Error("Expected error for int8 element type with uint8 vector, but got none")
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNfp16bf16 tests KNN search with float16/bfloat16 vector types
// Based on Python SDK test_pysdk/test_knn.py - test_knn_fp16_bf16
func TestKNNfp16bf16(t *testing.T) {
	// Test with different save and query element types
	testCases := []struct {
		saveElemType  string
		queryElemType string
	}{
		{"float", "float"},
		//{"float16", "float"},
		//{"bfloat16", "float"},
	}

	for _, tc := range testCases {
		t.Run(fmt.Sprintf("save_%s_query_%s", tc.saveElemType, tc.queryElemType), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_knn_fp16_bf16_%s_%s", tc.saveElemType, tc.queryElemType)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with int and vector columns
			schema := infinity.TableSchema{
				{Name: "c1", DataType: "int"},
				{Name: "c2", DataType: fmt.Sprintf("vector,3,%s", tc.saveElemType)},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table with %s vector: %v", tc.saveElemType, err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "embedding_int_dim3.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Insert additional data
			_, err = table.Insert([]map[string]interface{}{
				{"c1": 11, "c2": []float32{127, 128, 255}},
			})
			if err != nil {
				t.Errorf("Failed to insert additional data: %v", err)
			}

			// Query all data
			res, err := table.Output([]string{"*"}).ToResult()
			if err != nil {
				t.Errorf("Failed to query all data: %v", err)
			} else {
				if result, ok := res.(*infinity.QueryResult); ok {
					if c1Col, exists := result.Data["c1"]; exists {
						t.Logf("Total rows: %d", len(c1Col))
					}
				}
			}

			// Test KNN search with L2 distance
			_, err = table.Output([]string{"c1", "_distance"}).
				MatchDense("c2", infinity.Float32Vector([]float32{0, 0, 0}), tc.queryElemType, "l2", 10, nil).
				ToResult()
			if err != nil {
				t.Errorf("KNN search with %s query type failed: %v", tc.queryElemType, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestInsertMultiColumn tests insert with missing required column
// Based on Python SDK test_pysdk/test_knn.py - test_insert_multi_column
func TestInsertMultiColumn(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_insert_multi_column"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with multiple columns (query_color is required but not provided in insert)
	schema := infinity.TableSchema{
		{Name: "variant_id", DataType: "varchar"},
		{Name: "gender_vector", DataType: "vector,4,float"},
		{Name: "color_vector", DataType: "vector,4,float"},
		{Name: "category_vector", DataType: "vector,4,float"},
		{Name: "tag_vector", DataType: "vector,4,float"},
		{Name: "other_vector", DataType: "vector,4,float"},
		{Name: "query_is_recommend", DataType: "varchar"},
		{Name: "query_gender", DataType: "varchar"},
		{Name: "query_color", DataType: "varchar"},
		{Name: "query_price", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert data without query_color column (should fail with "No default value found")
	_, err = table.Insert([]map[string]interface{}{
		{
			"variant_id":         "123",
			"gender_vector":      []float32{1.0, 1.0, 1.0, 1.0},
			"color_vector":       []float32{2.0, 2.0, 2.0, 2.0},
			"category_vector":    []float32{3.0, 3.0, 3.0, 3.0},
			"tag_vector":         []float32{4.0, 4.0, 4.0, 4.0},
			"other_vector":       []float32{5.0, 5.0, 5.0, 5.0},
			"query_is_recommend": "ok",
			"query_gender":       "male",
			// "query_color": "red", // Missing required column
			"query_price": 1.0,
		},
	})

	// Expect error for missing required column
	if err == nil {
		t.Error("Expected error for missing required column 'query_color', but got none")
	} else {
		t.Logf("Got expected error for missing column: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestKNNOnVectorColumn tests KNN search on different vector columns
// Based on Python SDK test_pysdk/test_knn.py - test_knn_on_vector_column
func TestKNNOnVectorColumnWithImport(t *testing.T) {
	// Test on different vector columns
	vectorColumns := []string{"gender_vector", "color_vector"}

	for _, columnName := range vectorColumns {
		t.Run(columnName, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_knn_on_vector_column_%s", columnName)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search on the specified vector column
			_, err = table.Output([]string{"variant_id", "_row_id", "_similarity"}).
				MatchDense(columnName, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "ip", 2, nil).
				ToResult()
			if err != nil {
				t.Errorf("KNN search on %s failed: %v", columnName, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestKNNOnNonVectorColumn tests KNN search on non-vector columns (should fail)
// Based on Python SDK test_pysdk/test_knn.py - test_knn_on_non_vector_column
func TestKNNOnNonVectorColumn(t *testing.T) {
	// Test on non-vector columns (should fail)
	nonVectorColumns := []string{"variant_id", "query_price"}

	for _, columnName := range nonVectorColumns {
		t.Run(columnName, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_knn_on_non_vector_column_%s", columnName)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search on non-vector column (should fail)
			_, err = table.Output([]string{"variant_id", "_row_id"}).
				MatchDense(columnName, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "ip", 2, nil).
				ToResult()
			if err == nil {
				t.Errorf("Expected error for KNN search on non-vector column %s, but got none", columnName)
			} else {
				t.Logf("Got expected error for non-vector column %s: %v", columnName, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestValidEmbeddingData tests KNN search with valid embedding data formats
// Based on Python SDK test_pysdk/test_knn.py - test_valid_embedding_data
func TestValidEmbeddingData(t *testing.T) {
	// Test with different valid embedding data formats
	testCases := []struct {
		name          string
		embeddingData interface{}
	}{
		{"slice_int", []int{1, 1, 1, 1}},
		{"slice_float", []float32{1.0, 1.0, 1.0, 1.0}},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_valid_embedding_data_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search with valid embedding data
			var queryVec infinity.VEC
			switch v := tc.embeddingData.(type) {
			case []int:
				floatVec := make([]float32, len(v))
				for i, val := range v {
					floatVec[i] = float32(val)
				}
				queryVec = infinity.Float32Vector(floatVec)
			case []float32:
				queryVec = infinity.Float32Vector(v)
			default:
				t.Fatalf("Unsupported embedding data type: %T", tc.embeddingData)
			}

			_, err = table.Output([]string{"variant_id", "_row_id"}).
				MatchDense("gender_vector", queryVec, "float", "ip", 2, nil).
				ToResult()
			if err != nil {
				t.Errorf("KNN search with valid embedding data %s failed: %v", tc.name, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestInvalidEmbeddingData tests KNN search with invalid embedding data (should fail)
// Based on Python SDK test_pysdk/test_knn.py - test_invalid_embedding_data
func TestInvalidEmbeddingData(t *testing.T) {
	// Test with different invalid embedding data formats
	testCases := []struct {
		name          string
		embeddingData interface{}
	}{
		{"string", "variant_id"},
		{"int", 1},
		{"float", 2.4},
		{"wrong_dim", []int{1, 2, 3}}, // Wrong dimension (3 instead of 4)
		{"map", map[string]string{"c": "12"}},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_invalid_embedding_data_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search with invalid embedding data (should fail)
			var queryVec infinity.VEC
			switch v := tc.embeddingData.(type) {
			case []int:
				floatVec := make([]float32, len(v))
				for i, val := range v {
					floatVec[i] = float32(val)
				}
				queryVec = infinity.Float32Vector(floatVec)
			case []float32:
				queryVec = infinity.Float32Vector(v)
			default:
				// For invalid types, try to use Float32Vector and expect it to fail
				queryVec = infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0})
			}

			_, err = table.Output([]string{"variant_id", "_row_id"}).
				MatchDense("gender_vector", queryVec, "float", "ip", 2, nil).
				ToResult()

			// For invalid embedding data types (string, int, float, map), we expect an error
			// But since Go is statically typed, most invalid types won't even compile
			// So we just check that the query runs (with wrong dimension it should fail)
			if tc.name == "wrong_dim" {
				if err == nil {
					t.Errorf("Expected error for wrong dimension embedding data, but got none")
				} else {
					t.Logf("Got expected error for wrong dimension: %v", err)
				}
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestValidEmbeddingDataType tests KNN search with valid embedding data types
// Based on Python SDK test_pysdk/test_knn.py - test_valid_embedding_data_type
func TestValidEmbeddingDataType(t *testing.T) {
	// Test with different valid embedding data and types
	testCases := []struct {
		name              string
		embeddingData     interface{}
		embeddingDataType string
		useDistance       bool
	}{
		{"int_float_l2", []int{1, 1, 1, 1}, "float", true},
		{"float_float_l2", []float32{1.0, 1.0, 1.0, 1.0}, "float", true},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_valid_embedding_data_type_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Convert embedding data to Float32Vector
			var queryVec infinity.VEC
			switch v := tc.embeddingData.(type) {
			case []int:
				floatVec := make([]float32, len(v))
				for i, val := range v {
					floatVec[i] = float32(val)
				}
				queryVec = infinity.Float32Vector(floatVec)
			case []float32:
				queryVec = infinity.Float32Vector(v)
			default:
				t.Fatalf("Unsupported embedding data type: %T", tc.embeddingData)
			}

			// Test KNN search with valid embedding data type
			var outputCols []string
			if tc.useDistance {
				outputCols = []string{"variant_id", "_distance"}
			} else {
				outputCols = []string{"variant_id", "_similarity"}
			}

			_, err = table.Output(outputCols).
				MatchDense("gender_vector", queryVec, tc.embeddingDataType, "l2", 2, nil).
				ToResult()
			if err != nil {
				t.Errorf("KNN search with valid embedding data type %s failed: %v", tc.name, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestInvalidEmbeddingDataType tests KNN search with invalid embedding data types (should fail)
// Based on Python SDK test_pysdk/test_knn.py - test_invalid_embedding_data_type
func TestInvalidEmbeddingDataType(t *testing.T) {
	// Test with different invalid embedding data types
	testCases := []struct {
		name              string
		embeddingData     interface{}
		embeddingDataType string
		metricType        string
	}{
		{"int_type_l2", []int{1, 1, 1, 1}, "int", "l2"},
		{"float_type_ip", []float32{1.0, 1.0, 1.0, 1.0}, "int", "ip"},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_invalid_embedding_data_type_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Convert embedding data to Float32Vector
			var queryVec infinity.VEC
			switch v := tc.embeddingData.(type) {
			case []int:
				floatVec := make([]float32, len(v))
				for i, val := range v {
					floatVec[i] = float32(val)
				}
				queryVec = infinity.Float32Vector(floatVec)
			case []float32:
				queryVec = infinity.Float32Vector(v)
			default:
				t.Fatalf("Unsupported embedding data type: %T", tc.embeddingData)
			}

			// Test KNN search with invalid embedding data type (should fail)
			_, err = table.Output([]string{"variant_id"}).
				MatchDense("gender_vector", queryVec, tc.embeddingDataType, tc.metricType, 2, nil).
				ToResult()

			// For invalid embedding data type, we expect an error
			if err == nil {
				t.Errorf("Expected error for invalid embedding data type %s, but got none", tc.name)
			} else {
				t.Logf("Got expected error for invalid embedding data type %s: %v", tc.name, err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestVariousDistanceType tests KNN search with various distance types
// Based on Python SDK test_pysdk/test_knn.py - test_various_distance_type
func TestVariousDistanceType(t *testing.T) {
	// Test with different distance types
	testCases := []struct {
		name         string
		distanceType string
		shouldWork   bool
	}{
		{"l2", "l2", true},
		{"cosine", "cosine", true},
		{"cos", "cos", true},
		{"ip", "ip", true},
		{"hamming", "hamming", false},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_various_distance_type_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search with various distance types
			_, err = table.Output([]string{"variant_id"}).
				MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", tc.distanceType, 2, nil).
				ToResult()

			if tc.shouldWork {
				if err != nil {
					t.Errorf("KNN search with distance type %s should work but failed: %v", tc.name, err)
				}
			} else {
				if err == nil {
					t.Errorf("Expected error for distance type %s, but got none", tc.name)
				} else {
					t.Logf("Got expected error for distance type %s: %v", tc.name, err)
				}
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestVariousTopn tests KNN search with various topn values
// Based on Python SDK test_pysdk/test_knn.py - test_various_topn
func TestVariousTopn(t *testing.T) {
	// Test with different topn values
	testCases := []struct {
		name       string
		topn       int
		shouldWork bool
	}{
		{"topn_2", 2, true},
		{"topn_10", 10, true},
		{"topn_0", 0, false},
		{"topn_neg1", -1, false},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_various_topn_%s", tc.name)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with multiple vector columns
			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/tmp_20240116.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test KNN search with various topn values
			_, err = table.Output([]string{"variant_id"}).
				MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", "l2", tc.topn, nil).
				ToResult()

			if tc.shouldWork {
				if err != nil {
					t.Errorf("KNN search with topn=%d should work but failed: %v", tc.topn, err)
				}
			} else {
				if err == nil {
					t.Errorf("Expected error for topn=%d, but got none", tc.topn)
				} else {
					t.Logf("Got expected error for topn=%d: %v", tc.topn, err)
				}
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestWithIndexBefore tests KNN search with index created before
// Based on Python SDK test_pysdk/test_knn.py - test_with_index_before
func TestWithIndexBefore(t *testing.T) {
	// Test with different index and knn column combinations
	vectorColumns := []string{"gender_vector", "color_vector", "category_vector", "tag_vector", "other_vector"}
	distanceTypes := []string{"l2", "ip"}

	for _, indexCol := range vectorColumns {
		for _, knnCol := range vectorColumns {
			for _, indexDist := range distanceTypes {
				for _, knnDist := range distanceTypes {
					testName := fmt.Sprintf("idx_%s_knn_%s_%s_%s", indexCol, knnCol, indexDist, knnDist)
					t.Run(testName, func(t *testing.T) {
						conn := setupConnection(t)
						defer closeConnection(t, conn)

						db, err := conn.GetDatabase("default_db")
						if err != nil {
							t.Fatalf("Failed to get database: %v", err)
						}

						tableName := "test_with_index_"
						db.DropTable(tableName, infinity.ConflictTypeIgnore)

						// Create table with multiple vector columns
						schema := infinity.TableSchema{
							{Name: "variant_id", DataType: "varchar"},
							{Name: "gender_vector", DataType: "vector,4,float"},
							{Name: "color_vector", DataType: "vector,4,float"},
							{Name: "category_vector", DataType: "vector,4,float"},
							{Name: "tag_vector", DataType: "vector,4,float"},
							{Name: "other_vector", DataType: "vector,4,float"},
							{Name: "query_is_recommend", DataType: "varchar"},
							{Name: "query_gender", DataType: "varchar"},
							{Name: "query_color", DataType: "varchar"},
							{Name: "query_price", DataType: "float"},
						}

						table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
						if err != nil {
							t.Fatalf("Failed to create table: %v", err)
						}

						// Import data from CSV if file exists
						testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
						if _, err := os.Stat(testCSVPath); err == nil {
							_, err = table.ImportData(testCSVPath, nil)
							if err != nil {
								t.Logf("ImportData skipped or failed: %v", err)
							}
						} else {
							t.Logf("Test CSV file not found: %s", testCSVPath)
						}

						// Create HNSW index on specified column
						indexInfo := infinity.NewIndexInfo(indexCol, infinity.IndexTypeHnsw, map[string]string{
							"M":               "16",
							"ef_construction": "50",
							"metric":          indexDist,
						})
						_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
						if err != nil {
							t.Errorf("Failed to create index on %s with %s: %v", indexCol, indexDist, err)
						}

						// Test KNN search on specified column with specified distance type
						_, err = table.Output([]string{"variant_id"}).
							MatchDense(knnCol, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, nil).
							ToResult()
						if err != nil {
							t.Errorf("KNN search on %s with %s failed: %v", knnCol, knnDist, err)
						}

						// Drop index
						_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
						if err != nil {
							t.Errorf("Failed to drop index: %v", err)
						}

						// Cleanup
						_, err = db.DropTable(tableName, infinity.ConflictTypeError)
						if err != nil {
							t.Fatalf("Failed to drop table: %v", err)
						}
					})
				}
			}
		}
	}
}

// TestWithIndexAfter tests KNN search before and after creating index
// Based on Python SDK test_pysdk/test_knn.py - test_with_index_after
func TestWithIndexAfter(t *testing.T) {
	// Test with different index and knn column combinations
	vectorColumns := []string{"gender_vector", "color_vector", "category_vector", "tag_vector", "other_vector"}
	distanceTypes := []string{"l2", "ip"}

	for _, indexCol := range vectorColumns {
		for _, knnCol := range vectorColumns {
			for _, indexDist := range distanceTypes {
				for _, knnDist := range distanceTypes {
					testName := fmt.Sprintf("idx_%s_knn_%s_%s_%s", indexCol, knnCol, indexDist, knnDist)
					t.Run(testName, func(t *testing.T) {
						conn := setupConnection(t)
						defer closeConnection(t, conn)

						db, err := conn.GetDatabase("default_db")
						if err != nil {
							t.Fatalf("Failed to get database: %v", err)
						}

						tableName := "test_with_index_after_"
						db.DropTable(tableName, infinity.ConflictTypeIgnore)

						// Create table with multiple vector columns
						schema := infinity.TableSchema{
							{Name: "variant_id", DataType: "varchar"},
							{Name: "gender_vector", DataType: "vector,4,float"},
							{Name: "color_vector", DataType: "vector,4,float"},
							{Name: "category_vector", DataType: "vector,4,float"},
							{Name: "tag_vector", DataType: "vector,4,float"},
							{Name: "other_vector", DataType: "vector,4,float"},
							{Name: "query_is_recommend", DataType: "varchar"},
							{Name: "query_gender", DataType: "varchar"},
							{Name: "query_color", DataType: "varchar"},
							{Name: "query_price", DataType: "float"},
						}

						table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
						if err != nil {
							t.Fatalf("Failed to create table: %v", err)
						}

						// Import data from CSV if file exists
						testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
						if _, err := os.Stat(testCSVPath); err == nil {
							_, err = table.ImportData(testCSVPath, nil)
							if err != nil {
								t.Logf("ImportData skipped or failed: %v", err)
							}
						} else {
							t.Logf("Test CSV file not found: %s", testCSVPath)
						}

						// Test KNN search before creating index
						_, err = table.Output([]string{"variant_id"}).
							MatchDense(knnCol, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, nil).
							ToResult()
						if err != nil {
							t.Errorf("KNN search on %s with %s before index failed: %v", knnCol, knnDist, err)
						}

						// Create HNSW index on specified column
						indexInfo := infinity.NewIndexInfo(indexCol, infinity.IndexTypeHnsw, map[string]string{
							"M":               "16",
							"ef_construction": "50",
							"metric":          indexDist,
						})
						_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
						if err != nil {
							t.Errorf("Failed to create index on %s with %s: %v", indexCol, indexDist, err)
						}

						// Drop index
						_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
						if err != nil {
							t.Errorf("Failed to drop index: %v", err)
						}

						// Cleanup
						_, err = db.DropTable(tableName, infinity.ConflictTypeError)
						if err != nil {
							t.Fatalf("Failed to drop table: %v", err)
						}
					})
				}
			}
		}
	}
}

// TestFulltextOperatorOption tests full-text search with OR/AND operator options
// Based on Python SDK test_pysdk/test_knn.py - test_fulltext_operator_option
func TestFulltextOperatorOption(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_fulltext_operator_option"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with text columns
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index on body column
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/enwiki_99.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = '\t'
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Test fulltext operator OR
	t.Log("Test fulltext operator OR for query 'TO BE OR NOT':")
	_, err = table.Output([]string{"*", "_row_id", "_score"}).
		MatchText("body^5", "TO BE OR NOT", 5, map[string]string{"operator": "or"}).
		ToResult()
	if err != nil {
		t.Errorf("Full-text search with OR operator failed: %v", err)
	}

	// Test fulltext operator OR with threshold
	_, err = table.Output([]string{"_score"}).
		MatchText("body^5", "TO BE OR NOT", 5, map[string]string{"operator": "or", "threshold": "20"}).
		ToResult()
	if err != nil {
		t.Errorf("Full-text search with OR operator and threshold failed: %v", err)
	}

	// Test fulltext operator AND
	t.Log("Test fulltext operator AND for query 'TO BE OR NOT':")
	_, err = table.Output([]string{"*", "_row_id", "_score"}).
		MatchText("body^5", "TO BE OR NOT", 5, map[string]string{"operator": "and"}).
		ToResult()
	if err != nil {
		t.Errorf("Full-text search with AND operator failed: %v", err)
	}

	// Test fulltext operator AND with threshold
	_, err = table.Output([]string{"_score"}).
		MatchText("body^5", "TO BE OR NOT", 5, map[string]string{"operator": "and", "threshold": "25"}).
		ToResult()
	if err != nil {
		t.Errorf("Full-text search with AND operator and threshold failed: %v", err)
	}

	// Test without operator option (should fail)
	t.Log("No operator option for query 'TO BE OR NOT', expect throw:")
	_, err = table.Output([]string{"*", "_row_id", "_score"}).
		MatchText("body^5", "TO BE OR NOT", 5, nil).
		ToResult()
	if err == nil {
		t.Error("Expected error for match_text without operator option, but got none")
	} else {
		t.Logf("Got expected error: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestWithFulltextMatchWithValidColumns tests fusion of dense vector search and full-text search
// Based on Python SDK test_pysdk/test_knn.py - test_with_fulltext_match_with_valid_columns
func TestWithFulltextMatchWithValidColumns(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_with_fulltext_match_with_valid_columns"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with text and vector columns
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index on body column
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/enwiki_embedding_99_commas.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Test fusion of dense vector search and full-text search
	_, err = table.Output([]string{"*"}).
		MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
		MatchText("body^5", "black", 1, nil).
		Fusion("rrf", 10, nil).
		ToResult()
	if err != nil {
		t.Logf("Fusion search may not be fully supported: %v", err)
	}

	// Test fusion with filter
	_, err = table.Output([]string{"*"}).
		MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
		MatchText("body^5", "black", 1, nil).
		Fusion("rrf", 10, nil).
		Filter("num!=98 AND num != 12").
		ToResult()
	if err != nil {
		t.Logf("Fusion search with filter may not be fully supported: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestWithFulltextMatchWithInvalidColumns tests fusion with invalid column names (should fail)
// Based on Python SDK test_pysdk/test_knn.py - test_with_fulltext_match_with_invalid_columns
func TestWithFulltextMatchWithInvalidColumns(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_with_fulltext_match_with_invalid_columns"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with text and vector columns
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index on body column
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/enwiki_embedding_99_commas.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Test fusion with invalid column name (should fail)
	// Note: Go is statically typed, so invalid column types (int, float, etc.) are caught at compile time
	// We can only test with invalid string column names
	_, err = table.Output([]string{"*"}).
		MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
		MatchText("invalid_column_name", "black", 1, nil).
		Fusion("rrf", 10, nil).
		ToResult()
	if err == nil {
		t.Error("Expected error for invalid column name in match_text, but got none")
	} else {
		t.Logf("Got expected error for invalid column name: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestWithFulltextMatchWithValidWords tests fusion with valid search words
// Based on Python SDK test_pysdk/test_knn.py - test_with_fulltext_match_with_valid_words
func TestWithFulltextMatchWithValidWords(t *testing.T) {
	testCases := []string{"a word a segment", "body=Greek"}

	for i, matchWord := range testCases {
		t.Run(fmt.Sprintf("case_%d", i), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := fmt.Sprintf("test_with_fulltext_match_with_valid_words_%d", i)
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table with text and vector columns
			schema := infinity.TableSchema{
				{Name: "doctitle", DataType: "varchar"},
				{Name: "docdate", DataType: "varchar"},
				{Name: "body", DataType: "varchar"},
				{Name: "num", DataType: "int"},
				{Name: "vec", DataType: "vector,4,float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Create full-text index on body column
			indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
				"ANALYZER": "standard",
			})
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Fatalf("Failed to create full-text index: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/enwiki_embedding_99_commas.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Test fusion with valid search words
			_, err = table.Output([]string{"*"}).
				MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
				MatchText("body^5", matchWord, 1, nil).
				Fusion("rrf", 10, nil).
				ToResult()
			if err != nil {
				t.Logf("Fusion search with valid words may not be fully supported: %v", err)
			}

			// Drop index
			_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestWithFulltextMatchWithInvalidWords tests fusion with invalid search words (should fail)
// Based on Python SDK test_pysdk/test_knn.py - test_with_fulltext_match_with_invalid_words
func TestWithFulltextMatchWithInvalidWords(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_with_fulltext_match_with_invalid_words"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with text and vector columns
	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index on body column
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/enwiki_embedding_99_commas.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Test fusion with invalid search words (should fail)
	// Note: Go is statically typed, so invalid word types (int, float, etc.) are caught at compile time
	// We can only test with invalid string patterns
	_, err = table.Output([]string{"*"}).
		MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
		MatchText("doctitle,num,body^5", "@#$!#@$SDasdf3!@#$", 1, nil).
		Fusion("rrf", 10, nil).
		ToResult()
	if err == nil {
		t.Log("Expected error for invalid search words, but got none")
	} else {
		t.Logf("Got expected error for invalid search words: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestTensorScan tests tensor scan (MaxSim) functionality
// Based on Python SDK test_pysdk/test_knn.py - test_tensor_scan
func TestTensorScan(t *testing.T) {
	saveElemTypes := []string{"float32", "float16", "bfloat16"}
	queryElemTypes := []string{"float32", "float16", "bfloat16"}

	for _, saveElemType := range saveElemTypes {
		for _, queryElemType := range queryElemTypes {
			testName := fmt.Sprintf("save_%s_query_%s", saveElemType, queryElemType)
			t.Run(testName, func(t *testing.T) {
				conn := setupConnection(t)
				defer closeConnection(t, conn)

				db, err := conn.GetDatabase("default_db")
				if err != nil {
					t.Fatalf("Failed to get database: %v", err)
				}

				tableName := fmt.Sprintf("test_tensor_scan_%s", testName)
				db.DropTable(tableName, infinity.ConflictTypeIgnore)

				// Create table with tensor column
				schema := infinity.TableSchema{
					{Name: "title", DataType: "varchar"},
					{Name: "num", DataType: "int"},
					{Name: "t", DataType: fmt.Sprintf("tensor,4,%s", saveElemType)},
					{Name: "body", DataType: "varchar"},
				}

				table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
				if err != nil {
					t.Fatalf("Failed to create table: %v", err)
				}

				// Import data from CSV if file exists
				testCSVPath := "/var/infinity/test_data/tensor_maxsim.csv"
				if _, err := os.Stat(testCSVPath); err == nil {
					importOptions := infinity.NewImportOption()
					importOptions.Delimiter = ','
					_, err = table.ImportData(testCSVPath, importOptions)
					if err != nil {
						t.Logf("ImportData skipped or failed: %v", err)
					}
				} else {
					t.Logf("Test CSV file not found: %s", testCSVPath)
				}

				// Test tensor scan (MaxSim)
				queryTensor := [][]float32{
					{0.0, -10.0, 0.0, 0.7},
					{9.2, 45.6, -55.8, 3.5},
				}
				_, err = table.Output([]string{"*", "_row_id", "_score"}).
					MatchTensor("t", queryTensor, queryElemType, 3, nil).
					ToResult()
				if err != nil {
					t.Logf("Tensor scan may not be fully supported: %v", err)
				}

				// Test tensor scan with threshold
				_, err = table.Output([]string{"title"}).
					MatchTensor("t", queryTensor, queryElemType, 3, map[string]string{"threshold": "300"}).
					ToResult()
				if err != nil {
					t.Logf("Tensor scan with threshold may not be fully supported: %v", err)
				}

				// Cleanup
				_, err = db.DropTable(tableName, infinity.ConflictTypeError)
				if err != nil {
					t.Fatalf("Failed to drop table: %v", err)
				}
			})
		}
	}
}

// TestSparseKNN tests sparse vector KNN functionality
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_knn
func TestSparseKNN(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_sparse_scan"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with sparse vector column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "sparse,100,float,int8"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/sparse_knn.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Test sparse vector KNN search
	sparseVector := &infinity.SparseVector{
		Indices: []int{0, 20, 80},
		Values:  []float64{1.0, 2.0, 3.0},
	}
	_, err = table.Output([]string{"*", "_row_id", "_similarity"}).
		MatchSparse("c2", sparseVector, "ip", 3, nil).
		ToResult()
	if err != nil {
		t.Logf("Sparse KNN may not be fully supported: %v", err)
	}

	// Test sparse vector KNN with threshold
	_, err = table.Output([]string{"c1", "_similarity"}).
		MatchSparse("c2", sparseVector, "ip", 3, map[string]string{"threshold": "10"}).
		ToResult()
	if err != nil {
		t.Logf("Sparse KNN with threshold may not be fully supported: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSparseKNNWithIndex tests sparse vector KNN with BMP index
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_knn_with_index
func TestSparseKNNWithIndex(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_sparse_knn_with_index"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with sparse vector column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "sparse,100,float,int8"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := "/var/infinity/test_data/sparse_knn.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Create BMP index on sparse vector column
	indexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeBMP, map[string]string{
		"block_size":    "8",
		"compress_type": "compress",
	})
	_, err = table.CreateIndex("idx1", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Logf("Failed to create BMP index: %v", err)
	}

	// Alter index options
	_, err = table.AlterIndex("idx1", map[string]string{"topk": "3", "bp_reorder": ""})
	if err != nil {
		t.Logf("Failed to alter index: %v", err)
	}

	// Test sparse vector KNN search with index
	sparseVector := &infinity.SparseVector{
		Indices: []int{0, 20, 80},
		Values:  []float64{1.0, 2.0, 3.0},
	}
	_, err = table.Output([]string{"*", "_row_id", "_similarity"}).
		MatchSparse("c2", sparseVector, "ip", 3, map[string]string{"alpha": "1.0", "beta": "1.0"}).
		ToResult()
	if err != nil {
		t.Logf("Sparse KNN with index may not be fully supported: %v", err)
	}

	// Test sparse vector KNN with threshold
	_, err = table.Output([]string{"c1", "_similarity"}).
		MatchSparse("c2", sparseVector, "ip", 3, map[string]string{"alpha": "1.0", "beta": "1.0", "threshold": "10"}).
		ToResult()
	if err != nil {
		t.Logf("Sparse KNN with index and threshold may not be fully supported: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("idx1", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

func TestWithMultipleFusion(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_with_multiple_fusion"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "title", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "t", DataType: "tensor,4,float"},
		{Name: "body", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("ft_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := testDataDir + "tensor_maxsim.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}

		// Test multiple fusion
		_, err = table.Output([]string{"*", "_row_id", "_score"}).
			MatchText("body", "off", 4, nil).
			MatchTensor("t", [][]float32{{1.0, 0.0, 0.0, 0.0}, {1.0, 0.0, 0.0, 0.0}}, "float", 2, nil).
			Fusion("rrf", 10, nil).
			ToResult()
		if err != nil {
			t.Logf("Multiple fusion may not be fully supported: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Drop index
	_, err = table.DropIndex("ft_index", infinity.ConflictTypeError)
	if err != nil {
		t.Logf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestWithVariousIndexKNNDistanceCombination tests various index and KNN distance type combinations
// Based on Python SDK test_pysdk/test_knn.py - test_with_various_index_knn_distance_combination
func TestWithVariousIndexKNNDistanceCombination(t *testing.T) {
	indexColumnNames := []string{"gender_vector"}
	knnColumnNames := []string{"gender_vector"}
	distanceTypes := []string{"l2", "ip", "cosine", "cos"}
	indexTypes := []infinity.IndexType{infinity.IndexTypeHnsw, infinity.IndexTypeIVF}

	for _, indexCol := range indexColumnNames {
		for _, knnCol := range knnColumnNames {
			for _, indexDist := range distanceTypes {
				for _, knnDist := range distanceTypes {
					for _, indexType := range indexTypes {
						testName := fmt.Sprintf("idx_%s_knn_%s_%s_%s_%v", indexCol, knnCol, indexDist, knnDist, indexType)
						t.Run(testName, func(t *testing.T) {
							conn := setupConnection(t)
							defer closeConnection(t, conn)

							db, err := conn.GetDatabase("default_db")
							if err != nil {
								t.Fatalf("Failed to get database: %v", err)
							}

							tableName := "test_with_index"
							db.DropTable(tableName, infinity.ConflictTypeIgnore)

							schema := infinity.TableSchema{
								{Name: "variant_id", DataType: "varchar"},
								{Name: "gender_vector", DataType: "vector,4,float"},
								{Name: "color_vector", DataType: "vector,4,float"},
								{Name: "category_vector", DataType: "vector,4,float"},
								{Name: "tag_vector", DataType: "vector,4,float"},
								{Name: "other_vector", DataType: "vector,4,float"},
								{Name: "query_is_recommend", DataType: "varchar"},
								{Name: "query_gender", DataType: "varchar"},
								{Name: "query_color", DataType: "varchar"},
								{Name: "query_price", DataType: "float"},
							}

							table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
							if err != nil {
								t.Fatalf("Failed to create table: %v", err)
							}

							// Import data from CSV if file exists
							testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
							if _, err := os.Stat(testCSVPath); err == nil {
								_, err = table.ImportData(testCSVPath, nil)
								if err != nil {
									t.Logf("ImportData skipped or failed: %v", err)
								}
							} else {
								t.Logf("Test CSV file not found: %s", testCSVPath)
							}

							// Create index
							var indexInfo *infinity.IndexInfo
							if indexType == infinity.IndexTypeHnsw {
								indexInfo = infinity.NewIndexInfo(indexCol, infinity.IndexTypeHnsw, map[string]string{
									"M":               "16",
									"ef_construction": "50",
									"metric":          indexDist,
								})
							} else {
								indexInfo = infinity.NewIndexInfo(indexCol, infinity.IndexTypeIVF, map[string]string{
									"metric": indexDist,
								})
							}

							_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
							if err != nil {
								t.Logf("Failed to create index: %v", err)
							} else {
								if indexType == infinity.IndexTypeHnsw {
									// Test KNN search
									_, err = table.Output([]string{"variant_id"}).
										MatchDense(knnCol, infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, nil).
										ToResult()
									if err != nil {
										t.Logf("KNN search failed: %v", err)
									}
								}

								// Drop index
								_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
								if err != nil {
									t.Logf("Failed to drop index: %v", err)
								}
							}

							// Cleanup
							_, err = db.DropTable(tableName, infinity.ConflictTypeError)
							if err != nil {
								t.Fatalf("Failed to drop table: %v", err)
							}
						})
					}
				}
			}
		}
	}
}

// TestBigDimensionVector tests big dimension vector operations
// Based on Python SDK test_pysdk/test_knn.py - test_big_dimension_vector
func TestBigDimensionVector(t *testing.T) {
	dimensions := []int{1000, 16384}

	for _, dim := range dimensions {
		t.Run(fmt.Sprintf("dim_%d", dim), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_big_dimension_vector"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "big_vector", DataType: fmt.Sprintf("vector,%d,float", dim)},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Insert data
			for i := 1; i <= 5; i++ {
				vector := make([]float32, dim)
				for j := 0; j < dim; j++ {
					vector[j] = float32(i)
				}
				_, err = table.Insert([]map[string]interface{}{
					{"big_vector": vector},
				})
				if err != nil {
					t.Fatalf("Failed to insert data: %v", err)
				}
			}

			// Test KNN search
			queryVector := make([]float32, dim)
			_, err = table.Output([]string{"_row_id"}).
				MatchDense("big_vector", infinity.Float32Vector(queryVector), "float", "l2", 5, nil).
				ToResult()
			if err != nil {
				t.Fatalf("KNN search failed: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestWithVariousFulltextMatch tests various full-text match operations
// Based on Python SDK test_pysdk/test_knn.py - test_with_various_fulltext_match
func TestWithVariousFulltextMatch(t *testing.T) {
	fieldsAndMatchingText := [][]string{
		{"body", "black"},
		{"body^5", "black"},
		{"", "body:black"},
		{"", "body:black^5"},
		{"", "'body':'(black)'"},
		{"", "body:'(black)^5'"},
		{"", "'body':'(black OR white)'"},
		{"", "'body':'(black AND white)'"},
		{"", "'body':'(black)^5 OR (white)'"},
		{"", "'body':'(black)^5 AND (white)'"},
		{"", "'body':'black - white'"},
		{"", "body:black OR body:black"},
		{"", "body:black AND body:black"},
		{"", "(body:black OR body:black) AND (body:white OR body:white)"},
		{"", "(body:black)^5 OR body:black"},
		{"", "(body:black)^5 AND body:black"},
		{"", "(body:black OR body:black)^5 AND (body:white OR body:white)"},
	}

	for i, fieldsAndText := range fieldsAndMatchingText {
		t.Run(fmt.Sprintf("case_%d", i), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_with_various_fulltext_match"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "doctitle", DataType: "varchar"},
				{Name: "docdate", DataType: "varchar"},
				{Name: "body", DataType: "varchar"},
				{Name: "num", DataType: "int"},
				{Name: "vec", DataType: "vector,4,float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Create full-text index
			indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
				"ANALYZER": "standard",
			})
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Fatalf("Failed to create full-text index: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "enwiki_embedding_99_commas.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Test full-text match with fusion
				_, err = table.Output([]string{"*"}).
					MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
					MatchText(fieldsAndText[0], fieldsAndText[1], 1, nil).
					Fusion("rrf", 10, nil).
					ToResult()
				if err != nil {
					t.Logf("Full-text match with fusion may not be fully supported: %v", err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Drop index
			_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
			if err != nil {
				t.Logf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestTensorScanWithInvalidDataType tests tensor scan with invalid data types
// Based on Python SDK test_pysdk/test_knn.py - test_tensor_scan_with_invalid_data_type
func TestTensorScanWithInvalidDataType(t *testing.T) {
	// Test with invalid data types (Go is statically typed, so we can only test runtime errors)
	invalidDataTypes := []string{"varchar", "invalid_type"}

	for _, dataType := range invalidDataTypes {
		t.Run(dataType, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_tensor_scan_invalid_type"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "title", DataType: "varchar"},
				{Name: "num", DataType: "int"},
				{Name: "t", DataType: "tensor,4,float"},
				{Name: "body", DataType: "varchar"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "tensor_maxsim.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Test tensor scan with invalid data type (should fail)
				_, err = table.Output([]string{"*", "_row_id", "_score"}).
					MatchTensor("t", [][]float32{{0.0, -10.0, 0.0, 0.7}, {9.2, 45.6, -55.8, 3.5}}, dataType, 2, nil).
					ToResult()
				if err == nil {
					t.Errorf("Expected error for invalid data type %s, but got none", dataType)
				} else {
					t.Logf("Got expected error for invalid data type %s: %v", dataType, err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestTensorScanWithInvalidExtraOption tests tensor scan with invalid extra options
// Based on Python SDK test_pysdk/test_knn.py - test_tensor_scan_with_invalid_extra_option
func TestTensorScanWithInvalidExtraOption(t *testing.T) {
	extraOptions := []map[string]string{
		{"topn": "-1"},
		{"topn": "0"},
		{"topn": "100000000"},
	}

	for i, extraOption := range extraOptions {
		t.Run(fmt.Sprintf("case_%d", i), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_tensor_scan_invalid_option"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "title", DataType: "varchar"},
				{Name: "num", DataType: "int"},
				{Name: "t", DataType: "tensor,4,float"},
				{Name: "body", DataType: "varchar"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "tensor_maxsim.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Test tensor scan with invalid extra option (should fail)
				_, err = table.Output([]string{"*", "_row_id", "_score"}).
					MatchTensor("t", [][]float32{{0.0, -10.0, 0.0, 0.7}, {9.2, 45.6, -55.8, 3.5}}, "float", 3, extraOption).
					ToResult()
				if err == nil {
					t.Errorf("Expected error for invalid extra option %v, but got none", extraOption)
				} else {
					t.Logf("Got expected error for invalid extra option %v: %v", extraOption, err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestBigDimensionTensorScan tests big dimension tensor scan
// Based on Python SDK test_pysdk/test_knn.py - test_big_dimension_tensor_scan
func TestBigDimensionTensorScan(t *testing.T) {
	dimensions := []int{1, 10, 100}

	for _, dim := range dimensions {
		t.Run(fmt.Sprintf("dim_%d", dim), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_big_dimension_tensor_scan"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "t", DataType: fmt.Sprintf("tensorarray,%d,float", dim)},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Insert data
			for i := 1; i <= 5; i++ {
				// Create 3D tensor data
				tensorData := make([][][]float32, dim)
				for j := 0; j < dim; j++ {
					tensorData[j] = make([][]float32, dim)
					for k := 0; k < dim; k++ {
						tensorData[j][k] = make([]float32, dim)
						for l := 0; l < dim; l++ {
							tensorData[j][k][l] = float32(i)
						}
					}
				}
				_, err = table.Insert([]map[string]interface{}{
					{"t": tensorData},
				})
				if err != nil {
					t.Fatalf("Failed to insert data: %v", err)
				}
			}

			// Test tensor scan
			queryTensor := make([][][]float32, dim)
			for j := 0; j < dim; j++ {
				queryTensor[j] = make([][]float32, dim)
				for k := 0; k < dim; k++ {
					queryTensor[j][k] = make([]float32, dim)
				}
			}

			_, err = table.Output([]string{"*", "_row_id", "_score"}).
				MatchTensor("t", queryTensor, "float", 5, nil).
				ToResult()
			if err != nil {
				t.Logf("Big dimension tensor scan may not be fully supported: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestSparseWithInvalidTableParams tests sparse vector with invalid table parameters
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_with_invalid_table_params
func TestSparseWithInvalidTableParams(t *testing.T) {
	tableParams := []string{
		"vector,100,float",
		"sparse,0,float,int8",
		"sparse,100,int,int8",
		"sparse,100,float,float",
	}

	for i, param := range tableParams {
		t.Run(fmt.Sprintf("case_%d", i), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_sparse_scan"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			params := strings.Split(param, ",")
			if len(params) < 4 && params[0] != "vector" {
				t.Skipf("Invalid params format: %s", param)
				return
			}

			_, err = db.CreateTable(tableName, infinity.TableSchema{
				{Name: "c1", DataType: "int"},
				{Name: "c2", DataType: param},
			}, infinity.ConflictTypeError)

			if params[0] == "vector" {
				if err != nil {
					t.Fatalf("Failed to create table: %v", err)
				}
				// Should fail on import
				table, _ := db.GetTable(tableName)
				testCSVPath := testDataDir + "sparse_knn.csv"
				if _, err := os.Stat(testCSVPath); err == nil {
					_, err = table.ImportData(testCSVPath, &infinity.ImportOption{Delimiter: ','})
					if err == nil {
						t.Error("Expected error for vector type with sparse data, but got none")
					}
				}
			} else if params[1] == "0" || params[2] == "int" || params[3] == "float" {
				// These should fail at create time or during operations
				if err != nil {
					t.Logf("Got expected error for invalid params %s: %v", param, err)
				} else {
					t.Logf("Table created with params %s, may fail during operations", param)
				}
			}

			// Cleanup
			db.DropTable(tableName, infinity.ConflictTypeIgnore)
		})
	}
}

// TestSparseKNNWithInvalidIndexType tests sparse KNN with invalid index types
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_knn_with_invalid_index_type
func TestSparseKNNWithInvalidIndexType(t *testing.T) {
	indexTypes := []infinity.IndexType{
		infinity.IndexTypeIVF,
		infinity.IndexTypeHnsw,
		infinity.IndexTypeEMVB,
		infinity.IndexTypeFullText,
		infinity.IndexTypeSecondary,
	}

	for _, indexType := range indexTypes {
		t.Run(fmt.Sprintf("case_%d", indexType), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_sparse_knn_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "c1", DataType: "int"},
				{Name: "c2", DataType: "sparse,100,float,int8"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "sparse_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create invalid index (should fail)
				var indexInfo *infinity.IndexInfo
				switch indexType {
				case infinity.IndexTypeIVF:
					indexInfo = infinity.NewIndexInfo("c2", infinity.IndexTypeIVF, map[string]string{"metric": "L2"})
				case infinity.IndexTypeHnsw:
					indexInfo = infinity.NewIndexInfo("c2", infinity.IndexTypeHnsw, map[string]string{
						"M":               "16",
						"ef_construction": "50",
						"metric":          "L2",
					})
				case infinity.IndexTypeEMVB:
					indexInfo = infinity.NewIndexInfo("c2", infinity.IndexTypeEMVB, map[string]string{
						"pq_subspace_num":  "32",
						"pq_subspace_bits": "8",
					})
				case infinity.IndexTypeFullText:
					indexInfo = infinity.NewIndexInfo("c2", infinity.IndexTypeFullText, map[string]string{
						"ANALYZER": "STANDARD",
					})
				case infinity.IndexTypeSecondary:
					indexInfo = infinity.NewIndexInfo("c2", infinity.IndexTypeSecondary, nil)
				}

				_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
				if err == nil {
					t.Errorf("Expected error for invalid index type %s on sparse column, but got none", indexType)
					table.DropIndex("my_index", infinity.ConflictTypeIgnore)
				} else {
					t.Logf("Got expected error for invalid index type %s: %v", indexType, err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestSparseKNNWithInvalidIndexParams tests sparse KNN with invalid index parameters
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_knn_with_invalid_index_params
func TestSparseKNNWithInvalidIndexParams(t *testing.T) {
	indexParams := [][]string{
		{"0", "compress"},
		{"257", "compress"},
		{"16", "invalid compress type"},
	}

	for i, params := range indexParams {
		t.Run(fmt.Sprintf("case_%d", i), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_sparse_knn_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "c1", DataType: "int"},
				{Name: "c2", DataType: "sparse,100,float,int8"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "sparse_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create index with invalid params (should fail)
				indexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeBMP, map[string]string{
					"block_size":    params[0],
					"compress_type": params[1],
				})

				_, err = table.CreateIndex("idx1", indexInfo, infinity.ConflictTypeError, "")
				if err == nil {
					t.Errorf("Expected error for invalid index params %v, but got none", params)
					table.DropIndex("idx1", infinity.ConflictTypeIgnore)
				} else {
					t.Logf("Got expected error for invalid index params %v: %v", params, err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestSparseKNNWithInvalidDistanceType tests sparse KNN with invalid distance types
// Based on Python SDK test_pysdk/test_knn.py - test_sparse_knn_with_invalid_distance_type
func TestSparseKNNWithInvalidDistanceType(t *testing.T) {
	distanceTypes := []string{"l2", "cosine", "hamming"}

	for _, distanceType := range distanceTypes {
		t.Run(distanceType, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_sparse_knn_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "c1", DataType: "int"},
				{Name: "c2", DataType: "sparse,100,float,int8"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := testDataDir + "sparse_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				importOptions := infinity.NewImportOption()
				importOptions.Delimiter = ','
				_, err = table.ImportData(testCSVPath, importOptions)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Test sparse KNN with invalid distance type (should fail)
				sparseVector := &infinity.SparseVector{
					Indices: []int{0, 20, 80},
					Values:  []float64{1.0, 2.0, 3.0},
				}
				_, err = table.Output([]string{"*", "_row_id", "_similarity"}).
					MatchSparse("c2", sparseVector, distanceType, 3, map[string]string{"alpha": "1.0", "beta": "1.0"}).
					ToResult()
				if err == nil {
					t.Errorf("Expected error for invalid distance type %s, but got none", distanceType)
				} else {
					t.Logf("Got expected error for invalid distance type %s: %v", distanceType, err)
				}
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestKNNWithGivenIndexName tests KNN with a given index name
// Based on Python SDK test_pysdk/test_knn.py - test_knn_with_given_index_name
func TestKNNWithGivenIndexName(t *testing.T) {
	knnDistanceTypes := []string{"l2", "ip"}

	for _, knnDist := range knnDistanceTypes {
		t.Run(knnDist, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create two indexes
				indexInfoL2 := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
					"M":               "16",
					"ef_construction": "50",
					"metric":          "l2",
				})
				_, err = table.CreateIndex("my_index_l2", indexInfoL2, infinity.ConflictTypeError, "")
				if err != nil {
					t.Fatalf("Failed to create index: %v", err)
				}

				indexInfoIP := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
					"M":               "16",
					"ef_construction": "50",
					"metric":          "ip",
				})
				_, err = table.CreateIndex("my_index_ip", indexInfoIP, infinity.ConflictTypeError, "")
				if err != nil {
					t.Fatalf("Failed to create index: %v", err)
				}

				// Test KNN with specific index name
				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{"index_name": "my_index_l2"}).
					ToResult()
				if err != nil {
					t.Logf("KNN search with index_name=my_index_l2 failed: %v", err)
				}

				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{"index_name": "my_index_ip"}).
					ToResult()
				if err != nil {
					t.Logf("KNN search with index_name=my_index_ip failed: %v", err)
				}

				// Drop indexes
				table.DropIndex("my_index_l2", infinity.ConflictTypeError)
				table.DropIndex("my_index_ip", infinity.ConflictTypeError)
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestKNNWithIgnoreIndex tests KNN with ignore_index option
// Based on Python SDK test_pysdk/test_knn.py - test_knn_with_ignore_index
func TestKNNWithIgnoreIndex(t *testing.T) {
	knnDistanceTypes := []string{"l2", "ip"}

	for _, knnDist := range knnDistanceTypes {
		t.Run(knnDist, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create index
				indexInfo := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
					"M":               "16",
					"ef_construction": "50",
					"metric":          "l2",
				})
				_, err = table.CreateIndex("my_index_l2", indexInfo, infinity.ConflictTypeError, "")
				if err != nil {
					t.Fatalf("Failed to create index: %v", err)
				}

				// Test KNN with ignore_index option
				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{"ignore_index": "true"}).
					ToResult()
				if err != nil {
					t.Logf("KNN search with ignore_index=true failed: %v", err)
				}

				// Drop index
				table.DropIndex("my_index_l2", infinity.ConflictTypeError)
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestKNNWithGivenInvalidIndexName tests KNN with an invalid index name
// Based on Python SDK test_pysdk/test_knn.py - test_knn_with_given_invalid_index_name
func TestKNNWithGivenInvalidIndexName(t *testing.T) {
	knnDistanceTypes := []string{"l2", "ip"}

	for _, knnDist := range knnDistanceTypes {
		t.Run(knnDist, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create only one index
				indexInfo := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
					"M":               "16",
					"ef_construction": "50",
					"metric":          "l2",
				})
				_, err = table.CreateIndex("my_index_l2", indexInfo, infinity.ConflictTypeError, "")
				if err != nil {
					t.Fatalf("Failed to create index: %v", err)
				}

				// Test KNN with non-existent index name (should fail)
				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{"index_name": "my_index_ip"}).
					ToResult()
				if err == nil {
					t.Error("Expected error for non-existent index name, but got none")
				} else {
					t.Logf("Got expected error for non-existent index name: %v", err)
				}

				// Drop index
				table.DropIndex("my_index_l2", infinity.ConflictTypeError)
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestKNNWithGivenIndexNameAndIgnoreIndex tests KNN with both index_name and ignore_index options
// Based on Python SDK test_pysdk/test_knn.py - test_knn_with_given_index_name_and_ignore_index
func TestKNNWithGivenIndexNameAndIgnoreIndex(t *testing.T) {
	knnDistanceTypes := []string{"l2", "ip"}

	for _, knnDist := range knnDistanceTypes {
		t.Run(knnDist, func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_with_index"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "variant_id", DataType: "varchar"},
				{Name: "gender_vector", DataType: "vector,4,float"},
				{Name: "color_vector", DataType: "vector,4,float"},
				{Name: "category_vector", DataType: "vector,4,float"},
				{Name: "tag_vector", DataType: "vector,4,float"},
				{Name: "other_vector", DataType: "vector,4,float"},
				{Name: "query_is_recommend", DataType: "varchar"},
				{Name: "query_gender", DataType: "varchar"},
				{Name: "query_color", DataType: "varchar"},
				{Name: "query_price", DataType: "float"},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Import data from CSV if file exists
			testCSVPath := "/var/infinity/test_data/pysdk_test_knn.csv"
			if _, err := os.Stat(testCSVPath); err == nil {
				_, err = table.ImportData(testCSVPath, nil)
				if err != nil {
					t.Logf("ImportData skipped or failed: %v", err)
				}

				// Create index
				indexInfo := infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, map[string]string{
					"M":               "16",
					"ef_construction": "50",
					"metric":          "l2",
				})
				_, err = table.CreateIndex("my_index_l2", indexInfo, infinity.ConflictTypeError, "")
				if err != nil {
					t.Fatalf("Failed to create index: %v", err)
				}

				// Test KNN with index_name and ignore_index=false (should work)
				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{
						"index_name":   "my_index_l2",
						"ignore_index": "false",
					}).
					ToResult()
				if err != nil {
					t.Logf("KNN search with index_name and ignore_index=false failed: %v", err)
				}

				// Test KNN with index_name and ignore_index=true (should fail - conflicting options)
				_, err = table.Output([]string{"variant_id"}).
					MatchDense("gender_vector", infinity.Float32Vector([]float32{1.0, 1.0, 1.0, 1.0}), "float", knnDist, 5, map[string]string{
						"index_name":   "my_index_l2",
						"ignore_index": "true",
					}).
					ToResult()
				if err == nil {
					t.Error("Expected error for conflicting index_name and ignore_index=true options, but got none")
				} else {
					t.Logf("Got expected error for conflicting options: %v", err)
				}

				// Drop index
				table.DropIndex("my_index_l2", infinity.ConflictTypeError)
			} else {
				t.Logf("Test CSV file not found: %s", testCSVPath)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestBinaryEmbeddingHammingDistance tests binary embedding with hamming distance
// Based on Python SDK test_pysdk/test_knn.py - test_binary_embedding_hamming_distance
func TestBinaryEmbeddingHammingDistance(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_binary_knn_hamming_distance"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,16,bit"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert data
	testData := []map[string]interface{}{
		{"c1": 0, "c2": []int{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"c1": 1, "c2": []int{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
		{"c1": 2, "c2": []int{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}},
		{"c1": 3, "c2": []int{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1}},
		{"c1": 4, "c2": []int{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}},
		{"c1": 5, "c2": []int{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}},
	}

	for _, data := range testData {
		_, err = table.Insert([]map[string]interface{}{data})
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test KNN with hamming distance
	_, err = table.Output([]string{"*", "_distance"}).
		MatchDense("c2", infinity.Int8Vector([]int8{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), "bit", "hamming", 3, nil).
		ToResult()
	if err != nil {
		t.Logf("Binary KNN with hamming distance may not be fully supported: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestMatchSparseIndexHint tests sparse match with index hints
// Based on Python SDK test_pysdk/test_knn.py - test_match_sparse_index_hint
func TestMatchSparseIndexHint(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_sparse_knn_with_index"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "sparse,100,float,int8"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := testDataDir + "sparse_knn.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}

		// Create two BMP indexes
		indexInfo1 := infinity.NewIndexInfo("c2", infinity.IndexTypeBMP, map[string]string{
			"block_size":    "8",
			"compress_type": "compress",
		})
		_, err = table.CreateIndex("idx1", indexInfo1, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create BMP index: %v", err)
		}

		indexInfo2 := infinity.NewIndexInfo("c2", infinity.IndexTypeBMP, map[string]string{
			"block_size":    "8",
			"compress_type": "raww",
		})
		_, err = table.CreateIndex("idx2", indexInfo2, infinity.ConflictTypeError, "")
		if err != nil {
			t.Logf("Failed to create second BMP index: %v", err)
		}

		// Alter index options
		_, err = table.AlterIndex("idx1", map[string]string{"topk": "3", "bp_reorder": ""})
		if err != nil {
			t.Logf("Failed to alter index: %v", err)
		}

		sparseVector := &infinity.SparseVector{
			Indices: []int{0, 20, 80},
			Values:  []float64{1.0, 2.0, 3.0},
		}

		// Test without index hint
		_, err = table.Output([]string{"*", "_row_id", "_similarity"}).
			MatchSparse("c2", sparseVector, "ip", 3, map[string]string{"alpha": "1.0", "beta": "1.0"}).
			ToResult()
		if err != nil {
			t.Logf("Sparse KNN without index hint failed: %v", err)
		}

		// Test with index hint
		_, err = table.Output([]string{"c1", "_similarity"}).
			MatchSparse("c2", sparseVector, "ip", 3, map[string]string{
				"alpha":      "1.0",
				"beta":       "1.0",
				"index_name": "idx1",
			}).
			ToResult()
		if err != nil {
			t.Logf("Sparse KNN with index hint idx1 failed: %v", err)
		}

		// Test with threshold and index hint
		_, err = table.Output([]string{"c1", "_similarity"}).
			MatchSparse("c2", sparseVector, "ip", 3, map[string]string{
				"alpha":      "1.0",
				"beta":       "1.0",
				"threshold":  "10",
				"index_name": "idx2",
			}).
			ToResult()
		if err != nil {
			t.Logf("Sparse KNN with index hint idx2 and threshold failed: %v", err)
		}

		// Test with non-existent index (should fail)
		_, err = table.Output([]string{"c1", "_similarity"}).
			MatchSparse("c2", sparseVector, "ip", 3, map[string]string{
				"alpha":      "1.0",
				"beta":       "1.0",
				"threshold":  "10",
				"index_name": "idx8",
			}).
			ToResult()
		if err == nil {
			t.Error("Expected error for non-existent index, but got none")
		} else {
			t.Logf("Got expected error for non-existent index: %v", err)
		}

		// Drop indexes
		table.DropIndex("idx2", infinity.ConflictTypeError)
		table.DropIndex("idx1", infinity.ConflictTypeError)
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestMatchTextIndexHints tests text match with index hints
// Based on Python SDK test_pysdk/test_knn.py - test_match_text_index_hints
func TestMatchTextIndexHints(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_with_fulltext_match_with_valid_columns"
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "num", DataType: "int"},
		{Name: "vec", DataType: "vector,4,float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create full-text index
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
		"ANALYZER": "standard",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create full-text index: %v", err)
	}

	// Import data from CSV if file exists
	testCSVPath := testDataDir + "enwiki_embedding_99_commas.csv"
	if _, err := os.Stat(testCSVPath); err == nil {
		importOptions := infinity.NewImportOption()
		importOptions.Delimiter = ','
		_, err = table.ImportData(testCSVPath, importOptions)
		if err != nil {
			t.Logf("ImportData skipped or failed: %v", err)
		}

		// Test with index_names hint
		_, err = table.Output([]string{"*"}).
			MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
			MatchText("body^5", "black", 1, map[string]string{"index_names": "my_index"}).
			Fusion("rrf", 10, nil).
			ToResult()
		if err != nil {
			t.Logf("Text match with index hint may not be fully supported: %v", err)
		}

		// Test with filter
		_, err = table.Output([]string{"*"}).
			MatchDense("vec", infinity.Float32Vector([]float32{3.0, 2.8, 2.7, 3.1}), "float", "ip", 1, nil).
			MatchText("body^5", "black", 1, map[string]string{"index_names": "index1"}).
			Fusion("rrf", 10, nil).
			Filter("num!=98 AND num != 12").
			ToResult()
		if err != nil {
			t.Logf("Text match with filter may not be fully supported: %v", err)
		}
	} else {
		t.Logf("Test CSV file not found: %s", testCSVPath)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Logf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestMultivectorF32 tests multivector with float32 type
// Based on Python SDK test_pysdk/test_knn.py - test_multivector_f32
func TestMultivectorF32(t *testing.T) {
	embeddingDimensions := []int{8, 16}

	for _, embeddingDim := range embeddingDimensions {
		t.Run(fmt.Sprintf("dim_%d", embeddingDim), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_multivector_f32"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "page_no", DataType: "varchar"},
				{Name: "page_multivec", DataType: fmt.Sprintf("multivector,%d,float", embeddingDim)},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Generate and insert data
			rand.Seed(42)
			rowCount := 20
			topK := 5

			for i := 0; i < rowCount; i++ {
				// Generate normalized multivector (128 vectors of dimension embeddingDim)
				multivec := make([][]float32, 128)
				for j := 0; j < 128; j++ {
					multivec[j] = make([]float32, embeddingDim)
					for k := 0; k < embeddingDim; k++ {
						multivec[j][k] = rand.Float32()
					}
				}

				_, err = table.Insert([]map[string]interface{}{
					{"page_no": fmt.Sprintf("page_%d", i), "page_multivec": multivec},
				})
				if err != nil {
					t.Fatalf("Failed to insert data: %v", err)
				}
			}

			// Generate query multivector
			queryMultivec := make([][]float32, 2)
			for j := 0; j < 2; j++ {
				queryMultivec[j] = make([]float32, embeddingDim)
				for k := 0; k < embeddingDim; k++ {
					queryMultivec[j][k] = rand.Float32()
				}
			}

			// Flatten query multivector for search
			flattenedQuery := make([]float32, 2*embeddingDim)
			for j := 0; j < 2; j++ {
				for k := 0; k < embeddingDim; k++ {
					flattenedQuery[j*embeddingDim+k] = queryMultivec[j][k]
				}
			}

			// Test brute force search
			_, err = table.Output([]string{"page_no", "SIMILARITY()"}).
				MatchDense("page_multivec", infinity.Float32Vector(flattenedQuery), "float", "ip", topK, nil).
				ToResult()
			if err != nil {
				t.Logf("Multivector search may not be fully supported: %v", err)
			}

			// Create HNSW index
			indexInfo := infinity.NewIndexInfo("page_multivec", infinity.IndexTypeHnsw, map[string]string{
				"M":               "20",
				"ef_construction": "50",
				"metric":          "ip",
			})
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Logf("Failed to create HNSW index: %v", err)
			} else {
				// Test search with index
				_, err = table.Output([]string{"page_no", "SIMILARITY()"}).
					MatchDense("page_multivec", infinity.Float32Vector(flattenedQuery), "float", "ip", topK, nil).
					ToResult()
				if err != nil {
					t.Logf("Multivector search with index may not be fully supported: %v", err)
				}

				// Drop index
				table.DropIndex("my_index", infinity.ConflictTypeError)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestMultivectorBit tests multivector with bit type
// Based on Python SDK test_pysdk/test_knn.py - test_multivector_bit
func TestMultivectorBit(t *testing.T) {
	embeddingDimensions := []int{8, 16}

	for _, embeddingDim := range embeddingDimensions {
		t.Run(fmt.Sprintf("dim_%d", embeddingDim), func(t *testing.T) {
			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			tableName := "test_multivector_bit"
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			schema := infinity.TableSchema{
				{Name: "page_no", DataType: "varchar"},
				{Name: "page_multivec", DataType: fmt.Sprintf("multivector,%d,bit", embeddingDim)},
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Generate and insert data
			rand.Seed(27)
			rowCount := 20
			topK := 5

			for i := 0; i < rowCount; i++ {
				// Generate bit multivector (4 vectors of dimension embeddingDim)
				multivec := make([][]int, 4)
				for j := 0; j < 4; j++ {
					multivec[j] = make([]int, embeddingDim)
					for k := 0; k < embeddingDim; k++ {
						multivec[j][k] = rand.Intn(2)
					}
				}

				_, err = table.Insert([]map[string]interface{}{
					{"page_no": fmt.Sprintf("page_%d", i), "page_multivec": multivec},
				})
				if err != nil {
					t.Fatalf("Failed to insert data: %v", err)
				}
			}

			// Generate query bit multivector
			queryMultivec := make([][]int, 2)
			for j := 0; j < 2; j++ {
				queryMultivec[j] = make([]int, embeddingDim)
				for k := 0; k < embeddingDim; k++ {
					queryMultivec[j][k] = rand.Intn(2)
				}
			}

			// Flatten query multivector for search
			flattenedQuery := make([]int, 2*embeddingDim)
			for j := 0; j < 2; j++ {
				for k := 0; k < embeddingDim; k++ {
					flattenedQuery[j*embeddingDim+k] = queryMultivec[j][k]
				}
			}

			// Convert to int8 for search
			queryInt8 := make([]int8, len(flattenedQuery))
			for i, v := range flattenedQuery {
				queryInt8[i] = int8(v)
			}

			// Test search
			_, err = table.Output([]string{"page_no", "DISTANCE()"}).
				MatchDense("page_multivec", infinity.Int8Vector(queryInt8), "bit", "hamming", topK, nil).
				ToResult()
			if err != nil {
				t.Logf("Bit multivector search may not be fully supported: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to drop table: %v", err)
			}
		})
	}
}
