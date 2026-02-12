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
	"strings"

	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// QueryBuilder provides a fluent interface for building queries
type QueryBuilder struct {
	columns          []*thriftapi.ParsedExpr
	highlight        []*thriftapi.ParsedExpr
	search           *thriftapi.SearchExpr
	filter           *thriftapi.ParsedExpr
	groupby          []*thriftapi.ParsedExpr
	having           *thriftapi.ParsedExpr
	limit            *thriftapi.ParsedExpr
	offset           *thriftapi.ParsedExpr
	sort             []*thriftapi.OrderByExpr
	totalHitsCount   bool
}

// NewQueryBuilder creates a new QueryBuilder instance
func NewQueryBuilder() *QueryBuilder {
	return &QueryBuilder{}
}

// Reset resets all query builder fields to their initial state
func (qb *QueryBuilder) Reset() *QueryBuilder {
	qb.columns = nil
	qb.highlight = nil
	qb.search = nil
	qb.filter = nil
	qb.groupby = nil
	qb.having = nil
	qb.limit = nil
	qb.offset = nil
	qb.sort = nil
	qb.totalHitsCount = false
	return qb
}

// MatchDense performs dense vector search
func (qb *QueryBuilder) MatchDense(
	vectorColumnName string,
	embeddingData interface{},
	embeddingDataType string,
	distanceType string,
	topn int,
	knnParams map[string]string,
) (*QueryBuilder, error) {
	if qb.search == nil {
		qb.search = thriftapi.NewSearchExpr()
	}

	if !isValidTopN(topn) {
		return nil, NewInfinityException(int(ErrorCodeInvalidTopKType), fmt.Sprintf("Invalid topn, type should be int, but got %T", topn))
	}

	// Check if this is an FDE object
	if fde, ok := embeddingData.(FDE); ok {
		return qb.handleFDEMatchDense(vectorColumnName, fde, embeddingDataType, distanceType, topn, knnParams)
	}

	// Convert embedding data to slice
	var dataSlice []float64
	switch v := embeddingData.(type) {
	case []float64:
		dataSlice = v
	case []float32:
		dataSlice = make([]float64, len(v))
		for i, val := range v {
			dataSlice[i] = float64(val)
		}
	case Float64Vector:
		dataSlice = []float64(v)
	case Float32Vector:
		dataSlice = make([]float64, len(v))
		for i, val := range v {
			dataSlice[i] = float64(val)
		}
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidDataType), fmt.Sprintf("Invalid embedding data, type should be vector, but got %T", embeddingData))
	}

	// Handle bit type special case
	if embeddingDataType == "bit" {
		if len(dataSlice)%8 != 0 {
			return nil, NewInfinityException(int(ErrorCodeInvalidEmbeddingDataType), "Embeddings with data bit must have dimension of times of 8!")
		}
		dimensions := len(dataSlice) / 8
		newData := make([]float64, dimensions)
		for i := 0; i < dimensions; i++ {
			bitunit := 0
			for bitIdx := 0; bitIdx < 8; bitIdx++ {
				if dataSlice[i*8+bitIdx] > 0 {
					bitunit |= (1 << bitIdx)
				}
			}
			newData[i] = float64(bitunit)
		}
		dataSlice = newData
	}

	// Convert to appropriate type based on embeddingDataType
	var elemType thriftapi.ElementType
	data := thriftapi.NewEmbeddingData()

	switch strings.ToLower(embeddingDataType) {
	case "bit":
		elemType = thriftapi.ElementType_ElementBit
		u8Array := make([]int16, len(dataSlice))
		for i, v := range dataSlice {
			u8Array[i] = int16(v)
		}
		data.U8ArrayValue = u8Array
	case "uint8":
		elemType = thriftapi.ElementType_ElementUInt8
		u8Array := make([]int16, len(dataSlice))
		for i, v := range dataSlice {
			u8Array[i] = int16(v)
		}
		data.U8ArrayValue = u8Array
	case "int8":
		elemType = thriftapi.ElementType_ElementInt8
		i8Array := make([]int16, len(dataSlice))
		for i, v := range dataSlice {
			i8Array[i] = int16(v)
		}
		data.I8ArrayValue = i8Array
	case "int16":
		elemType = thriftapi.ElementType_ElementInt16
		i16Array := make([]int16, len(dataSlice))
		for i, v := range dataSlice {
			i16Array[i] = int16(v)
		}
		data.I16ArrayValue = i16Array
	case "int", "int32":
		elemType = thriftapi.ElementType_ElementInt32
		i32Array := make([]int32, len(dataSlice))
		for i, v := range dataSlice {
			i32Array[i] = int32(v)
		}
		data.I32ArrayValue = i32Array
	case "int64":
		elemType = thriftapi.ElementType_ElementInt64
		i64Array := make([]int64, len(dataSlice))
		for i, v := range dataSlice {
			i64Array[i] = int64(v)
		}
		data.I64ArrayValue = i64Array
	case "float", "float32":
		elemType = thriftapi.ElementType_ElementFloat32
		data.F32ArrayValue = dataSlice
	case "double", "float64":
		elemType = thriftapi.ElementType_ElementFloat64
		data.F64ArrayValue = dataSlice
	case "float16":
		elemType = thriftapi.ElementType_ElementFloat16
		data.F16ArrayValue = dataSlice
	case "bfloat16":
		elemType = thriftapi.ElementType_ElementBFloat16
		data.Bf16ArrayValue = dataSlice
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidEmbeddingDataType), fmt.Sprintf("Invalid embedding data type: %s", embeddingDataType))
	}

	// Convert distance type
	var distType thriftapi.KnnDistanceType
	switch strings.ToLower(distanceType) {
	case "l2":
		distType = thriftapi.KnnDistanceType_L2
	case "cosine", "cos":
		distType = thriftapi.KnnDistanceType_Cosine
	case "ip":
		distType = thriftapi.KnnDistanceType_InnerProduct
	case "hamming":
		distType = thriftapi.KnnDistanceType_Hamming
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidKNNDistanceType), fmt.Sprintf("Invalid distance type: %s", distanceType))
	}

	// Handle optional parameters
	var knnOptParams []*thriftapi.InitParameter
	var optionalFilter *thriftapi.ParsedExpr
	if knnParams != nil {
		optionalFilter = getSearchOptionalFilterFromOptParams(knnParams)
		for k, v := range knnParams {
			if strings.ToLower(k) == "filter" {
				continue
			}
			param := thriftapi.NewInitParameter()
			param.ParamName = strings.ToLower(k)
			param.ParamValue = strings.ToLower(v)
			knnOptParams = append(knnOptParams, param)
		}
	}

	// Create KnnExpr
	columnExpr := thriftapi.NewColumnExpr()
	columnExpr.ColumnName = []string{vectorColumnName}
	columnExpr.Star = false

	knnExpr := thriftapi.NewKnnExpr()
	knnExpr.ColumnExpr = columnExpr
	knnExpr.EmbeddingData = data
	knnExpr.EmbeddingDataType = elemType
	knnExpr.DistanceType = distType
	knnExpr.Topn = int64(topn)
	knnExpr.OptParams = knnOptParams
	knnExpr.FilterExpr = optionalFilter

	genericMatchExpr := thriftapi.NewGenericMatchExpr()
	genericMatchExpr.MatchVectorExpr = knnExpr

	qb.search.MatchExprs = append(qb.search.MatchExprs, genericMatchExpr)
	return qb, nil
}

