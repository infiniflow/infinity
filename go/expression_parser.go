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
	"fmt"
	"regexp"
	"strconv"
	"strings"

	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// ExpressionType represents the type of expression
type ExpressionType int

const (
	ExprTypeUnknown ExpressionType = iota
	ExprTypeColumn
	ExprTypeConstant
	ExprTypeFunction
	ExprTypeBinary
	ExprTypeUnary
	ExprTypeStar
	ExprTypeCast
	ExprTypeIn
	ExprTypeBetween
	ExprTypeParen
)

// Expression represents a parsed expression
type Expression struct {
	Type         ExpressionType
	ColumnName   []string
	Constant     *thriftapi.ConstantExpr
	FunctionName string
	Arguments    []*Expression
	Operator     string
	Left         *Expression
	Right        *Expression
	IsStar       bool
	Alias        string
	CastType     *thriftapi.DataType
	InValues     []*Expression
	InType       bool // true for IN, false for NOT IN
}

// ParseExpr parses an expression string and returns a ParsedExpr
func ParseExpr(expr string) (*thriftapi.ParsedExpr, error) {
	if expr == "" {
		return nil, NewInfinityException(int(ErrorCodeInvalidExpression), "Empty expression")
	}

	// Parse the expression
	parsedExpr, err := parseExpressionInternal(expr)
	if err != nil {
		return nil, err
	}

	// Convert to thrift ParsedExpr
	return convertToThriftParsedExpr(parsedExpr)
}

// parseExpressionInternal parses an expression string into an Expression struct
func parseExpressionInternal(expr string) (*Expression, error) {
	expr = strings.TrimSpace(expr)

	// Check for parentheses
	if strings.HasPrefix(expr, "(") && strings.HasSuffix(expr, ")") {
		// Remove outer parentheses
		return parseExpressionInternal(expr[1 : len(expr)-1])
	}

	// Check for star
	if expr == "*" {
		return &Expression{Type: ExprTypeStar, IsStar: true}, nil
	}

	// Check for CAST expression
	if strings.HasPrefix(strings.ToUpper(expr), "CAST(") {
		return parseCastExpression(expr)
	}

	// Check for unary NOT (must be before binary operators)
	if strings.HasPrefix(strings.ToUpper(expr), "NOT ") {
		inner, err := parseExpressionInternal(expr[4:])
		if err != nil {
			return nil, err
		}
		return &Expression{
			Type:     ExprTypeUnary,
			Operator: "not",
			Left:     inner,
		}, nil
	}

	// Check for binary operators with lowest precedence (must be before IN/BETWEEN)
	for _, op := range []string{"OR", "AND"} {
		if left, right, found := splitByOperator(expr, op); found {
			return &Expression{
				Type:     ExprTypeBinary,
				Operator: strings.ToLower(op),
				Left:     left,
				Right:    right,
			}, nil
		}
	}

	// Check for comparison operators
	for _, op := range []string{">=", "<=", "!=", "<>", ">", "<", "="} {
		if left, right, found := splitByOperator(expr, op); found {
			return &Expression{
				Type:     ExprTypeBinary,
				Operator: op,
				Left:     left,
				Right:    right,
			}, nil
		}
	}

	// Check for arithmetic operators
	for _, op := range []string{"+", "-", "*", "/", "%"} {
		if left, right, found := splitByOperator(expr, op); found {
			return &Expression{
				Type:     ExprTypeBinary,
				Operator: op,
				Left:     left,
				Right:    right,
			}, nil
		}
	}

	// Check for IN expression (after binary operators to allow AND/OR to work)
	if inExpr := parseInExpression(expr); inExpr != nil {
		return inExpr, nil
	}

	// Check for BETWEEN expression (after binary operators to allow AND to work)
	if betweenExpr := parseBetweenExpression(expr); betweenExpr != nil {
		return betweenExpr, nil
	}

	// Check for unary minus
	if strings.HasPrefix(expr, "-") {
		inner, err := parseExpressionInternal(expr[1:])
		if err != nil {
			return nil, err
		}
		return &Expression{
			Type:     ExprTypeUnary,
			Operator: "-",
			Left:     inner,
		}, nil
	}

	// Check for function call
	if isFunctionCall(expr) {
		return parseFunctionCall(expr)
	}

	// Try to parse as constant first (handles null, true, false, etc.)
	constant, err := parseConstant(expr)
	if err == nil {
		return &Expression{
			Type:     ExprTypeConstant,
			Constant: constant,
		}, nil
	}

	// Check for column reference
	if isColumnReference(expr) {
		return parseColumnReference(expr)
	}

	return nil, NewInfinityException(int(ErrorCodeInvalidExpression), fmt.Sprintf("Unable to parse expression: %s", expr))
}

