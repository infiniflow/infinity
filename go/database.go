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
	index := 0
	for columnName, columnInfo := range columnsDefinition {
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

		columnDefs = append(columnDefs, colDef)
		index++
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
	// TODO: Implement thrift call
	return nil, nil
}

// RestoreTableSnapshot restores a table from a snapshot
func (d *Database) RestoreTableSnapshot(snapshotName string) (interface{}, error) {
	// TODO: Implement thrift call
	return nil, nil
}

// parseDataType parses a string data type into thrift DataType
func parseDataType(typeStr string) (*thriftapi.DataType, error) {
	dt := thriftapi.NewDataType()
	pt := thriftapi.NewPhysicalType()

	switch typeStr {
	case "bool", "boolean":
		dt.LogicType = thriftapi.LogicType_Boolean
		pt.NumberType = &thriftapi.NumberType{}
	case "tinyint", "int8":
		dt.LogicType = thriftapi.LogicType_TinyInt
		pt.NumberType = &thriftapi.NumberType{}
	case "smallint", "int16":
		dt.LogicType = thriftapi.LogicType_SmallInt
		pt.NumberType = &thriftapi.NumberType{}
	case "int", "integer", "int32":
		dt.LogicType = thriftapi.LogicType_Integer
		pt.NumberType = &thriftapi.NumberType{}
	case "bigint", "int64":
		dt.LogicType = thriftapi.LogicType_BigInt
		pt.NumberType = &thriftapi.NumberType{}
	case "hugeint":
		dt.LogicType = thriftapi.LogicType_HugeInt
		pt.NumberType = &thriftapi.NumberType{}
	case "float", "float32":
		dt.LogicType = thriftapi.LogicType_Float
		pt.NumberType = &thriftapi.NumberType{}
	case "double", "float64":
		dt.LogicType = thriftapi.LogicType_Double
		pt.NumberType = &thriftapi.NumberType{}
	case "varchar", "string":
		dt.LogicType = thriftapi.LogicType_Varchar
		pt.VarcharType = &thriftapi.VarcharType{}
	case "date":
		dt.LogicType = thriftapi.LogicType_Date
		pt.NumberType = &thriftapi.NumberType{}
	case "time":
		dt.LogicType = thriftapi.LogicType_Time
		pt.NumberType = &thriftapi.NumberType{}
	case "datetime":
		dt.LogicType = thriftapi.LogicType_DateTime
		pt.NumberType = &thriftapi.NumberType{}
	case "timestamp":
		dt.LogicType = thriftapi.LogicType_Timestamp
		pt.NumberType = &thriftapi.NumberType{}
	default:
		// For complex types like "vector,1024,float32", we need more parsing
		// For now, return as Varchar for unsupported types
		if len(typeStr) > 0 {
			dt.LogicType = thriftapi.LogicType_Varchar
			pt.VarcharType = &thriftapi.VarcharType{}
		} else {
			return nil, fmt.Errorf("unsupported data type: %s", typeStr)
		}
	}

	dt.PhysicalType = pt
	return dt, nil
}

// parseDefaultValue parses a default value into thrift ConstantExpr
func parseDefaultValue(value interface{}) (*thriftapi.ConstantExpr, error) {
	expr := thriftapi.NewConstantExpr()

	switch v := value.(type) {
	case bool:
		expr.LiteralType = thriftapi.LiteralType_Boolean
		expr.BoolValue = &v
	case int:
		expr.LiteralType = thriftapi.LiteralType_Int64
		i64 := int64(v)
		expr.I64Value = &i64
	case int8:
		expr.LiteralType = thriftapi.LiteralType_Int64
		i64 := int64(v)
		expr.I64Value = &i64
	case int16:
		expr.LiteralType = thriftapi.LiteralType_Int64
		i64 := int64(v)
		expr.I64Value = &i64
	case int32:
		expr.LiteralType = thriftapi.LiteralType_Int64
		i64 := int64(v)
		expr.I64Value = &i64
	case int64:
		expr.LiteralType = thriftapi.LiteralType_Int64
		expr.I64Value = &v
	case float32:
		expr.LiteralType = thriftapi.LiteralType_Double
		f64 := float64(v)
		expr.F64Value = &f64
	case float64:
		expr.LiteralType = thriftapi.LiteralType_Double
		expr.F64Value = &v
	case string:
		expr.LiteralType = thriftapi.LiteralType_String
		expr.StrValue = &v
	case nil:
		expr.LiteralType = thriftapi.LiteralType_Null
	default:
		return nil, fmt.Errorf("unsupported default value type: %T", value)
	}

	return expr, nil
}