// handleFDEMatchDense handles FDE function call in match_dense
func (qb *QueryBuilder) handleFDEMatchDense(
	vectorColumnName string,
	fdeData FDE,
	embeddingDataType string,
	distanceType string,
	topn int,
	knnParams map[string]string,
) (*QueryBuilder, error) {
	// Create FDE function expression
	fdeFunction := thriftapi.NewFunctionExpr()
	fdeFunction.FunctionName = "FDE"

	// Create tensor data constant expression
	tensorConstant := thriftapi.NewConstantExpr()
	tensorConstant.LiteralType = thriftapi.LiteralType_DoubleTensor
	tensorConstant.F64TensorValue = fdeData.TensorData

	// Create target dimension constant expression
	targetDimConstant := thriftapi.NewConstantExpr()
	targetDimConstant.LiteralType = thriftapi.LiteralType_Int64
	targetDimConstant.I64Value = &[]int64{int64(fdeData.TargetDimension)}[0]

	// Create parsed expressions for arguments
	tensorParsedExpr := &thriftapi.ParsedExpr{
		Type: &thriftapi.ParsedExprType{
			ConstantExpr: tensorConstant,
		},
	}

	targetDimParsedExpr := &thriftapi.ParsedExpr{
		Type: &thriftapi.ParsedExprType{
			ConstantExpr: targetDimConstant,
		},
	}

	fdeFunction.Arguments = []*thriftapi.ParsedExpr{tensorParsedExpr, targetDimParsedExpr}

	// Convert distance type
	var distType thriftapi.KnnDistanceType
	switch strings.ToLower(distanceType) {
	case "l2":
		distType = thriftapi.KnnDistanceType_L2
	case "cosine", "cos":
		distType = thriftapi.KnnDistanceType_Cosine
	case "ip":
		distType = thriftapi.KnnDistanceType_InnerProduct
	case "hamming":
		distType = thriftapi.KnnDistanceType_Hamming
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidKNNDistanceType), fmt.Sprintf("Invalid distance type: %s", distanceType))
	}

	// Convert embedding data type
	var elemType thriftapi.ElementType
	switch strings.ToLower(embeddingDataType) {
	case "float", "float32":
		elemType = thriftapi.ElementType_ElementFloat32
	case "double", "float64":
		elemType = thriftapi.ElementType_ElementFloat64
	case "float16":
		elemType = thriftapi.ElementType_ElementFloat16
	case "bfloat16":
		elemType = thriftapi.ElementType_ElementBFloat16
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidEmbeddingDataType), fmt.Sprintf("Invalid embedding data type: %s", embeddingDataType))
	}

	// Handle optional parameters
	var knnOptParams []*thriftapi.InitParameter
	var optionalFilter *thriftapi.ParsedExpr
	if knnParams != nil {
		optionalFilter = getSearchOptionalFilterFromOptParams(knnParams)
		for k, v := range knnParams {
			if strings.ToLower(k) == "filter" {
				continue
			}
			param := thriftapi.NewInitParameter()
			param.ParamName = strings.ToLower(k)
			param.ParamValue = strings.ToLower(v)
			knnOptParams = append(knnOptParams, param)
		}
	}

	// Create KnnExpr with FDE function as query_embedding_expr
	columnExpr := thriftapi.NewColumnExpr()
	columnExpr.ColumnName = []string{vectorColumnName}
	columnExpr.Star = false

	knnExpr := thriftapi.NewKnnExpr()
	knnExpr.ColumnExpr = columnExpr
	knnExpr.EmbeddingDataType = elemType
	knnExpr.DistanceType = distType
	knnExpr.Topn = int64(topn)
	knnExpr.OptParams = knnOptParams
	knnExpr.FilterExpr = optionalFilter
	knnExpr.QueryEmbeddingExpr = fdeFunction

	genericMatchExpr := thriftapi.NewGenericMatchExpr()
	genericMatchExpr.MatchVectorExpr = knnExpr

	qb.search.MatchExprs = append(qb.search.MatchExprs, genericMatchExpr)
	return qb, nil
}

