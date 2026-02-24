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
	"math"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestRankFeaturesBasic tests basic rank_features functionality
func TestRankFeaturesBasic(t *testing.T) {
	tableName := "test_rank_features_basic"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "id", DataType: "varchar"},
		{Name: "content", DataType: "varchar"},
		{Name: "tag", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"id": "2024", "content": "hello world", "tag": `[{"Tag1":1},{"Tag2":2}]`},
		{"id": "2025", "content": "hello world", "tag": `[{"Tag1":2},{"Tag2":1}]`},
		{"id": "2026", "content": "hello world", "tag": `[{"Tag2":3},{"Tag3":4}]`},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create fulltext index on content
	indexInfo := infinity.NewIndexInfo("content", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("content_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create content index: %v", err)
	}
	defer table.DropIndex("content_index", infinity.ConflictTypeError)

	// Create rank_features index on tag
	indexInfo2 := infinity.NewIndexInfo("tag", infinity.IndexTypeFullText, map[string]string{"analyzer": "rankfeatures"})
	_, err = table.CreateIndex("tag_index", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create tag index: %v", err)
	}
	defer table.DropIndex("tag_index", infinity.ConflictTypeError)

	// First test: basic fulltext search without rank_features
	table2 := table.Output([]string{"id", "_score"})
	table2.MatchText("content", "hello world", 10, nil)
	res, err := table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query basic fulltext: %v", err)
	}

	var basicScores []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if idCol, exists := result.Data["id"]; exists {
			if len(idCol) != 3 {
				t.Errorf("Basic search: expected 3 rows, got %d", len(idCol))
			}
		}
		if scoreCol, exists := result.Data["SCORE"]; exists {
			basicScores = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					basicScores[i] = val
				case float32:
					basicScores[i] = float64(val)
				}
			}
			// All scores should be positive
			for _, score := range basicScores {
				if score <= 0 {
					t.Error("All basic scores should be positive")
				}
			}
		}
	}

	// Second test: fulltext search with rank_features
	table3 := table.Output([]string{"id", "_score"})
	table3.MatchText("content", "hello world", 10, map[string]string{"rank_features": "tag^Tag1^1.0,tag^Tag2^2.0"})
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with rank_features: %v", err)
	}

	var rankScores []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if idCol, exists := result.Data["id"]; exists {
			if len(idCol) != 3 {
				t.Errorf("Rank features search: expected 3 rows, got %d", len(idCol))
			}
		}
		if scoreCol, exists := result.Data["SCORE"]; exists {
			rankScores = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					rankScores[i] = val
				case float32:
					rankScores[i] = float64(val)
				}
			}
		}
	}

	// Verify that rank_features changed the scores
	scoresEqual := true
	if len(basicScores) == len(rankScores) {
		for i := range basicScores {
			if math.Abs(basicScores[i]-rankScores[i]) > 0.001 {
				scoresEqual = false
				break
			}
		}
	}
	if scoresEqual {
		t.Error("Rank features should change the scores compared to basic search")
	}
}

// TestRankFeaturesDifferentWeights tests rank_features with different feature weights
func TestRankFeaturesDifferentWeights(t *testing.T) {
	tableName := "test_rank_features_weights"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "id", DataType: "varchar"},
		{Name: "content", DataType: "varchar"},
		{Name: "features", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"id": "doc1", "content": "test document", "features": `[{"feature1":0.8},{"feature2":0.2}]`},
		{"id": "doc2", "content": "test document", "features": `[{"feature1":0.3},{"feature2":0.7}]`},
		{"id": "doc3", "content": "test document", "features": `[{"feature1":0.5},{"feature3":0.9}]`},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create indexes
	indexInfo := infinity.NewIndexInfo("content", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("content_idx", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create content index: %v", err)
	}
	defer table.DropIndex("content_idx", infinity.ConflictTypeError)

	indexInfo2 := infinity.NewIndexInfo("features", infinity.IndexTypeFullText, map[string]string{"analyzer": "rankfeatures"})
	_, err = table.CreateIndex("features_idx", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create features index: %v", err)
	}
	defer table.DropIndex("features_idx", infinity.ConflictTypeError)

	// First test: basic fulltext search without rank_features
	table2 := table.Output([]string{"id", "_score"})
	table2.MatchText("content", "test document", 10, nil)
	res, err := table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query basic fulltext: %v", err)
	}

	var basicScores []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if scoreCol, exists := result.Data["SCORE"]; exists {
			basicScores = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					basicScores[i] = val
				case float32:
					basicScores[i] = float64(val)
				}
			}
		}
	}

	// Second test: search with feature1 boost
	table3 := table.Output([]string{"id", "_score"})
	table3.MatchText("content", "test document", 10, map[string]string{"rank_features": "features^feature1^2.0"})
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with feature1 boost: %v", err)
	}

	var scores1 []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if scoreCol, exists := result.Data["SCORE"]; exists {
			scores1 = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					scores1[i] = val
				case float32:
					scores1[i] = float64(val)
				}
			}
		}
	}

	// Verify that feature1 boost changed scores
	scoresEqual := true
	if len(basicScores) == len(scores1) {
		for i := range basicScores {
			if math.Abs(basicScores[i]-scores1[i]) > 0.001 {
				scoresEqual = false
				break
			}
		}
	}
	if scoresEqual {
		t.Error("Feature1 boost should change scores from basic search")
	}

	// Third test: search with multiple features and different boosts
	table4 := table.Output([]string{"id", "_score"})
	table4.MatchText("content", "test document", 10, map[string]string{"rank_features": "features^feature1^1.0,features^feature2^3.0"})
	res, err = table4.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with multiple features: %v", err)
	}

	var scores2 []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if scoreCol, exists := result.Data["SCORE"]; exists {
			scores2 = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					scores2[i] = val
				case float32:
					scores2[i] = float64(val)
				}
			}
		}
	}

	// Verify that different feature combinations produce different scores
	scoresEqual = true
	if len(basicScores) == len(scores2) {
		for i := range basicScores {
			if math.Abs(basicScores[i]-scores2[i]) > 0.001 {
				scoresEqual = false
				break
			}
		}
	}
	if scoresEqual {
		t.Error("Multiple features should change scores from basic search")
	}

	// Verify all scores are positive
	for _, score := range scores2 {
		if score <= 0 {
			t.Error("All scores should be positive")
		}
	}
}

