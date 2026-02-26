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

package infinity

// SearchExpr represents a search expression
type SearchExpr struct {
	MatchExprs []interface{}
	FusionExpr interface{}
}

// MatchExpr represents a match expression
type MatchExpr struct {
	Fields        []string
	MatchingText  string
	OptionsText   string
}

// MatchDenseExpr represents a dense vector match expression
type MatchDenseExpr struct {
	ColumnName       string
	EmbeddingData    VEC
	EmbeddingDataType string
	DistanceType     string
	TopN             int
	Options          map[string]string
}

// MatchSparseExpr represents a sparse vector match expression
type MatchSparseExpr struct {
	ColumnName   string
	SparseData   *SparseVector
	DistanceType string
	TopN         int
	Options      map[string]string
}

// MatchTensorExpr represents a tensor match expression
type MatchTensorExpr struct {
	ColumnName    string
	QueryData     VEC
	QueryDataType string
	TopN          int
	Options       map[string]interface{}
}

// FusionExpr represents a fusion expression
type FusionExpr struct {
	Method      string
	TopN        int
	FusionParams map[string]interface{}
}

// OrderByExpr represents an order by expression
type OrderByExpr struct {
	Expr     interface{}
	SortType SortType
}

// ParsedExpr represents a parsed expression
type ParsedExpr struct {
	Type interface{}
}

// Query represents a query structure
type Query struct {
	Columns          []interface{}
	Highlight        []interface{}
	Search           *SearchExpr
	Filter           interface{}
	GroupBy          []interface{}
	Having           interface{}
	Limit            interface{}
	Offset           interface{}
	Sort             []*OrderByExpr
	TotalHitsCount   bool
}

// ExplainQuery represents an explain query structure
type ExplainQuery struct {
	Columns     []interface{}
	Highlight   []interface{}
	Search      *SearchExpr
	Filter      interface{}
	Limit       interface{}
	Offset      interface{}
	Sort        []*OrderByExpr
	ExplainType ExplainType
}
