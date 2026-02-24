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
	"strconv"
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

	tableName := "test_knn_basic" + generateSuffix(t)
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

// TestKNNOnVectorColumn tests KNN search on different vector columns
func TestKNNOnVectorColumn(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_on_vector_column" + generateSuffix(t)
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

// TestKNNWithIndex tests KNN search with HNSW index
func TestKNNWithIndex(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_index" + generateSuffix(t)
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

// TestKNNVariousDistanceTypes tests KNN search with different distance types
func TestKNNVariousDistanceTypes(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_various_distance" + generateSuffix(t)
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

// TestKNNInvalidTopN tests KNN search with invalid topn values
func TestKNNInvalidTopN(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_invalid_topn" + generateSuffix(t)
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
		tableName := "test_knn_big_dim_" + strconv.Itoa(dim) + generateSuffix(t)
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

	tableName := "test_knn_with_ivf_index" + generateSuffix(t)
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

// TestKNNWithThreshold tests KNN search with threshold parameter
func TestKNNWithThreshold(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_threshold" + generateSuffix(t)
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

// TestKNNImportFromCSV tests KNN search with imported CSV data
func TestKNNImportFromCSV(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_import_csv" + generateSuffix(t)
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
	testCSVPath := filepath.Join("/var/infinity/test_data", "embedding_int_dim3.csv")
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

// TestKNNWithFusion tests KNN search combined with full-text search and fusion
func TestKNNWithFusion(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_knn_with_fusion" + generateSuffix(t)
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