// parseCastExpression parses a CAST expression
func parseCastExpression(expr string) (*Expression, error) {
	// CAST(expr AS type)
	expr = strings.TrimSpace(expr[5 : len(expr)-1]) // Remove "CAST(" and ")"

	// Find AS keyword
	asIdx := strings.Index(strings.ToUpper(expr), " AS ")
	if asIdx == -1 {
		return nil, NewInfinityException(int(ErrorCodeInvalidExpression), "Invalid CAST expression, missing AS keyword")
	}

	innerExpr := strings.TrimSpace(expr[:asIdx])
	targetType := strings.TrimSpace(expr[asIdx+4:])

	// Parse the inner expression
	parsedInner, err := parseExpressionInternal(innerExpr)
	if err != nil {
		return nil, err
	}

	// Map target type to thrift DataType
	dataType := mapSQLTypeToDataType(targetType)

	return &Expression{
		Type:     ExprTypeCast,
		Left:     parsedInner,
		CastType: dataType,
	}, nil
}

// parseInExpression parses an IN or NOT IN expression
func parseInExpression(expr string) *Expression {
	exprUpper := strings.ToUpper(expr)

	// Check for NOT IN
	notInIdx := strings.Index(exprUpper, " NOT IN ")
	if notInIdx != -1 {
		leftExpr := strings.TrimSpace(expr[:notInIdx])
		valuesStr := strings.TrimSpace(expr[notInIdx+8:])

		left, err := parseExpressionInternal(leftExpr)
		if err != nil {
			return nil
		}

		values := parseInValues(valuesStr)

		return &Expression{
			Type:     ExprTypeIn,
			Left:     left,
			InValues: values,
			InType:   false,
		}
	}

	// Check for IN
	inIdx := strings.Index(exprUpper, " IN ")
	if inIdx != -1 {
		leftExpr := strings.TrimSpace(expr[:inIdx])
		valuesStr := strings.TrimSpace(expr[inIdx+4:])

		left, err := parseExpressionInternal(leftExpr)
		if err != nil {
			return nil
		}

		values := parseInValues(valuesStr)

		return &Expression{
			Type:     ExprTypeIn,
			Left:     left,
			InValues: values,
			InType:   true,
		}
	}

	return nil
}

// parseInValues parses the values in an IN expression
func parseInValues(valuesStr string) []*Expression {
	// Remove parentheses
	valuesStr = strings.TrimSpace(valuesStr)
	if strings.HasPrefix(valuesStr, "(") && strings.HasSuffix(valuesStr, ")") {
		valuesStr = valuesStr[1 : len(valuesStr)-1]
	}

	values := []*Expression{}
	parts := splitByComma(valuesStr)
	for _, part := range parts {
		expr, err := parseExpressionInternal(part)
		if err == nil {
			values = append(values, expr)
		}
	}

	return values
}

