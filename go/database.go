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
	"reflect"
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
			constExpr, err := ParseConstantValue(columnInfo.Default)
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

// ParseConstantValue parses a default value into thrift ConstantExpr
func ParseConstantValue(value interface{}) (*thriftapi.ConstantExpr, error) {
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
		// Use reflection for complex types
		rv := reflect.ValueOf(value)
		switch rv.Kind() {
		case reflect.Slice, reflect.Array:
			return parseSliceConstantValue(rv, expr)
		case reflect.Map:
			return parseMapConstantValue(rv, expr)
		default:
			return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), fmt.Sprintf("Invalid constant type: %T", value))
		}
	}

	return expr, nil
}

// parseSliceConstantValue handles slices and arrays for constant expressions
func parseSliceConstantValue(rv reflect.Value, expr *thriftapi.ConstantExpr) (*thriftapi.ConstantExpr, error) {
	if rv.Len() == 0 {
		// Empty slice - cannot determine type, treat as empty integer array
		expr.LiteralType = thriftapi.LiteralType_IntegerArray
		expr.I64ArrayValue = []int64{}
		return expr, nil
	}

	// Determine dimension and element type
	dim := 1
	elemType := rv.Type().Elem()
	for elemType.Kind() == reflect.Slice {
		dim++
		elemType = elemType.Elem()
	}

	// Check if element type is supported
	switch elemType.Kind() {
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
		// Integer type
		switch dim {
		case 1:
			expr.LiteralType = thriftapi.LiteralType_IntegerArray
			arr := make([]int64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				arr[i] = rv.Index(i).Int()
			}
			expr.I64ArrayValue = arr
		case 2:
			expr.LiteralType = thriftapi.LiteralType_IntegerTensor
			tensor := make([][]int64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				inner := rv.Index(i)
				if inner.Kind() != reflect.Slice {
					return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "2D slice must contain slices")
				}
				tensor[i] = make([]int64, inner.Len())
				for j := 0; j < inner.Len(); j++ {
					tensor[i][j] = inner.Index(j).Int()
				}
			}
			expr.I64TensorValue = tensor
		case 3:
			expr.LiteralType = thriftapi.LiteralType_IntegerTensorArray
			tensorArray := make([][][]int64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				mid := rv.Index(i)
				if mid.Kind() != reflect.Slice {
					return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "3D slice must contain slices")
				}
				tensorArray[i] = make([][]int64, mid.Len())
				for j := 0; j < mid.Len(); j++ {
					inner := mid.Index(j)
					if inner.Kind() != reflect.Slice {
						return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "3D slice must contain slices of slices")
					}
					tensorArray[i][j] = make([]int64, inner.Len())
					for k := 0; k < inner.Len(); k++ {
						tensorArray[i][j][k] = inner.Index(k).Int()
					}
				}
			}
			expr.I64TensorArrayValue = tensorArray
		default:
			return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), fmt.Sprintf("Unsupported slice dimension: %d", dim))
		}
	case reflect.Float32, reflect.Float64:
		// Float type
		switch dim {
		case 1:
			expr.LiteralType = thriftapi.LiteralType_DoubleArray
			arr := make([]float64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				arr[i] = rv.Index(i).Float()
			}
			expr.F64ArrayValue = arr
		case 2:
			expr.LiteralType = thriftapi.LiteralType_DoubleTensor
			tensor := make([][]float64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				inner := rv.Index(i)
				if inner.Kind() != reflect.Slice {
					return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "2D slice must contain slices")
				}
				tensor[i] = make([]float64, inner.Len())
				for j := 0; j < inner.Len(); j++ {
					tensor[i][j] = inner.Index(j).Float()
				}
			}
			expr.F64TensorValue = tensor
		case 3:
			expr.LiteralType = thriftapi.LiteralType_DoubleTensorArray
			tensorArray := make([][][]float64, rv.Len())
			for i := 0; i < rv.Len(); i++ {
				mid := rv.Index(i)
				if mid.Kind() != reflect.Slice {
					return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "3D slice must contain slices")
				}
				tensorArray[i] = make([][]float64, mid.Len())
				for j := 0; j < mid.Len(); j++ {
					inner := mid.Index(j)
					if inner.Kind() != reflect.Slice {
						return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "3D slice must contain slices of slices")
				}
					tensorArray[i][j] = make([]float64, inner.Len())
					for k := 0; k < inner.Len(); k++ {
						tensorArray[i][j][k] = inner.Index(k).Float()
					}
				}
			}
			expr.F64TensorArrayValue = tensorArray
		default:
			return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), fmt.Sprintf("Unsupported slice dimension: %d", dim))
		}
	case reflect.Interface:
		// []interface{} - try to treat as CurlyBracketsArray
		children := make([]*thriftapi.ConstantExpr, 0, rv.Len())
		for i := 0; i < rv.Len(); i++ {
			elem := rv.Index(i).Interface()
			childExpr, err := ParseConstantValue(elem)
			if err != nil {
				return nil, err
			}
			children = append(children, childExpr)
		}
		expr.LiteralType = thriftapi.LiteralType_CurlyBracketsArray
		expr.CurlyBracketsArray = children
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), fmt.Sprintf("Unsupported slice element type: %v", elemType.Kind()))
	}

	return expr, nil
}

// parseMapConstantValue handles maps for sparse array constant expressions
func parseMapConstantValue(rv reflect.Value, expr *thriftapi.ConstantExpr) (*thriftapi.ConstantExpr, error) {
	if rv.Len() == 0 {
		return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "Empty sparse vector")
	}

	// Check key type - must be integer
	if rv.Type().Key().Kind() != reflect.Int && rv.Type().Key().Kind() != reflect.Int64 {
		return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), "Sparse vector keys must be integers")
	}

	// Determine value type
	valueType := rv.Type().Elem()
	switch valueType.Kind() {
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
		// Sparse integer array
		expr.LiteralType = thriftapi.LiteralType_SparseIntegerArray
		indices := make([]int64, 0, rv.Len())
		values := make([]int64, 0, rv.Len())
		iter := rv.MapRange()
		for iter.Next() {
			indices = append(indices, iter.Key().Int())
			values = append(values, iter.Value().Int())
		}
		expr.I64ArrayIdx = indices
		expr.I64ArrayValue = values
	case reflect.Float32, reflect.Float64:
		// Sparse double array
		expr.LiteralType = thriftapi.LiteralType_SparseDoubleArray
		indices := make([]int64, 0, rv.Len())
		values := make([]float64, 0, rv.Len())
		iter := rv.MapRange()
		for iter.Next() {
			indices = append(indices, iter.Key().Int())
			values = append(values, iter.Value().Float())
		}
		expr.I64ArrayIdx = indices
		expr.F64ArrayValue = values
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConstantType), fmt.Sprintf("Unsupported sparse vector value type: %v", valueType.Kind()))
	}

	return expr, nil
}
