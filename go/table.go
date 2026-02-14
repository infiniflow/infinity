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
	err       error // accumulated error from chainable methods
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
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert ConflictType to CreateConflict
	var createConflict thriftapi.CreateConflict
	switch conflictType {
	case ConflictTypeIgnore:
		createConflict = thriftapi.CreateConflict_Ignore
	case ConflictTypeError:
		createConflict = thriftapi.CreateConflict_Error
	case ConflictTypeReplace:
		createConflict = thriftapi.CreateConflict_Replace
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConflictType), "Invalid conflict type")
	}

	// Convert IndexType to thrift IndexType
	var thriftIndexType thriftapi.IndexType
	switch indexInfo.IndexType {
	case IndexTypeIVF:
		thriftIndexType = thriftapi.IndexType_IVF
	case IndexTypeHnsw:
		thriftIndexType = thriftapi.IndexType_Hnsw
	case IndexTypeFullText:
		thriftIndexType = thriftapi.IndexType_FullText
	case IndexTypeSecondary:
		thriftIndexType = thriftapi.IndexType_Secondary
	case IndexTypeSecondaryFunctional:
		thriftIndexType = thriftapi.IndexType_SecondaryFunctional
	case IndexTypeEMVB:
		thriftIndexType = thriftapi.IndexType_EMVB
	case IndexTypeBMP:
		thriftIndexType = thriftapi.IndexType_BMP
	case IndexTypeDiskAnn:
		thriftIndexType = thriftapi.IndexType_DiskAnn
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidIndexType), "Invalid index type")
	}

	// Build InitParameter list
	var initParams []*thriftapi.InitParameter
	if indexInfo.Params != nil {
		for key, value := range indexInfo.Params {
			param := thriftapi.NewInitParameter()
			param.ParamName = key
			param.ParamValue = value
			initParams = append(initParams, param)
		}
	}

	// Build thrift IndexInfo
	thriftIndexInfo := thriftapi.NewIndexInfo()
	thriftIndexInfo.ColumnName = indexInfo.TargetName
	thriftIndexInfo.IndexType = thriftIndexType
	thriftIndexInfo.IndexParamList = initParams

	// Create request
	req := thriftapi.NewCreateIndexRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.IndexName = indexName
	req.IndexComment = indexComment
	req.IndexInfo = thriftIndexInfo
	req.CreateOption = thriftapi.NewCreateOption()
	req.CreateOption.ConflictType = createConflict

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.CreateIndex(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create index: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create index: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// DropIndex drops an index
func (t *Table) DropIndex(indexName string, conflictType ConflictType) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert ConflictType to DropConflict
	var dropConflict thriftapi.DropConflict
	switch conflictType {
	case ConflictTypeIgnore:
		dropConflict = thriftapi.DropConflict_Ignore
	case ConflictTypeError:
		dropConflict = thriftapi.DropConflict_Error
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConflictType), "Invalid conflict type")
	}

	// Create request
	req := thriftapi.NewDropIndexRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.IndexName = indexName
	req.DropOption = thriftapi.NewDropOption()
	req.DropOption.ConflictType = dropConflict

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.DropIndex(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to drop index: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to drop index: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowIndex shows index details
func (t *Table) ShowIndex(indexName string) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowIndexRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.IndexName = indexName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowIndex(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show index: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show index: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ListIndexes lists all indexes
func (t *Table) ListIndexes() (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewListIndexRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ListIndex(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to list indexes: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to list indexes: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// DumpIndex dumps index information
func (t *Table) DumpIndex(indexName string) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewDumpIndexRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.IndexName = indexName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.DumpIndex(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to dump index: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to dump index: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowColumns shows all columns
func (t *Table) ShowColumns() (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowColumnsRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowColumns(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show columns: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show columns: %s", resp.ErrorMsg),
		)
	}

	// Build and return result
	return buildResult(resp)
}

// ShowSegments shows all segments
func (t *Table) ShowSegments() (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowSegmentsRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowSegments(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show segments: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show segments: %s", resp.ErrorMsg),
		)
	}

	// Build and return result
	return buildResult(resp)
}

// ShowSegment shows segment details
func (t *Table) ShowSegment(segmentID int) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowSegmentRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SegmentID = int64(segmentID)

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowSegment(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show segment: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show segment: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowBlocks shows all blocks in a segment
func (t *Table) ShowBlocks(segmentID int) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowBlocksRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SegmentID = int64(segmentID)

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowBlocks(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show blocks: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show blocks: %s", resp.ErrorMsg),
		)
	}

	// Build and return result
	return buildResult(resp)
}

// ShowBlock shows block details
func (t *Table) ShowBlock(segmentID int, blockID int) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowBlockRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SegmentID = int64(segmentID)
	req.BlockID = int64(blockID)

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowBlock(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show block: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show block: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowBlockColumn shows column information for a block
func (t *Table) ShowBlockColumn(segmentID int, blockID int, columnID int) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowBlockColumnRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SegmentID = int64(segmentID)
	req.BlockID = int64(blockID)
	req.ColumnID = int64(columnID)

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.ShowBlockColumn(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show block column: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show block column: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// Insert inserts data
// data can be a single map (one row) or a slice of maps (multiple rows)
// Each map represents a row with column names as keys and values as column values
func (t *Table) Insert(data interface{}) (interface{}, error) {
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert data to slice of maps
	var rows []map[string]interface{}
	switch d := data.(type) {
	case map[string]interface{}:
		rows = []map[string]interface{}{d}
	case []map[string]interface{}:
		rows = d
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidExpression), "Invalid data type for insert, expected map or slice of maps")
	}

	if len(rows) == 0 {
		return nil, NewInfinityException(int(ErrorCodeDataNotExist), "No data to insert")
	}

	// Build fields
	var fields []*thriftapi.Field
	for _, row := range rows {
		field := thriftapi.NewField()
		var columnNames []string
		var parseExprs []*thriftapi.ParsedExpr

		for columnName, value := range row {
			columnNames = append(columnNames, columnName)

			var parsedExpr *thriftapi.ParsedExpr

			// Check if value is FDE object
			if fde, ok := value.(*FDE); ok && fde != nil {
				// Create function expression for FDE
				functionExpr := GetRemoteFunctionExprFromFDE(fde)
				exprType := thriftapi.NewParsedExprType()
				exprType.FunctionExpr = functionExpr
				parsedExpr = thriftapi.NewParsedExpr()
				parsedExpr.Type = exprType
			} else {
				// Create constant expression for regular values
				constantExpr, err := ParseConstantValue(value)
				if err != nil {
					return nil, err
				}
				exprType := thriftapi.NewParsedExprType()
				exprType.ConstantExpr = constantExpr
				parsedExpr = thriftapi.NewParsedExpr()
				parsedExpr.Type = exprType
			}

			parseExprs = append(parseExprs, parsedExpr)
		}

		field.ColumnNames = columnNames
		field.ParseExprs = parseExprs
		fields = append(fields, field)
	}

	// Create request
	req := thriftapi.NewInsertRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.Fields = fields

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.Insert(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to insert data: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to insert data: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
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
	if t.db == nil || t.db.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Database or connection is nil")
	}

	if !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Use default export options if nil
	if exportOptions == nil {
		exportOptions = NewExportOption()
	}

	// Create thrift ExportOption
	thriftExportOption := thriftapi.NewExportOption()
	thriftExportOption.Delimiter = string(exportOptions.Delimiter)
	thriftExportOption.HasHeader = exportOptions.HasHeader
	thriftExportOption.Offset = int64(exportOptions.Offset)
	thriftExportOption.Limit = int64(exportOptions.Limit)
	thriftExportOption.RowLimit = int64(exportOptions.RowLimit)

	// Convert CopyFileType to thrift CopyFileType
	var thriftCopyFileType thriftapi.CopyFileType
	switch exportOptions.CopyFileType {
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
		return nil, NewInfinityException(int(ErrorCodeImportFileFormatError), fmt.Sprintf("Invalid copy file type: %d", exportOptions.CopyFileType))
	}
	thriftExportOption.CopyFileType = thriftCopyFileType

	// Create request
	req := thriftapi.NewExportRequest()
	req.SessionID = t.db.conn.GetSessionID()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.FileName = filePath
	req.ExportOption = thriftExportOption
	req.Columns = columns

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.Export(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to export data: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to export data: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// Delete deletes rows
func (t *Table) Delete(cond string) (*thriftapi.DeleteResponse, error) {
	if t.db == nil || t.db.conn == nil || !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Parse the condition if provided
	var whereExpr *thriftapi.ParsedExpr
	var err error
	if cond != "" {
		whereExpr, err = ParseFilter(cond)
		if err != nil {
			return nil, NewInfinityException(
				int(ErrorCodeInvalidParameterValue),
				fmt.Sprintf("Failed to parse delete condition: %v", err),
			)
		}
	}

	// Create request
	req := thriftapi.NewDeleteRequest()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SessionID = t.db.conn.GetSessionID()
	req.WhereExpr = whereExpr

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.Delete(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to delete rows: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to delete rows: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// Update updates rows
func (t *Table) Update(cond string, data map[string]interface{}) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// MatchDense performs dense vector search
func (t *Table) MatchDense(vectorColumnName string, embeddingData interface{}, embeddingDataType string, distanceType string, topN int, knnParams map[string]string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchDense(vectorColumnName, embeddingData, embeddingDataType, distanceType, topN, knnParams)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchText performs full-text search
func (t *Table) MatchText(fields string, matchingText string, topN int, extraOptions map[string]string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchText(fields, matchingText, topN, extraOptions)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchTensor performs tensor search
func (t *Table) MatchTensor(columnName string, queryData interface{}, queryDataType string, topN int, extraOption map[string]string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchTensor(columnName, queryData, queryDataType, topN, extraOption)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// MatchSparse performs sparse vector search
func (t *Table) MatchSparse(vectorColumnName string, sparseData *SparseVector, distanceType string, topN int, optParams map[string]string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.MatchSparse(vectorColumnName, sparseData, distanceType, topN, optParams)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Fusion combines multiple search results
func (t *Table) Fusion(method string, topN int, fusionParams map[string]interface{}) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Fusion(method, topN, fusionParams)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Output specifies output columns
func (t *Table) Output(columns []string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Output(columns)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Highlight specifies highlight columns
func (t *Table) Highlight(columns []string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Highlight(columns)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Filter applies filter
func (t *Table) Filter(filter string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Filter(filter)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Limit sets limit
func (t *Table) Limit(limit int) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	t.queryBuilder = t.queryBuilder.Limit(limit)
	return t
}

// Offset sets offset
func (t *Table) Offset(offset int) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	t.queryBuilder = t.queryBuilder.Offset(offset)
	return t
}

// GroupBy groups results
func (t *Table) GroupBy(groupByExprList interface{}) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.GroupBy(groupByExprList)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Having applies having clause
func (t *Table) Having(havingExpr string) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Having(havingExpr)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Sort sorts results
func (t *Table) Sort(orderByExprList [][2]interface{}) *Table {
	if t.err != nil {
		return t
	}
	if t.queryBuilder == nil {
		t.queryBuilder = NewQueryBuilder()
	}
	queryBuilder, err := t.queryBuilder.Sort(orderByExprList)
	if err != nil {
		t.err = err
		return t
	}
	t.queryBuilder = queryBuilder
	return t
}

// Option sets options
func (t *Table) Option(optionKV map[string]interface{}) *Table {
	if t.err != nil {
		return t
	}
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
func (t *Table) AddColumns(columnDefs TableSchema) (interface{}, error) {
	if t.db == nil || t.db.conn == nil || !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert column definitions to thrift ColumnDefs
	columnDefsList := make([]*thriftapi.ColumnDef, 0, len(columnDefs))
	for index, columnInfo := range columnDefs {
		colDef := thriftapi.NewColumnDef()
		colDef.ID = int32(index)
		colDef.Name = columnInfo.Name
		colDef.Comment = ""

		// Parse data type
		dataType, err := parseDataType(columnInfo.DataType)
		if err != nil {
			return nil, NewInfinityException(int(ErrorCodeInvalidDataType), fmt.Sprintf("Invalid data type for column %s: %v", columnInfo.Name, err))
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
			constExpr, err := ParseConstantValue(columnInfo.Default)
			if err != nil {
				return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), fmt.Sprintf("Invalid default value for column %s: %v", columnInfo.Name, err))
			}
			colDef.ConstantExpr = constExpr
		}

		columnDefsList = append(columnDefsList, colDef)
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
	if t.db == nil || t.db.conn == nil || !t.db.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewCompactRequest()
	req.DbName = t.db.dbName
	req.TableName = t.tableName
	req.SessionID = t.db.conn.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := t.db.conn.client.Compact(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to compact table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to compact table: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
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

	// Process column fields to get data
	for idx, colField := range resp.ColumnFields {
		colDef := resp.ColumnDefs[idx]
		if colDef.DataType != nil {
			result.DataTypes[colDef.Name] = colDef.DataType.LogicType.String()
		}

		columnName := resp.ColumnDefs[idx].Name
		columnType := colField.ColumnType
		physicalDataType := colDef.DataType.PhysicalType
		columnVectors := colField.ColumnVectors

		// Parse column vectors based on column type
		values := parseColumnVectors(columnType, physicalDataType, columnVectors, colField.Bitmasks)
		result.Data[columnName] = values
	}

	return result, nil
}

// parseColumnVectors parses column vectors based on column type
func parseColumnVectors(columnType thriftapi.ColumnType, physicalType *thriftapi.PhysicalType, columnVectors [][]byte, bitmasks []bool) []interface{} {
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
			ints := parseIntVector(vector, columnType)
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
			floats := parseFloatVector(vector, columnType)
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
	case thriftapi.ColumnType_ColumnEmbedding:
		// Embedding type - parse based on element type and dimension
		if physicalType != nil && physicalType.EmbeddingType != nil {
			for _, vector := range columnVectors {
				embeddings := parseEmbeddingVector(vector, physicalType.EmbeddingType)
				for _, v := range embeddings {
					values = append(values, v)
				}
			}
		} else {
			for _, vector := range columnVectors {
				values = append(values, vector)
			}
		}
	case thriftapi.ColumnType_ColumnMultiVector,
		thriftapi.ColumnType_ColumnTensor:
		// Tensor types - parse with length prefix
		if physicalType != nil && physicalType.EmbeddingType != nil {
			for _, vector := range columnVectors {
				tensors := parseTensorVector(vector, physicalType.EmbeddingType)
				for _, v := range tensors {
					values = append(values, v)
				}
			}
		} else {
			for _, vector := range columnVectors {
				values = append(values, vector)
			}
		}
	case thriftapi.ColumnType_ColumnTensorArray:
		// TensorArray type - parse array of tensors
		if physicalType != nil && physicalType.EmbeddingType != nil {
			for _, vector := range columnVectors {
				tensorArrays := parseTensorArrayVector(vector, physicalType.EmbeddingType)
				for _, v := range tensorArrays {
					values = append(values, v)
				}
			}
		} else {
			for _, vector := range columnVectors {
				values = append(values, vector)
			}
		}
	case thriftapi.ColumnType_ColumnSparse:
		// Sparse type - parse based on element type and index type
		if physicalType != nil && physicalType.SparseType != nil {
			for _, vector := range columnVectors {
				sparseVectors := parseSparseVector(vector, physicalType.SparseType)
				for _, v := range sparseVectors {
					values = append(values, v)
				}
			}
		} else {
			for _, vector := range columnVectors {
				values = append(values, vector)
			}
		}
	case thriftapi.ColumnType_ColumnArray:
		// Array type - parse based on element data type
		if physicalType != nil && physicalType.ArrayType != nil {
			for _, vector := range columnVectors {
				arrays := parseArrayVector(vector, physicalType.ArrayType)
				for _, v := range arrays {
					values = append(values, v)
				}
			}
		} else {
			for _, vector := range columnVectors {
				values = append(values, vector)
			}
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

// parseIntVector parses integer vector from bytes based on column type
func parseIntVector(data []byte, columnType thriftapi.ColumnType) []interface{} {
	switch columnType {
	case thriftapi.ColumnType_ColumnInt8:
		result := make([]interface{}, len(data))
		for i, b := range data {
			result[i] = int8(b)
		}
		return result
	case thriftapi.ColumnType_ColumnInt16:
		if len(data)%2 != 0 {
			return []interface{}{}
		}
		count := len(data) / 2
		result := make([]interface{}, count)
		for i := 0; i < count; i++ {
			val := int16(data[i*2]) |
				int16(data[i*2+1])<<8
			result[i] = val
		}
		return result
	case thriftapi.ColumnType_ColumnInt32:
		if len(data)%4 != 0 {
			return []interface{}{}
		}
		count := len(data) / 4
		result := make([]interface{}, count)
		for i := 0; i < count; i++ {
			val := int32(data[i*4]) |
				int32(data[i*4+1])<<8 |
				int32(data[i*4+2])<<16 |
				int32(data[i*4+3])<<24
			result[i] = val
		}
		return result
	case thriftapi.ColumnType_ColumnInt64:
		if len(data)%8 != 0 {
			return []interface{}{}
		}
		count := len(data) / 8
		result := make([]interface{}, count)
		for i := 0; i < count; i++ {
			val := int64(data[i*8]) |
				int64(data[i*8+1])<<8 |
				int64(data[i*8+2])<<16 |
				int64(data[i*8+3])<<24 |
				int64(data[i*8+4])<<32 |
				int64(data[i*8+5])<<40 |
				int64(data[i*8+6])<<48 |
				int64(data[i*8+7])<<56
			result[i] = val
		}
		return result
	default:
		return []interface{}{}
	}
}

// parseFloatVector parses float vector from bytes based on column type
func parseFloatVector(data []byte, columnType thriftapi.ColumnType) []interface{} {
	switch columnType {
	case thriftapi.ColumnType_ColumnFloat32:
		if len(data)%4 != 0 {
			return []interface{}{}
		}
		count := len(data) / 4
		result := make([]interface{}, count)
		for i := 0; i < count; i++ {
			bits := uint32(data[i*4]) |
				uint32(data[i*4+1])<<8 |
				uint32(data[i*4+2])<<16 |
				uint32(data[i*4+3])<<24
			result[i] = math.Float32frombits(bits)
		}
		return result
	case thriftapi.ColumnType_ColumnFloat64:
		if len(data)%8 != 0 {
			return []interface{}{}
		}
		count := len(data) / 8
		result := make([]interface{}, count)
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
	case thriftapi.ColumnType_ColumnFloat16, thriftapi.ColumnType_ColumnBFloat16:
		// Float16 and BFloat16 are stored as 2 bytes, convert to float32
		if len(data)%2 != 0 {
			return []interface{}{}
		}
		count := len(data) / 2
		result := make([]interface{}, count)
		for i := 0; i < count; i++ {
			bits := uint16(data[i*2]) |
				uint16(data[i*2+1])<<8
			result[i] = float16ToFloat32(bits)
		}
		return result
	default:
		return []interface{}{}
	}
}

// float16ToFloat32 converts a float16 (IEEE 754 half-precision) value to float32
func float16ToFloat32(bits uint16) float32 {
	// Extract sign, exponent, and mantissa
	sign := uint32(bits >> 15)
	exp := uint32((bits >> 10) & 0x1F)
	mant := uint32(bits & 0x3FF)

	var floatBits uint32
	if exp == 0 {
		if mant == 0 {
			// Zero
			floatBits = sign << 31
		} else {
			// Subnormal number
			floatBits = (sign << 31) | (mant << 13)
		}
	} else if exp == 0x1F {
		// Infinity or NaN
		floatBits = (sign << 31) | (0xFF << 23) | (mant << 13)
	} else {
		// Normal number
		floatBits = (sign << 31) | ((exp + 112) << 23) | (mant << 13)
	}
	return math.Float32frombits(floatBits)
}

// parseStringVector parses string vector from bytes
// It first tries length-prefixed format (4-byte little-endian length + string bytes),
// then falls back to null terminator separated format for backward compatibility
func parseStringVector(data []byte) []string {
	result := make([]string, 0)
	offset := 0

	// First, try length-prefixed parsing
	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}

		// Read 4-byte length prefix (little-endian)
		length := int(data[offset]) | int(data[offset+1])<<8 |
			int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		if offset+length > len(data) {
			// Invalid length, fallback to null terminator separated format
			return parseStringVectorFallback(data)
		}

		// Extract string
		strBytes := data[offset : offset+length]
		result = append(result, string(strBytes))
		offset += length
	}

	// If no strings parsed and data is not empty, fallback to null terminator format
	if len(result) == 0 && len(data) > 0 {
		return parseStringVectorFallback(data)
	}

	return result
}

// parseStringVectorFallback parses string vector using null terminator separator
// This is for backward compatibility
func parseStringVectorFallback(data []byte) []string {
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

// parseEmbeddingVector parses embedding vector from bytes based on embedding type
func parseEmbeddingVector(data []byte, embeddingType *thriftapi.EmbeddingType) []interface{} {
	dimension := embeddingType.Dimension
	elementType := embeddingType.ElementType

	var allValues []interface{}

	switch elementType {
	case thriftapi.ElementType_ElementBit:
		// Bit type: each byte contains 8 bits
		if dimension%8 != 0 {
			return []interface{}{}
		}
		subDim := dimension / 8
		for i := 0; i < len(data); i += int(subDim) {
			if i+int(subDim) > len(data) {
				break
			}
			midRes := data[i : i+int(subDim)]
			midResInt := uint64(0)
			for j := len(midRes) - 1; j >= 0; j-- {
				midResInt = midResInt*256 + uint64(midRes[j])
			}
			// Format as binary string with leading zeros
			formatStr := fmt.Sprintf("%%0%db", dimension)
			binaryStr := fmt.Sprintf(formatStr, midResInt)
			// Reverse the string
			runes := []rune(binaryStr)
			for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
				runes[i], runes[j] = runes[j], runes[i]
			}
			allValues = append(allValues, string(runes))
		}
		return allValues
	case thriftapi.ElementType_ElementUInt8:
		allValues = parseBytesToSlice(data, 1, func(b []byte) interface{} { return uint8(b[0]) })
	case thriftapi.ElementType_ElementInt8:
		allValues = parseBytesToSlice(data, 1, func(b []byte) interface{} { return int8(b[0]) })
	case thriftapi.ElementType_ElementInt16:
		allValues = parseBytesToSlice(data, 2, func(b []byte) interface{} {
			return int16(b[0]) | int16(b[1])<<8
		})
	case thriftapi.ElementType_ElementInt32:
		allValues = parseBytesToSlice(data, 4, func(b []byte) interface{} {
			return int32(b[0]) | int32(b[1])<<8 | int32(b[2])<<16 | int32(b[3])<<24
		})
	case thriftapi.ElementType_ElementInt64:
		allValues = parseBytesToSlice(data, 8, func(b []byte) interface{} {
			return int64(b[0]) | int64(b[1])<<8 | int64(b[2])<<16 | int64(b[3])<<24 |
				int64(b[4])<<32 | int64(b[5])<<40 | int64(b[6])<<48 | int64(b[7])<<56
		})
	case thriftapi.ElementType_ElementFloat32:
		allValues = parseBytesToSlice(data, 4, func(b []byte) interface{} {
			bits := uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
			return math.Float32frombits(bits)
		})
	case thriftapi.ElementType_ElementFloat64:
		allValues = parseBytesToSlice(data, 8, func(b []byte) interface{} {
			bits := uint64(b[0]) | uint64(b[1])<<8 | uint64(b[2])<<16 | uint64(b[3])<<24 |
				uint64(b[4])<<32 | uint64(b[5])<<40 | uint64(b[6])<<48 | uint64(b[7])<<56
			return math.Float64frombits(bits)
		})
	case thriftapi.ElementType_ElementFloat16:
		allValues = parseBytesToSlice(data, 2, func(b []byte) interface{} {
			bits := uint16(b[0]) | uint16(b[1])<<8
			return float16ToFloat32(bits)
		})
	case thriftapi.ElementType_ElementBFloat16:
		allValues = parseBytesToSlice(data, 2, func(b []byte) interface{} {
			// BFloat16: convert to float32 by padding with zeros in the lower 16 bits
			bits := uint32(b[0]) | uint32(b[1])<<8
			return math.Float32frombits(bits << 16)
		})
	default:
		return []interface{}{}
	}

	// Split into embeddings of dimension size
	result := make([]interface{}, 0)
	for i := 0; i < len(allValues); i += int(dimension) {
		end := i + int(dimension)
		if end > len(allValues) {
			end = len(allValues)
		}
		embedding := make([]interface{}, end-i)
		copy(embedding, allValues[i:end])
		result = append(result, embedding)
	}
	return result
}

// parseBytesToSlice is a helper function to parse bytes into a slice
func parseBytesToSlice(data []byte, elementSize int, parser func([]byte) interface{}) []interface{} {
	if len(data)%elementSize != 0 {
		return []interface{}{}
	}
	count := len(data) / elementSize
	result := make([]interface{}, count)
	for i := 0; i < count; i++ {
		result[i] = parser(data[i*elementSize : (i+1)*elementSize])
	}
	return result
}

// parseTensorVector parses tensor vector (multivector/tensor) from bytes
func parseTensorVector(data []byte, embeddingType *thriftapi.EmbeddingType) []interface{} {
	result := make([]interface{}, 0)
	offset := 0

	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}
		// Read length prefix (4 bytes, little-endian)
		length := int(data[offset]) | int(data[offset+1])<<8 | int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		if offset+length > len(data) {
			break
		}

		// Parse tensor data using embedding parser
		tensorData := data[offset : offset+length]
		offset += length

		embeddings := parseEmbeddingVector(tensorData, embeddingType)
		if len(embeddings) > 0 {
			result = append(result, embeddings[0])
		}
	}

	return result
}

// parseTensorArrayVector parses tensor array vector from bytes
func parseTensorArrayVector(data []byte, embeddingType *thriftapi.EmbeddingType) []interface{} {
	result := make([]interface{}, 0)
	offset := 0

	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}
		// Read tensor count (4 bytes, little-endian)
		tensorCount := int(data[offset]) | int(data[offset+1])<<8 | int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		tensorArray := make([]interface{}, 0, tensorCount)
		for i := 0; i < tensorCount; i++ {
			if offset+4 > len(data) {
				break
			}
			// Read length prefix
			length := int(data[offset]) | int(data[offset+1])<<8 | int(data[offset+2])<<16 | int(data[offset+3])<<24
			offset += 4

			if offset+length > len(data) {
				break
			}

			tensorData := data[offset : offset+length]
			offset += length

			embeddings := parseEmbeddingVector(tensorData, embeddingType)
			if len(embeddings) > 0 {
				tensorArray = append(tensorArray, embeddings[0])
			}
		}
		result = append(result, tensorArray)
	}

	return result
}

// parseSparseVector parses sparse vector from bytes
func parseSparseVector(data []byte, sparseType *thriftapi.SparseType) []interface{} {
	result := make([]interface{}, 0)
	offset := 0

	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}
		// Read nnz (number of non-zero elements)
		nnz := int(data[offset]) | int(data[offset+1])<<8 | int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		// Parse indices based on index type
		var indices []interface{}
		switch sparseType.IndexType {
		case thriftapi.ElementType_ElementInt8:
			if offset+nnz > len(data) {
				break
			}
			indices = parseBytesToSlice(data[offset:offset+nnz], 1, func(b []byte) interface{} { return int8(b[0]) })
			offset += nnz
		case thriftapi.ElementType_ElementInt16:
			if offset+nnz*2 > len(data) {
				break
			}
			indices = parseBytesToSlice(data[offset:offset+nnz*2], 2, func(b []byte) interface{} {
				return int16(b[0]) | int16(b[1])<<8
			})
			offset += nnz * 2
		case thriftapi.ElementType_ElementInt32:
			if offset+nnz*4 > len(data) {
				break
			}
			indices = parseBytesToSlice(data[offset:offset+nnz*4], 4, func(b []byte) interface{} {
				return int32(b[0]) | int32(b[1])<<8 | int32(b[2])<<16 | int32(b[3])<<24
			})
			offset += nnz * 4
		case thriftapi.ElementType_ElementInt64:
			if offset+nnz*8 > len(data) {
				break
			}
			indices = parseBytesToSlice(data[offset:offset+nnz*8], 8, func(b []byte) interface{} {
				return int64(b[0]) | int64(b[1])<<8 | int64(b[2])<<16 | int64(b[3])<<24 |
					int64(b[4])<<32 | int64(b[5])<<40 | int64(b[6])<<48 | int64(b[7])<<56
			})
			offset += nnz * 8
		default:
			return []interface{}{}
		}

		// Parse values based on element type
		var values []interface{}
		switch sparseType.ElementType {
		case thriftapi.ElementType_ElementUInt8:
			if offset+nnz > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz], 1, func(b []byte) interface{} { return uint8(b[0]) })
			offset += nnz
		case thriftapi.ElementType_ElementInt8:
			if offset+nnz > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz], 1, func(b []byte) interface{} { return int8(b[0]) })
			offset += nnz
		case thriftapi.ElementType_ElementInt16:
			if offset+nnz*2 > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz*2], 2, func(b []byte) interface{} {
				return int16(b[0]) | int16(b[1])<<8
			})
			offset += nnz * 2
		case thriftapi.ElementType_ElementInt32:
			if offset+nnz*4 > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz*4], 4, func(b []byte) interface{} {
				return int32(b[0]) | int32(b[1])<<8 | int32(b[2])<<16 | int32(b[3])<<24
			})
			offset += nnz * 4
		case thriftapi.ElementType_ElementInt64:
			if offset+nnz*8 > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz*8], 8, func(b []byte) interface{} {
				return int64(b[0]) | int64(b[1])<<8 | int64(b[2])<<16 | int64(b[3])<<24 |
					int64(b[4])<<32 | int64(b[5])<<40 | int64(b[6])<<48 | int64(b[7])<<56
			})
			offset += nnz * 8
		case thriftapi.ElementType_ElementFloat32:
			if offset+nnz*4 > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz*4], 4, func(b []byte) interface{} {
				bits := uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
				return math.Float32frombits(bits)
			})
			offset += nnz * 4
		case thriftapi.ElementType_ElementFloat64:
			if offset+nnz*8 > len(data) {
				break
			}
			values = parseBytesToSlice(data[offset:offset+nnz*8], 8, func(b []byte) interface{} {
				bits := uint64(b[0]) | uint64(b[1])<<8 | uint64(b[2])<<16 | uint64(b[3])<<24 |
					uint64(b[4])<<32 | uint64(b[5])<<40 | uint64(b[6])<<48 | uint64(b[7])<<56
				return math.Float64frombits(bits)
			})
			offset += nnz * 8
		default:
			return []interface{}{}
		}

		// Create sparse vector map
		sparseMap := make(map[string]interface{})
		sparseMap["indices"] = indices
		sparseMap["values"] = values
		result = append(result, sparseMap)
	}

	return result
}