// TestRankFeaturesWithTopn tests rank_features with topn parameter
func TestRankFeaturesWithTopn(t *testing.T) {
	tableName := "test_rank_features_topn"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "id", DataType: "varchar"},
		{Name: "content", DataType: "varchar"},
		{Name: "tags", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert test data
	testData := make([]map[string]interface{}, 10)
	for i := 0; i < 10; i++ {
		value := fmt.Sprintf("%d", i+1)
		testData[i] = map[string]interface{}{
			"id":      "doc" + string(rune('0'+i)),
			"content": "search query",
			"tags":    `[{"tag` + string(rune('0'+i%3)) + `":` + value + `}]`,
		}
	}
	_, err = table.Insert(testData)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create indexes
	indexInfo := infinity.NewIndexInfo("content", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("content_idx", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create content index: %v", err)
	}
	defer table.DropIndex("content_idx", infinity.ConflictTypeError)

	indexInfo2 := infinity.NewIndexInfo("tags", infinity.IndexTypeFullText, map[string]string{"analyzer": "rankfeatures"})
	_, err = table.CreateIndex("tags_idx", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create tags index: %v", err)
	}
	defer table.DropIndex("tags_idx", infinity.ConflictTypeError)

	// First test: basic search without rank_features
	table2 := table.Output([]string{"id", "_score"})
	table2.MatchText("content", "search query", 10, nil)
	res, err := table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query basic fulltext: %v", err)
	}

	var basicScores []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if idCol, exists := result.Data["id"]; exists {
			if len(idCol) != 10 {
				t.Errorf("Basic search: expected 10 rows, got %d", len(idCol))
			}
		}
		if scoreCol, exists := result.Data["SCORE"]; exists {
			basicScores = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					basicScores[i] = val
				case float32:
					basicScores[i] = float64(val)
				}
			}
		}
	}

	// Second test: search with rank_features and topn=3
	table3 := table.Output([]string{"id", "_score"})
	table3.MatchText("content", "search query", 3, map[string]string{"rank_features": "tags^tag0^1.0,tags^tag1^1.0,tags^tag2^1.0"})
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with topn=3: %v", err)
	}

	var scores3 []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if idCol, exists := result.Data["id"]; exists {
			if len(idCol) != 3 {
				t.Errorf("Topn=3 search: expected 3 rows, got %d", len(idCol))
			}
		}
		if scoreCol, exists := result.Data["SCORE"]; exists {
			scores3 = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					scores3[i] = val
				case float32:
					scores3[i] = float64(val)
				}
			}
		}
	}

	// Third test: search with rank_features and topn=5
	table4 := table.Output([]string{"id", "_score"})
	table4.MatchText("content", "search query", 5, map[string]string{"rank_features": "tags^tag0^1.0,tags^tag1^1.0,tags^tag2^1.0"})
	res, err = table4.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with topn=5: %v", err)
	}

	var scores5 []float64
	if result, ok := res.(*infinity.QueryResult); ok {
		if idCol, exists := result.Data["id"]; exists {
			if len(idCol) != 5 {
				t.Errorf("Topn=5 search: expected 5 rows, got %d", len(idCol))
			}
		}
		if scoreCol, exists := result.Data["SCORE"]; exists {
			scores5 = make([]float64, len(scoreCol))
			for i, v := range scoreCol {
				switch val := v.(type) {
				case float64:
					scores5[i] = val
				case float32:
					scores5[i] = float64(val)
				}
			}
		}
	}

	// Verify that rank_features changed the scores
	scoresEqual := true
	if len(basicScores) >= 3 && len(scores3) == 3 {
		for i := 0; i < 3; i++ {
			if math.Abs(basicScores[i]-scores3[i]) > 0.001 {
				scoresEqual = false
				break
			}
		}
	}
	if scoresEqual {
		t.Error("Rank features should change scores from basic search")
	}
}
