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

// TestNewQueryBuilder tests creating a new QueryBuilder
func TestNewQueryBuilder(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	if qb == nil {
		t.Fatal("QueryBuilder should not be nil")
	}

	// Verify initial state
	if qb.GetColumns() != nil {
		t.Error("Initial columns should be nil")
	}
	if qb.GetSearch() != nil {
		t.Error("Initial search should be nil")
	}
	if qb.GetFilter() != nil {
		t.Error("Initial filter should be nil")
	}
	if qb.GetLimit() != nil {
		t.Error("Initial limit should be nil")
	}
	if qb.GetOffset() != nil {
		t.Error("Initial offset should be nil")
	}
}

// TestQueryBuilderReset tests the Reset method
func TestQueryBuilderReset(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	// Build a query with various components
	qb.Limit(10).Offset(5)
	qb.Filter("c1 = 1")

	// Reset
	qb.Reset()

	// Verify reset state
	if qb.GetColumns() != nil {
		t.Error("After reset, columns should be nil")
	}
	if qb.GetSearch() != nil {
		t.Error("After reset, search should be nil")
	}
	if qb.GetFilter() != nil {
		t.Error("After reset, filter should be nil")
	}
	if qb.GetLimit() != nil {
		t.Error("After reset, limit should be nil")
	}
	if qb.GetOffset() != nil {
		t.Error("After reset, offset should be nil")
	}
}

// TestQueryBuilderLimit tests the Limit method
func TestQueryBuilderLimit(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	qb.Limit(100)

	limit := qb.GetLimit()
	if limit == nil {
		t.Fatal("Limit should not be nil")
	}

	if limit.Type == nil || limit.Type.ConstantExpr == nil {
		t.Fatal("Limit type or constant expr should not be nil")
	}

	if *limit.Type.ConstantExpr.I64Value != 100 {
		t.Errorf("Expected limit 100, got %d", *limit.Type.ConstantExpr.I64Value)
	}
}

// TestQueryBuilderOffset tests the Offset method
func TestQueryBuilderOffset(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	qb.Offset(50)

	offset := qb.GetOffset()
	if offset == nil {
		t.Fatal("Offset should not be nil")
	}

	if offset.Type == nil || offset.Type.ConstantExpr == nil {
		t.Fatal("Offset type or constant expr should not be nil")
	}

	if *offset.Type.ConstantExpr.I64Value != 50 {
		t.Errorf("Expected offset 50, got %d", *offset.Type.ConstantExpr.I64Value)
	}
}

// TestQueryBuilderFilter tests the Filter method
func TestQueryBuilderFilter(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Filter("c1 = 1")
	if err != nil {
		t.Fatalf("Filter failed: %v", err)
	}

	filter := qb.GetFilter()
	if filter == nil {
		t.Fatal("Filter should not be nil")
	}
}

// TestQueryBuilderFilterInvalid tests the Filter method with invalid expression
func TestQueryBuilderFilterInvalid(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Filter("@@@invalid@@@")
	if err == nil {
		t.Error("Filter should return error for invalid expression")
	}
}

// TestQueryBuilderOutput tests the Output method
func TestQueryBuilderOutput(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Output([]string{"c1", "c2", "*"})
	if err != nil {
		t.Fatalf("Output failed: %v", err)
	}

	columns := qb.GetColumns()
	if columns == nil {
		t.Fatal("Columns should not be nil")
	}

	if len(columns) != 3 {
		t.Errorf("Expected 3 columns, got %d", len(columns))
	}
}

// TestQueryBuilderOutputSpecialColumns tests the Output method with special columns
func TestQueryBuilderOutputSpecialColumns(t *testing.T) {
	specialColumns := []string{
		"_row_id",
		"_create_timestamp",
		"_delete_timestamp",
		"_score",
		"_similarity",
		"_distance",
		"_score_factors",
		"_similarity_factors",
		"_distance_factors",
	}

	qb := infinity.NewQueryBuilder()
	_, err := qb.Output(specialColumns)
	if err != nil {
		t.Fatalf("Output failed: %v", err)
	}

	columns := qb.GetColumns()
	if columns == nil {
		t.Fatal("Columns should not be nil")
	}

	if len(columns) != len(specialColumns) {
		t.Errorf("Expected %d columns, got %d", len(specialColumns), len(columns))
	}

	// Verify that special columns are converted to function expressions
	for i, col := range columns {
		if col.Type == nil || col.Type.FunctionExpr == nil {
			t.Errorf("Column %d should be a function expression", i)
		}
	}
}

