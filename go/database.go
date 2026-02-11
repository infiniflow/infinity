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

// Database defines the interface for database operations
type Database interface {
	// CreateTable creates a new table in the database
	// columnsDefinition is a map of column name to column definition
	// Example:
	//   db.CreateTable("my_table", TableSchema{
	//       "c1": {Name: "c1", DataType: "int", Constraints: []ColumnConstraint{ConstraintPrimaryKey}},
	//       "c2": {Name: "c2", DataType: "vector,1024,float32"},
	//   }, ConflictTypeError)
	CreateTable(tableName string, columnsDefinition TableSchema, conflictType ConflictType) (Table, error)

	// DropTable drops a table from the database
	DropTable(tableName string, conflictType ConflictType) (interface{}, error)

	// ListTables lists all tables in the database
	ListTables() (interface{}, error)

	// ShowTable shows details of a specific table
	ShowTable(tableName string) (interface{}, error)

	// GetTable gets a table object for operations
	GetTable(tableName string) (Table, error)

	// CreateTableSnapshot creates a snapshot of a table
	CreateTableSnapshot(snapshotName string, tableName string) (interface{}, error)

	// RestoreTableSnapshot restores a table from a snapshot
	RestoreTableSnapshot(snapshotName string) (interface{}, error)
}

// RemoteDatabase implements Database interface for remote connections
type RemoteDatabase struct {
	conn   *InfinityConnection
	dbName string
}

// CreateTable creates a new table
func (d *RemoteDatabase) CreateTable(tableName string, columnsDefinition TableSchema, conflictType ConflictType) (Table, error) {
	// TODO: Implement thrift call
	return &RemoteTable{db: d, tableName: tableName}, nil
}

// DropTable drops a table
func (d *RemoteDatabase) DropTable(tableName string, conflictType ConflictType) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ListTables lists all tables
func (d *RemoteDatabase) ListTables() (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// ShowTable shows table details
func (d *RemoteDatabase) ShowTable(tableName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// GetTable gets a table object
func (d *RemoteDatabase) GetTable(tableName string) (Table, error) {
	// TODO: Implement thrift call to verify table exists
	return &RemoteTable{db: d, tableName: tableName}, nil
}

// CreateTableSnapshot creates a snapshot of a table
func (d *RemoteDatabase) CreateTableSnapshot(snapshotName string, tableName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// RestoreTableSnapshot restores a table from a snapshot
func (d *RemoteDatabase) RestoreTableSnapshot(snapshotName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}