// parseArrayVector parses array vector from bytes
func parseArrayVector(data []byte, arrayType *thriftapi.ArrayType) []interface{} {
	result := make([]interface{}, 0)
	offset := 0

	elementDataType := arrayType.ElementDataType
	if elementDataType == nil {
		return []interface{}{}
	}

	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}
		// Read element count (4 bytes, little-endian)
		elementCount := int(data[offset]) | int(data[offset+1])<<8 | int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		arrayData := make([]interface{}, 0, elementCount)
		for i := 0; i < elementCount; i++ {
			elementValue, newOffset := parseSingleArrayElement(data, offset, elementDataType)
			if newOffset <= offset {
				break
			}
			offset = newOffset
			arrayData = append(arrayData, elementValue)
		}
		result = append(result, arrayData)
	}

	return result
}

// parseSingleArrayElement parses a single array element from bytes
func parseSingleArrayElement(data []byte, offset int, elementDataType *thriftapi.DataType) (interface{}, int) {
	if elementDataType == nil || elementDataType.PhysicalType == nil {
		return nil, offset
	}

	physicalType := elementDataType.PhysicalType
	logicType := elementDataType.LogicType

	switch logicType {
	case thriftapi.LogicType_TinyInt:
		if offset+1 > len(data) {
			return nil, offset
		}
		return int8(data[offset]), offset + 1
	case thriftapi.LogicType_SmallInt:
		if offset+2 > len(data) {
			return nil, offset
		}
		val := int16(data[offset]) | int16(data[offset+1])<<8
		return val, offset + 2
	case thriftapi.LogicType_Integer:
		if offset+4 > len(data) {
			return nil, offset
		}
		val := int32(data[offset]) | int32(data[offset+1])<<8 | int32(data[offset+2])<<16 | int32(data[offset+3])<<24
		return val, offset + 4
	case thriftapi.LogicType_BigInt:
		if offset+8 > len(data) {
			return nil, offset
		}
		val := int64(data[offset]) | int64(data[offset+1])<<8 | int64(data[offset+2])<<16 | int64(data[offset+3])<<24 |
			int64(data[offset+4])<<32 | int64(data[offset+5])<<40 | int64(data[offset+6])<<48 | int64(data[offset+7])<<56
		return val, offset + 8
	case thriftapi.LogicType_Float:
		if offset+4 > len(data) {
			return nil, offset
		}
		bits := uint32(data[offset]) | uint32(data[offset+1])<<8 | uint32(data[offset+2])<<16 | uint32(data[offset+3])<<24
		return math.Float32frombits(bits), offset + 4
	case thriftapi.LogicType_Double:
		if offset+8 > len(data) {
			return nil, offset
		}
		bits := uint64(data[offset]) | uint64(data[offset+1])<<8 | uint64(data[offset+2])<<16 | uint64(data[offset+3])<<24 |
			uint64(data[offset+4])<<32 | uint64(data[offset+5])<<40 | uint64(data[offset+6])<<48 | uint64(data[offset+7])<<56
		return math.Float64frombits(bits), offset + 8
	case thriftapi.LogicType_Varchar:
		// For varchar, read until null terminator
		start := offset
		for offset < len(data) && data[offset] != 0 {
			offset++
		}
		str := string(data[start:offset])
		if offset < len(data) && data[offset] == 0 {
			offset++ // Skip null terminator
		}
		return str, offset
	default:
		// For complex types, try to use physical type info
		if physicalType.EmbeddingType != nil {
			embeddings := parseEmbeddingVector(data[offset:], physicalType.EmbeddingType)
			if len(embeddings) > 0 {
				return embeddings[0], offset + calculateEmbeddingSize(physicalType.EmbeddingType)
			}
		}
		return nil, offset
	}
}

// calculateEmbeddingSize calculates the size in bytes of an embedding
func calculateEmbeddingSize(embeddingType *thriftapi.EmbeddingType) int {
	dimension := int(embeddingType.Dimension)
	switch embeddingType.ElementType {
	case thriftapi.ElementType_ElementBit:
		return (dimension + 7) / 8
	case thriftapi.ElementType_ElementUInt8, thriftapi.ElementType_ElementInt8:
		return dimension
	case thriftapi.ElementType_ElementInt16, thriftapi.ElementType_ElementFloat16, thriftapi.ElementType_ElementBFloat16:
		return dimension * 2
	case thriftapi.ElementType_ElementInt32, thriftapi.ElementType_ElementFloat32:
		return dimension * 4
	case thriftapi.ElementType_ElementInt64, thriftapi.ElementType_ElementFloat64:
		return dimension * 8
	default:
		return dimension * 4
	}
}