// TestQueryBuilderHighlight tests the Highlight method
func TestQueryBuilderHighlight(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Highlight([]string{"c1", "c2"})
	if err != nil {
		t.Fatalf("Highlight failed: %v", err)
	}

	highlight := qb.GetHighlight()
	if highlight == nil {
		t.Fatal("Highlight should not be nil")
	}

	if len(highlight) != 2 {
		t.Errorf("Expected 2 highlight columns, got %d", len(highlight))
	}
}

// TestQueryBuilderGroupBy tests the GroupBy method
func TestQueryBuilderGroupBy(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.GroupBy([]string{"c1", "c2"})
	if err != nil {
		t.Fatalf("GroupBy failed: %v", err)
	}

	groupby := qb.GetGroupBy()
	if groupby == nil {
		t.Fatal("GroupBy should not be nil")
	}

	if len(groupby) != 2 {
		t.Errorf("Expected 2 group by columns, got %d", len(groupby))
	}
}

// TestQueryBuilderGroupBySingleColumn tests the GroupBy method with single column
func TestQueryBuilderGroupBySingleColumn(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.GroupBy("c1")
	if err != nil {
		t.Fatalf("GroupBy failed: %v", err)
	}

	groupby := qb.GetGroupBy()
	if groupby == nil {
		t.Fatal("GroupBy should not be nil")
	}

	if len(groupby) != 1 {
		t.Errorf("Expected 1 group by column, got %d", len(groupby))
	}
}

// TestQueryBuilderHaving tests the Having method
func TestQueryBuilderHaving(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Having("count(*) > 1")
	if err != nil {
		t.Fatalf("Having failed: %v", err)
	}

	having := qb.GetHaving()
	if having == nil {
		t.Fatal("Having should not be nil")
	}
}

// TestQueryBuilderSort tests the Sort method
func TestQueryBuilderSort(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	_, err := qb.Sort([][2]interface{}{
		{"c1", infinity.SortTypeAsc},
		{"c2", infinity.SortTypeDesc},
	})
	if err != nil {
		t.Fatalf("Sort failed: %v", err)
	}

	sort := qb.GetSort()
	if sort == nil {
		t.Fatal("Sort should not be nil")
	}

	if len(sort) != 2 {
		t.Errorf("Expected 2 sort expressions, got %d", len(sort))
	}

	// Verify sort order
	if !sort[0].Asc {
		t.Error("First sort should be ascending")
	}
	if sort[1].Asc {
		t.Error("Second sort should be descending")
	}
}

// TestQueryBuilderSortSpecialColumns tests the Sort method with special columns
func TestQueryBuilderSortSpecialColumns(t *testing.T) {
	specialColumns := [][2]interface{}{
		{"_row_id", infinity.SortTypeAsc},
		{"_create_timestamp", infinity.SortTypeDesc},
		{"_delete_timestamp", infinity.SortTypeAsc},
		{"_score", infinity.SortTypeDesc},
		{"_similarity", infinity.SortTypeAsc},
		{"_distance", infinity.SortTypeDesc},
	}

	qb := infinity.NewQueryBuilder()
	_, err := qb.Sort(specialColumns)
	if err != nil {
		t.Fatalf("Sort failed: %v", err)
	}

	sort := qb.GetSort()
	if sort == nil {
		t.Fatal("Sort should not be nil")
	}

	if len(sort) != len(specialColumns) {
		t.Errorf("Expected %d sort expressions, got %d", len(specialColumns), len(sort))
	}
}

// TestQueryBuilderOption tests the Option method
func TestQueryBuilderOption(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	qb.Option(map[string]interface{}{
		"total_hits_count": true,
	})

	if !qb.GetTotalHitsCount() {
		t.Error("TotalHitsCount should be true")
	}
}

// TestQueryBuilderMatchDense tests the MatchDense method
func TestQueryBuilderMatchDense(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"l2",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchDense failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.MatchExprs) != 1 {
		t.Errorf("Expected 1 match expression, got %d", len(search.MatchExprs))
	}

	// Verify KnnExpr
	knnExpr := search.MatchExprs[0].MatchVectorExpr
	if knnExpr == nil {
		t.Fatal("KnnExpr should not be nil")
	}

	if knnExpr.Topn != 10 {
		t.Errorf("Expected topn 10, got %d", knnExpr.Topn)
	}
}

// TestQueryBuilderMatchDenseInvalidTopN tests MatchDense with invalid topn
func TestQueryBuilderMatchDenseInvalidTopN(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"l2",
		0,
		nil,
	)
	if err == nil {
		t.Error("MatchDense should return error for invalid topn")
	}
}

// TestQueryBuilderMatchDenseInvalidDataType tests MatchDense with invalid data type
func TestQueryBuilderMatchDenseInvalidDataType(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"invalid_type",
		"l2",
		10,
		nil,
	)
	if err == nil {
		t.Error("MatchDense should return error for invalid data type")
	}
}