// parseBetweenExpression parses a BETWEEN expression
func parseBetweenExpression(expr string) *Expression {
	exprUpper := strings.ToUpper(expr)

	betweenIdx := strings.Index(exprUpper, " BETWEEN ")
	andIdx := strings.LastIndex(exprUpper, " AND ")

	if betweenIdx != -1 && andIdx != -1 && andIdx > betweenIdx {
		valueExpr := strings.TrimSpace(expr[:betweenIdx])
		lowerExpr := strings.TrimSpace(expr[betweenIdx+9 : andIdx])
		upperExpr := strings.TrimSpace(expr[andIdx+5:])

		value, err1 := parseExpressionInternal(valueExpr)
		lower, err2 := parseExpressionInternal(lowerExpr)
		upper, err3 := parseExpressionInternal(upperExpr)

		if err1 == nil && err2 == nil && err3 == nil {
			return &Expression{
				Type:      ExprTypeBetween,
				Left:      value,
				Arguments: []*Expression{lower, upper},
			}
		}
	}

	return nil
}

// isFunctionCall checks if the expression is a function call
func isFunctionCall(expr string) bool {
	// Match pattern: name(...)
	pattern := regexp.MustCompile(`^[a-zA-Z_][a-zA-Z0-9_]*\s*\(`)
	return pattern.MatchString(expr) && strings.HasSuffix(expr, ")")
}

// parseFunctionCall parses a function call expression
func parseFunctionCall(expr string) (*Expression, error) {
	// Find the opening parenthesis
	openIdx := strings.Index(expr, "(")
	if openIdx == -1 {
		return nil, fmt.Errorf("invalid function call")
	}

	funcName := strings.TrimSpace(expr[:openIdx])
	argsStr := strings.TrimSpace(expr[openIdx+1 : len(expr)-1])

	args := []*Expression{}
	if argsStr != "" {
		argParts := splitByComma(argsStr)
		for _, part := range argParts {
			arg, err := parseExpressionInternal(part)
			if err != nil {
				return nil, err
			}
			args = append(args, arg)
		}
	}

	return &Expression{
		Type:         ExprTypeFunction,
		FunctionName: strings.ToLower(funcName),
		Arguments:    args,
	}, nil
}

// isColumnReference checks if the expression is a column reference
func isColumnReference(expr string) bool {
	// Column reference: name, table.name, db.table.name
	pattern := regexp.MustCompile(`^[a-zA-Z_][a-zA-Z0-9_]*(\.[a-zA-Z_][a-zA-Z0-9_]*)*$`)
	return pattern.MatchString(expr)
}

// parseColumnReference parses a column reference
func parseColumnReference(expr string) (*Expression, error) {
	parts := strings.Split(expr, ".")

	// Check for special columns
	specialColumns := map[string]bool{
		"_row_id":             true,
		"_create_timestamp":   true,
		"_delete_timestamp":   true,
		"_score":              true,
		"_similarity":         true,
		"_distance":           true,
		"_score_factors":      true,
		"_similarity_factors": true,
		"_distance_factors":   true,
	}

	if len(parts) == 1 && specialColumns[strings.ToLower(parts[0])] {
		// Special column, treat as function call
		return &Expression{
			Type:         ExprTypeFunction,
			FunctionName: strings.ToLower(parts[0]),
			Arguments:    []*Expression{},
		}, nil
	}

	return &Expression{
		Type:       ExprTypeColumn,
		ColumnName: parts,
	}, nil
}

// parseConstant parses a constant value
func parseConstant(expr string) (*thriftapi.ConstantExpr, error) {
	expr = strings.TrimSpace(expr)

	// Try boolean
	lower := strings.ToLower(expr)
	if lower == "true" {
		trueVal := true
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_Boolean,
			BoolValue:   &trueVal,
		}, nil
	}
	if lower == "false" {
		falseVal := false
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_Boolean,
			BoolValue:   &falseVal,
		}, nil
	}
	if lower == "null" {
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_Null,
		}, nil
	}

	// Try integer
	if intVal, err := strconv.ParseInt(expr, 10, 64); err == nil {
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_Int64,
			I64Value:    &intVal,
		}, nil
	}

	// Try float
	if floatVal, err := strconv.ParseFloat(expr, 64); err == nil {
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_Double,
			F64Value:    &floatVal,
		}, nil
	}

	// Try string literal (single or double quotes)
	if (strings.HasPrefix(expr, "'") && strings.HasSuffix(expr, "'")) ||
		(strings.HasPrefix(expr, "\"") && strings.HasSuffix(expr, "\"")) {
		strVal := expr[1 : len(expr)-1]
		return &thriftapi.ConstantExpr{
			LiteralType: thriftapi.LiteralType_String,
			StrValue:    &strVal,
		}, nil
	}

	return nil, fmt.Errorf("unable to parse constant: %s", expr)
}

