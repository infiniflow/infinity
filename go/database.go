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
type TableSchema map[string]*ColumnDefinition

// Database represents a database for operations
type Database struct {
	conn   *InfinityConnection
	dbName string
}

// CreateTable creates a new table
func (d *Database) CreateTable(tableName string, columnsDefinition TableSchema, conflictType ConflictType) (*Table, error) {
	// TODO: Implement thrift call
	return &Table{db: d, tableName: tableName}, nil
}

// DropTable drops a table
func (d *Database) DropTable(tableName string, conflictType ConflictType) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ListTables lists all tables
func (d *Database) ListTables() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowTable shows table details
func (d *Database) ShowTable(tableName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
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
	// TODO: Implement thrift call
	return nil, nil
}

// RestoreTableSnapshot restores a table from a snapshot
func (d *Database) RestoreTableSnapshot(snapshotName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}
