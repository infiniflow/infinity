// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

// Table defines the interface for table operations
type Table interface {
	// Rename renames the table
	Rename(newTableName string) (interface{}, error)

	// CreateIndex creates an index on the table
	CreateIndex(indexName string, indexInfo *IndexInfo, conflictType ConflictType, indexComment string) (interface{}, error)

	// DropIndex drops an index from the table
	DropIndex(indexName string, conflictType ConflictType) (interface{}, error)

	// ShowIndex shows details of a specific index
	ShowIndex(indexName string) (interface{}, error)

	// ListIndexes lists all indexes on the table
	ListIndexes() (interface{}, error)

	// DumpIndex dumps index information
	DumpIndex(indexName string) (interface{}, error)

	// ShowColumns shows all columns in the table
	ShowColumns() (interface{}, error)

	// ShowSegments shows all segments in the table
	ShowSegments() (interface{}, error)

	// ShowSegment shows details of a specific segment
	ShowSegment(segmentID int) (interface{}, error)

	// ShowBlocks shows all blocks in a segment
	ShowBlocks(segmentID int) (interface{}, error)

	// ShowBlock shows details of a specific block
	ShowBlock(segmentID int, blockID int) (interface{}, error)

	// ShowBlockColumn shows column information for a block
	ShowBlockColumn(segmentID int, blockID int, columnID int) (interface{}, error)

	// Insert inserts data into the table
	// data can be a single map or a slice of maps
	Insert(data interface{}) (interface{}, error)

	// ImportData imports data from a file
	ImportData(filePath string, importOptions *ImportOption) (interface{}, error)

	// ExportData exports data to a file
	ExportData(filePath string, exportOptions *ExportOption, columns []string) (interface{}, error)

	// Delete deletes rows from the table based on a condition
	Delete(cond string) (interface{}, error)

	// Update updates rows in the table based on a condition
	Update(cond string, data map[string]interface{}) (interface{}, error)

	// MatchDense performs dense vector search (KNN)
	MatchDense(vectorColumnName string, embeddingData VEC, embeddingDataType string, distanceType string, topN int, knnParams map[string]string) Table

	// MatchText performs full-text search
	MatchText(fields string, matchingText string, topN int, extraOptions map[string]interface{}) Table

	// MatchTensor performs tensor search
	MatchTensor(columnName string, queryData VEC, queryDataType string, topN int, extraOption map[string]interface{}) Table

	// MatchSparse performs sparse vector search
	MatchSparse(vectorColumnName string, sparseData interface{}, distanceType string, topN int, optParams map[string]string) Table

	// Fusion combines multiple search results
	Fusion(method string, topN int, fusionParams map[string]interface{}) Table

	// Output specifies which columns to return in the result
	Output(columns []string) Table

	// Highlight specifies which columns to highlight
	Highlight(columns []string) Table

	// Filter applies a filter condition to the query
	Filter(filter string) Table

	// Limit sets the maximum number of results to return
	Limit(limit int) Table

	// Offset sets the number of results to skip
	Offset(offset int) Table

	// GroupBy groups results by the given expressions
	GroupBy(groupByExprList interface{}) Table

	// Having applies a having clause to grouped results
	Having(havingExpr string) Table

	// Sort sorts the results by the given expressions
	Sort(orderByExprList [][2]interface{}) Table

	// Option sets query options
	Option(optionKV map[string]interface{}) Table

	// ToString returns the query as a string representation
	ToString() string

	// ToResult executes the query and returns the result
	ToResult() (interface{}, error)

	// ToDF executes the query and returns a DataFrame-like result
	ToDF() (interface{}, error)

	// ToPL executes the query and returns a Polars-like result
	ToPL() (interface{}, error)

	// ToArrow executes the query and returns an Arrow-like result
	ToArrow() (interface{}, error)

	// Explain returns the query execution plan
	Explain(explainType ExplainType) (interface{}, error)

	// Optimize optimizes the table
	Optimize() (interface{}, error)

	// AlterIndex alters an index on the table
	AlterIndex(indexName string, optParams map[string]string) (interface{}, error)

	// AddColumns adds columns to the table
	AddColumns(columnDefs map[string]*ColumnDefinition) (interface{}, error)

	// DropColumns drops columns from the table
	DropColumns(columnNames interface{}) (interface{}, error)

	// Compact compacts the table
	Compact() (interface{}, error)
}