// splitByOperator splits an expression by an operator, respecting parentheses
func splitByOperator(expr, op string) (*Expression, *Expression, bool) {
	expr = strings.TrimSpace(expr)
	opLen := len(op)

	parenCount := 0
	for i := 0; i <= len(expr)-opLen; i++ {
		if expr[i] == '(' {
			parenCount++
		} else if expr[i] == ')' {
			parenCount--
		} else if parenCount == 0 {
			// Check if this is the operator
			if strings.EqualFold(expr[i:i+opLen], op) {
				// Make sure it's not part of a larger word (for word operators)
				if i+opLen < len(expr) && isAlphaNum(expr[i+opLen]) {
					continue
				}
				if i > 0 && isAlphaNum(expr[i-1]) {
					continue
				}

				leftExpr := strings.TrimSpace(expr[:i])
				rightExpr := strings.TrimSpace(expr[i+opLen:])

				left, err1 := parseExpressionInternal(leftExpr)
				right, err2 := parseExpressionInternal(rightExpr)

				if err1 == nil && err2 == nil {
					return left, right, true
				}
			}
		}
	}

	return nil, nil, false
}

// splitByComma splits a string by commas, respecting parentheses
func splitByComma(expr string) []string {
	parts := []string{}
	current := ""
	parenCount := 0

	for _, ch := range expr {
		if ch == '(' {
			parenCount++
		} else if ch == ')' {
			parenCount--
		}

		if ch == ',' && parenCount == 0 {
			parts = append(parts, strings.TrimSpace(current))
			current = ""
		} else {
			current += string(ch)
		}
	}

	if current != "" {
		parts = append(parts, strings.TrimSpace(current))
	}

	return parts
}

// isAlphaNum checks if a character is alphanumeric
func isAlphaNum(ch byte) bool {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_'
}

// mapSQLTypeToDataType maps SQL type names to thrift DataType
func mapSQLTypeToDataType(typeName string) *thriftapi.DataType {
	typeName = strings.ToUpper(typeName)

	dataType := &thriftapi.DataType{}

	switch typeName {
	case "INT", "INTEGER":
		dataType.LogicType = thriftapi.LogicType_Integer
	case "TINYINT":
		dataType.LogicType = thriftapi.LogicType_TinyInt
	case "SMALLINT":
		dataType.LogicType = thriftapi.LogicType_SmallInt
	case "BIGINT":
		dataType.LogicType = thriftapi.LogicType_BigInt
	case "FLOAT":
		dataType.LogicType = thriftapi.LogicType_Float
	case "DOUBLE":
		dataType.LogicType = thriftapi.LogicType_Double
	case "VARCHAR", "TEXT", "STRING":
		dataType.LogicType = thriftapi.LogicType_Varchar
	case "BOOL", "BOOLEAN":
		dataType.LogicType = thriftapi.LogicType_Boolean
	case "DATE":
		dataType.LogicType = thriftapi.LogicType_Date
	case "TIME":
		dataType.LogicType = thriftapi.LogicType_Time
	case "DATETIME":
		dataType.LogicType = thriftapi.LogicType_DateTime
	case "TIMESTAMP":
		dataType.LogicType = thriftapi.LogicType_Timestamp
	case "JSON":
		dataType.LogicType = thriftapi.LogicType_Json
	default:
		dataType.LogicType = thriftapi.LogicType_Varchar
	}

	return dataType
}

