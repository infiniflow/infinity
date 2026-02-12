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
	// TODO: Implement query builder
	return t
}

// ToString returns query as string
func (t *Table) ToString() string {
	// TODO: Implement query builder
	return ""
}

// ToResult executes query and returns result
func (t *Table) ToResult() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// Explain returns execution plan
func (t *Table) Explain(explainType ExplainType) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
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
