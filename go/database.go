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

// ColumnConstraint represents a constraint on a column
type ColumnConstraint string

const (
	ConstraintPrimaryKey ColumnConstraint = "PRIMARY KEY"
	ConstraintUnique     ColumnConstraint = "UNIQUE"
	ConstraintNotNull    ColumnConstraint = "NOT NULL"
	ConstraintNull       ColumnConstraint = "NULL"
)

// ColumnDefinition defines a column in a table
type ColumnDefinition struct {
	Name        string
	DataType    string
	Constraints []ColumnConstraint
	Default     interface{}
}

// TableSchema defines the schema of a table
type TableSchema []*ColumnDefinition

// Database represents a database for operations
type Database struct {
	conn   *InfinityConnection
	dbName string
}

// CreateTable creates a new table
func (d *Database) CreateTable(tableName string, columnsDefinition TableSchema, conflictType ConflictType) (*Table, error) {
	if d.conn == nil || !d.conn.IsConnected() {
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

	// Convert column definitions to thrift ColumnDefs
	columnDefs := make([]*thriftapi.ColumnDef, 0, len(columnsDefinition))
	for index, columnInfo := range columnsDefinition {
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
		constExpr, err := ParseConstantValue(columnInfo.Default)
		if err != nil {
			return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), fmt.Sprintf("Invalid default value for column %s: %v", columnInfo.Name, err))
		}
		colDef.ConstantExpr = constExpr

		columnDefs = append(columnDefs, colDef)
	}

	// Create request
	req := thriftapi.NewCreateTableRequest()
	req.DbName = d.dbName
	req.TableName = tableName
	req.SessionID = d.conn.GetSessionID()
	req.ColumnDefs = columnDefs
	req.CreateOption = &thriftapi.CreateOption{
		ConflictType: createConflict,
	}

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.CreateTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create table: %s", resp.ErrorMsg),
		)
	}

	return &Table{db: d, tableName: tableName}, nil
}

// DropTable drops a table
func (d *Database) DropTable(tableName string, conflictType ConflictType) (interface{}, error) {
	if d.conn == nil || !d.conn.IsConnected() {
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
		return nil, NewInfinityException(int(ErrorCodeInvalidConflictType), "Invalid conflict type for drop")
	}

	// Create request
	req := thriftapi.NewDropTableRequest()
	req.DbName = d.dbName
	req.TableName = tableName
	req.SessionID = d.conn.GetSessionID()
	req.DropOption = &thriftapi.DropOption{
		ConflictType: dropConflict,
	}

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.DropTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to drop table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to drop table: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ListTables lists all tables
func (d *Database) ListTables() (interface{}, error) {
	if d.conn == nil || !d.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewListTableRequest()
	req.DbName = d.dbName
	req.SessionID = d.conn.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.ListTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to list tables: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to list tables: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowTable shows table details
func (d *Database) ShowTable(tableName string) (interface{}, error) {
	if d.conn == nil || !d.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowTableRequest()
	req.DbName = d.dbName
	req.TableName = tableName
	req.SessionID = d.conn.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.ShowTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show table: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// GetTable gets a table object
func (d *Database) GetTable(tableName string) (*Table, error) {
	if d.conn == nil || !d.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create get table request
	req := thriftapi.NewGetTableRequest()
	req.DbName = d.dbName
	req.TableName = tableName
	req.SessionID = d.conn.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.GetTable(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to get table: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to get table: %s", resp.ErrorMsg),
		)
	}

	return &Table{db: d, tableName: tableName}, nil
}

// CreateTableSnapshot creates a snapshot of a table
func (d *Database) CreateTableSnapshot(snapshotName string, tableName string) (interface{}, error) {
	if d.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is nil")
	}

	if !d.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewCreateTableSnapshotRequest()
	req.SessionID = d.conn.GetSessionID()
	req.DbName = d.dbName
	req.TableName = tableName
	req.SnapshotName = snapshotName

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.CreateTableSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create table snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create table snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// RestoreTableSnapshot restores a table from a snapshot
func (d *Database) RestoreTableSnapshot(snapshotName string) (interface{}, error) {
	if d.conn == nil {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is nil")
	}

	if !d.conn.IsConnected() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewRestoreSnapshotRequest()
	req.SessionID = d.conn.GetSessionID()
	req.SnapshotName = snapshotName
	req.Scope = "table"

	// Call thrift
	ctx := context.Background()
	resp, err := d.conn.client.RestoreSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to restore table snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to restore table snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}