// TestQueryBuilderMatchDenseInvalidDistanceType tests MatchDense with invalid distance type
func TestQueryBuilderMatchDenseInvalidDistanceType(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"invalid_distance",
		10,
		nil,
	)
	if err == nil {
		t.Error("MatchDense should return error for invalid distance type")
	}
}

// TestQueryBuilderMatchSparse tests the MatchSparse method
func TestQueryBuilderMatchSparse(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	sparseData := &infinity.SparseVector{
		Indices: []int{1, 3, 5},
		Values:  []float64{0.1, 0.2, 0.3},
	}

	_, err := qb.MatchSparse(
		"sparse_col",
		sparseData,
		"ip",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchSparse failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.MatchExprs) != 1 {
		t.Errorf("Expected 1 match expression, got %d", len(search.MatchExprs))
	}

	// Verify MatchSparseExpr
	matchSparseExpr := search.MatchExprs[0].MatchSparseExpr
	if matchSparseExpr == nil {
		t.Fatal("MatchSparseExpr should not be nil")
	}

	if matchSparseExpr.Topn != 10 {
		t.Errorf("Expected topn 10, got %d", matchSparseExpr.Topn)
	}
}

// TestQueryBuilderMatchText tests the MatchText method
func TestQueryBuilderMatchText(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	_, err := qb.MatchText(
		"title,content",
		"search query",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchText failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.MatchExprs) != 1 {
		t.Errorf("Expected 1 match expression, got %d", len(search.MatchExprs))
	}

	// Verify MatchExpr
	matchExpr := search.MatchExprs[0].MatchTextExpr
	if matchExpr == nil {
		t.Fatal("MatchTextExpr should not be nil")
	}

	if matchExpr.Fields != "title,content" {
		t.Errorf("Expected fields 'title,content', got '%s'", matchExpr.Fields)
	}

	if matchExpr.MatchingText != "search query" {
		t.Errorf("Expected matching text 'search query', got '%s'", matchExpr.MatchingText)
	}
}

// TestQueryBuilderMatchTensor tests the MatchTensor method
func TestQueryBuilderMatchTensor(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	queryData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchTensor(
		"tensor_col",
		infinity.Float64Vector(queryData),
		"float",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchTensor failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.MatchExprs) != 1 {
		t.Errorf("Expected 1 match expression, got %d", len(search.MatchExprs))
	}

	// Verify MatchTensorExpr
	matchTensorExpr := search.MatchExprs[0].MatchTensorExpr
	if matchTensorExpr == nil {
		t.Fatal("MatchTensorExpr should not be nil")
	}
}

// TestQueryBuilderFusion tests the Fusion method
func TestQueryBuilderFusion(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	_, err := qb.Fusion(
		"rrf",
		10,
		map[string]interface{}{
			"k": 60,
		},
	)
	if err != nil {
		t.Fatalf("Fusion failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.FusionExprs) != 1 {
		t.Errorf("Expected 1 fusion expression, got %d", len(search.FusionExprs))
	}

	// Verify FusionExpr
	fusionExpr := search.FusionExprs[0]
	if fusionExpr.Method != "rrf" {
		t.Errorf("Expected method 'rrf', got '%s'", fusionExpr.Method)
	}
}

// TestQueryBuilderFusionInvalidMethod tests Fusion with invalid method
func TestQueryBuilderFusionInvalidMethod(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	_, err := qb.Fusion(
		"invalid_method",
		10,
		nil,
	)
	if err == nil {
		t.Error("Fusion should return error for invalid method")
	}
}

// TestQueryBuilderFusionTopNInParams tests Fusion with topn in params (should fail)
func TestQueryBuilderFusionTopNInParams(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	_, err := qb.Fusion(
		"rrf",
		10,
		map[string]interface{}{
			"topn": 20,
		},
	)
	if err == nil {
		t.Error("Fusion should return error when topn is in params")
	}
}

// TestQueryBuilderChainedCalls tests chaining multiple query builder methods
func TestQueryBuilderChainedCalls(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"l2",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchDense failed: %v", err)
	}

	_, err = qb.Filter("c1 = 1")
	if err != nil {
		t.Fatalf("Filter failed: %v", err)
	}

	qb.Limit(100).Offset(10)

	_, err = qb.Output([]string{"c1", "c2", "_score"})
	if err != nil {
		t.Fatalf("Output failed: %v", err)
	}

	_, err = qb.Sort([][2]interface{}{
		{"_score", infinity.SortTypeDesc},
	})
	if err != nil {
		t.Fatalf("Sort failed: %v", err)
	}

	// Verify all components
	if qb.GetSearch() == nil {
		t.Error("Search should not be nil")
	}
	if qb.GetFilter() == nil {
		t.Error("Filter should not be nil")
	}
	if qb.GetLimit() == nil {
		t.Error("Limit should not be nil")
	}
	if qb.GetOffset() == nil {
		t.Error("Offset should not be nil")
	}
	if len(qb.GetColumns()) != 3 {
		t.Errorf("Expected 3 columns, got %d", len(qb.GetColumns()))
	}
	if len(qb.GetSort()) != 1 {
		t.Errorf("Expected 1 sort expression, got %d", len(qb.GetSort()))
	}
}