// MatchSparse performs sparse vector search
func (qb *QueryBuilder) MatchSparse(
	vectorColumnName string,
	sparseData *SparseVector,
	metricType string,
	topn int,
	optParams map[string]string,
) (*QueryBuilder, error) {
	if qb.search == nil {
		qb.search = thriftapi.NewSearchExpr()
	}

	optionalFilter := getSearchOptionalFilterFromOptParams(optParams)
	matchSparseExpr := makeMatchSparseExpr(vectorColumnName, sparseData, metricType, topn, optParams, optionalFilter)

	genericMatchExpr := thriftapi.NewGenericMatchExpr()
	genericMatchExpr.MatchSparseExpr = matchSparseExpr

	qb.search.MatchExprs = append(qb.search.MatchExprs, genericMatchExpr)
	return qb, nil
}

// MatchText performs full-text search
func (qb *QueryBuilder) MatchText(
	fields string,
	matchingText string,
	topn int,
	extraOptions map[string]string,
) (*QueryBuilder, error) {
	if qb.search == nil {
		qb.search = thriftapi.NewSearchExpr()
	}

	matchExpr := thriftapi.NewMatchExpr()
	matchExpr.Fields = fields
	matchExpr.MatchingText = matchingText

	optionsText := fmt.Sprintf("topn=%d", topn)
	if extraOptions != nil {
		matchExpr.FilterExpr = getSearchOptionalFilterFromOptParams(extraOptions)
		for k, v := range extraOptions {
			if strings.ToLower(k) == "filter" {
				continue
			}
			optionsText += fmt.Sprintf(";%s=%s", k, v)
		}
	}
	matchExpr.OptionsText = optionsText

	genericMatchExpr := thriftapi.NewGenericMatchExpr()
	genericMatchExpr.MatchTextExpr = matchExpr

	qb.search.MatchExprs = append(qb.search.MatchExprs, genericMatchExpr)
	return qb, nil
}

