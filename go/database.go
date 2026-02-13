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
	"strconv"
	"strings"

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
		if columnInfo.Default != nil {
			constExpr, err := parseDefaultValue(columnInfo.Default)
			if err != nil {
				return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), fmt.Sprintf("Invalid default value for column %s: %v", columnInfo.Name, err))
		}
			colDef.ConstantExpr = constExpr
		}

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
	typeStr = strings.ToLower(strings.TrimSpace(typeStr))
	if typeStr == "" {
		return nil, fmt.Errorf("empty data type")
	}

	// Split by comma for complex types like "vector,1024,float32"
	parts := splitAndTrim(typeStr, ",")
	if len(parts) == 0 {
		return nil, fmt.Errorf("empty data type")
	}

	baseType := parts[0]

	dt := thriftapi.NewDataType()
	pt := thriftapi.NewPhysicalType()

	switch baseType {
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
	case "hugeint", "int128":
		dt.LogicType = thriftapi.LogicType_HugeInt
		pt.NumberType = &thriftapi.NumberType{}
	case "float", "float32":
		dt.LogicType = thriftapi.LogicType_Float
		pt.NumberType = &thriftapi.NumberType{}
	case "double", "float64":
		dt.LogicType = thriftapi.LogicType_Double
		pt.NumberType = &thriftapi.NumberType{}
	case "float16":
		dt.LogicType = thriftapi.LogicType_Float16
		pt.NumberType = &thriftapi.NumberType{}
	case "bfloat16":
		dt.LogicType = thriftapi.LogicType_BFloat16
		pt.NumberType = &thriftapi.NumberType{}
	case "varchar", "string":
		dt.LogicType = thriftapi.LogicType_Varchar
		pt.VarcharType = &thriftapi.VarcharType{}
	case "json":
		dt.LogicType = thriftapi.LogicType_Json
		pt.NumberType = &thriftapi.NumberType{}
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
	case "interval":
		dt.LogicType = thriftapi.LogicType_Interval
		pt.NumberType = &thriftapi.NumberType{}
	case "vector", "multivector", "tensor", "tensorarray":
		// Embedding types: vector,dimension,element_type
		if len(parts) < 3 {
			return nil, fmt.Errorf("invalid %s type format, expected: %s,dimension,element_type", baseType, baseType)
		}
		embeddingType, err := parseEmbeddingType(baseType, parts[1], parts[2])
		if err != nil {
			return nil, err
		}
		return embeddingType, nil
	case "sparse":
		// Sparse type: sparse,dimension,value_type,index_type
		if len(parts) < 4 {
			return nil, fmt.Errorf("invalid sparse type format, expected: sparse,dimension,value_type,index_type")
		}
		sparseType, err := parseSparseType(parts[1], parts[2], parts[3])
		if err != nil {
			return nil, err
		}
		return sparseType, nil
	case "array":
		// Array type: array,element_type
		if len(parts) < 2 {
			return nil, fmt.Errorf("invalid array type format, expected: array,element_type")
		}
		dt.LogicType = thriftapi.LogicType_Array
		pt.ArrayType = &thriftapi.ArrayType{}
		elementType, err := parseDataType(strings.Join(parts[1:], ","))
		if err != nil {
			return nil, fmt.Errorf("invalid array element type: %v", err)
		}
		pt.ArrayType.ElementDataType = elementType
	default:
		return nil, fmt.Errorf("unsupported data type: %s", baseType)
	}

	dt.PhysicalType = pt
	return dt, nil
}

// splitAndTrim splits a string by separator and trims spaces from each part
func splitAndTrim(s, sep string) []string {
	parts := strings.Split(s, sep)
	result := make([]string, 0, len(parts))
	for _, p := range parts {
		p = strings.TrimSpace(p)
		if p != "" {
			result = append(result, p)
		}
	}
	return result
}

// parseEmbeddingType parses embedding types (vector, multivector, tensor, tensorarray)
func parseEmbeddingType(baseType, dimensionStr, elementTypeStr string) (*thriftapi.DataType, error) {
	dt := thriftapi.NewDataType()
	pt := thriftapi.NewPhysicalType()

	switch baseType {
	case "vector":
		dt.LogicType = thriftapi.LogicType_Embedding
	case "multivector":
		dt.LogicType = thriftapi.LogicType_MultiVector
	case "tensor":
		dt.LogicType = thriftapi.LogicType_Tensor
	case "tensorarray":
		dt.LogicType = thriftapi.LogicType_TensorArray
	}

	dimension, err := strconv.ParseInt(dimensionStr, 10, 32)
	if err != nil {
		return nil, fmt.Errorf("invalid dimension: %s", dimensionStr)
	}

	elementType, err := parseElementType(elementTypeStr)
	if err != nil {
		return nil, err
	}

	embeddingType := &thriftapi.EmbeddingType{
		Dimension:   int32(dimension),
		ElementType: elementType,
	}
	pt.EmbeddingType = embeddingType
	dt.PhysicalType = pt
	return dt, nil
}

// parseSparseType parses sparse type
func parseSparseType(dimensionStr, valueTypeStr, indexTypeStr string) (*thriftapi.DataType, error) {
	dt := thriftapi.NewDataType()
	pt := thriftapi.NewPhysicalType()
	dt.LogicType = thriftapi.LogicType_Sparse

	dimension, err := strconv.ParseInt(dimensionStr, 10, 64)
	if err != nil {
		return nil, fmt.Errorf("invalid dimension: %s", dimensionStr)
	}

	elementType, err := parseElementType(valueTypeStr)
	if err != nil {
		return nil, err
	}

	idxType, err := parseElementType(indexTypeStr)
	if err != nil {
		return nil, err
	}

	sparseType := &thriftapi.SparseType{
		Dimension:   dimension,
		ElementType: elementType,
		IndexType:   idxType,
	}
	pt.SparseType = sparseType
	dt.PhysicalType = pt
	return dt, nil
}

// parseElementType parses element type string to ElementType
func parseElementType(typeStr string) (thriftapi.ElementType, error) {
	switch typeStr {
	case "bit":
		return thriftapi.ElementType_ElementBit, nil
	case "float32", "float", "f32":
		return thriftapi.ElementType_ElementFloat32, nil
	case "float64", "double", "f64":
		return thriftapi.ElementType_ElementFloat64, nil
	case "float16", "f16":
		return thriftapi.ElementType_ElementFloat16, nil
	case "bfloat16", "bf16":
		return thriftapi.ElementType_ElementBFloat16, nil
	case "uint8", "u8":
		return thriftapi.ElementType_ElementUInt8, nil
	case "int8", "i8":
		return thriftapi.ElementType_ElementInt8, nil
	case "int16", "i16":
		return thriftapi.ElementType_ElementInt16, nil
	case "int32", "int", "i32":
		return thriftapi.ElementType_ElementInt32, nil
	case "int64", "i64":
		return thriftapi.ElementType_ElementInt64, nil
	default:
		return thriftapi.ElementType_ElementBit, fmt.Errorf("unsupported element type: %s", typeStr)
	}
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
