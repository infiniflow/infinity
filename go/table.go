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

import (
	"context"
	"fmt"
	"math"

	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// Table represents a table for remote operations
type Table struct {
	db        *Database
	tableName string
	// Query builder fields
	outputColumns []string
	queryBuilder  *QueryBuilder
}

// Rename renames the table
func (t *Table) Rename(newTableName string) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewRenameTableRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.NewTableName_ = newTableName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.RenameTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to rename table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to rename table: %s", resp.ErrorMsg),
		)
	}

	// Update table name on success
	t.tableName = newTableName

	return resp, nil
}

// CreateIndex creates an index
func (t *Table) CreateIndex(indexName string, indexInfo *IndexInfo, conflictType ConflictType, indexComment string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// DropIndex drops an index
func (t *Table) DropIndex(indexName string, conflictType ConflictType) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowIndex shows index details
func (t *Table) ShowIndex(indexName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ListIndexes lists all indexes
func (t *Table) ListIndexes() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// DumpIndex dumps index information
func (t *Table) DumpIndex(indexName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowColumns shows all columns
func (t *Table) ShowColumns() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowSegments shows all segments
func (t *Table) ShowSegments() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowSegment shows segment details
func (t *Table) ShowSegment(segmentID int) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowBlocks shows all blocks in a segment
func (t *Table) ShowBlocks(segmentID int) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowBlock shows block details
func (t *Table) ShowBlock(segmentID int, blockID int) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowBlockColumn shows column information for a block
func (t *Table) ShowBlockColumn(segmentID int, blockID int, columnID int) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// Insert inserts data
func (t *Table) Insert(data interface{}) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ImportData imports data from a file
func (t *Table) ImportData(filePath string, importOptions *ImportOption) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Use default import options if nil
	if importOptions == nil {
		importOptions = NewImportOption()
	}

	// Create thrift ImportOption
	thriftImportOption := thriftapi.NewImportOption()
	thriftImportOption.Delimiter = string(importOptions.Delimiter)
	thriftImportOption.HasHeader = importOptions.HasHeader

	// Convert CopyFileType to thrift CopyFileType
	var thriftCopyFileType thriftapi.CopyFileType
	switch importOptions.CopyFileType {
	case CopyFileTypeCSV:
		thriftCopyFileType = thriftapi.CopyFileType_CSV
	case CopyFileTypeJSON:
		thriftCopyFileType = thriftapi.CopyFileType_JSON
	case CopyFileTypeJSONL:
		thriftCopyFileType = thriftapi.CopyFileType_JSONL
	case CopyFileTypeFVECS:
		thriftCopyFileType = thriftapi.CopyFileType_FVECS
	case CopyFileTypeCSR:
		thriftCopyFileType = thriftapi.CopyFileType_CSR
	case CopyFileTypeBVECS:
		thriftCopyFileType = thriftapi.CopyFileType_BVECS
	default:
		return nil, NewInfinityException(int(ErrorCodeImportFileFormatError), fmt.Sprintf("Invalid copy file type: %d", importOptions.CopyFileType))
	}
	thriftImportOption.CopyFileType = thriftCopyFileType

	// Create request
	req := thriftapi.NewImportRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.FileName = filePath
	req.ImportOption = thriftImportOption

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.Import(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to import data: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to import data: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ExportData exports data to a file
func (t *Table) ExportData(filePath string, exportOptions *ExportOption, columns []string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// Delete deletes rows
func (t *Table) Delete(cond string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// Update updates rows
func (t *Table) Update(cond string, data map[string]interface{}) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// MatchDense performs dense vector search
func (t *Table) MatchDense(vectorColumnName string, embeddingData interface{}, embeddingDataType string, distanceType string, topN int, knnParams map[string]string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchDense(vectorColumnName, embeddingData, embeddingDataType, distanceType, topN, knnParams)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchText performs full-text search
func (t *Table) MatchText(fields string, matchingText string, topN int, extraOptions map[string]string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchText(fields, matchingText, topN, extraOptions)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchTensor performs tensor search
func (t *Table) MatchTensor(columnName string, queryData interface{}, queryDataType string, topN int, extraOption map[string]string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchTensor(columnName, queryData, queryDataType, topN, extraOption)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchSparse performs sparse vector search
func (t *Table) MatchSparse(vectorColumnName string, sparseData *SparseVector, distanceType string, topN int, optParams map[string]string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchSparse(vectorColumnName, sparseData, distanceType, topN, optParams)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Fusion combines multiple search results
func (t *Table) Fusion(method string, topN int, fusionParams map[string]interface{}) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Fusion(method, topN, fusionParams)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Output specifies output columns
func (t *Table) Output(columns []string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Output(columns)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Highlight specifies highlight columns
func (t *Table) Highlight(columns []string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Highlight(columns)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Filter applies filter
func (t *Table) Filter(filter string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Filter(filter)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Limit sets limit
func (t *Table) Limit(limit int) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	t.queryBuilder = t.queryBuilder.Limit(limit)
	return t
}

// Offset sets offset
func (t *Table) Offset(offset int) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	t.queryBuilder = t.queryBuilder.Offset(offset)
	return t
}

// GroupBy groups results
func (t *Table) GroupBy(groupByExprList interface{}) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.GroupBy(groupByExprList)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Having applies having clause
func (t *Table) Having(havingExpr string) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Having(havingExpr)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Sort sorts results
func (t *Table) Sort(orderByExprList [][2]interface{}) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Sort(orderByExprList)
	if err != nil {
		return nil
	}
	t.queryBuilder = queryBuilder
	return t
}

// Option sets options
func (t *Table) Option(optionKV map[string]interface{}) *Table {
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	t.queryBuilder.Option(optionKV)
	return t
}

// ToString returns query as string
func (t *Table) ToString() string {
	// TODO: Implement query builder
	return ""
}

// ToResult executes query and returns result
func (t *Table) ToResult() (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Initialize query builder if not exists
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}

	// Build select request from query builder
	req := thriftapi.NewSelectRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName

	// Set select list (columns)
	if columns := t.queryBuilder.GetColumns(); columns != nil {
		req.SelectList = columns
	}

	// Set highlight list
	if highlight := t.queryBuilder.GetHighlight(); highlight != nil {
		req.HighlightList = &highlight
	}

	// Set search expression
	if search := t.queryBuilder.GetSearch(); search != nil {
		req.SearchExpr = search
	}

	// Set where expression (filter)
	if filter := t.queryBuilder.GetFilter(); filter != nil {
		req.WhereExpr = filter
	}

	// Set limit expression
	if limit := t.queryBuilder.GetLimit(); limit != nil {
		req.LimitExpr = limit
	}

	// Set offset expression
	if offset := t.queryBuilder.GetOffset(); offset != nil {
		req.OffsetExpr = offset
	}

	// Set order by list (sort)
	if sort := t.queryBuilder.GetSort(); sort != nil {
		req.OrderByList = &sort
	}

	// Set total hits count
	totalHitsCount := t.queryBuilder.GetTotalHitsCount()
	req.TotalHitsCount = &totalHitsCount

	// Call thrift Select
	ctx := context.Background()
	resp, err := t.db.conn.client.Select(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to execute query: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to execute query: %s", resp.ErrorMsg),
		)
	}

	// Reset query builder after execution
	t.queryBuilder.Reset()

	// Build and return result
	return buildResult(resp)
}

// Explain returns execution plan
func (t *Table) Explain(explainType ExplainType) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Initialize query builder if not exists
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}

	// Build explain request from query builder
	req := thriftapi.NewExplainRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName

	// Set select list (columns)
	if columns := t.queryBuilder.GetColumns(); columns != nil {
		req.SelectList = columns
	}

	// Set highlight list
	if highlight := t.queryBuilder.GetHighlight(); highlight != nil {
		req.HighlightList = &highlight
	}

	// Set search expression
	if search := t.queryBuilder.GetSearch(); search != nil {
		req.SearchExpr = search
	}

	// Set where expression (filter)
	if filter := t.queryBuilder.GetFilter(); filter != nil {
		req.WhereExpr = filter
	}

	// Set limit expression
	if limit := t.queryBuilder.GetLimit(); limit != nil {
		req.LimitExpr = limit
	}

	// Set offset expression
	if offset := t.queryBuilder.GetOffset(); offset != nil {
		req.OffsetExpr = offset
	}

	// Set order by list (sort)
	if sort := t.queryBuilder.GetSort(); sort != nil {
		req.OrderByList = &sort
	}

	// Set explain type
	var thriftExplainType thriftapi.ExplainType
	switch explainType {
	case ExplainTypeAnalyze:
		thriftExplainType = thriftapi.ExplainType_Analyze
	case ExplainTypeAst:
		thriftExplainType = thriftapi.ExplainType_Ast
	case ExplainTypeUnOpt:
		thriftExplainType = thriftapi.ExplainType_UnOpt
	case ExplainTypeOpt:
		thriftExplainType = thriftapi.ExplainType_Opt
	case ExplainTypePhysical:
		thriftExplainType = thriftapi.ExplainType_Physical
	case ExplainTypePipeline:
		thriftExplainType = thriftapi.ExplainType_Pipeline
	case ExplainTypeFragment:
		thriftExplainType = thriftapi.ExplainType_Fragment
	default:
		thriftExplainType = thriftapi.ExplainType_Physical
	}
	req.ExplainType = thriftExplainType

	// Call thrift Explain
	ctx := context.Background()
	resp, err := t.db.conn.client.Explain(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to execute explain: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to execute explain: %s", resp.ErrorMsg),
		)
	}

	// Reset query builder after execution
	t.queryBuilder.Reset()

	// Return result
	return buildResult(resp)
}

// Optimize optimizes the table
func (t *Table) Optimize() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// AlterIndex alters an index
func (t *Table) AlterIndex(indexName string, optParams map[string]string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// AddColumns adds columns to the table
func (t *Table) AddColumns(columnDefs map[string]*ColumnDefinition) (interface{}, error) {
	if t.db == nil || t.db.conn == nil || !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert column definitions to thrift ColumnDefs
	columnDefsList := make([]*thriftapi.ColumnDef, 0, len(columnDefs))
	index := 0
	for columnName, columnInfo := range columnDefs {
		colDef := thriftapi.NewColumnDef()
		colDef.ID = int32(index)
		colDef.Name = columnName
		colDef.Comment = ""

		// Parse data type
		dataType, err := parseDataType(columnInfo.DataType)
		if err != nil {
			return nil, NewInfinityException(int(ErrorCodeInvalidDataType), fmt.Sprintf("Invalid data type for column %s: %v", columnName, err))
		}
		colDef.DataType = dataType

		// Convert constraints
		constraints := make([]thriftapi.Constraint, 0, len(columnInfo.Constraints))
		for _, constraint := range columnInfo.Constraints {
			var thriftConstraint thriftapi.Constraint
			switch constraint {
			case ConstraintPrimaryKey:
				thriftConstraint = thriftapi.Constraint_PrimaryKey
			case ConstraintNotNull:
				thriftConstraint = thriftapi.Constraint_NotNull
			case ConstraintNull:
				thriftConstraint = thriftapi.Constraint_Null
			case ConstraintUnique:
				thriftConstraint = thriftapi.Constraint_Unique
			default:
				return nil, NewInfinityException(int(ErrorCodeInvalidConstraintType), fmt.Sprintf("Invalid constraint type: %s", constraint))
			}
			constraints = append(constraints, thriftConstraint)
		}
		colDef.Constraints = constraints

		// Handle default value if provided
		if columnInfo.Default != nil {
			constExpr, err := parseDefaultValue(columnInfo.Default)
			if err != nil {
				return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), fmt.Sprintf("Invalid default value for column %s: %v", columnName, err))
			}
			colDef.ConstantExpr = constExpr
		}

		columnDefsList = append(columnDefsList, colDef)
		index++
	}

	// Create request
	req := thriftapi.NewAddColumnsRequest()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SessionID = t.db.conn.GetSessionID()
	req.ColumnDefs = columnDefsList

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.AddColumns(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to add columns: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to add columns: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// DropColumns drops columns from the table
func (t *Table) DropColumns(columnNames interface{}) (interface{}, error) {
	if t.db == nil || t.db.conn == nil || !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert columnNames to []string
	var columnNamesList []string
	switch v := columnNames.(type) {
	case string:
		columnNamesList = []string{v}
	case []string:
		columnNamesList = v
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), "columnNames must be a string or []string")
	}

	// Create request
	req := thriftapi.NewDropColumnsRequest()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SessionID = t.db.conn.GetSessionID()
	req.ColumnNames = columnNamesList

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.DropColumns(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to drop columns: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to drop columns: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// Compact compacts the table
func (t *Table) Compact() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// QueryResult represents the result of a query
type QueryResult struct {
	Data      map[string][]interface{}
	DataTypes map[string]string
	ExtraInfo string
}

// buildResult builds a QueryResult from SelectResponse
func buildResult(resp *thriftapi.SelectResponse) (*QueryResult, error) {
	result := &QueryResult{
		Data:      make(map[string][]interface{}),
		DataTypes: make(map[string]string),
		ExtraInfo: resp.ExtraResult_,
	}

	// Process column definitions to get data types
	for _, colDef := range resp.ColumnDefs {
		if colDef.DataType != nil {
			result.DataTypes[colDef.Name] = colDef.DataType.LogicType.String()
		}
	}

	// Process column fields to get data
	for _, colField := range resp.ColumnFields {
		columnName := colField.ColumnName
		columnType := colField.ColumnType
		columnVectors := colField.ColumnVectors

		// Parse column vectors based on column type
		values := parseColumnVectors(columnType, columnVectors, colField.Bitmasks)
		result.Data[columnName] = values
	}

	return result, nil
}

// parseColumnVectors parses column vectors based on column type
func parseColumnVectors(columnType thriftapi.ColumnType, columnVectors [][]byte, bitmasks []bool) []interface{} {
	if len(columnVectors) == 0 {
		return []interface{}{}
	}

	values := make([]interface{}, 0)

	switch columnType {
	case thriftapi.ColumnType_ColumnInt8,
		thriftapi.ColumnType_ColumnInt16,
		thriftapi.ColumnType_ColumnInt32,
		thriftapi.ColumnType_ColumnInt64:
		// Integer types
		for _, vector := range columnVectors {
			ints := parseIntVector(vector)
			for _, v := range ints {
				values = append(values, v)
			}
		}
	case thriftapi.ColumnType_ColumnFloat32,
		thriftapi.ColumnType_ColumnFloat64,
		thriftapi.ColumnType_ColumnFloat16,
		thriftapi.ColumnType_ColumnBFloat16:
		// Float types
		for _, vector := range columnVectors {
			floats := parseFloatVector(vector)
			for _, v := range floats {
				values = append(values, v)
			}
		}
	case thriftapi.ColumnType_ColumnVarchar:
		// String type
		for _, vector := range columnVectors {
			strs := parseStringVector(vector)
			for _, v := range strs {
				values = append(values, v)
			}
		}
	case thriftapi.ColumnType_ColumnBool:
		// Boolean type
		for _, vector := range columnVectors {
			bools := parseBoolVector(vector)
			for _, v := range bools {
				values = append(values, v)
			}
		}
	case thriftapi.ColumnType_ColumnEmbedding,
		thriftapi.ColumnType_ColumnMultiVector,
		thriftapi.ColumnType_ColumnTensor,
		thriftapi.ColumnType_ColumnTensorArray,
		thriftapi.ColumnType_ColumnSparse:
		// Complex types - store as bytes for now
		for _, vector := range columnVectors {
			values = append(values, vector)
		}
	default:
		// Default: store as bytes
		for _, vector := range columnVectors {
			values = append(values, vector)
		}
	}

	// Apply bitmasks if present
	if len(bitmasks) > 0 && len(bitmasks) == len(values) {
		filteredValues := make([]interface{}, 0)
		for i, v := range values {
			if i < len(bitmasks) && !bitmasks[i] {
				filteredValues = append(filteredValues, v)
			}
		}
		return filteredValues
	}

	return values
}

// parseIntVector parses integer vector from bytes
func parseIntVector(data []byte) []int64 {
	// Simple implementation: assume 8-byte integers
	if len(data)%8 != 0 {
		return []int64{}
	}
	count := len(data) / 8
	result := make([]int64, count)
	for i := 0; i < count; i++ {
		result[i] = int64(data[i*8]) |
			int64(data[i*8+1])<<8 |
			int64(data[i*8+2])<<16 |
			int64(data[i*8+3])<<24 |
			int64(data[i*8+4])<<32 |
			int64(data[i*8+5])<<40 |
			int64(data[i*8+6])<<48 |
			int64(data[i*8+7])<<56
	}
	return result
}

// parseFloatVector parses float vector from bytes
func parseFloatVector(data []byte) []float64 {
	// Simple implementation: assume 8-byte floats
	if len(data)%8 != 0 {
		return []float64{}
	}
	count := len(data) / 8
	result := make([]float64, count)
	for i := 0; i < count; i++ {
		bits := uint64(data[i*8]) |
			uint64(data[i*8+1])<<8 |
			uint64(data[i*8+2])<<16 |
			uint64(data[i*8+3])<<24 |
			uint64(data[i*8+4])<<32 |
			uint64(data[i*8+5])<<40 |
			uint64(data[i*8+6])<<48 |
			uint64(data[i*8+7])<<56
		result[i] = math.Float64frombits(bits)
	}
	return result
}

// parseStringVector parses string vector from bytes
func parseStringVector(data []byte) []string {
	// Simple implementation: split by null terminator
	result := make([]string, 0)
	start := 0
	for i, b := range data {
		if b == 0 {
			if i > start {
				result = append(result, string(data[start:i]))
			}
			start = i + 1
		}
	}
	if start < len(data) {
		result = append(result, string(data[start:]))
	}
	return result
}

// parseBoolVector parses boolean vector from bytes
func parseBoolVector(data []byte) []bool {
	result := make([]bool, len(data))
	for i, b := range data {
		result[i] = b != 0
	}
	return result
}