// MatchTensor performs tensor search
func (qb *QueryBuilder) MatchTensor(
	columnName string,
	queryData interface{},
	queryDataType string,
	topn int,
	extraOption map[string]string,
) (*QueryBuilder, error) {
	if qb.search == nil {
		qb.search = thriftapi.NewSearchExpr()
	}

	optionStr := fmt.Sprintf("topn=%d", topn)
	var optionalFilter *thriftapi.ParsedExpr
	if extraOption != nil {
		optionalFilter = getSearchOptionalFilterFromOptParams(extraOption)
		for k, v := range extraOption {
			if strings.ToLower(k) == "filter" {
				continue
			}
			optionStr += fmt.Sprintf(";%s=%s", k, v)
		}
	}

	matchTensorExpr := makeMatchTensorExpr(columnName, queryData, queryDataType, "maxsim", optionStr, optionalFilter)

	genericMatchExpr := thriftapi.NewGenericMatchExpr()
	genericMatchExpr.MatchTensorExpr = matchTensorExpr

	qb.search.MatchExprs = append(qb.search.MatchExprs, genericMatchExpr)
	return qb, nil
}

// Fusion combines multiple search results
func (qb *QueryBuilder) Fusion(
	method string,
	topn int,
	fusionParams map[string]interface{},
) (*QueryBuilder, error) {
	if qb.search == nil {
		qb.search = thriftapi.NewSearchExpr()
	}

	fusionExpr := thriftapi.NewFusionExpr()
	fusionExpr.Method = method

	finalOptionText := fmt.Sprintf("topn=%d", topn)
	methodLower := strings.ToLower(method)

	switch methodLower {
	case "rrf", "weighted_sum":
		if fusionParams != nil {
			for k, v := range fusionParams {
				if strings.ToLower(k) == "topn" {
					return nil, NewInfinityException(int(ErrorCodeInvalidExpression), "topn is not allowed in fusion params")
				}
				finalOptionText += fmt.Sprintf(";%s=%v", k, v)
			}
		}
	case "match_tensor":
		if fusionParams != nil {
			field, _ := fusionParams["field"].(string)
			queryTensor, _ := fusionParams["query_tensor"].(VEC)
			elementType, _ := fusionParams["element_type"].(string)
			fusionExpr.OptionalMatchTensorExpr = makeMatchTensorExpr(field, queryTensor, elementType, "maxsim", "", nil)
		}
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidExpression), "Invalid fusion method")
	}

	fusionExpr.OptionsText = finalOptionText
	qb.search.FusionExprs = append(qb.search.FusionExprs, fusionExpr)
	return qb, nil
}

// Filter applies a filter condition
func (qb *QueryBuilder) Filter(where string) (*QueryBuilder, error) {
	whereExpr, err := ParseFilter(where)
	if err != nil {
		return nil, err
	}
	qb.filter = whereExpr
	return qb, nil
}

// Limit sets the limit
func (qb *QueryBuilder) Limit(limit int) *QueryBuilder {
	constantExp := &thriftapi.ConstantExpr{
		LiteralType: thriftapi.LiteralType_Int64,
		I64Value:    &[]int64{int64(limit)}[0],
	}
	exprType := &thriftapi.ParsedExprType{
		ConstantExpr: constantExp,
	}
	limitExpr := &thriftapi.ParsedExpr{
		Type: exprType,
	}
	qb.limit = limitExpr
	return qb
}