// convertToThriftParsedExpr converts an Expression to a thrift ParsedExpr
func convertToThriftParsedExpr(expr *Expression) (*thriftapi.ParsedExpr, error) {
	if expr == nil {
		return nil, nil
	}

	parsedExpr := &thriftapi.ParsedExpr{}
	exprType := &thriftapi.ParsedExprType{}

	switch expr.Type {
	case ExprTypeColumn:
		columnExpr := &thriftapi.ColumnExpr{
			ColumnName: expr.ColumnName,
			Star:       false,
		}
		exprType.ColumnExpr = columnExpr

	case ExprTypeStar:
		columnExpr := &thriftapi.ColumnExpr{
			ColumnName: []string{},
			Star:       true,
		}
		exprType.ColumnExpr = columnExpr

	case ExprTypeConstant:
		if expr.Constant != nil {
			exprType.ConstantExpr = expr.Constant
		}

	case ExprTypeFunction:
		funcExpr := &thriftapi.FunctionExpr{
			FunctionName: expr.FunctionName,
		}
		for _, arg := range expr.Arguments {
			thriftArg, err := convertToThriftParsedExpr(arg)
			if err != nil {
				return nil, err
			}
			funcExpr.Arguments = append(funcExpr.Arguments, thriftArg)
		}
		exprType.FunctionExpr = funcExpr

	case ExprTypeBinary:
		funcExpr := &thriftapi.FunctionExpr{
			FunctionName: mapBinaryOperator(expr.Operator),
		}
		leftArg, err := convertToThriftParsedExpr(expr.Left)
		if err != nil {
			return nil, err
		}
		rightArg, err := convertToThriftParsedExpr(expr.Right)
		if err != nil {
			return nil, err
		}
		funcExpr.Arguments = []*thriftapi.ParsedExpr{leftArg, rightArg}
		exprType.FunctionExpr = funcExpr

	case ExprTypeUnary:
		funcExpr := &thriftapi.FunctionExpr{
			FunctionName: expr.Operator,
		}
		arg, err := convertToThriftParsedExpr(expr.Left)
		if err != nil {
			return nil, err
		}
		funcExpr.Arguments = []*thriftapi.ParsedExpr{arg}
		exprType.FunctionExpr = funcExpr

	case ExprTypeCast:
		castExpr := &thriftapi.CastExpr{
			DataType: expr.CastType,
		}
		innerExpr, err := convertToThriftParsedExpr(expr.Left)
		if err != nil {
			return nil, err
		}
		castExpr.Expr = innerExpr
		exprType.CastExpr = castExpr

	case ExprTypeIn:
		inExpr := &thriftapi.InExpr{
			InType: expr.InType,
		}
		leftOperand, err := convertToThriftParsedExpr(expr.Left)
		if err != nil {
			return nil, err
		}
		inExpr.LeftOperand = leftOperand

		for _, val := range expr.InValues {
			thriftVal, err := convertToThriftParsedExpr(val)
			if err != nil {
				return nil, err
			}
			inExpr.Arguments = append(inExpr.Arguments, thriftVal)
		}
		exprType.InExpr = inExpr

	case ExprTypeBetween:
		// BETWEEN is represented as a function: between(value, lower, upper)
		funcExpr := &thriftapi.FunctionExpr{
			FunctionName: "between",
		}
		valueArg, err := convertToThriftParsedExpr(expr.Left)
		if err != nil {
			return nil, err
		}
		funcExpr.Arguments = append(funcExpr.Arguments, valueArg)

		for _, arg := range expr.Arguments {
			thriftArg, err := convertToThriftParsedExpr(arg)
			if err != nil {
				return nil, err
			}
			funcExpr.Arguments = append(funcExpr.Arguments, thriftArg)
		}
		exprType.FunctionExpr = funcExpr

	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidExpression), fmt.Sprintf("Unknown expression type: %v", expr.Type))
	}

	parsedExpr.Type = exprType
	return parsedExpr, nil
}

// mapBinaryOperator maps SQL operators to function names
func mapBinaryOperator(op string) string {
	switch op {
	case "=", "==":
		return "="
	case "!=", "<>":
		return "!="
	case ">":
		return ">"
	case "<":
		return "<"
	case ">=":
		return ">="
	case "<=":
		return "<="
	case "AND", "and":
		return "and"
	case "OR", "or":
		return "or"
	case "+":
		return "+"
	case "-":
		return "-"
	case "*":
		return "*"
	case "/":
		return "/"
	case "%":
		return "%"
	default:
		return op
	}
}

