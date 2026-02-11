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

import (
	"fmt"
)

// NetworkAddress represents a network address with IP and port
type NetworkAddress struct {
	IP   string
	Port int
}

func (na NetworkAddress) String() string {
	return fmt.Sprintf("IP: %s, Port: %d", na.IP, na.Port)
}

// SparseVector represents a sparse vector with indices and values
type SparseVector struct {
	Indices []int
	Values  []float64
}

func (sv SparseVector) ToDict() map[string]float64 {
	result := make(map[string]float64)
	for i, v := range sv.Values {
		result[fmt.Sprintf("%d", sv.Indices[i])] = v
	}
	return result
}

// Array represents an array of elements
type Array struct {
	Elements []interface{}
}

func NewArray(elements ...interface{}) *Array {
	return &Array{Elements: elements}
}

func (a *Array) Append(element interface{}) {
	a.Elements = append(a.Elements, element)
}

func (a Array) String() string {
	return fmt.Sprintf("Array(%v)", a.Elements)
}

// FDE (Fixed Dimensional Encoding) represents a function call for insert operations
type FDE struct {
	TensorData     [][]float64
	TargetDimension int
}

func (fde FDE) String() string {
	return fmt.Sprintf("FDE(tensor_data=%v, target_dimension=%d)", fde.TensorData, fde.TargetDimension)
}

// ConflictType represents the conflict handling type
type ConflictType int

const (
	ConflictTypeIgnore  ConflictType = 0
	ConflictTypeError   ConflictType = 1
	ConflictTypeReplace ConflictType = 2
)

// SortType represents the sort order type
type SortType int

const (
	SortTypeAsc  SortType = 0
	SortTypeDesc SortType = 1
)

// ExplainType represents the type of query explanation
type ExplainType int

const (
	ExplainTypeAnalyze  ExplainType = 1
	ExplainTypeAst      ExplainType = 2
	ExplainTypeUnOpt    ExplainType = 3
	ExplainTypeOpt      ExplainType = 4
	ExplainTypePhysical ExplainType = 5
	ExplainTypePipeline ExplainType = 6
	ExplainTypeFragment ExplainType = 7
)

// InfinityException represents an error from Infinity
type InfinityException struct {
	ErrorCode int
	ErrorMsg  string
}

func (e InfinityException) Error() string {
	return fmt.Sprintf("InfinityException(%d, %s)", e.ErrorCode, e.ErrorMsg)
}

func NewInfinityException(errorCode int, errorMsg string) *InfinityException {
	return &InfinityException{
		ErrorCode: errorCode,
		ErrorMsg:  errorMsg,
	}
}

// Constants
const (
	DefaultMatchVectorTopN = 10
	DefaultMatchSparseTopN = 10
)

var (
	LocalHost = NetworkAddress{IP: "127.0.0.1", Port: 23817}
)

// URI represents a URI which can be either a network address or a path
type URI interface {
	String() string
}

// VEC represents vector data which can be a slice or FDE
type VEC interface {
	IsVector() bool
}

// Float32Vector represents a vector of float32 values
type Float32Vector []float32

func (v Float32Vector) IsVector() bool {
	return true
}

// Float64Vector represents a vector of float64 values
type Float64Vector []float64

func (v Float64Vector) IsVector() bool {
	return true
}

// Int8Vector represents a vector of int8 values
type Int8Vector []int8

func (v Int8Vector) IsVector() bool {
	return true
}

// InsertData represents data for insert operations
type InsertData map[string]interface{}