// Offset sets the offset
func (qb *QueryBuilder) Offset(offset int) *QueryBuilder {
	constantExp := &thriftapi.ConstantExpr{
		LiteralType: thriftapi.LiteralType_Int64,
		I64Value:    &[]int64{int64(offset)}[0],
	}
	exprType := &thriftapi.ParsedExprType{
		ConstantExpr: constantExp,
	}
	offsetExpr := &thriftapi.ParsedExpr{
		Type: exprType,
	}
	qb.offset = offsetExpr
	return qb
}

// GroupBy sets group by columns
func (qb *QueryBuilder) GroupBy(columns interface{}) (*QueryBuilder, error) {
	var columnNames []string
	switch v := columns.(type) {
	case string:
		columnNames = []string{strings.ToLower(v)}
	case []string:
		columnNames = make([]string, len(v))
		for i, col := range v {
			columnNames[i] = strings.ToLower(col)
		}
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), "columns must be a string or []string")
	}

	groupByList := make([]*thriftapi.ParsedExpr, 0, len(columnNames))
	for _, column := range columnNames {
		parsedExpr, err := ParseExpr(column)
		if err != nil {
			return nil, err
		}
		groupByList = append(groupByList, parsedExpr)
	}
	qb.groupby = groupByList
	return qb, nil
}

// Having applies having clause
func (qb *QueryBuilder) Having(having string) (*QueryBuilder, error) {
	havingExpr, err := ParseFilter(having)
	if err != nil {
		return nil, err
	}
	qb.having = havingExpr
	return qb, nil
}

// Output specifies output columns
func (qb *QueryBuilder) Output(columns []string) (*QueryBuilder, error) {
	selectList := make([]*thriftapi.ParsedExpr, 0, len(columns))

	for _, column := range columns {
		columnLower := strings.ToLower(column)

		switch columnLower {
		case "*":
			columnExpr := &thriftapi.ColumnExpr{
				Star:       true,
				ColumnName: []string{},
			}
			exprType := &thriftapi.ParsedExprType{
				ColumnExpr: columnExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_row_id":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "row_id",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_create_timestamp":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "create_timestamp",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_delete_timestamp":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "delete_timestamp",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_score":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "score",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_similarity":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "similarity",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_distance":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "distance",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_score_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "score_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_similarity_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "similarity_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		case "_distance_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "distance_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr := &thriftapi.ParsedExpr{
				Type: exprType,
			}
			selectList = append(selectList, parsedExpr)
		default:
			parsedExpr, err := ParseExpr(column)
			if err != nil {
				return nil, err
			}
			selectList = append(selectList, parsedExpr)
		}
	}

	qb.columns = selectList
	return qb, nil
}

// Highlight specifies highlight columns
func (qb *QueryBuilder) Highlight(columns []string) (*QueryBuilder, error) {
	highlightList := make([]*thriftapi.ParsedExpr, 0, len(columns))
	for _, column := range columns {
		columnLower := strings.ToLower(column)
		parsedExpr, err := ParseExpr(columnLower)
		if err != nil {
			return nil, err
		}
		highlightList = append(highlightList, parsedExpr)
	}
	qb.highlight = highlightList
	return qb, nil
}

// Option sets options
func (qb *QueryBuilder) Option(optionKV map[string]interface{}) *QueryBuilder {
	if val, ok := optionKV["total_hits_count"]; ok {
		if boolVal, ok := val.(bool); ok {
			qb.totalHitsCount = boolVal
		}
	}
	return qb
}

// Sort sets sort order
func (qb *QueryBuilder) Sort(orderByExprList [][2]interface{}) (*QueryBuilder, error) {
	sortList := make([]*thriftapi.OrderByExpr, 0, len(orderByExprList))

	for _, orderByExpr := range orderByExprList {
		if len(orderByExpr) != 2 {
			return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), "order by expression must have 2 elements")
		}

		orderByExprStr, ok := orderByExpr[0].(string)
		if !ok {
			return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), "order by column must be a string")
		}
		orderByExprStr = strings.ToLower(orderByExprStr)

		sortType, ok := orderByExpr[1].(SortType)
		if !ok {
			return nil, NewInfinityException(int(ErrorCodeInvalidParameterValue), "order by sort type must be SortType")
		}

		var parsedExpr *thriftapi.ParsedExpr
		switch orderByExprStr {
		case "*":
			columnExpr := &thriftapi.ColumnExpr{
				Star:       true,
				ColumnName: []string{},
			}
			exprType := &thriftapi.ParsedExprType{
				ColumnExpr: columnExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_row_id":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "row_id",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_create_timestamp":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "create_timestamp",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_delete_timestamp":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "delete_timestamp",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_score":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "score",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_similarity":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "similarity",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_distance":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "distance",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_score_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "score_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_similarity_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "similarity_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		case "_distance_factors":
			funcExpr := &thriftapi.FunctionExpr{
				FunctionName: "distance_factors",
				Arguments:    []*thriftapi.ParsedExpr{},
			}
			exprType := &thriftapi.ParsedExprType{
				FunctionExpr: funcExpr,
			}
			parsedExpr = &thriftapi.ParsedExpr{
				Type: exprType,
			}
		default:
			var err error
			parsedExpr, err = ParseExpr(orderByExprStr)
			if err != nil {
				return nil, err
			}
		}

		orderByExprObj := &thriftapi.OrderByExpr{
			Expr: parsedExpr,
			Asc:  sortType == SortTypeAsc,
		}
		sortList = append(sortList, orderByExprObj)
	}

	qb.sort = sortList
	return qb, nil
}