// ParseFilter parses a filter condition string
func ParseFilter(filter string) (*thriftapi.ParsedExpr, error) {
	if filter == "" {
		return nil, nil
	}
	return ParseExpr(filter)
}

// ParsedExprToString converts a ParsedExpr back to a string representation
func ParsedExprToString(expr *thriftapi.ParsedExpr) string {
	if expr == nil {
		return ""
	}

	exprType := expr.Type

	if exprType.ConstantExpr != nil {
		return constantExprToString(exprType.ConstantExpr)
	}

	if exprType.ColumnExpr != nil {
		if exprType.ColumnExpr.Star {
			return "*"
		}
		return strings.Join(exprType.ColumnExpr.ColumnName, ".")
	}

	if exprType.FunctionExpr != nil {
		funcName := exprType.FunctionExpr.FunctionName
		args := []string{}
		for _, arg := range exprType.FunctionExpr.Arguments {
			args = append(args, ParsedExprToString(arg))
		}
		return fmt.Sprintf("%s(%s)", funcName, strings.Join(args, ", "))
	}

	if exprType.CastExpr != nil {
		inner := ParsedExprToString(exprType.CastExpr.Expr)
		typeName := logicTypeToString(exprType.CastExpr.DataType.LogicType)
		return fmt.Sprintf("CAST(%s AS %s)", inner, typeName)
	}

	if exprType.InExpr != nil {
		left := ParsedExprToString(exprType.InExpr.LeftOperand)
		values := []string{}
		for _, arg := range exprType.InExpr.Arguments {
			values = append(values, ParsedExprToString(arg))
		}
		if exprType.InExpr.InType {
			return fmt.Sprintf("%s IN (%s)", left, strings.Join(values, ", "))
		}
		return fmt.Sprintf("%s NOT IN (%s)", left, strings.Join(values, ", "))
	}

	return ""
}

// constantExprToString converts a ConstantExpr to string
func constantExprToString(expr *thriftapi.ConstantExpr) string {
	if expr == nil {
		return ""
	}

	switch expr.LiteralType {
	case thriftapi.LiteralType_Boolean:
		if expr.BoolValue != nil {
			return fmt.Sprintf("%v", *expr.BoolValue)
		}
	case thriftapi.LiteralType_Int64:
		if expr.I64Value != nil {
			return fmt.Sprintf("%d", *expr.I64Value)
		}
	case thriftapi.LiteralType_Double:
		if expr.F64Value != nil {
			return fmt.Sprintf("%f", *expr.F64Value)
		}
	case thriftapi.LiteralType_String:
		if expr.StrValue != nil {
			return fmt.Sprintf("'%s'", *expr.StrValue)
		}
	case thriftapi.LiteralType_Null:
		return "NULL"
	}

	return ""
}

// logicTypeToString converts a LogicType to string
func logicTypeToString(logicType thriftapi.LogicType) string {
	switch logicType {
	case thriftapi.LogicType_Integer:
		return "INTEGER"
	case thriftapi.LogicType_TinyInt:
		return "TINYINT"
	case thriftapi.LogicType_SmallInt:
		return "SMALLINT"
	case thriftapi.LogicType_BigInt:
		return "BIGINT"
	case thriftapi.LogicType_Float:
		return "FLOAT"
	case thriftapi.LogicType_Double:
		return "DOUBLE"
	case thriftapi.LogicType_Varchar:
		return "VARCHAR"
	case thriftapi.LogicType_Boolean:
		return "BOOLEAN"
	case thriftapi.LogicType_Date:
		return "DATE"
	case thriftapi.LogicType_Time:
		return "TIME"
	case thriftapi.LogicType_DateTime:
		return "DATETIME"
	case thriftapi.LogicType_Timestamp:
		return "TIMESTAMP"
	case thriftapi.LogicType_Json:
		return "JSON"
	default:
		return "UNKNOWN"
	}
}