// TestQueryBuilderMultipleMatchExprs tests adding multiple match expressions
func TestQueryBuilderMultipleMatchExprs(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	// First match dense
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}
	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"l2",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("First MatchDense failed: %v", err)
	}

	// Second match text
	_, err = qb.MatchText(
		"title",
		"query",
		5,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchText failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	if len(search.MatchExprs) != 2 {
		t.Errorf("Expected 2 match expressions, got %d", len(search.MatchExprs))
	}
}

// TestQueryBuilderMatchDenseWithParams tests MatchDense with optional parameters
func TestQueryBuilderMatchDenseWithParams(t *testing.T) {
	qb := infinity.NewQueryBuilder()
	embeddingData := []float64{0.1, 0.2, 0.3, 0.4}

	params := map[string]string{
		"ef":     "100",
		"filter": "c1 > 0",
	}

	_, err := qb.MatchDense(
		"vector_col",
		infinity.Float64Vector(embeddingData),
		"float",
		"l2",
		10,
		params,
	)
	if err != nil {
		t.Fatalf("MatchDense failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	knnExpr := search.MatchExprs[0].MatchVectorExpr
	if knnExpr == nil {
		t.Fatal("KnnExpr should not be nil")
	}

	// Verify opt params
	foundEf := false
	for _, param := range knnExpr.OptParams {
		if param.ParamName == "ef" && param.ParamValue == "100" {
			foundEf = true
			break
		}
	}
	if !foundEf {
		t.Error("Expected 'ef' parameter with value '100'")
	}

	// Verify filter
	if knnExpr.FilterExpr == nil {
		t.Error("FilterExpr should not be nil")
	}
}

// TestQueryBuilderMatchDenseWithDifferentDataTypes tests MatchDense with various data types
func TestQueryBuilderMatchDenseWithDifferentDataTypes(t *testing.T) {
	testCases := []struct {
		name            string
		embeddingData   interface{}
		embeddingType   string
		expectedSuccess bool
	}{
		{"float32", infinity.Float32Vector{0.1, 0.2, 0.3}, "float32", true},
		{"float64", infinity.Float64Vector{0.1, 0.2, 0.3}, "float64", true},
		{"int8", infinity.Float64Vector{1, 2, 3}, "int8", true},
		{"int32", infinity.Float64Vector{1, 2, 3}, "int32", true},
		{"invalid", "not a vector", "float", false},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			qb := infinity.NewQueryBuilder()

			_, err := qb.MatchDense(
				"vector_col",
				tc.embeddingData,
				tc.embeddingType,
				"l2",
				10,
				nil,
			)

			if tc.expectedSuccess && err != nil {
				t.Errorf("Expected success but got error: %v", err)
			}
			if !tc.expectedSuccess && err == nil {
				t.Error("Expected error but got success")
			}
		})
	}
}

// TestQueryBuilderFDE tests MatchDense with FDE (Fixed Dimensional Encoding)
func TestQueryBuilderFDE(t *testing.T) {
	qb := infinity.NewQueryBuilder()

	fde := infinity.FDE{
		TensorData:      [][]float64{{0.1, 0.2}, {0.3, 0.4}},
		TargetDimension: 2,
	}

	_, err := qb.MatchDense(
		"vector_col",
		fde,
		"float",
		"l2",
		10,
		nil,
	)
	if err != nil {
		t.Fatalf("MatchDense with FDE failed: %v", err)
	}

	search := qb.GetSearch()
	if search == nil {
		t.Fatal("Search should not be nil")
	}

	knnExpr := search.MatchExprs[0].MatchVectorExpr
	if knnExpr == nil {
		t.Fatal("KnnExpr should not be nil")
	}

	if knnExpr.QueryEmbeddingExpr == nil {
		t.Error("QueryEmbeddingExpr should not be nil for FDE")
	}

	if knnExpr.QueryEmbeddingExpr.FunctionName != "FDE" {
		t.Errorf("Expected function name 'FDE', got '%s'", knnExpr.QueryEmbeddingExpr.FunctionName)
	}
}