// GetColumns returns the output columns
func (qb *QueryBuilder) GetColumns() []*thriftapi.ParsedExpr {
	return qb.columns
}

// GetHighlight returns the highlight expressions
func (qb *QueryBuilder) GetHighlight() []*thriftapi.ParsedExpr {
	return qb.highlight
}

// GetSearch returns the search expression
func (qb *QueryBuilder) GetSearch() *thriftapi.SearchExpr {
	return qb.search
}

// GetFilter returns the filter expression
func (qb *QueryBuilder) GetFilter() *thriftapi.ParsedExpr {
	return qb.filter
}

// GetGroupBy returns the group by expressions
func (qb *QueryBuilder) GetGroupBy() []*thriftapi.ParsedExpr {
	return qb.groupby
}

// GetHaving returns the having expression
func (qb *QueryBuilder) GetHaving() *thriftapi.ParsedExpr {
	return qb.having
}

// GetLimit returns the limit expression
func (qb *QueryBuilder) GetLimit() *thriftapi.ParsedExpr {
	return qb.limit
}

// GetOffset returns the offset expression
func (qb *QueryBuilder) GetOffset() *thriftapi.ParsedExpr {
	return qb.offset
}

// GetSort returns the sort expressions
func (qb *QueryBuilder) GetSort() []*thriftapi.OrderByExpr {
	return qb.sort
}

// GetTotalHitsCount returns the total hits count flag
func (qb *QueryBuilder) GetTotalHitsCount() bool {
	return qb.totalHitsCount
}

// isValidTopN checks if topn is a valid integer
func isValidTopN(topn int) bool {
	return topn > 0
}

// getSearchOptionalFilterFromOptParams extracts filter from opt params
func getSearchOptionalFilterFromOptParams(optParams map[string]string) *thriftapi.ParsedExpr {
	if filterStr, ok := optParams["filter"]; ok {
		filterExpr, err := ParseFilter(filterStr)
		if err == nil {
			return filterExpr
		}
	}
	return nil
}

