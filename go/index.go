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

import "fmt"

// IndexType represents the type of index
type IndexType int

const (
	IndexTypeIVF                IndexType = 1
	IndexTypeHnsw               IndexType = 2
	IndexTypeFullText           IndexType = 3
	IndexTypeSecondary          IndexType = 4
	IndexTypeSecondaryFunctional IndexType = 5
	IndexTypeEMVB               IndexType = 6
	IndexTypeBMP                IndexType = 7
	IndexTypeDiskAnn            IndexType = 8
)

func (it IndexType) String() string {
	switch it {
	case IndexTypeIVF:
		return "IVF"
	case IndexTypeHnsw:
		return "Hnsw"
	case IndexTypeFullText:
		return "FullText"
	case IndexTypeSecondary:
		return "Secondary"
	case IndexTypeSecondaryFunctional:
		return "SecondaryFunctional"
	case IndexTypeEMVB:
		return "EMVB"
	case IndexTypeBMP:
		return "BMP"
	case IndexTypeDiskAnn:
		return "DiskAnn"
	default:
		return "Unknown"
	}
}

// InitParameter represents an initialization parameter for index
type InitParameter struct {
	ParamName  string
	ParamValue string
}

func (ip InitParameter) String() string {
	return fmt.Sprintf("InitParameter(%s, %s)", ip.ParamName, ip.ParamValue)
}

// IndexInfo represents information about an index
type IndexInfo struct {
	TargetName string
	IndexType  IndexType
	Params     map[string]string
}

func NewIndexInfo(targetName string, indexType IndexType, params map[string]string) *IndexInfo {
	if params == nil {
		params = make(map[string]string)
	}
	return &IndexInfo{
		TargetName: targetName,
		IndexType:  indexType,
		Params:     params,
	}
}

func (ii IndexInfo) String() string {
	return fmt.Sprintf("IndexInfo(%s, %s, %v)", ii.TargetName, ii.IndexType.String(), ii.Params)
}

// CopyFileType represents the type of file for import/export
type CopyFileType int

const (
	CopyFileTypeCSV   CopyFileType = 0
	CopyFileTypeJSON  CopyFileType = 1
	CopyFileTypeJSONL CopyFileType = 2
	CopyFileTypeFVECS CopyFileType = 3
	CopyFileTypeCSR   CopyFileType = 4
	CopyFileTypeBVECS CopyFileType = 5
)

// ImportOption represents options for importing data
type ImportOption struct {
	HasHeader    bool
	Delimiter    rune
	CopyFileType CopyFileType
}

func NewImportOption() *ImportOption {
	return &ImportOption{
		HasHeader:    false,
		Delimiter:    ',',
		CopyFileType: CopyFileTypeCSV,
	}
}

// ExportOption represents options for exporting data
type ExportOption struct {
	HasHeader    bool
	Delimiter    rune
	CopyFileType CopyFileType
	Offset       int
	Limit        int
	RowLimit     int
}

func NewExportOption() *ExportOption {
	return &ExportOption{
		HasHeader:    false,
		Delimiter:    ',',
		CopyFileType: CopyFileTypeCSV,
		Offset:       0,
		Limit:        0,
		RowLimit:     0,
	}
}

// AlterIndexOptions represents options for altering an index
type AlterIndexOptions struct {
	IndexName  string
	OptParams  []*InitParameter
}