// makeMatchSparseExpr creates a MatchSparseExpr
func makeMatchSparseExpr(
	vectorColumnName string,
	sparseData *SparseVector,
	metricType string,
	topn int,
	optParams map[string]string,
	optionalFilter *thriftapi.ParsedExpr,
) *thriftapi.MatchSparseExpr {
	columnExpr := thriftapi.NewColumnExpr()
	columnExpr.ColumnName = []string{vectorColumnName}
	columnExpr.Star = false

	// Create sparse data constant expression
	querySparseExpr := thriftapi.NewConstantExpr()
	querySparseExpr.LiteralType = thriftapi.LiteralType_SparseDoubleArray
	querySparseExpr.I64ArrayIdx = make([]int64, len(sparseData.Indices))
	for i, idx := range sparseData.Indices {
		querySparseExpr.I64ArrayIdx[i] = int64(idx)
	}
	querySparseExpr.F64ArrayValue = sparseData.Values

	// Handle optional parameters
	var sparseOptParams []*thriftapi.InitParameter
	for k, v := range optParams {
		if strings.ToLower(k) == "filter" {
			continue
		}
		param := thriftapi.NewInitParameter()
		param.ParamName = strings.ToLower(k)
		param.ParamValue = v
		sparseOptParams = append(sparseOptParams, param)
	}

	matchSparseExpr := thriftapi.NewMatchSparseExpr()
	matchSparseExpr.ColumnExpr = columnExpr
	matchSparseExpr.QuerySparseExpr = querySparseExpr
	matchSparseExpr.MetricType = metricType
	matchSparseExpr.Topn = int64(topn)
	matchSparseExpr.OptParams = sparseOptParams
	matchSparseExpr.FilterExpr = optionalFilter

	return matchSparseExpr
}

// makeMatchTensorExpr creates a MatchTensorExpr
func makeMatchTensorExpr(
	vectorColumnName string,
	embeddingData interface{},
	embeddingDataType string,
	methodType string,
	extraOption string,
	filterExpr *thriftapi.ParsedExpr,
) *thriftapi.MatchTensorExpr {
	// Convert embedding data to slice
	var tensorData [][]float64
	switch v := embeddingData.(type) {
	case [][]float64:
		tensorData = v
	case []float64:
		tensorData = [][]float64{v}
	case []float32:
		inner := make([]float64, len(v))
		for i, val := range v {
			inner[i] = float64(val)
		}
		tensorData = [][]float64{inner}
	case [][]float32:
		tensorData = make([][]float64, len(v))
		for i, inner := range v {
			tensorData[i] = make([]float64, len(inner))
			for j, val := range inner {
				tensorData[i][j] = float64(val)
			}
		}
	case Float64Vector:
		tensorData = [][]float64{[]float64(v)}
	case Float32Vector:
		inner := make([]float64, len(v))
		for i, val := range v {
			inner[i] = float64(val)
		}
		tensorData = [][]float64{inner}
	default:
		tensorData = [][]float64{}
	}

	// Determine element type
	var elemType thriftapi.ElementType
	switch strings.ToLower(embeddingDataType) {
	case "bit":
		elemType = thriftapi.ElementType_ElementBit
	case "uint8":
		elemType = thriftapi.ElementType_ElementUInt8
	case "int8":
		elemType = thriftapi.ElementType_ElementInt8
	case "int16":
		elemType = thriftapi.ElementType_ElementInt16
	case "int", "int32":
		elemType = thriftapi.ElementType_ElementInt32
	case "int64":
		elemType = thriftapi.ElementType_ElementInt64
	case "float", "float32":
		elemType = thriftapi.ElementType_ElementFloat32
	case "double", "float64":
		elemType = thriftapi.ElementType_ElementFloat64
	case "float16":
		elemType = thriftapi.ElementType_ElementFloat16
	case "bfloat16":
		elemType = thriftapi.ElementType_ElementBFloat16
	default:
		elemType = thriftapi.ElementType_ElementFloat32
	}

	// Create column expression
	columnExpr := thriftapi.NewColumnExpr()
	columnExpr.ColumnName = []string{vectorColumnName}
	columnExpr.Star = false

	// Create embedding data - flatten tensor to array for storage
	data := thriftapi.NewEmbeddingData()
	if len(tensorData) > 0 {
		data.F64ArrayValue = tensorData[0]
	}

	matchTensorExpr := thriftapi.NewMatchTensorExpr()
	matchTensorExpr.SearchMethod = methodType
	matchTensorExpr.ColumnExpr = columnExpr
	matchTensorExpr.EmbeddingDataType = elemType
	matchTensorExpr.EmbeddingData = data
	matchTensorExpr.ExtraOptions = extraOption
	matchTensorExpr.FilterExpr = filterExpr

	return matchTensorExpr
}
